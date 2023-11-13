#pragma once
#include <memory>

class ClientConnection;
class GameSession;
class ServerListenSocket;

// Expected Message
// LGN -> LGS/LGF -> RGM -> RGS/RGF -> RGC -> CAK
class ClientMessageWorker
{
public:

	ClientMessageWorker(std::weak_ptr<ClientConnection> ClientConnection);

	void Run();

	std::weak_ptr<ServerListenSocket> ServerSocket;
	std::weak_ptr<ClientConnection> Client;
	std::weak_ptr<GameSession> Session;

private:






	void HandleLoginRequestMessage();

	void HandleSuccessfulLoginMessage();

	void HandleFailedLoginMessage();

	void HandleRequestGameMessage();

	void HandleSuccessfulRequestGameMessage();

	void HandleFailedRequestGameMessage();

	void HandleRequestGamemodeConnectionMessage();

	void CleanupClient();

};

