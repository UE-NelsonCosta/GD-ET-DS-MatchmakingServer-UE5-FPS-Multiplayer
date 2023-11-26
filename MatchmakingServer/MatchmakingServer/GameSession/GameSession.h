#pragma once
#include <string>
#include <vector>
#include <memory>
#include <mutex>
#include <UEServerManager/UEServerInstance.h>

static int GameSessionIDTracker = 0;

class ClientConnection;

enum class EGameSessionState
{
    NONE,
    FindingPlayers,
    ReadyToLaunch,
    InProgress,
    ReadyForCleanup,
};

class GameSession
{
    friend class ClientMessageJob;

public:

    GameSession(std::string ServerAddressForSession, std::string ServerPortForSession);

    bool IsGameSessionFull();
    bool IsGameServerReady();
    bool IsGameSessionReadyToBeLaunched();

    int  GetSessionID();
    EGameSessionState GetGameSessionState();
    void SetGameSessionState(EGameSessionState NewState);

    bool AddClientConnectionToGameSession(std::weak_ptr<ClientConnection> ClientConnection);

    std::weak_ptr<UEServerInstance> GetServerInstance();

    std::string GetServerInstanceIPnPort();

private:
    int SessionID;

    EGameSessionState GameSessionState = EGameSessionState::NONE;

    std::weak_ptr<UEServerInstance> ServerInstance;

    std::mutex WorkerThreadLock;
    std::condition_variable CV_AwaitGameSessionFill;

    // Note This Just Cares About All Clients, If you Want To Split Them Into Teams MMR Etc Decide How This Should Work Or Have Other Vectors With That Data
    std::mutex SessionClientsLock;
    std::vector<std::weak_ptr<ClientConnection>> SessionClients;

    const int SessionMaxPlayers = 2;
};
