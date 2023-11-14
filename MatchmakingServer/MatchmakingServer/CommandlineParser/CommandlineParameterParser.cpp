#include "CommandlineParameterParser.h"

#include "..\ProjectMacros.h"
#include <WinSock2.h>
#include <ws2tcpip.h>

void CommandlineParameterParser::ParseCommandlineArguments(int argc, char* argv[])
{
	// Note: This one is always Guaranteed To Exist 
	ParsedConsoleParameters.insert( {"ExecutionPath", argv[0]} );

	std::string CachedArgument;
	for (int i = 1; i < argc; ++i)
	{
		CachedArgument = argv[i];

		// Let's Look for the IPv4 Address
		if (CanStringBeConsideredAnIPv4Address(CachedArgument))
		{
			if (IsValidIPv4Address(CachedArgument))
			{
				// Override The IPAddress
				ParsedConsoleParameters.insert({ "ServerIP", CachedArgument });
			}
		}

		// Let's Look For The Port
		if (CanStringBeConsideredAPort(CachedArgument))
		{
			ParsedConsoleParameters.insert({ "ServerPort", CachedArgument });
		}
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

bool CommandlineParameterParser::CanStringBeConsideredAnIPv4Address(std::string& IPv4Address)
{
	// IP's have a basic minimum of 7 characters (0.0.0.0) with 3x '.' characters in it
	return IPv4Address.length() >= 7 && std::count(IPv4Address.begin(), IPv4Address.end(), '.') == 3;
}

bool CommandlineParameterParser::IsValidIPv4Address(std::string& IPAddress)
{
	// Note: Stack Allocate This Temporary Object To Help Us Parse If This Is Valid Or Not, Gets Destroyed When Leaving The Scope
	SOCKADDR_IN sa;
	return inet_pton(AF_INET, IPAddress.c_str(), &(sa.sin_addr)) == 1;
}

bool CommandlineParameterParser::IsStringAValidIPv4Address(std::string& IPv4Address)
{
	return CanStringBeConsideredAnIPv4Address(IPv4Address) && IsValidIPv4Address(IPv4Address);
}

inline bool CommandlineParameterParser::CanStringBeConsideredAPort(std::string& IPv4Address)
{
	int value = std::atoi(IPv4Address.c_str());
	return value >= MinimumSupportedSocketPort && value <= MaximumSupportedSocketPort;
}
