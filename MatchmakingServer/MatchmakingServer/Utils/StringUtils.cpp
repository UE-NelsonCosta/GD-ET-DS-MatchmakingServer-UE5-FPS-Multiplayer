#include "StringUtils.h"

int StringUtils::FindNOccurenceReverse(const std::string& StringToSearch, const char* SearchSequence, int TotalOccurences)
{
	int Index = StringToSearch.size();
	for(int i = 0; i < TotalOccurences; ++i)
	{
		Index = StringToSearch.rfind(SearchSequence, Index-1);
	}
	return Index;
}
