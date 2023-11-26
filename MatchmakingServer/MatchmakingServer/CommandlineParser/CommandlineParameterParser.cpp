#include "CommandlineParameterParser.h"

#include <ProjectMacros.h>
#include <algorithm>
#include <Utils/StringUtils.h>


void CommandlineParameterParser::ParseCommandlineArguments(int argc, char* argv[])
{
	// Note: This one is always Guaranteed To Exist 
	// We Extract the application path which is 4 / backwards
	std::string ApplicationPath = argv[0];
	std::replace(ApplicationPath.begin(), ApplicationPath.end(), '\\', '/');
	
	int Seperator = StringUtils::FindNOccurenceReverse(ApplicationPath, "/", 4);
	ParsedConsoleParameters.insert( {"ProjectBasePath", ApplicationPath.substr(0, Seperator+1)} );

	std::string CachedArgument;
	for (int i = 1; i < argc; ++i)
	{
		CachedArgument = argv[i];

		int Seperator = CachedArgument.find("=");

		std::string KeyValue = CachedArgument.substr(0, Seperator);
		std::string ParamValue = CachedArgument.substr(Seperator+1, CachedArgument.size() - Seperator);
		ParsedConsoleParameters.insert({ KeyValue, ParamValue });
	}
}

bool CommandlineParameterParser::GetArgumentWithKey(const std::string& Key, std::string& Output)
{
	auto Result = ParsedConsoleParameters.find(Key);
	if(Result != ParsedConsoleParameters.end())
	{
		Output = Result->second;
		return true;
	}

	return false;
}
