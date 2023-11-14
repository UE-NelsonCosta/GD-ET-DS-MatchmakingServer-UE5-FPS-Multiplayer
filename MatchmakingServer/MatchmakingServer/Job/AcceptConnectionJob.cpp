#include "AcceptConnectionJob.h"
#include <ProjectStatics.h>
#include <Utils/Logging.h>
#include <winerror.h>
#include <ws2tcpip.h>
#include <ServerSocketManager/ServerSocketManager.h>

bool AcceptConnectionJob::InitializeJob()
{        
    SocketManager = ServerSocketManager::InstanceAsStrongPointer();

    return true;
}

void AcceptConnectionJob::RunJob()
{
    Worker = std::thread(&AcceptConnectionJob::HandleAcceptConnection, this);
}

void AcceptConnectionJob::TerminateJob()
{
}

void AcceptConnectionJob::HandleAcceptConnection()
{
    while (ProjectStatics::IsApplicationRunning)
    {
        int ErrorCode = AcceptConnection();
        if (ErrorCode != NO_ERROR)
        {
            int WSAErrorCode = LogWSAErrorToConsole();

            // TODO: Maybe support WSAEINPROGRESS under certain circumstances
            if (WSAErrorCode == WSAEWOULDBLOCK || WSAErrorCode == WSAECONNRESET || WSAErrorCode == WSAEINPROGRESS)
            {
                continue;
            }

            ProjectStatics::IsApplicationRunning = false;
        }
    }
}

int AcceptConnectionJob::AcceptConnection()
{
    std::shared_ptr<ServerSocketManager> SharedSocketManager = SocketManager.lock();
    if (!SharedSocketManager)
        return SOCKET_ERROR;

    std::shared_ptr<ClientConnection> NewClientConnection = SharedSocketManager->ClientConnections.emplace_back(std::make_shared<ClientConnection>());

    SOCKADDR_IN Client_Address;
    int ClientAddressSize = sizeof(Client_Address);

    NewClientConnection->ClientSocket = accept(SharedSocketManager->ServerSocket, (struct sockaddr*)&Client_Address, &ClientAddressSize);
    if (NewClientConnection->ClientSocket == INVALID_SOCKET)
    {
        return SOCKET_ERROR;
    }

    char addrstr[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &(Client_Address.sin_addr), addrstr, INET_ADDRSTRLEN);

    std::string OutputMessage;
    OutputMessage += "Accepted Connection From:";
    OutputMessage += addrstr;
    LogMessage(OutputMessage.c_str());

    SharedSocketManager->AddNewClientMessageHandler(NewClientConnection);

    return NO_ERROR;
}