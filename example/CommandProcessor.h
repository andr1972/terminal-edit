#pragma once
#include "Terminal.h"
#include "FileTab.h"

struct CommandProcessor;

struct CmdMap
{
	void (CommandProcessor::*command)(int argc, const char** argv);
	char *cmdName;
	char *doc;
};

struct CommandProcessor
{
	Terminal *term;
	FileTab *tab;
	CommandProcessor(Terminal *term, FileTab *tab);
	void list(int argc, const char** argv);
	void history(int argc, const char** argv);
	void pwd(int argc, const char** argv);
	void help(int argc, const char** argv);
	void cd(int argc, const char** argv);
	void quit(int argc, const char** argv);
	void call(int argc, const char** argv);
	void (CommandProcessor::*ptr)(int argc, const char** argv);
	void run(CmdMap *cmdMap, int argc, const char** argv);
};

const int CmdCount = 7;