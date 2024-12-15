#include "GameSession.h"
#include <UEServerManager/UE5ServerManager.h>

#include "ClientConnection/ClientConnection.h"

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

void GameSession::NotifyClientsToStartGame()
{
    CV_AwaitGameSessionFill.notify_all();

    SetGameSessionState(EGameSessionState::InProgress);
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

std::string GameSession::FormatClientConnectionsForUEServerInstance()
{
    std::string FinalMessageBuffer;
    FinalMessageBuffer += "SessionID=" + std::to_string( SessionID ) + "|";
    
    for(int i = 0; i < SessionClients.size(); ++i)
    {
        auto Client = SessionClients[i].lock();
        FinalMessageBuffer += "?Name="; // First Question Mark Is Required By The Option Parser In UE5
        FinalMessageBuffer += Client->Username;
        FinalMessageBuffer += "?AuthToken=";
        FinalMessageBuffer += Client->AuthToken;
        FinalMessageBuffer += "|"; // Seperator We Use Between Client Data
    }

    return FinalMessageBuffer;
}
