#pragma once
#include <string>
#include <vector>

using namespace std;

namespace terminal {
	class ColumnPrinter
	{
		const int minimalSpace = 2;
		vector<int> sizes;
		double meanWidth(vector<string> &vecNames);
		int tryColumns(int ncolumns, vector<string> &vecNames);
		void tryToWidth(vector<string> &vecNames, int maxx);
		string pad(string str, int width);
	public:
		ColumnPrinter() {};
		vector<string> format(vector<string> &vecNames, int maxx);
	};
}