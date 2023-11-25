#include "NetworkingUtilities.h"

#include <ws2tcpip.h>
#include <ProjectMacros.h>

bool NetworkingUtilities::CanStringBeConsideredAnIPv4Address(std::string& IPv4Address)
{
	// IP's have a basic minimum of 7 characters (0.0.0.0) and a maximum of 15 (255.255.255.255) with 3x '.' characters in it
	return IPv4Address.length() >= 7 && IPv4Address.length() <= 15 && std::count(IPv4Address.begin(), IPv4Address.end(), '.') == 3;
}

bool NetworkingUtilities::IsValidIPv4Address(std::string& IPAddress)
{
	// Note: Stack Allocate This Temporary Object To Help Us Parse If This Is Valid Or Not, Gets Destroyed When Leaving The Scope
	SOCKADDR_IN sa;
	return inet_pton(AF_INET, IPAddress.c_str(), &(sa.sin_addr)) == 1;
}

bool NetworkingUtilities::IsStringAValidIPv4Address(std::string& IPv4Address)
{
	return CanStringBeConsideredAnIPv4Address(IPv4Address) && IsValidIPv4Address(IPv4Address);
}

inline bool NetworkingUtilities::CanStringBeConsideredAPort(std::string& IPv4Address)
{
	int value = std::atoi(IPv4Address.c_str());
	return value >= MinimumSupportedSocketPort && value <= MaximumSupportedSocketPort;
}
