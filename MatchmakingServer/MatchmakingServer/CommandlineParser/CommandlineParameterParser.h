/**
* 
* NOVICE NOTE: Just search for all references of CommandlineParameterParser to figure out what this application can expect as I will not be maintaining this until the end of the development lifecycle
* 
* As Of Creating This We Expect A Few Override Parameters (24/11/2023)
* OverrideClientListenPort      - For Clients That Connect Looking For Games                (Default is in ProjectMacros Under ClientListenSocketPort)
* OverrideUEServerListenPort    - For UE Servers Letting The Matchmaker Know Of It's State  (Default is in ProjectMacros Under UEServerListenSocketPort)
* 
* UEServerIP                   - The IP Where The Clients Will Know A UE5Server Is Located To Connect To (Currently Support 1 Only)
* 
**/

#pragma once

#include <Utils\ASingleton.h>

#include <map>
#include <string>

class CommandlineParameterParser : public ASingleton<CommandlineParameterParser>
{
public:

	void ParseCommandlineArguments(int argc, char* argv[]);

    bool GetArgumentWithKey(const std::string& Key, std::string& Output);

private: // Internal Variables

	std::map<std::string, std::string> ParsedConsoleParameters;

};

