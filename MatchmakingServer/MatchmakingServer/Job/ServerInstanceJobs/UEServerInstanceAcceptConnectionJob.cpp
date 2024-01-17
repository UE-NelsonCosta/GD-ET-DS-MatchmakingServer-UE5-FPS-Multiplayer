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
}

void UEServerInstanceAcceptConnectionJob::HandleAcceptConnection()
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

int UEServerInstanceAcceptConnectionJob::AcceptConnection()
{
    std::shared_ptr<ServerSocketManager> SharedSocketManager = SocketManager.lock();
    if (!SharedSocketManager)
        return SOCKET_ERROR;

    SOCKET ServerInstanceSocket = 0;

    SOCKADDR_IN ServerInstance_Address;
    int ClientAddressSize = sizeof(ServerInstance_Address);

    ServerInstanceSocket = accept(SharedSocketManager->ServerInstance_ListenSocket, (struct sockaddr*)&ServerInstance_Address, &ClientAddressSize);
    if (ServerInstanceSocket == INVALID_SOCKET)
    {
        return SOCKET_ERROR;
    }

    //char addrstr[INET_ADDRSTRLEN];
    //inet_ntop(AF_INET, &(Client_Address.sin_addr), addrstr, INET_ADDRSTRLEN);

    //std::string OutputMessage;
    //OutputMessage += "Accepted Connection From:";
    //OutputMessage += addrstr;
    //LogMessage(OutputMessage.c_str());


    
    // Read Port From Socket
    char ReadBuffer[10];
    ZeroMemory(&ReadBuffer,10);
    SharedSocketManager->ReceiveData(ServerInstanceSocket, ReadBuffer);

    std::weak_ptr<UEServerInstance> UEServerInstance = UEServerManager::Instance().GetServerInstance(ReadBuffer);

    // Find The Clients Associated With A Game Session And Concatenate/Send it all back
    std::weak_ptr<GameSession> GameSession = GameSessionManager::Instance().FindGameSessionFrom(UEServerInstance);

    if(GameSession.lock()->GetGameSessionState() == EGameSessionState::Launching)
    {
        std::string FormatedClientConnections = GameSession.lock()->FormatClientConnectionsForUEServerInstance();
        // Send Data To Server
        ServerSocketManager::Instance().SendData(ServerInstanceSocket,FormatedClientConnections.c_str(), FormatedClientConnections.length());
        
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