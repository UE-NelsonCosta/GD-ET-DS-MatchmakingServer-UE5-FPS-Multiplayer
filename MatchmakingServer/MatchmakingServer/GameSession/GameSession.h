#pragma once
#include <string>
#include <vector>
#include <memory>

static int GameSessionIDTracker = 0;

class ClientConnection;

class GameSession
{
public:

    GameSession(std::string& ServerAddressForSession, std::string& ServerPortForSession);

    bool IsGameSessionFull();

    int         GetSessionID();
    std::string GetServerAddress();
    std::string GetServerPort();

    void AddClientConnectionToGameSession(std::weak_ptr<ClientConnection> ClientConnection);

private:
    int SessionID;

    std::string ServerAddress;
    std::string ServerPort;

    // Note This Just Cares About All Clients, If you Want To Split Them Into Teams MMR Etc Decide How This Should Work Or Have Other Vectors With That Data
    std::vector<std::weak_ptr<ClientConnection>> SessionClients;

    const int SessionMaxPlayers = 2;
};



//std::shared_ptr<GameSession> FindAndAddClientToGameSession(const std::shared_ptr<ClientConnection>& Client)
//{
//    // We Kinda Just fill These as we go
//    std::scoped_lock ScopedLock(GameSessionMutex);
//
//    std::shared_ptr<GameSession> GameSessionJoined;
//
//    // Search For An Available One
//    for (int i = 0; i < GameSessions.size(); ++i)
//    {
//        GameSessionJoined = GameSessions[i];
//
//        if (GameSessionJoined.get() && !GameSessionJoined->IsGameSessionFull())
//        {
//            GameSessionJoined->SessionClients.push_back(Client);
//        }
//    }
//
//    // If none found lets make one! :D
//    if (!GameSessionJoined.get())
//    {
//        GameSessionJoined = std::make_shared<GameSession>();
//        GameSessions.emplace_back(std::move(GameSessionJoined));
//
//        GameSessionJoined->SessionClients.push_back(Client);
//    }
//
//    return GameSessionJoined;
//}
