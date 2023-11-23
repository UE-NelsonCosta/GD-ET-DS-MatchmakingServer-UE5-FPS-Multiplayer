#include "GameSession.h"

GameSession::GameSession(std::string ServerAddressForSession, std::string ServerPortForSession)
    : SessionID(++GameSessionIDTracker)
    , UEServerAddress(ServerAddressForSession)
    , UEServerPort(ServerPortForSession)
{}

bool GameSession::IsGameSessionFull()
{
    // For testing purposes setting this arbitrarily to 2
    return SessionClients.size() == SessionMaxPlayers;
}

bool GameSession::IsGameServerReady()
{
    return IsServerReady;
}

bool GameSession::IsGameSessionReadyToBeLaunched()
{
    return IsGameSessionFull() && IsGameServerReady();
}

int GameSession::GetSessionID()
{
    return SessionID;
}

std::string GameSession::GetServerAddress()
{
    return UEServerAddress;
}

std::string GameSession::GetServerPort()
{
    return UEServerPort;
}

void GameSession::AddClientConnectionToGameSession(std::weak_ptr<ClientConnection> ClientConnection)
{
    SessionClients.push_back(ClientConnection);
}
