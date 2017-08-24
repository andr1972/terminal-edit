#include "CommandProcessor.h"
#include "strutils.h"
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <boost/filesystem.hpp>
#include <boost/range/iterator_range.hpp>
#include <sys/stat.h>

using namespace boost::filesystem;

CmdMap cmdMap[CmdCount] = {
	{ &CommandProcessor::list,"ls", "List files in DIR" },
	{ &CommandProcessor::history,"history", "List editline history" },
	{ &CommandProcessor::pwd,"pwd", "Print the current working directory" },
	{ &CommandProcessor::help,"help", "Display this text" },
	{ &CommandProcessor::cd,"cd","Change to directory DIR" },
	{ &CommandProcessor::quit,"quit", "Quit using Fileman" },
	{ &CommandProcessor::quit,"exit", "Synonym for Quit" }
};


//http://www.geeksforgeeks.org/wildcard-character-matching/
// The main function that checks if two given strings
// match. The mask string may contain wildcard characters
bool match(const char *mask, const char *str)
{
	// If we reach at the end of both strings, we are done
	if (*mask == '\0' && *str == '\0')
		return true;

	// Make sure that the characters after '*' are present
	// in str string. This function assumes that the mask
	// string will not contain two consecutive '*'
	if (*mask == '*' && *(mask + 1) != '\0' && *str == '\0')
		return false;

	// If the mask string contains '?', or current characters
	// of both strings match
	if (*mask == '?' || *mask == *str)
		return match(mask + 1, str + 1);

	// If there is *, then there are two possibilities
	// a) We consider current character of str string
	// b) We ignore current character of str string.
	if (*mask == '*')
		return match(mask + 1, str) || match(mask, str + 1);
	return false;
}

CommandProcessor::CommandProcessor(Terminal *term, FileTab *tab) :term(term), tab(tab) {
	tab->readDir();
}

void CommandProcessor::list(int argc, const char** argv) {
	bool matchAll = argc <= 2;
	string mask = "";
	if (!matchAll)
		mask = argv[2];
	path p(".");
	if (is_directory(p)) {
		vector<string> vecNames;
		for (directory_entry& entry : boost::make_iterator_range(directory_iterator(p), {}))
		{
			auto path = entry.path();
			string filenameStr = path.filename().string();
			size_t size;
			if (is_regular_file(path))
				size = file_size(entry.path());
			else
			{
				size = 0;
				filenameStr += "/";
			}
			if (matchAll || match(mask.c_str(), filenameStr.c_str()))
				vecNames.push_back(filenameStr + ": " + to_string(size));
		}
		term->writeColumns(vecNames);
	}
}

void CommandProcessor::history(int argc, const char** argv) {
	vector<string> histList = term->history.getList();
	for (int i = 0; i < histList.size(); i++)
		term->writeln(histList[i]);
}

void CommandProcessor::pwd(int argc, const char** argv) {
	path full_path(current_path());
	term->writeln(full_path.string());
}

void CommandProcessor::help(int argc, const char** argv) {
	for (int i = 0; i < CmdCount; i++)
		term->writeln((string)cmdMap[i].cmdName + " : " + cmdMap[i].doc);
}

void CommandProcessor::cd(int argc, const char** argv) {
	if (argc <= 2)
		term->writeln("give directory");
	else
	{
		current_path(argv[2]);
		path full_path(current_path());
		term->setPrompt(full_path.string() + "$");
		tab->readDir();
	}
}

void CommandProcessor::quit(int argc, const char** argv) {
	exit(0);
}

string searchInPATH(string name)
{
	const char* env_p;
	env_p = std::getenv("PATH");
	if (!env_p) return "";
	vector<string> paths = splitPath(env_p);
	for (int i = 0; i < paths.size(); i++)
	{
		string path = paths[i];
		if (path == "") continue;
		if (path.back() != dirDelim)
			path += dirDelim;
		path += name;
		if (exists(path))
			return path;
	}
	return "";
}

bool isExecutable(const char *filename)
{
	struct stat  st;
	if (stat(filename, &st) < 0)
		return false;
	if ((st.st_mode & S_IEXEC) != 0)
		return true;
	return false;
}

void CommandProcessor::call(int argc, const char** argv) {
	string name = argv[1];
	string pathStr;
	if (name.find(dirDelim) == string::npos)
	{
		pathStr = searchInPATH(name);
		if (pathStr == "")
		{
			term->writeln(name + " not found in path");
			return;
		}
	}
	else
	{
		path rel_path(name);
		path full_path = absolute(rel_path);
		full_path.normalize();
		pathStr = full_path.string();
		if (pathStr == "")
		{
			term->writeln(name + " not found");
			return;
		}
	}
	if (isExecutable(pathStr.c_str()))
		term->writeln("call " + pathStr);
	else
		term->writeln("is not executable " + pathStr);
}

void (CommandProcessor::*ptr)(int argc, const char** argv);
void CommandProcessor::run(CmdMap *cmdMap, int argc, const char** argv)
{
	(this->*(cmdMap->command))(argc, argv);
	term->writeln("");
}