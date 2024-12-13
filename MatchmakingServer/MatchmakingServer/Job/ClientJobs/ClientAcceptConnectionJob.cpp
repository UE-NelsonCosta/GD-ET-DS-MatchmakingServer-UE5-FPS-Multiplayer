#include "ClientAcceptConnectionJob.h"
#include <ProjectStatics.h>
#include <Utils/Logging.h>
#include <winerror.h>
#include <ws2tcpip.h>
#include <ServerSocketManager/ServerSocketManager.h>

bool ClientAcceptConnectionJob::InitializeJob()
{        
    SocketManager = ServerSocketManager::InstanceAsStrongPointer();

    return true;
}

void ClientAcceptConnectionJob::RunJob()
{
    Worker = std::thread(&ClientAcceptConnectionJob::HandleAcceptConnection, this);
}

void ClientAcceptConnectionJob::TerminateJob()
{
}

void ClientAcceptConnectionJob::HandleAcceptConnection()
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

int ClientAcceptConnectionJob::AcceptConnection()
{
    // Ensure We Have The SocketManager Pointer And It's Valid
    std::shared_ptr<ServerSocketManager> SharedSocketManager = SocketManager.lock();
    if (!SharedSocketManager)
        return SOCKET_ERROR;

    // Construct A New Client Connection In The Array
    std::shared_ptr<ClientConnection> NewClientConnection = SharedSocketManager->ClientConnections.emplace_back(std::make_shared<ClientConnection>());
    SOCKADDR_IN Client_Address;
    int ClientAddressSize = sizeof(Client_Address);

    // Accept The Socket Object And Pull Out Important Information On The Client
    NewClientConnection->ClientSocket = accept(SharedSocketManager->Client_ListenSocket, reinterpret_cast<sockaddr*>(&Client_Address), &ClientAddressSize);
    if (NewClientConnection->ClientSocket == INVALID_SOCKET)
    {
        return SOCKET_ERROR;
    }

    // Get Information Of The Incomming Connection (IP In This Case)
    char addrstr[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &(Client_Address.sin_addr), addrstr, INET_ADDRSTRLEN);
    NewClientConnection->ConnectionSource = addrstr;
    
    // Output the string of the incomming connection
    std::string OutputMessage;
    OutputMessage = "Client Connection Accepted From:" + NewClientConnection->ConnectionSource;
    LogMessage(OutputMessage.c_str());

    // Spawn a thread to handle all the communication between itself and the clients
    SharedSocketManager->AddNewClientMessageHandler(NewClientConnection);

    return NO_ERROR;
}