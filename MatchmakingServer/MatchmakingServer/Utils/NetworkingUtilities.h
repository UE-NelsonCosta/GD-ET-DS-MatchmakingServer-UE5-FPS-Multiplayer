#pragma once
#include <string>

class NetworkingUtilities
{
public:

    static bool CanStringBeConsideredAnIPv4Address(std::string& IPv4Address);

    static bool IsValidIPv4Address(std::string& IPAddress);

    static bool IsStringAValidIPv4Address(std::string& IPv4Address);

    /*
     * Little function for us to check if the input is a valid IP Address
     * Takes an IP as a string, and tries to convert the result into its binary form to use later
     * https://learn.microsoft.com/en-us/windows/win32/api/ws2tcpip/nf-ws2tcpip-inet_pton
     */

    static bool CanStringBeConsideredAPort(std::string& IPv4Address);

};

