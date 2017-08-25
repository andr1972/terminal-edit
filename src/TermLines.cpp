/*
  This file is part of https://github.com/borneq/terminal-edit
  Apache License - see LICENSE file
*/
#include "TermLines.h"
#include <assert.h>
#include <curses.h>
#include <algorithm>

namespace terminal {
	vector<Line> partialRange(Line line, int y, Pos &from, Pos &to)
	{
		vector<Line> res;
		if (from.y == to.y)
		{
			Line l;
			l.s = line.s.substr(0, from.x);
			l.lf = false;
			res.push_back(l);
			l.s = line.s.substr(from.x, to.x - from.x);
			res.push_back(l);
			l.s = line.s.substr(to.x);
			l.lf = line.lf;
			res.push_back(l);
		}
		else if (from.y == y)
		{
			Line l;
			l.s = line.s.substr(0, from.x);
			l.lf = false;
			res.push_back(l);
			l.s = line.s.substr(from.x);
			l.lf = line.lf;
			res.push_back(l);
		}
		else //to.y == y
		{
			Line l;
			l.s = "";
			l.lf = false;
			res.push_back(l);
			l.s = line.s.substr(0, to.x);
			l.lf = false;
			res.push_back(l);
			l.s = line.s.substr(to.x);
			l.lf = line.lf;
			res.push_back(l);
		}
		return res;
	}

	vector<Line> yRange(Line line, int y, Pos &from, Pos &to)
	{
		bool hi;
		if (y < from.y) hi = false;
		else if (y > to.y) hi = false;
		else if (y > from.y && y < to.y) hi = true;
		else return partialRange(line, y, from, to);
		vector<Line> res;
		Line l("", false);
		if (hi) res.push_back(l);
		res.push_back(line);
		return res;
	}

	TermLines::TermLines(int maxx, int maxy) : maxx(maxx), maxy(maxy)
	{
		v.push_back(Line("", true));
		offset = 0;
		deleteHi();
	}

	string TermLines::getString(int n)
	{
		return v[n].s;
	}

	char TermLines::getCurChar()
	{
		return v[cur.y].s[cur.x];
	}

	void TermLines::updateMarkersLeft(Pos &pos)
	{
		leftMarkAfter(tabPos, pos);
		leftMarkAfter(tabStart, pos);
		leftMarkAfter(begHi, pos);
		leftMarkAfter(endHi, pos);
	}

	void TermLines::updateMarkersRight(Pos &pos)
	{
		rightMarkAfter(tabPos, pos);
		rightMarkAfter(tabStart, pos);
		rightMarkAfter(begHi, pos);
		rightMarkAfter(endHi, pos);
	}

	void TermLines::newLine(bool lf)
	{
		v[cur.y].lf = lf;
		cur.y++;
		cur.x = 0;
		v.push_back(Line("", true));
		if (cur.y - offset >= stdscr->_maxy)
			offset = cur.y - stdscr->_maxy + 1;
	}

	void TermLines::moveLeftChain()
	{
		size_t n = cur.y + 1;
		while (n < v.size() && !v[n - 1].lf)
		{
			if (v[n].s.length() > 0)
			{
				char c = v[n].s[0];
				v[n - 1].s += c;
				v[n].s.erase(0, 1);
			}
			n++;
		}
		if (n - 1 != cur.y && v[n - 1].s.length() == 0) //"n-1 != cur.y" must be at least one editable line
			v.erase(v.begin() + n - 1);
	}

	void TermLines::moveRightChain()
	{
		size_t n = cur.y + 1;
		while (n < v.size() && !v[n - 1].lf)
		{
			char c = v[n - 1].s.back();
			v[n - 1].s.pop_back();
			v[n].s = string(1, c) + v[n].s;
			n++;
		}
	}

	void TermLines::backSpaceRow()
	{
		if (cur.y == 0) return;
		if (v[cur.y - 1].lf) return;
		beep();
		if (v[cur.y - 1].s.length() > 0)
			v[cur.y - 1].s.pop_back();
		cur.x = v[cur.y - 1].s.length();
		cur.y--;
	}

	void TermLines::deleteCharBefore()
	{
		if (isStarting()) return;
		deleteHi();
		if (cur.x == 0)
			backSpaceRow();
		else
		{
			v[cur.y].s.erase(cur.x - 1, 1);
			cur.x--;
		}
		moveLeftChain();
		updateMarkersLeft(cur);
	}

	void TermLines::deleteCharAtPos(Pos &pos)
	{
		if (isEndingAtPos(pos)) return;
		deleteHi();
		v[pos.y].s.erase(pos.x, 1);
		moveLeftChain();
		updateMarkersLeft(pos);
	}

	void TermLines::deleteCharAfter()
	{
		deleteCharAtPos(cur);
		updateMarkersLeft(cur);
	}

	void TermLines::insertChar(char c)
	{
		if (c == '\n')
			newLine(true);
		else
		{
			if (c == '\t')
				c = ' ';
			moveRightChain();
			v[cur.y].s.insert(cur.x, 1, c);
			cur.x++;
			if (cur.x >= maxx)
				newLine(false);
		}
		updateMarkersRight(cur);
	}

	///append at end
	void TermLines::appendString(string s, bool bMove)
	{
		assert(cur.y == v.size() - 1);
		string str = v[cur.y].s + s;
		string substr;
		int pos = 0;
		v.pop_back();
		do {
			substr = str.substr(pos, min((size_t)maxx, str.length() - pos));
			Line l;
			l.s = substr;
			if (substr.length() < maxx)
				l.lf = true;
			else
				l.lf = false;
			v.push_back(l);
			pos += maxx;
		} while (substr.length() == maxx);
		if (bMove) end();
	}

	string TermLines::getTextAfterCursor()
	{
		string res = v[cur.y].s.substr(cur.x);
		int n = cur.y + 1;
		while (n < v.size())
		{
			res += v[n].s;
			n++;
		}
		return res;
	}

	void TermLines::eraseTextAfterCursor()
	{
		v[cur.y].s = v[cur.y].s.substr(0, cur.x);
		v.resize(cur.y + 1);
	}

	void TermLines::insertString(const char *s)
	{
		string str = getTextAfterCursor();
		eraseTextAfterCursor();
		appendString(s, true);
		appendString(str, false);
	}

	void TermLines::insertProposal(const char *s)
	{
		while (tabPos.greaterThan(tabStart))
			deleteCharAtPos(tabStart);
		cur = tabStart;
		insertString(s);
	}

	void TermLines::writePrompt()
	{
		assert(v[cur.y].s == "");
		appendString(prompt, true);
		begPos = cur;
	}

	string TermLines::getWholeText()
	{
		string res = "";
		for (int i = 0; i < v.size(); i++)
		{
			res += v[i].s;
			if (v[i].lf)
				res += "\n";
		}
		return res;
	}

	string TermLines::getMarkedText(Pos &from, Pos &to)
	{
		if (from.noValue() || to.noValue()) return "";
		string res = "";
		for (int i = from.y; i <= to.y; i++)
		{
			vector<Line> lines = yRange(v[i], i, from, to);
			if (lines.size() > 1)
			{
				Line l = lines[1];
				res += l.s;
				if (l.lf)
					res += "\n";
			}
		}
		return res;
	}

	string TermLines::getSelectedText()
	{
		return getMarkedText(begHi, endHi);
	}

	string TermLines::getEditText()
	{
		int n = begPos.y;
		string res = v[n].s.substr(begPos.x);
		n++;
		while (n < v.size() && !v[n - 1].lf)
		{
			res += v[n].s;
			n++;
		}
		return res;
	}

	void TermLines::deleteHi()
	{
		begHi.setNoValue();
		endHi.setNoValue();
	}

	void TermLines::leftMark(Pos &pos)
	{
		if (isStarting()) return;
		if (pos.noValue()) return;
		if (pos.x > 0) pos.x--;
		if (pos.x == 0 && pos.y > 0 && !v[pos.y - 1].lf)
		{
			pos.y--;
			pos.x = v[pos.y].s.length() - 1;
		}
	}

	void TermLines::leftMarkAfter(Pos &pos, Pos &basepos)
	{
		if (pos.greaterThan(basepos))
			leftMark(pos);
	}


	void TermLines::left(bool isShift)
	{
		if (isStarting()) return;
		if (isShift)
		{
			if (endHi.noValue()) endHi = cur;
		}
		else deleteHi();
		leftMark(cur);
		if (isShift) begHi = cur;
	}

	void TermLines::leftWord()
	{
		if (isStarting()) return;
		left(false);
		while (!isStarting() && getCurChar() == ' ')
			left(false);
		while (!isStarting() && getCurChar() != ' ')
			left(false);
		if (getCurChar() == ' ') right(false);
	}

	void TermLines::leftWordNoSpace()
	{
		if (isStarting()) return;
		left(false);
		while (!isStarting() && getCurChar() != ' ')
			left(false);
		if (getCurChar() == ' ') right(false);
	}

	void TermLines::rightMark(Pos &pos)
	{
		if (isEnding()) return;
		if (pos.noValue()) return;
		if (pos.x < v[pos.y].s.length() - 1)
			pos.x++;
		else if (!v[pos.y].lf)
		{
			pos.y++;
			pos.x = 0;
		}
		else if (pos.x == v[pos.y].s.length() - 1)
			pos.x++;
	}

	void TermLines::rightMarkAfter(Pos &pos, Pos &basepos)
	{
		if (pos.greaterThan(basepos))
			rightMark(pos);
	}

	void TermLines::right(bool isShift)
	{
		if (isEnding()) return;
		if (isShift)
		{
			if (begHi.noValue()) begHi = cur;
		}
		else deleteHi();
		rightMark(cur);
		if (isShift) endHi = cur;
	}

	void TermLines::rightWord()
	{
		if (isEnding()) return;
		while (!isEnding() && getCurChar() != ' ')
			right(false);
		while (!isEnding() && getCurChar() == ' ')
			right(false);
		//if (getCurChar() == ' ') left(false);
	}

	void TermLines::down()
	{
		if (v.size() > maxy + offset)
			offset++;
	}

	void TermLines::up()
	{
		if (offset > 0)
			offset--;
	}

	void TermLines::home()
	{
		cur = begPos;
	}

	void TermLines::end()
	{
		size_t n = cur.y + 1;
		while (n < v.size() && !v[n - 1].lf)
		{
			n++;
		}
		cur.y = n - 1;
		cur.x = v[cur.y].s.length();
		if (cur.y - offset >= maxy)
			offset = cur.y - maxy + 1;
	}

	bool TermLines::isStarting()
	{
		return cur.y == begPos.y && cur.x == begPos.x;
	}

	bool TermLines::isEndingAtPos(Pos &pos)
	{
		if (v.size() == 0) return true;
		return pos.y == v.size() - 1 && pos.x == v[pos.y].s.length();
	}

	bool TermLines::isEnding()
	{
		return isEndingAtPos(cur);
	}

	void TermLines::redrawAll()
	{
		clear();
		for (int i = offset; i < min(offset + maxy, v.size()); i++)
		{
			move(i - offset, 0);
			vector<Line> lines = yRange(v[i], i, begHi, endHi);
			bool bReverse = false;
			for (int j = 0; j < lines.size(); j++)
			{
				if (bReverse) attron(A_REVERSE);
				if (lines[j].s.length() > 0)
					printw("%s", lines[j].s.c_str());
				if (bReverse) attroff(A_REVERSE);
				bReverse = !bReverse;
			}
		}
		move(cur.y - offset, cur.x);
		refresh();			/* Print it on to the real screen */
	}

	void TermLines::emptyLine()
	{
		home();
		eraseTextAfterCursor();
	}

	void TermLines::replaceLine(const char *s)
	{
		emptyLine();
		insertString(s);
	}

	string TermLines::getTabPrefix()
	{
		tabPos = cur;
		leftWordNoSpace();
		tabStart = cur;
		string result = getMarkedText(tabStart, tabPos);
		cur = tabPos;
		return result;
	}
}
