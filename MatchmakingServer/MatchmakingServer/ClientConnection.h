#pragma once
#include <string>
#include <WinSock2.h>
#include "ProjectMacros.h"

class ClientConnection
{
public:
	SOCKET ClientSocket = 0;

	std::string Username;
	std::string Password;

	std::string AuthToken;

	// Input Buffer For Each Thread To Make Use Of
	char InputBuffer[MessageBufferSize];
};

