#include "ServerSocketManager.h"
#include "../Utils/Logging.h"
#include <ws2tcpip.h>
#include "../CommandlineParser/CommandlineParameterParser.h"
#include "Job/ServerInstanceJobs/UEServerInstanceAcceptConnectionJob.h"

ServerSocketManager::ServerSocketManager()
{
}

int ServerSocketManager::InitializeServerSocket()
{
    int ErrorCode = 0;

    InitializeServerData();

    // Initializes The WSA Process So We Can use Sockets
    ErrorCode = InitializeWSAStartup();
    if (ErrorCode != NO_ERROR)
    {
        LogWSAErrorToConsole();

        return ErrorCode;
    }

    // Initialize The Client Listen Socket
    ErrorCode = InitializeClientListenSocket();
    if (ErrorCode != NO_ERROR)
    {
        LogWSAErrorToConsole();

        return ErrorCode;
    }

    ErrorCode = InitializeServerInstanceListenSocket();
    if (ErrorCode != NO_ERROR)
    {
        LogWSAErrorToConsole();

        return ErrorCode;
    }

    return NO_ERROR;
}

int ServerSocketManager::RunServerSocket()
{
    ClientConnectionJob = std::make_shared<ClientAcceptConnectionJob>();

    ClientConnectionJob->InitializeJob();
    ClientConnectionJob->RunJob();

    ServerInstanceConnectionJob = std::make_shared<UEServerInstanceAcceptConnectionJob>();

    ServerInstanceConnectionJob->InitializeJob();
    ServerInstanceConnectionJob->RunJob();
    
    return NO_ERROR;
}

int ServerSocketManager::TerminateServerSocket()
{
    int ErrorCode = 0;

    // Gracefully Terminate The Connection So The Other Side Knows We're Done here
    ErrorCode = ShutdownSocketConnections();
    if (ErrorCode != NO_ERROR)
    {
        LogWSAErrorToConsole();
    }

    ErrorCode = CloseSocketConnections();
    if (ErrorCode != NO_ERROR)
    {
        LogWSAErrorToConsole();
    }

    // Initializes The WSA Process So We Can use Sockets
    ErrorCode = TerminateWSA();
    if (ErrorCode != NO_ERROR)
    {
        LogWSAErrorToConsole();
    }

    CleanupWorkers();

    CleanupServerData();

    return ErrorCode;
}

void ServerSocketManager::AddNewClientMessageHandler(std::weak_ptr<ClientConnection> Client)
{
    std::scoped_lock DataLock(ServerSocketManager::Instance().ClientMessageJobMutex);

    std::shared_ptr<ClientMessageJob> NewClientMessageHandler = ClientMessageJobs.emplace_back(std::make_shared<ClientMessageJob>(Client));

    NewClientMessageHandler->InitializeJob();
    NewClientMessageHandler->RunJob();
}

void ServerSocketManager::RemoveClientConnection(std::weak_ptr<ClientConnection> Client)
{
    std::shared_ptr<ClientConnection> ObjectToFind = Client.lock();
    for(auto iterator = ClientConnections.begin(); iterator !=  ClientConnections.end(); ++iterator)
    {
        if((*iterator) == ObjectToFind)
        {
            ClientConnections.erase(iterator);
            return;
        }
    }
}

void ServerSocketManager::InitializeServerData()
{
    CommandlineParameterParser& Parser = CommandlineParameterParser::Instance();

    Parser.GetArgumentWithKey("ExecutionPath", ExecutionPath);

    std::string Value;
    if(Parser.GetArgumentWithKey("OverrideClientListenPort", Value))
    {
        ClientListenPort = atoi(Value.c_str());
    }

    if(Parser.GetArgumentWithKey("OverrideUEServerListenPort", Value))
    {
        ServerInstanceListenPort = atoi(Value.c_str());
    }
}

int ServerSocketManager::InitializeWSAStartup()
{
    // Intializes the WindowSocketApplication (WSA For Short) Just A DLL Process To Help With Everything
    // https://learn.microsoft.com/en-us/windows/win32/api/winsock/nf-winsock-wsastartup
    return WSAStartup(MAKEWORD(2, 0), &WSASocketInformation);
}

int ServerSocketManager::InitializeClientListenSocket()
{
    // Boolean Or On Results As They Are -1 or 0
    return CreateClientListenSocket() | BindClientListenSocketToAddress() | SetClientListenSocketToListenState();
}

// Actually Creates The Socket Object
int ServerSocketManager::CreateClientListenSocket()
{
    Client_ListenSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    return Client_ListenSocket == INVALID_SOCKET ? SOCKET_ERROR : NO_ERROR;
}

int ServerSocketManager::BindClientListenSocketToAddress()
{
    // Clear Out Server Socket Memory Then Fill In The Important Information
    ZeroMemory(&Client_ListenSocketAddress, sizeof(Client_ListenSocketAddress));
    Client_ListenSocketAddress.sin_family = AF_INET;
    Client_ListenSocketAddress.sin_addr.s_addr = htonl(INADDR_ANY); // Accepts A Connection Form Any IP
    Client_ListenSocketAddress.sin_port = htons(ClientListenSocketPort);  // Port

    // Uses The Valid Socket Object To Bind To An Address So People Can Connect To It
    return bind(Client_ListenSocket, (struct sockaddr*)&Client_ListenSocketAddress, sizeof(Client_ListenSocketAddress));
}

int ServerSocketManager::SetClientListenSocketToListenState()
{
    //listen: places a socket in a state of listening for incoming connection;  accept: permits an incoming connection attempt on a socket;  
    return listen(Client_ListenSocket, MaxPendingConnections);
}

int  ServerSocketManager::InitializeServerInstanceListenSocket()
{
    return CreateServerInstanceListenSocket() | BindServerInstanceListenSocketToAddress() | SetServerInstanceListenSocketToListenState();
}

int  ServerSocketManager::CreateServerInstanceListenSocket()
{
    ServerInstance_ListenSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    return ServerInstance_ListenSocket == INVALID_SOCKET ? SOCKET_ERROR : NO_ERROR;
}

int  ServerSocketManager::BindServerInstanceListenSocketToAddress()
{
    // Clear Out Server Socket Memory Then Fill In The Important Information
    ZeroMemory(&ServerInstance_ListenSocketAddress, sizeof(ServerInstance_ListenSocketAddress));
    ServerInstance_ListenSocketAddress.sin_family = AF_INET;
    ServerInstance_ListenSocketAddress.sin_addr.s_addr = htonl(INADDR_ANY); // Accepts A Connection Form Any IP
    ServerInstance_ListenSocketAddress.sin_port = htons(UEServerListenSocketPort);  // Port

    // Uses The Valid Socket Object To Bind To An Address So People Can Connect To It
    return bind(ServerInstance_ListenSocket, (struct sockaddr*)&ServerInstance_ListenSocketAddress, sizeof(ServerInstance_ListenSocketAddress));
}

int  ServerSocketManager::SetServerInstanceListenSocketToListenState()
{
    //listen: places a socket in a state of listening for incoming connection;  accept: permits an incoming connection attempt on a socket;  
    return listen(ServerInstance_ListenSocket, MaxPendingConnections);
}

int ServerSocketManager::ShutdownSocketConnections()
{
    // https://learn.microsoft.com/en-us/windows/win32/api/winsock/nf-winsock-shutdown
    // https://learn.microsoft.com/en-gb/windows/win32/winsock/graceful-shutdown-linger-options-and-socket-closure-2?redirectedfrom=MSDN
    return shutdown(Client_ListenSocket, SD_BOTH);
}

int ServerSocketManager::CloseSocketConnections()
{
    // https://learn.microsoft.com/en-us/windows/win32/api/winsock/nf-winsock-closesocket
    return closesocket(Client_ListenSocket);
}

int ServerSocketManager::TerminateWSA()
{
    // https://learn.microsoft.com/en-us/windows/win32/api/winsock/nf-winsock-wsacleanup
    return WSACleanup();
}

void ServerSocketManager::CleanupWorkers()
{
}

void ServerSocketManager::CleanupServerData()
{
}
