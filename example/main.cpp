#include <string>
#include <iostream>
#include <vector>
#include <boost/filesystem.hpp>
#include <boost/range/iterator_range.hpp>
#include "../src/strutils.h"
#include "../src/Terminal.h"
#include "CommandProcessor.h"
#include "FileTab.h"

using namespace std;
using namespace boost::filesystem;


extern CmdMap cmdMap[];

CmdMap *findCmd(string name_to_find)
{
	for (int i = 0; i < CmdCount; i++)
	{
		if (cmdMap[i].cmdName == name_to_find)
			return &cmdMap[i];
	}
	return nullptr;
}

void loop()
{
	path full_path(current_path());
	FileTab fileTab;
	Terminal term("terminal", full_path.string() + "$", &fileTab);
	CommandProcessor cp(&term, &fileTab);
	while (true)
	{
		term.redraw();
		term.read();
		if (term.argc <= 1) continue;
		CmdMap *cmdMap = findCmd(term.argv[1]);
		if (cmdMap)
			cp.run(cmdMap, term.argc, term.argv);
		else
			cp.call(term.argc, term.argv);
	}
}

int main(int argc, const char** argv)
{
	loop();
	return 0;
}
