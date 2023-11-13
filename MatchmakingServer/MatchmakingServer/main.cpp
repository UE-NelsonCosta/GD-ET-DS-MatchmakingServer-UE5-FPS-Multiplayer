/*
* Example Project To Showcase How To Create A Matchmaking Server
* This Should Be Able To Run A UE5 Server On Demand When A Session Is Ready To Go
*
* We Use Winsocks Here As The Implementation, Please Note This Is Not Platform Portable Code
* https://learn.microsoft.com/en-us/windows/win32/winsock/getting-started-with-winsock
*
* Lifetime For Server Sockets Should Be Something Of The Sort
*
* Initialization:
* WSAStartup: initiates the Winsock DLL;
* socket: creates a socket that is bound to a transport service provider;  bind: associates a local address with a socket;
* listen: sets the socket to listen mode to let it accept communications;
*
* Lifetime Communication:
* accept: accept a connection
* recv: receives data from a connected socket;
* send: sends data on a connected socket;
*
* Termination:
* closesocket: closes an existing socket;
* WSACleanup: terminates use of the Winsock DLL.
*
*/

#pragma once

#include "ServerListenSocket/ServerListenSocket.h"
#include "CommandlineParser/CommandlineParameterParser.h"
#include "Utils/Logging.h"

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
	ErrorCode = ServerSocket.RunServerSocket();
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

#pragma region RuntimeLogic



//// TODO: This should handle when the connection from the client dissapears more gracefully
//void ProducerThread_ReadMessages(ClientConnection& ClientConnection)
//{
//	char SocketReadBuffer[MessageBufferSize];
//	char SocketExcessReadBuffer[MessageBufferSize];
//
//	int ReadByteOffset = 0;
//	int TotalBytesRead = 0;
//	do
//	{
//		ZeroMemory(SocketReadBuffer, sizeof(SocketReadBuffer));
//		TotalBytesRead = 0;
//		ReadByteOffset = 0;
//
//		bool ReadingCompleteMessage = true;
//		while (ReadingCompleteMessage)
//		{
//			int BytesReceived = ReceiveData(ClientConnection.ClientSocket, (SocketReadBuffer + ReadByteOffset));
//
//			//  0 Means It Closed Gracefully AKA Client Is Done
//			// -1 Means It's Failed AKA SOCKET_ERROR, use GetLastWSAError() for specifics on the error
//			// In this case if either happens I'll just kill anything left over and clean it up
//			if (BytesReceived == SOCKET_ERROR || BytesReceived == 0)
//			{
//				break;
//			}
//
//			// Let's Read The TotalBytes
//			TotalBytesRead += BytesReceived;
//			ReadByteOffset += BytesReceived;
//
//			// Only Go Ahead If We Have Enough Data To Start Reading From
//			if (TotalBytesRead < sizeof(int))
//			{
//				continue;
//			}
//
//
//			int MessageSize = *((int*)SocketReadBuffer);
//			if (TotalBytesRead >= (MessageSize + sizeof(int) * 2))
//			{
//				// Should Have Received The Full Message At This Point So Let's Extract The Bit We Need And Reset The Excess To The Start Of The Buffer
//				Command_ConnectionAuthentication C;
//				C.Deserialize(SocketReadBuffer);
//
//
//				if (TotalBytesRead)
//				{
//					// Let's Parse The Message
//				}
//			}
//		}
//
//		//std::cout << "Received Message From Client " << ClientID << " with Length: " << BytesReceived << " Bytes!" << std::endl;
//
//		// This is an example of soemthing we can save out later for analytics or just bookeeping
//		ClientMessage& EmplacedClientMessage = MessageHistory.emplace_back(ClientConnection.ClientID, SocketReadBuffer);
//
//		// TODO: This Requires Locking
//		// Let's place this into a nice little queue that our friend the consumer can use
//		ProducerConsumerDataLock.lock();
//
//		ProducerDataToConsume.emplace(std::ref(EmplacedClientMessage));
//
//		ProducerConsumerDataLock.unlock();
//
//		// Let's Notify That Lovable Nugget That He Needs To Get Back To Work
//		std::lock_guard lock(ConditionalVariableMutex);
//		ProducerToConsumerNotifier.notify_one();
//
//	} while (true);
//
//	// TODO: Cleanup Data As Your Dedicated Socket To Listen To Broke Down It's Connection
//}
//
//void ConsumerThread_BroadcastMessages()
//{
//	// Consume Ad Infinitum
//	while (true)
//	{
//		// We Lock This Critical Section Right Off The Bat
//		std::unique_lock ConsumerLock(ConditionalVariableMutex);
//
//		// We Want To Wait Right Off The Bat As Well, So We Release The Lock Till Notified As We Do Not Have Anything To Do Yet, So We Go To Sleep Until Told Otherwise
//		ProducerToConsumerNotifier.wait(ConsumerLock);
//		// Alternatively If We Can Have Data Already Ready To Be Processed, Let's Go Ahead And Give It A Proper Condition (Imagine You Can Load And Broadcast Older Messages)
//		//ProducerToConsumerNotifier.wait
//		//(
//		//	ConsumerLock, 
//		//	[]()
//		//	{ 
//		//		return ProducerDataToConsume.size() > 0 ? true : false; 
//		//	} 
//		//);
//
//		ProducerConsumerDataLock.lock();
//
//		ClientMessage& MessageToSend = ProducerDataToConsume.front();
//		ProducerDataToConsume.pop();
//
//		ProducerConsumerDataLock.unlock();
//
//		for (int i = 0; i < ClientConnections.size(); ++i)
//		{
//			SOCKET& ClientSocket = ClientConnections[i].ClientSocket;
//			if (ClientSocket == 0)
//			{
//				continue;
//			}
//
//			SendData(ClientSocket, MessageToSend.Message.c_str(), MessageToSend.Message.length() * sizeof(char));
//		}
//	}
//}


#pragma endregion RuntimeLogic

