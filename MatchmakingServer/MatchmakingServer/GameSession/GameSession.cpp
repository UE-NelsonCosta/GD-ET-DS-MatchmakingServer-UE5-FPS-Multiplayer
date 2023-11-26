#include "GameSession.h"
#include <UEServerManager/UE5ServerManager.h>

GameSession::GameSession(std::string ServerAddressForSession, std::string ServerPortForSession)
    : SessionID(++GameSessionIDTracker)
{
    GameSessionState = EGameSessionState::FindingPlayers;

    ServerInstance = UEServerManager::Instance().ReserveServerInstance();
}

bool GameSession::IsGameSessionFull()
{
    // For testing purposes setting this arbitrarily to 2
    return SessionClients.size() == SessionMaxPlayers;
}

bool GameSession::IsGameServerReady()
{
    if(ServerInstance.expired())
        return false;

    return ServerInstance.lock()->GetServerInstanceState() != EServerInstanceState::Running;
}

bool GameSession::IsGameSessionReadyToBeLaunched()
{
    return IsGameSessionFull() && IsGameServerReady();
}

int GameSession::GetSessionID()
{
    return SessionID;
}

EGameSessionState GameSession::GetGameSessionState()
{
    return GameSessionState;
}

void GameSession::SetGameSessionState(EGameSessionState NewState)
{
    GameSessionState = NewState;

    // TODO: Stick this in a better place
    if(GameSessionState == EGameSessionState::InProgress)
    {
        CV_AwaitGameSessionFill.notify_all();
    }
}

// This Should Already Be Locked From The Previous Function
bool GameSession::AddClientConnectionToGameSession(std::weak_ptr<ClientConnection> ClientConnection)
{
    SessionClients.push_back(ClientConnection);

    if(SessionClients.size() == SessionMaxPlayers)
    {
        GameSessionState = EGameSessionState::ReadyToLaunch;
    }

    return true;
}

std::weak_ptr<UEServerInstance> GameSession::GetServerInstance()
{
    return ServerInstance;
}

std::string GameSession::GetServerInstanceIPnPort()
{
    return ServerInstance.lock()->GetIP() + ":" + ServerInstance.lock()->GetPort();
}
