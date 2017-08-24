#include "Terminal.h"
#include "strutils.h"
#include "ColumnPrinter.h"
#include <curses.h>

namespace terminal {
	Terminal::Terminal(string name, string prompt, ITab *tab) : name(name), tab(tab)
	{
		initscr(); // Start curses mode
		keypad(stdscr, true); // enable extended keyboard
		noecho();// no automatic echo
		tl = make_unique<TermLines>(stdscr->_maxx, stdscr->_maxy);
		tl->prompt = prompt;
	}

	Terminal::~Terminal() {
		endwin();			/* End curses mode		  */
	}

	void Terminal::redraw()
	{
		tl->redrawAll();
	}

	string Terminal::getLine()
	{
		while (true)
		{
			int code = getch();			/* Wait for user input */
			int xxx = getcurx(stdscr);
			if (code == 26/*^Z*/) exit(0);
			else if (code >= 0x20 && code < 0x100)
			{
				tl->insertChar((char)code);
				tab->changePrefix(tl->getTabPrefix());
			}
			else if (code == 8)
			{
				tl->deleteCharBefore();
				tab->changePrefix(tl->getTabPrefix());
			}
			else if (code == 9)
			{
				string prefix = tl->getTabPrefix();
				tl->insertProposal(tab->getNextProposal().c_str());
			}
			else if (code == KEY_DC)
			{
				tl->deleteCharAfter();
				tab->changePrefix(tl->getTabPrefix());
			}
			else if (code == 3) //^C
			{
				string text = tl->getSelectedText();
				int ret = PDC_setclipboard(text.c_str(), text.length());
				switch (ret)
				{
				case PDC_CLIP_ACCESS_ERROR://"There was an error accessing the clipboard"
					break;
				case PDC_CLIP_MEMORY_ERROR://"Unable to allocate memory for clipboard contents"
					break;
				default:;//"The string was placed in the clipboard successfully"
				}
			}
			else if (code == 22) //^V
			{
				char *ptr = NULL;
				long ret, length = 0;
				ret = PDC_getclipboard(&ptr, &length);
				switch (ret)
				{
				case PDC_CLIP_ACCESS_ERROR:// "There was an error accessing the clipboard"
					break;
				case PDC_CLIP_MEMORY_ERROR://"Unable to allocate memory for clipboard contents"
					break;
				case PDC_CLIP_EMPTY://"There was no text in the clipboard"
					break;
				default: tl->insertString(ptr);
				}
			}
			else if (code == 10)
			{
				string res = tl->getEditText();
				tl->newLine(true);
				return res;
			}
			else if (code == KEY_LEFT || code == KEY_SLEFT)
				tl->left(code == KEY_SLEFT);
			else if (code == KEY_RIGHT || code == KEY_SRIGHT)
				tl->right(code == KEY_SRIGHT);
			else if (code == CTL_LEFT)
				tl->leftWord();
			else if (code == CTL_RIGHT)
				tl->rightWord();
			else if (code == KEY_DOWN)
				tl->replaceLine(history.down().c_str());
			else if (code == KEY_UP)
				tl->replaceLine(history.up().c_str());
			else if (code == KEY_END)
				tl->end();
			else if (code == KEY_HOME)
				tl->home();
			tl->redrawAll();
		}
		return "";
	}

	void Terminal::read()
	{
		tl->writePrompt();
		tl->redrawAll();
		string line = getLine();
		history.put(line);
		line = trim(line);
		argvStr = lineToArgVec(line);
		argvSz.clear();
		argvSz.push_back(name.c_str());//simulate argv[0] element
		for (size_t i = 0; i < argvStr.size(); i++)
			argvSz.push_back(argvStr[i].c_str());
		argc = argvSz.size();
		argv = argvSz.data();
	}

	void Terminal::writeln(string str)
	{
		tl->insertString(str.c_str());
		tl->newLine(true);
	}

	void Terminal::writeColumns(vector<string> &vecNames)
	{
		ColumnPrinter colp;
		vector<string> lines = colp.format(vecNames, stdscr->_maxx);
		for (int i = 0; i < lines.size(); i++)
			tl->insertString(lines[i].c_str());
	}
}