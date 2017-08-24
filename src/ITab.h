#pragma once
#include <string>
using namespace std;

namespace terminal {
	class ITab
	{
	public:
		virtual string getNextProposal() = 0;
		virtual void changePrefix(string prefix) = 0;
	};
}
