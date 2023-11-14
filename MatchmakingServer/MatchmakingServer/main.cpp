/*
* 
* Example Project To Showcase How To Create A Matchmaking Server
* This Should Be Able To Run A UE5 Server On Demand When A Session Is Ready To Go
*
* We Use Winsocks Here As The Implementation, Please Note This Is Not Platform Portable Code
* https://learn.microsoft.com/en-us/windows/win32/winsock/getting-started-with-winsock
*
*/

#pragma once

#include "ServerListenSocket/ServerListenSocket.h"
#include "CommandlineParser/CommandlineParameterParser.h"
#include "Utils/Logging.h"
#include "GameSession/GameSessionManager.h"
#include "ProjectStatics.h"

int RunApplication(ServerListenSocket& ServerSocket);

int main(int argc, char* argv[])
{
	int ErrorCode = 0;
	ServerListenSocket ServerSocket;

	// Parse Any Commandline Arguments Namely IP and Port To Connect To
	CommandlineParameterParser::Instance().ParseCommandlineArguments(argc, argv);

	// If There Is An Error Somewhere That Code Should Handle It And Return Early
	ErrorCode = ServerSocket.InitializeServerSocket();
	if (ErrorCode != NO_ERROR)
	{
		LogMessage("Failed To Initialize Server Application");

		return ErrorCode;
	}
	LogMessage("Socket Intialization Complete");

	// Run Our Client Socket
	ErrorCode = RunApplication(ServerSocket);
	if (ErrorCode != NO_ERROR)
	{
		return ErrorCode;
	}

	// Cleanup Client Socket As It Should Be Done Now
	ErrorCode = ServerSocket.TerminateServerSocket();
	if (ErrorCode != NO_ERROR)
	{
		return ErrorCode;
	}

	// No Error Reported
	return 0;
}

int RunApplication(ServerListenSocket& ServerSocket)
{
	// This runs a thread on the background that accepts all connections, firing new threads to handle each clients chain of function calls
	int ErrorCode = ServerSocket.RunServerSocket();
	if (ErrorCode != NO_ERROR)
	{
		return ErrorCode;
	}

	// Minor Control Mechanism That Main Thread Changes And Threads Can Poll And Close
	while (ProjectStatics::IsApplicationRunning)
	{
		// Check State Of Game Sessions And Launch Unreal Server Instances
		// Note: This version generally fires and forgets instances, you can control these if you intend to do so or reuse server instances
		GameSessionManager::Instance().Run();

		// Check up on state of threads that can be cleaned up
	}
}
