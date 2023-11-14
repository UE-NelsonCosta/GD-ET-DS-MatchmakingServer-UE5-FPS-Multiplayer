#pragma once
#include <memory>

class ClientConnection;
class GameSession;
class ServerListenSocket;

// Expected Message
// LGN -> LGS/LGF -> RGM -> RGS/RGF -> RGC -> CAK
class ClientMessageJob
{
public:

	ClientMessageJob(std::weak_ptr<ClientConnection> ClientConnection);

	void Run();

	std::weak_ptr<GameSession> Session;
	std::weak_ptr<ClientConnection> Client;
	std::weak_ptr<ServerListenSocket> ServerSocket;

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

