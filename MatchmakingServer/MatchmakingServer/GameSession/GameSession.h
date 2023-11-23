#pragma once
#include <string>
#include <vector>
#include <memory>
#include <mutex>

static int GameSessionIDTracker = 0;

class ClientConnection;

class GameSession
{
    friend class ClientMessageJob;

public:

    GameSession(std::string ServerAddressForSession, std::string ServerPortForSession);

    bool IsGameSessionFull();
    bool IsGameServerReady();
    bool IsGameSessionReadyToBeLaunched();

    int         GetSessionID();
    std::string GetServerAddress();
    std::string GetServerPort();

    void AddClientConnectionToGameSession(std::weak_ptr<ClientConnection> ClientConnection);

private:
    int SessionID;

    std::string UEServerAddress;
    std::string UEServerPort;
    bool IsServerReady = false;

    std::mutex WorkerThreadLock;
    std::condition_variable CV_AwaitGameSessionFill;

    // Note This Just Cares About All Clients, If you Want To Split Them Into Teams MMR Etc Decide How This Should Work Or Have Other Vectors With That Data
    std::vector<std::weak_ptr<ClientConnection>> SessionClients;

    const int SessionMaxPlayers = 2;
};
