#include "CommandlineParameterParser.h"

#include <ProjectMacros.h>

void CommandlineParameterParser::ParseCommandlineArguments(int argc, char* argv[])
{
	// Note: This one is always Guaranteed To Exist 
	ParsedConsoleParameters.insert( {"ExecutionPath", argv[0]} );

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
