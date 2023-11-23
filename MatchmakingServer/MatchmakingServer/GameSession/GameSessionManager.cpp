#include "GameSessionManager.h"

void GameSessionManager::Run()
{
    // Check State Of GameSessions And Notify Those That Are Ready To Be Shipped Off!
    std::scoped_lock Lock(SessionMutex);
    for(int i = 0; i < GameSessions.size(); ++i)
    {
        std::shared_ptr<GameSession> CurrentGameSession = GameSessions[i];
        if(CurrentGameSession && CurrentGameSession->IsGameSessionFull() && !CurrentGameSession->IsGameServerReady())
        {
            // Let's Launch The UE5 Game Server
            // Normally we would have a system just to handle what servers are available or not and creates one, for sake of simplicity for now, we just incrementing a single port
            // TODO: Also relative pathing is weird use absolutes as they are clearer
            system("start /B \"\" \"../../../LaunchTestUE5Server.bat\" 1337");
        }
    }
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
        // TODO: Have a system to keep track and request new servers from
        BestMatch = GameSessions.emplace_back(std::move(std::make_shared<GameSession>("127.0.0.1", "1337"))) ;
    }

    BestMatch->AddClientConnectionToGameSession(Client);

    return BestMatch;
}
