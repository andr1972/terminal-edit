#include "ColumnPrinter.h"
#include <algorithm>

namespace terminal {
	double ColumnPrinter::meanWidth(vector<string> &vecNames)
	{
		int sum = 0;
		for (size_t i = 0; i < vecNames.size(); i++)
			sum += vecNames[i].length();
		return double(sum) / vecNames.size();
	}

	int ColumnPrinter::tryColumns(int ncolumns, vector<string> &vecNames)
	{
		size_t columnSize = (vecNames.size() + ncolumns - 1) / ncolumns;
		sizes.clear();
		for (int i = 0; i < ncolumns - 1; i++)
		{
			size_t maxsize = 0;
			for (size_t j = i*columnSize; j < min(vecNames.size(), (i + 1)*columnSize); j++)
			{
				maxsize = max(maxsize, vecNames[j].size());
			}
			sizes.push_back(maxsize);
		}
		size_t maxsize = 0;
		if ((ncolumns - 1)*columnSize < vecNames.size())
		{
			for (size_t j = (ncolumns - 1)*columnSize; j < vecNames.size(); j++)
			{
				maxsize = max(maxsize, vecNames[j].size());
			}
			sizes.push_back(maxsize);
		}
		int result = -minimalSpace;
		for (size_t i = 0; i < sizes.size(); i++)
			result += minimalSpace + sizes[i];
		return result;
	}

	void ColumnPrinter::tryToWidth(vector<string> &vecNames, int maxx)
	{
		double mean = meanWidth(vecNames);
		int estimColumns = (int)round(maxx / (mean + 2));
		int ntry = estimColumns + 1;
		while (ntry >= 1)
		{
			int x = tryColumns(ntry, vecNames);
			if (x <= maxx) break;
			ntry--;
		}
	}

	string ColumnPrinter::pad(string str, int width)
	{
		int npad = width - str.length();
		for (int i = 0; i < npad; i++)
			str = str + " ";
		return str;
	}

	vector<string> ColumnPrinter::format(vector<string> &vecNames, int maxx)
	{
		vector<string> result;
		tryToWidth(vecNames, maxx);
		size_t ncolumns = sizes.size();
		size_t columnSize = (vecNames.size() + ncolumns - 1) / ncolumns;
		for (size_t i = 0; i < columnSize; i++)
		{
			string row = "";
			for (size_t j = 0; j < ncolumns; j++)
			{
				size_t index = j*columnSize + i;
				if (index < vecNames.size())
				{
					if (j > 0) row += pad("", minimalSpace);
					row += pad(vecNames[index], sizes[j]);
				}
			}
			row = pad(row, maxx);
			result.push_back(row);
		}
		return result;
	}
}