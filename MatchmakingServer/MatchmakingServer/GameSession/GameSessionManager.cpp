#include "GameSessionManager.h"
#include <UEServerManager/UE5ServerManager.h>

void GameSessionManager::ValidateStateOfGameSessions()
{
    // Check State Of GameSessions And Notify Those That Are Ready To Be Shipped Off!
    std::scoped_lock Lock(SessionMutex);
    for(int i = 0; i < GameSessions.size(); ++i)
    {
        std::shared_ptr<GameSession> CurrentGameSession = GameSessions[i];
        if(!CurrentGameSession.get())
        {
            continue;
        }

        // This Is The Sequence Of States For The Game Sessions
        switch(CurrentGameSession->GetGameSessionState())
        {
        case EGameSessionState::FindingPlayers:
            {
                // Nothing To Do Here Till This Is Populated By Client Threads So Let's Leave This Session Alone For Now
                continue;
            }
        case EGameSessionState::ReadyToLaunch:
            {
                // All Players Found So Let's Launch A UE Server Instance At Some Port
                
                // Normally we would have a system just to handle what servers are available or not and creates one, for sake of simplicity for now, we just incrementing a single port
                if(UEServerManager::Instance().RunServer(CurrentGameSession->GetServerInstance()))
                {
                    CurrentGameSession->SetGameSessionState(EGameSessionState::Launching);
                }
                else
                {
                    // TODO: Otherwise Try Again Later Or Disband And Remake The Session Or Find Another Port 
                }
                
                break;
            }
        case EGameSessionState::Launching:
            {
                // Server Thread Will Transition This State When It's Done Initializing
                break;
            }
        case EGameSessionState::InProgress:
            {
                // This Is Here To Know It's In Progress, Only Transitions When The Server Says It's Done
                continue;
            }
        case EGameSessionState::ReadyForCleanup:
            {
                // State To Cleanup Once This Has All Been Validated
                break;
            }
        default:
            break; // TODO: Log Some Error As It Shouldn't Be Any Other State
        }
    }
}

std::weak_ptr<GameSession> GameSessionManager::RegisterClientToGameSession(std::weak_ptr<ClientConnection> Client)
{
    std::scoped_lock Lock(SessionMutex);

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

std::weak_ptr<GameSession> GameSessionManager::FindGameSessionFrom(
    const std::weak_ptr<UEServerInstance>& ServerInstance)
{
    for(int i = 0; i < GameSessions.size(); ++i)
    {
        if(GameSessions[i]->GetServerInstance().lock().get() == ServerInstance.lock().get())
        {
            return GameSessions[i];
        }
    }
}
