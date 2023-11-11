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
#include "Utils_Server.h"
#include "Utils_MessageParsing.h"

// Dynamically linking of a lib file, you normally do this in your projects settings
#pragma comment(lib, "ws2_32.lib")

int  RunServerApplication();
int  AcceptConnection();
void Thread_HandleClient(ClientConnection& Client);


int main(int argc, char* argv[])
{
	int ErrorCode = 0;

	// Parse Any Commandline Arguments Namely IP and Port To Connect To
	ParseCommandlineArguments(argc, argv);

	// If There Is An Error Somewhere That Code Should Handle It And Return Early
	ErrorCode = InitializeServerApplication();
	if (ErrorCode != NO_ERROR)
	{
		LogMessage("Failed To Initialize Server Application");

		return ErrorCode;
	}
	LogMessage("Socket Intialization Complete");


	// Run Our Client Socket
	ErrorCode = RunServerApplication();
	if (ErrorCode != NO_ERROR)
	{

		return ErrorCode;
	}

	// Cleanup Client Socket As It Should Be Done Now
	ErrorCode = TerminateServerApplication();
	if (ErrorCode != NO_ERROR)
	{
		return ErrorCode;
	}

	// No Error Reported
	return 0;
}

#pragma region RuntimeLogic

int RunServerApplication()
{
	int ErrorCode = 0;
	LogMessage("Accepting New Connections");

	while (ServerData::IsApplicationRunning)
	{
		//if(JoinAndCleanupCompletedThreads())

		// Set This To Busy Waiting.
		// TODO: Fix This Busy Waiting
		if (!DoesServerDataHaveSpaceForAdditionalConnections())
		{
			continue;
		}

		// Report The Error But Keep Searching For More Connections
		ErrorCode = AcceptConnection();
		if (ErrorCode != NO_ERROR)
		{
			int WSAErrorCode = LogWSAErrorToConsole();

			// TODO: Maybe support WSAEINPROGRESS under certain circumstances
			if (WSAErrorCode == WSAEWOULDBLOCK || WSAErrorCode == WSAECONNRESET || WSAErrorCode == WSAEINPROGRESS)
			{
				continue;
			}

			ServerData::IsApplicationRunning = false;
		}
	}

	return NO_ERROR;
}

int AcceptConnection()
{
	int IndexOfAvailableSocket = GetIndexOfAvailableSocket();
	if (IndexOfAvailableSocket == -1)
	{
		// Something odd happened here, are you messing up soething with threads clearing up sections of the array? :l
		return SOCKET_ERROR;
	}

	// IMPORTANT NOTE:
	// Keep an explicit reference instaed of object to ensure it doesn't copy and we write directly to the data in the array
	ClientConnection& AvailableClientConnection = ServerData::ClientConnections[IndexOfAvailableSocket];
	
	SOCKET& IncommingConnection = AvailableClientConnection.ClientSocket;
	SOCKADDR_IN Client_Address;
	int ClientAddressSize = sizeof(Client_Address);

	IncommingConnection = accept(ServerData::ServerSocket, (struct sockaddr*)&Client_Address, &ClientAddressSize);
	if (IncommingConnection == INVALID_SOCKET)
	{
		return SOCKET_ERROR;
	}

	char addrstr[INET_ADDRSTRLEN];
	inet_ntop(AF_INET, &(Client_Address.sin_addr), addrstr, INET_ADDRSTRLEN);

	std::string OutputMessage;
	OutputMessage += "Accepted Connection From:";
	OutputMessage += addrstr;
	LogMessage(OutputMessage.c_str());

	// TODO: Launch Initial Chain Of Commands Through The Thread
	ServerData::RunningJobs.emplace_back(Thread_HandleClient, std::ref(AvailableClientConnection));

	return NO_ERROR;
}

void Thread_HandleClient(ClientConnection& Client)
{
	char InputBuffer[MessageBufferSize];
	ZeroMemory(InputBuffer, MessageBufferSize);
	// Receive Login Message
	int size = ReceiveData(Client.ClientSocket, InputBuffer);

	std::vector<std::string> ParsedMessage;
	EClientMessageType MessageType = ParseMessage(InputBuffer, ParsedMessage);

	if(MessageType != EClientMessageType::UKN)
	{
		
		const char* Reply = "LGS";
		SendData(Client.ClientSocket, Reply, 3);
	}

	// Send Result 

	// Find A Session For The Player Or Pass Onto Other System


}

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
//		ClientMessage& EmplacedClientMessage = ServerData::MessageHistory.emplace_back(ClientConnection.ClientID, SocketReadBuffer);
//
//		// TODO: This Requires Locking
//		// Let's place this into a nice little queue that our friend the consumer can use
//		ServerData::ProducerConsumerDataLock.lock();
//
//		ServerData::ProducerDataToConsume.emplace(std::ref(EmplacedClientMessage));
//
//		ServerData::ProducerConsumerDataLock.unlock();
//
//		// Let's Notify That Lovable Nugget That He Needs To Get Back To Work
//		std::lock_guard lock(ServerData::ConditionalVariableMutex);
//		ServerData::ProducerToConsumerNotifier.notify_one();
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
//		std::unique_lock ConsumerLock(ServerData::ConditionalVariableMutex);
//
//		// We Want To Wait Right Off The Bat As Well, So We Release The Lock Till Notified As We Do Not Have Anything To Do Yet, So We Go To Sleep Until Told Otherwise
//		ServerData::ProducerToConsumerNotifier.wait(ConsumerLock);
//		// Alternatively If We Can Have Data Already Ready To Be Processed, Let's Go Ahead And Give It A Proper Condition (Imagine You Can Load And Broadcast Older Messages)
//		//ServerData::ProducerToConsumerNotifier.wait
//		//(
//		//	ConsumerLock, 
//		//	[]()
//		//	{ 
//		//		return ServerData::ProducerDataToConsume.size() > 0 ? true : false; 
//		//	} 
//		//);
//
//		ServerData::ProducerConsumerDataLock.lock();
//
//		ClientMessage& MessageToSend = ServerData::ProducerDataToConsume.front();
//		ServerData::ProducerDataToConsume.pop();
//
//		ServerData::ProducerConsumerDataLock.unlock();
//
//		for (int i = 0; i < ServerData::ClientConnections.size(); ++i)
//		{
//			SOCKET& ClientSocket = ServerData::ClientConnections[i].ClientSocket;
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

