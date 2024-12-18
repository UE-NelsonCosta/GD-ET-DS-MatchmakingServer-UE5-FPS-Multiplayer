#include "UEServerInstanceAcceptConnectionJob.h"
#include <ProjectStatics.h>
#include <Utils/Logging.h>
#include <winerror.h>
#include <WS2tcpip.h>
#include <ServerSocketManager/ServerSocketManager.h>

#include "GameSession/GameSessionManager.h"
#include "UEServerManager/UE5ServerManager.h"

bool UEServerInstanceAcceptConnectionJob::InitializeJob()
{        
    SocketManager = ServerSocketManager::InstanceAsStrongPointer();

    return true;
}

void UEServerInstanceAcceptConnectionJob::RunJob()
{
    Worker = std::thread(&UEServerInstanceAcceptConnectionJob::HandleAcceptConnection, this);
}

void UEServerInstanceAcceptConnectionJob::TerminateJob()
{
    // TODO: Clean
}

void UEServerInstanceAcceptConnectionJob::HandleAcceptConnection() const
{
    while (ProjectStatics::IsApplicationRunning)
    {
        const int ErrorCode = AcceptConnection();
        if (ErrorCode != NO_ERROR)
        {
            const int WSAErrorCode = LogWSAErrorToConsole();

            // TODO: Maybe support WSAEINPROGRESS under certain circumstances
            if (WSAErrorCode == WSAEWOULDBLOCK || WSAErrorCode == WSAECONNRESET || WSAErrorCode == WSAEINPROGRESS)
            {
                continue;
            }

            ProjectStatics::IsApplicationRunning = false;
        }
    }
}

int UEServerInstanceAcceptConnectionJob::AcceptConnection() const
{
    const std::shared_ptr<ServerSocketManager> SharedSocketManager = SocketManager.lock();
    if (!SharedSocketManager)
        return SOCKET_ERROR;

    SOCKET ServerInstanceSocket = 0;
    SOCKADDR_IN ServerInstance_Address;
    int ClientAddressSize = sizeof(ServerInstance_Address);

    ServerInstanceSocket = accept(SharedSocketManager->ServerInstance_ListenSocket, reinterpret_cast<sockaddr*>(&ServerInstance_Address), &ClientAddressSize);
    if (ServerInstanceSocket == INVALID_SOCKET)
    {
        return SOCKET_ERROR;
    }

    char addrstr[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &(ServerInstance_Address.sin_addr), addrstr, INET_ADDRSTRLEN);

    std::string OutputMessage;
    OutputMessage = OutputMessage + "Accepted Connection From:" + addrstr;
    LogMessage(OutputMessage.c_str());

    // Read Port From Socket
    char ReadBuffer[MessageBufferSize];
    ZeroMemory(&ReadBuffer, MessageBufferSize);
    int BytesRead = SharedSocketManager->ReceiveData(ServerInstanceSocket, ReadBuffer);

    const std::weak_ptr<UEServerInstance> UEServerInstance = UEServerManager::Instance().GetServerInstance(ReadBuffer);
    if (UEServerInstance.expired() || !UEServerInstance.lock().get())
        return SOCKET_ERROR;

    // Find The Clients Associated With A Game Session And Concatenate/Send it all back
    std::weak_ptr<GameSession> GameSession = GameSessionManager::Instance().FindGameSessionFrom(UEServerInstance);
    if (GameSession.expired() || !UEServerInstance.lock().get())
        return SOCKET_ERROR;

    // If the session is launching then we can send all that information across
    if(GameSession.lock()->GetGameSessionState() == EGameSessionState::Launching)
    {
        std::string FormatedClientConnections = GameSession.lock()->FormatClientConnectionsForUEServerInstance();
        // Send Data To Server
        ServerSocketManager::Instance().SendData(ServerInstanceSocket,FormatedClientConnections.c_str(), FormatedClientConnections.length());
        
        char ReceiveBuffer[1024];
        ZeroMemory(&ReceiveBuffer, sizeof(char) * 1024);
        ServerSocketManager::Instance().ReceiveData(ServerInstanceSocket, ReceiveBuffer);

        if (strcmp(ReceiveBuffer, "Server Ready") != 0)
        {
            // TODO: Log Some Error D:
            return SOCKET_ERROR;
        }

        // This unlocks the conditional variable lock
        GameSession.lock()->NotifyClientsToStartGame();

        closesocket(ServerInstanceSocket);

        return NO_ERROR;
    }

    if(GameSession.lock()->GetGameSessionState() == EGameSessionState::InProgress)
    {
        GameSession.lock()->SetGameSessionState(EGameSessionState::ReadyForCleanup);
        // Don't want to gather analytics or anything, but we could do this here if we needed to

        return NO_ERROR;
    }
    
    return NO_ERROR;
}
