#include "History.h"

namespace terminal {
	void History::put(string item)
	{
		vector<string>::iterator it = find(v.begin(), v.end(), item);
		if (it != v.end())
			v.erase(it);
		v.push_back(item);
		historyPointer = v.size();
	}

	string History::up()
	{
		if (historyPointer > 0)
		{
			historyPointer--;
			return v[historyPointer];
		}
		else return "";
	}

	string History::down()
	{
		if (historyPointer < v.size())
		{
			string s = v[historyPointer];
			historyPointer++;
			return s;
		}
		else return "";
	}

	vector<string> History::getList()
	{
		return v;
	}
}