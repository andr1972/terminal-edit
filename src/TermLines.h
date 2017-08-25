/*
  This file is part of https://github.com/borneq/terminal-edit
  Apache License - see LICENSE file
*/
#pragma once
#include <string>
#include <vector>

using namespace std;

namespace terminal {
	struct Line
	{
		string s;
		bool lf;
		Line() {}
		Line(string s, bool lf) :s(s), lf(lf) {}
	};

	struct Pos
	{
		int y, x;
		Pos() { y = 0, x = 0; }
		bool noValue() { return y < 0 || x < 0; }
		void setNoValue() { y = -1; x = -1; }
		bool eq(Pos &other) { return y == other.y && x == other.x; }
		bool greaterThan(Pos &other) {
			return y > other.y || x > other.x;
		}
	};

	class TermLines
	{
	private:
		vector<Line> v;
		unsigned int maxx;
		unsigned int maxy;
		Pos cur;
		Pos begPos; //begin editable, after prompt
		Pos tabPos, tabStart;
		Pos begHi, endHi;
		size_t offset;
		void moveLeftChain();
		void moveRightChain();
		void backSpaceRow();
		void appendString(string s, bool bMove);
		string getTextAfterCursor();
		void eraseTextAfterCursor();
		string getMarkedText(Pos &from, Pos &to);
		void deleteCharAtPos(Pos &pos);
		bool isEndingAtPos(Pos &pos);
		void leftMark(Pos &cur);
		void leftMarkAfter(Pos &pos, Pos &basepos);
		void rightMark(Pos &cur);
		void rightMarkAfter(Pos &pos, Pos &baseposr);
		void updateMarkersLeft(Pos &pos);
		void updateMarkersRight(Pos &pos);
		void leftWordNoSpace();
	public:
		string prompt;
		TermLines(int maxx, int maxy);
		string getString(int n);
		char getCurChar();
		void newLine(bool lf);
		void deleteCharBefore();
		void deleteCharAfter();
		void insertChar(char c);
		void insertString(const char *s);
		void insertProposal(const char *s);
		void emptyLine();
		void replaceLine(const char *s);
		void writePrompt();
		string getWholeText();
		string getSelectedText();
		string getEditText();
		void deleteHi();
		void left(bool isShift);
		void leftWord();
		void right(bool isShift);
		void rightWord();
		void down();
		void up();
		void home();
		void end();
		bool isStarting();
		bool isEnding();
		void redrawAll();
		string getTabPrefix();
	};
}
