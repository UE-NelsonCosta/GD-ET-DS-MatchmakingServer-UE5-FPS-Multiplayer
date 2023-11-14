#include "ClientMessageJob.h"
#include "../ProjectMacros.h"
//#include <minwinbase.h>
#include "../ClientConnection/ClientConnection.h"
#include "../ServerListenSocket/ServerListenSocket.h"
#include "../Utils/MessageParsing.h"
#include "../GameSession/GameSessionManager.h"
#include "../GameSession/GameSession.h"


ClientMessageJob::ClientMessageJob(std::weak_ptr<ClientConnection> ClientConnection)
	: Client(ClientConnection)
{}

void ClientMessageJob::Run()
{
	// Starting Point, Thread Handles The Login Message In Here First Of Many
	HandleLoginRequestMessage();

	CleanupClient();
}

inline void ClientMessageJob::HandleLoginRequestMessage()
{
	std::shared_ptr<ClientConnection> ClientConnection = Client.lock();

	// TODO: Check The Issues With Including This Post Major Refactor
	//ZeroMemory(ClientConnection->InputBuffer, MessageBufferSize);

	// Receive Login Message (assuming we receive this in a single stroke)
	int size = ServerSocket.lock()->ReceiveData(ClientConnection->ClientSocket, ClientConnection->InputBuffer);

	std::vector<std::string> ParsedMessage;
	EClientMessageType MessageType = ParseMessage(ClientConnection->InputBuffer, ParsedMessage);
	if (MessageType == EClientMessageType::LGN)
	{
		ClientConnection->Username = ParsedMessage[1];
		ClientConnection->Password = ParsedMessage[2];

		// TODO: Needs to a do an sql query to ensure the login is correct.

		// TODO: Edit This Once We Have A Query Result
		bool WasQuerySuccessful = true;
		if (WasQuerySuccessful)
		{
			// Generate Some Auth Token
			HandleSuccessfulLoginMessage();
			return;
		}
	}

	// Send The Message And This Should Clean Itself Due To Function Stack
	HandleFailedLoginMessage();
}

inline void ClientMessageJob::HandleSuccessfulLoginMessage()
{
	ServerSocket.lock()->SendData(Client.lock()->ClientSocket, "LGS", 3);

	HandleRequestGameMessage();
}

inline void ClientMessageJob::HandleFailedLoginMessage()
{
	ServerSocket.lock()->SendData(Client.lock()->ClientSocket, "LGF", 3);
}

inline void ClientMessageJob::HandleRequestGameMessage()
{
	std::shared_ptr<ClientConnection> ClientConnection = Client.lock();

	ZeroMemory(ClientConnection->InputBuffer, MessageBufferSize);
	int size = ServerSocket.lock()->ReceiveData(ClientConnection->ClientSocket, ClientConnection->InputBuffer);

	std::vector<std::string> ParsedMessage;
	EClientMessageType MessageType = ParseMessage(ClientConnection->InputBuffer, ParsedMessage);

	if (MessageType == EClientMessageType::RGM)
	{
		// Find A Open Session Or Make One


		bool WasQuerySuccessful = true;
		if (WasQuerySuccessful)
		{
			HandleRequestGamemodeConnectionMessage();
			return;
		}
	}

}

inline void ClientMessageJob::HandleSuccessfulRequestGameMessage()
{
	ServerSocket.lock()->SendData(Client.lock()->ClientSocket, "RGS", 3);

	HandleRequestGamemodeConnectionMessage();
}

inline void ClientMessageJob::HandleFailedRequestGameMessage()
{
	ServerSocket.lock()->SendData(Client.lock()->ClientSocket, "RGF", 3);
}

inline void ClientMessageJob::HandleRequestGamemodeConnectionMessage()
{
	// Wait For Conditional Variable

	// This should only happen when a thread that checks the state of game sessions decides that session is good to go.
	std::string Message;
	Message += "RGC|";
	Message += Session.lock()->GetServerAddress();
	Message += Session.lock()->GetServerPort();

	ServerSocket.lock()->SendData(Client.lock()->ClientSocket, Message.c_str(), Message.length());
}

inline void ClientMessageJob::CleanupClient()
{

}
