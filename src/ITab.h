/*
  This file is part of https://github.com/borneq/terminal-edit
  Apache License - see LICENSE file
*/
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
