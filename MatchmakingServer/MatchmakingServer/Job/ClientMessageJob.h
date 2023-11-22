// Expected Message
// LGN -> LGS/LGF -> RGM -> RGS/RGF -> RGC -> CAK
// TODO: Documentation

#pragma once
#include <memory>
#include "IThreadableJob.h"

class ClientConnection;
class GameSession;
class ServerSocketManager;

class ClientMessageJob : public IThreadableJob
{
public:

	ClientMessageJob() = delete;
	ClientMessageJob(std::weak_ptr<ClientConnection> ClientConnection);
	~ClientMessageJob(){}

	virtual bool InitializeJob() override;

	virtual void RunJob()		 override;

	virtual void TerminateJob()  override;

	std::weak_ptr<ClientConnection> GetClientConnection() { return Client; }

private: // Internal Functions

	void HandleLoginRequestMessage();

	void HandleSuccessfulLoginMessage();

	void HandleFailedLoginMessage();

	void HandleRequestGameMessage(int AttemptsLeft);

	void HandleSuccessfulRequestGameMessage();

	void HandleFailedRequestGameMessage();

	void HandleRequestGamemodeConnectionMessage();

private:

	std::weak_ptr<GameSession> Session;
	std::weak_ptr<ClientConnection> Client;
	std::weak_ptr<ServerSocketManager> ServerSocket;

};

