#pragma once
#include <vector>
#include "GameSession.h"
#include "../Utils/ASingleton.h"

class GameSessionManager : public ASingleton<GameSessionManager>
{
public:

	void ValidateStateOfGameSessions();

	std::weak_ptr<GameSession> RegisterClientToGameSession(std::weak_ptr<ClientConnection> Client);

	std::weak_ptr<GameSession> FindGameSessionFrom(const std::weak_ptr<UEServerInstance>& ServerInstance);
	
	
private:

	std::mutex SessionMutex;
	std::vector<std::shared_ptr<GameSession>> GameSessions;

};

