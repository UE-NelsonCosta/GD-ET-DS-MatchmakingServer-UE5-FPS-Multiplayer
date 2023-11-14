#include "GameSession.h"

GameSession::GameSession(std::string& ServerAddressForSession, std::string& ServerPortForSession)
    : SessionID(++GameSessionIDTracker)
    , ServerAddress(ServerAddressForSession)
    , ServerPort(ServerPortForSession)
{}

bool GameSession::IsGameSessionFull()
{
    // For testing purposes setting this arbitrarily to 2
    return SessionClients.size() == SessionMaxPlayers;
}

int GameSession::GetSessionID()
{
    return SessionID;
}

std::string GameSession::GetServerAddress()
{
    return ServerAddress;
}

std::string GameSession::GetServerPort()
{
    return ServerPort;
}

void GameSession::AddClientConnectionToGameSession(std::weak_ptr<ClientConnection> ClientConnection)
{
    SessionClients.push_back(ClientConnection);
}
