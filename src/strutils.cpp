/*
  This file is part of https://github.com/borneq/terminal-edit
  Apache License - see LICENSE file
*/
#include <algorithm>
#include <functional>
#include <cctype>
#include <locale>
#include "strutils.h"

namespace terminal {
	// trim from start
	string &ltrim(string &s)
	{
		s.erase(s.begin(), find_if(s.begin(), s.end(),
			not1(ptr_fun<int, int>(isspace))));
		return s;
	}

	// trim from end
	string &rtrim(string &s)
	{
		s.erase(find_if(s.rbegin(), s.rend(),
			not1(ptr_fun<int, int>(isspace))).base(), s.end());
		return s;
	}

	// trim from both ends
	string &trim(string &s)
	{
		return ltrim(rtrim(s));
	}

	vector<string> lineToArgVec(string line)
	{
		vector<string> result;
		int pos = line.find_first_not_of(' ');
		int begpos = pos;
		while (pos != string::npos)
		{
			pos = line.find_first_of(' ', pos);
			string ss;
			if (pos != string::npos)
			{
				ss = line.substr(begpos, pos - begpos);
				pos = line.find_first_not_of(' ', pos + 1);
			}
			else ss = line.substr(begpos);
			result.push_back(ss);
			begpos = pos;
		}
		return result;
	}

	vector<string> splitPath(string path)
	{
		vector<string> result;
		if (path == "") return result;
		size_t previous = 0;
		size_t index = path.find(pathDelim);
		while (index != string::npos)
		{
			result.push_back(path.substr(previous, index - previous));
			previous = index + 1;
			index = path.find(pathDelim, previous);
		}
		result.push_back(path.substr(previous));
		return result;
	}
}
