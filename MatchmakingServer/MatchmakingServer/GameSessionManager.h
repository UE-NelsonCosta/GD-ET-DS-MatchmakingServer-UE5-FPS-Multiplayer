#pragma once
#include <vector>
#include "GameSession.h"
#include "Utils/ASingleton.h"

class GameSessionManager : public ASingleton<GameSessionManager>
{
public:



private:

	std::vector<std::shared_ptr<GameSession>> GameSessions;

};

