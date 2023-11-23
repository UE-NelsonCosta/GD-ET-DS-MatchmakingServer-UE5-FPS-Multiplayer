#pragma once
#include <vector>
#include "GameSession.h"
#include "../Utils/ASingleton.h"

class GameSessionManager : public ASingleton<GameSessionManager>
{
public:

	void Run();

	std::weak_ptr<GameSession> RegisterClientToGameSession(std::weak_ptr<ClientConnection> Client);

private:

	std::mutex SessionMutex;
	std::vector<std::shared_ptr<GameSession>> GameSessions;

};

