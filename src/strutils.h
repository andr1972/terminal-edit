/*
  This file is part of https://github.com/borneq/terminal-edit
  Apache License - see LICENSE file
*/
#pragma once
#include <string>
#include <vector>

using namespace std;

namespace terminal {
#if _WIN32
	const char pathDelim = ';';
	const char dirDelim = '\\';
#else
	const char pathDelim = ':';
	const char dirDelim = '/';
#endif

	string &ltrim(string &s);
	string &rtrim(string &s);
	string &trim(string &s);
	vector<string> lineToArgVec(string line);
	vector<string> splitPath(string path);
}
