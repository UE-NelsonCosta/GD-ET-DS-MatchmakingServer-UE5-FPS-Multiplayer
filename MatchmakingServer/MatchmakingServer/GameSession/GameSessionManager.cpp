#include "GameSessionManager.h"

void GameSessionManager::Run()
{
    // Check State Of GameSessions And Notify Those That Are Ready To Be Shipped Off!
}

std::weak_ptr<GameSession> GameSessionManager::RegisterClientToGameSession(std::weak_ptr<ClientConnection> Client)
{
    // TODO: Lock this behind a scopedlock
    std::shared_ptr<GameSession> BestMatch;
    for(int i = 0; i < GameSessions.size(); ++i)
    {
        if(!GameSessions[i]->IsGameSessionFull())
        {
            BestMatch = GameSessions[i];
            break;
        }
    }

    // Make A New Session If One Doesn't Exist!
    if(!BestMatch)
    {
        BestMatch = GameSessions.emplace_back();
    }

    BestMatch->AddClientConnectionToGameSession(Client);

    return BestMatch;
}
