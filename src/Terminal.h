#pragma once
#include <string>
#include <vector>
#include <memory>
#include "TermLines.h"
#include "History.h"
#include "ITab.h"

using namespace std;

namespace terminal {
	class Terminal
	{
	private:
		string name;
		vector<string> argvStr;
		vector<const char*> argvSz;
		unique_ptr<TermLines> tl;
		ITab *tab;
	public:
		Terminal(string name, string prompt, ITab *tab);
		~Terminal();
		void setPrompt(string prompt) { tl->prompt = prompt; }
		string getPrompt() { return tl->prompt; }
		History history;
		int argc;
		const char **argv;
		void redraw();
		string getLine();
		void read();
		void writeln(string str);
		void writeColumns(vector<string> &vecNames);
	};
}