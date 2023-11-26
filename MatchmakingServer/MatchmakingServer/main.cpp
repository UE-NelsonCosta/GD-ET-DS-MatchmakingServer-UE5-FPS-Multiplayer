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

#include <ServerSocketManager/ServerSocketManager.h>
#include <CommandlineParser/CommandlineParameterParser.h>
#include <Utils/Logging.h>
#include <GameSession/GameSessionManager.h>
#include <ProjectStatics.h>

int RunApplication();

int main(int argc, char* argv[])
{
	int ErrorCode = 0;

	std::shared_ptr<ServerSocketManager> ServerSocket = ServerSocketManager::InstanceAsStrongPointer();

	// Parse Any Commandline Arguments Namely IP and Port To Connect To
	CommandlineParameterParser::Instance().ParseCommandlineArguments(argc, argv);

	// If There Is An Error Somewhere That Code Should Handle It And Return Early
	ErrorCode = ServerSocket->InitializeServerSocket();
	if (ErrorCode != NO_ERROR)
	{
		LogMessage("Failed To Initialize Server Application");

		return ErrorCode;
	}
	LogMessage("Socket Intialization Complete");

	// Run Our Client Socket
	ErrorCode = RunApplication();
	if (ErrorCode != NO_ERROR)
	{
		return ErrorCode;
	}

	// Cleanup Client Socket As It Should Be Done Now
	ErrorCode = ServerSocket->TerminateServerSocket();
	if (ErrorCode != NO_ERROR)
	{
		return ErrorCode;
	}

	// No Error Reported
	return 0;
}

int RunApplication()
{
	// This runs a thread on the background that accepts all connections, firing new threads to handle each clients chain of function calls
	int ErrorCode = ServerSocketManager::InstanceAsStrongPointer()->RunServerSocket();
	if (ErrorCode != NO_ERROR)
	{
		return ErrorCode;
	}

	// Minor Control Mechanism That Main Thread Changes And Threads Can Poll And Close
	while (ProjectStatics::IsApplicationRunning)
	{
		// Check State Of Game Sessions And Launch Unreal Server Instances
		// Note: This version generally fires and forgets instances, you can control these if you intend to do so or reuse server instances
		GameSessionManager::Instance().ValidateStateOfGameSessions();
		
		continue;

		// Check up on state of threads that can be cleaned up
		// Note: Was using auto previously, but it was causing a copy of this vector, which resulted in erroneous data! Try not use autos!
		std::scoped_lock DataLock(ServerSocketManager::Instance().ClientMessageJobMutex);
		std::vector<std::shared_ptr<ClientMessageJob>>& ClientJobs = ServerSocketManager::Instance().ClientMessageJobs;
		for(auto iterator = ClientJobs.begin(); iterator != ClientJobs.end(); ++iterator)
		{
			std::shared_ptr<ClientMessageJob> CurrentClientJob = *iterator;

			if(!CurrentClientJob)
			{
				iterator = ClientJobs.erase(iterator);
				continue;
			}

			// If This Thread Has Completed It's Task Clean It Up As Well As It's Client Connection
			if(CurrentClientJob->IsJobComplete())
			{
				ServerSocketManager::Instance().RemoveClientConnection(CurrentClientJob->GetClientConnection());
				CurrentClientJob->Worker.join();
				iterator = ClientJobs.erase(iterator);
				continue;
			}

			// Check The State Of The Socket Connection (recv with msg_peek)
			//if(CurrentClientJob)
		}
	}

	return 0;
}
