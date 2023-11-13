#pragma once

#include "..\Utils\ASingleton.h"

#include <map>
#include <string>

class CommandlineParameterParser :public ASingleton<CommandlineParameterParser>
{
public:

	void ParseCommandlineArguments(int argc, char* argv[]);

	void GetArgumentWithKey(const std::string& Key, std::string& Output);

private: // Utility Functions

    bool CanStringBeConsideredAnIPv4Address(std::string& IPv4Address);

    bool IsValidIPv4Address(std::string& IPAddress);

    bool IsStringAValidIPv4Address(std::string& IPv4Address);

    /*
     * Little function for us to check if the input is a valid IP Address
     * Takes an IP as a string, and tries to convert the result into its binary form to use later
     * https://learn.microsoft.com/en-us/windows/win32/api/ws2tcpip/nf-ws2tcpip-inet_pton
     */

    bool CanStringBeConsideredAPort(std::string& IPv4Address);

private: // Internal Variables

	std::map<std::string, std::string> ParsedConsoleParameters;

};

