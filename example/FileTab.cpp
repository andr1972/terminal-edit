#include "FileTab.h"
#include <boost/filesystem.hpp>
#include <boost/range/iterator_range.hpp>

using namespace boost::filesystem;

void FileTab::readDir()
{
	path p(".");
	if (is_directory(p)) {
		for (directory_entry& entry : boost::make_iterator_range(directory_iterator(p), {}))
		{
			auto path = entry.path();
			string filenameStr = path.filename().string();
			if (is_directory(path))
				filenameStr += "/";
			vecNames.push_back(filenameStr);
		}
	}
}

string FileTab::getNextProposal()
{
	if (vecFiltered.size() == 0) return "";
	string res = vecFiltered[index];
	index++;
	if (index >= vecFiltered.size()) index = 0;
	return res;
}

void FileTab::changePrefix(string prefix)
{
	vecFiltered.clear();
	index = 0;
	size_t n = prefix.length();
	for (int i = 0; i < vecNames.size(); i++)
		if (vecNames[i].substr(0, n) == prefix)
			vecFiltered.push_back(vecNames[i]);
}
