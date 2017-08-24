#pragma once
#include "ITab.h"
#include <string>
#include <vector>

using namespace std;
using namespace terminal;

class FileTab : public ITab
{
	vector<string> vecNames;
	vector<string> vecFiltered;
	int index = 0;
public:
	FileTab() {};
	void readDir();
	string getNextProposal();
	void changePrefix(string prefix);
};

