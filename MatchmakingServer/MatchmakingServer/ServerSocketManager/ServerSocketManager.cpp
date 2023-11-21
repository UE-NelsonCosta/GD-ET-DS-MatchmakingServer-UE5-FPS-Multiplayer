#include "ServerSocketManager.h"
#include "../Utils/Logging.h"
#include <ws2tcpip.h>
#include "../CommandlineParser/CommandlineParameterParser.h"

ServerSocketManager::ServerSocketManager()
{
}

int ServerSocketManager::InitializeServerSocket()
{
    int ErrorCode = 0;

    InitializeServerData();

    ErrorCode = WSAStartup(MAKEWORD(2, 0), &WSASocketInformation);
    if (ErrorCode != NO_ERROR)
    {
        LogWSAErrorToConsole();

        return ErrorCode;
    }

    // Actually Creates The Socket Object
    ErrorCode = CreateServerSocket();
    if (ErrorCode != NO_ERROR)
    {
        LogWSAErrorToConsole();

        return ErrorCode;
    }

    // Uses The Valid Socket Object To Bind To An Address So People Can Connect To It
    ErrorCode = BindSocketToAddress();
    if (ErrorCode != NO_ERROR)
    {
        LogWSAErrorToConsole();

        return ErrorCode;
    }

    // Set the socket to listen mode so it can accept incomming connections
    ErrorCode = SetSocketToListenState();
    if (ErrorCode != NO_ERROR)
    {
        LogWSAErrorToConsole();

        return ErrorCode;
    }

    return NO_ERROR;
}

int ServerSocketManager::RunServerSocket()
{
    ConnectionJob = std::make_shared<AcceptConnectionJob>();

    ConnectionJob->InitializeJob();
    ConnectionJob->RunJob();

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

    // Initializes The WSA Process So We Can use Sockets
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
    if(Parser.GetArgumentWithKey("ServerIP", Value))
    {
        ServerIPAddress = std::wstring(Value.begin(), Value.end());
    }

    if(Parser.GetArgumentWithKey("ServerPort", Value))
    {
        ServerSocketPort = atoi(Value.c_str());
    }
}

int ServerSocketManager::InitializeWSAStartup()
{
    // Intializes the WindowSocketApplication (WSA For Short) Just A DLL Process To Help With Everything
    // https://learn.microsoft.com/en-us/windows/win32/api/winsock/nf-winsock-wsastartup
    return WSAStartup(MAKEWORD(2, 0), &WSASocketInformation);
}

int ServerSocketManager::CreateServerSocket()
{
    ServerSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    return ServerSocket == INVALID_SOCKET ? SOCKET_ERROR : NO_ERROR;
}

int ServerSocketManager::BindSocketToAddress()
{
    // Clear Out Server Socket Memory Then Fill In The Important Information
    ZeroMemory(&ServerAddress, sizeof(ServerAddress));
    ServerAddress.sin_family = AF_INET;
    ServerAddress.sin_addr.s_addr = htonl(INADDR_ANY); // Accepts A Connection Form Any IP
    ServerAddress.sin_port = htons(ServerSocketPort);  // Port

    return bind(ServerSocket, (struct sockaddr*)&ServerAddress, sizeof(ServerAddress));
}

int ServerSocketManager::SetSocketToListenState()
{
    //listen: places a socket in a state of listening for incoming connection;  accept: permits an incoming connection attempt on a socket;  
    return listen(ServerSocket, MaxPendingConnections);
}

int ServerSocketManager::ShutdownSocketConnections()
{
    // https://learn.microsoft.com/en-us/windows/win32/api/winsock/nf-winsock-shutdown
    // https://learn.microsoft.com/en-gb/windows/win32/winsock/graceful-shutdown-linger-options-and-socket-closure-2?redirectedfrom=MSDN
    return shutdown(ServerSocket, SD_BOTH);
}

int ServerSocketManager::CloseSocketConnections()
{
    // https://learn.microsoft.com/en-us/windows/win32/api/winsock/nf-winsock-closesocket
    return closesocket(ServerSocket);
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
