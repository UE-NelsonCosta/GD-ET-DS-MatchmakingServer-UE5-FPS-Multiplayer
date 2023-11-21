#include "ClientMessageJob.h"
#include <ProjectMacros.h>
#include <ClientConnection/ClientConnection.h>
#include <ServerSocketManager/ServerSocketManager.h>
#include <Utils/MessageParsing.h>
#include <GameSession/GameSessionManager.h>
#include <GameSession/GameSession.h>

ClientMessageJob::ClientMessageJob(std::weak_ptr<ClientConnection> ClientConnection)
	: Client ( ClientConnection )
{}

bool ClientMessageJob::InitializeJob()
{
	return true;
}

void ClientMessageJob::RunJob()
{
	Worker = std::thread(&ClientMessageJob::HandleLoginRequestMessage, this);
}

void ClientMessageJob::TerminateJob()
{
}

void ClientMessageJob::HandleLoginRequestMessage()
{
	std::shared_ptr<ClientConnection> ClientConnection = Client.lock();

	// TODO: Check The Issues With Including This Post Major Refactor
	ZeroMemory(ClientConnection->InputBuffer, MessageBufferSize);

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

void ClientMessageJob::HandleSuccessfulLoginMessage()
{
	ServerSocket.lock()->SendData(Client.lock()->ClientSocket, "LGS", 3);

	HandleRequestGameMessage(5);
}

void ClientMessageJob::HandleFailedLoginMessage()
{
	ServerSocket.lock()->SendData(Client.lock()->ClientSocket, "LGF", 3);

	WasSuccessful = false;
	JobComplete   = true;
}

void ClientMessageJob::HandleRequestGameMessage(int AttemptsLeft)
{
	std::shared_ptr<ClientConnection> ClientConnection = Client.lock();

	ZeroMemory(ClientConnection->InputBuffer, MessageBufferSize);
	int size = ServerSocket.lock()->ReceiveData(ClientConnection->ClientSocket, ClientConnection->InputBuffer);

	std::vector<std::string> ParsedMessage;
	EClientMessageType MessageType = ParseMessage(ClientConnection->InputBuffer, ParsedMessage);

	if (MessageType == EClientMessageType::RGM)
	{
		// Find A Open Session Or Make One
		Session = GameSessionManager::Instance().RegisterClientToGameSession(Client);
		if (!Session.expired())
		{
			HandleSuccessfulRequestGameMessage();
			return;
		}
	}

	// Note: Right now I'm not allowing this to have a chance to recover, just outright killing it
	// Removing this ability for now
	// We give the user the ability to send us another attempt
	//if(--AttemptsLeft > 0)
	//{
		// Send Message Back To Client
	//	HandleRequestGameMessage(AttemptsLeft);
	//}


	HandleFailedRequestGameMessage();
}

void ClientMessageJob::HandleSuccessfulRequestGameMessage()
{
	ServerSocket.lock()->SendData(Client.lock()->ClientSocket, "RGS", 3);

	HandleRequestGamemodeConnectionMessage();
}

void ClientMessageJob::HandleFailedRequestGameMessage()
{
	ServerSocket.lock()->SendData(Client.lock()->ClientSocket, "RGF", 3);

	WasSuccessful = false;
	JobComplete = true;
}

void ClientMessageJob::HandleRequestGamemodeConnectionMessage()
{
	std::shared_ptr<GameSession> GameSession = Session.lock();

	// Wait For Conditional Variable
	std::unique_lock<std::mutex> Lock(GameSession->WorkerThreadLock);

	// TODO: Check to see if this predicate is enough to send them all off and finalize their connection messages
	GameSession->CV_AwaitGameSessionFill.wait(Lock, [GameSession]() { return GameSession->IsGameSessionFull(); } );

	// This should only happen when a thread that checks the state of game sessions decides that session is good to go.
	std::string Message;
	Message += "RGC|";
	Message += Session.lock()->GetServerAddress();
	Message += Session.lock()->GetServerPort();

	ServerSocket.lock()->SendData(Client.lock()->ClientSocket, Message.c_str(), Message.length());

	WasSuccessful = true;
	JobComplete   = true;
}
