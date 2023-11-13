#include "ServerListenSocket.h"
#include "../Utils/Logging.h"
#include <ws2tcpip.h>

ServerListenSocket::ServerListenSocket()
{
}

int ServerListenSocket::InitializeServerSocket()
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

int ServerListenSocket::RunServerSocket()
{
    int ErrorCode = 0;
    LogMessage("Accepting New Connections");

    while (IsApplicationRunning)
    {
        //if(JoinAndCleanupCompletedThreads())

        // Set This To Busy Waiting.
        // TODO: Fix This Busy Waiting
        if (!DoesServerDataHaveSpaceForAdditionalConnections())
        {
            continue;
        }

        // Report The Error But Keep Searching For More Connections
        ErrorCode = AcceptConnection();
        if (ErrorCode != NO_ERROR)
        {
            int WSAErrorCode = LogWSAErrorToConsole();

            // TODO: Maybe support WSAEINPROGRESS under certain circumstances
            if (WSAErrorCode == WSAEWOULDBLOCK || WSAErrorCode == WSAECONNRESET || WSAErrorCode == WSAEINPROGRESS)
            {
                continue;
            }

            IsApplicationRunning = false;
        }
    }

    return NO_ERROR;
}

int ServerListenSocket::TerminateServerSocket()
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

void ServerListenSocket::InitializeServerData()
{}

int ServerListenSocket::InitializeWSAStartup()
{
    // Intializes the WindowSocketApplication (WSA For Short) Just A DLL Process To Help With Everything
    // https://learn.microsoft.com/en-us/windows/win32/api/winsock/nf-winsock-wsastartup
    return WSAStartup(MAKEWORD(2, 0), &WSASocketInformation);
}

int ServerListenSocket::CreateServerSocket()
{
    ServerSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    return ServerSocket == INVALID_SOCKET ? SOCKET_ERROR : NO_ERROR;
}

int ServerListenSocket::BindSocketToAddress()
{
    // Clear Out Server Socket Memory Then Fill In The Important Information
    ZeroMemory(&ServerAddress, sizeof(ServerAddress));
    ServerAddress.sin_family = AF_INET;
    ServerAddress.sin_addr.s_addr = htonl(INADDR_ANY); // Accepts A Connection Form Any IP
    ServerAddress.sin_port = htons(ServerSocketPort);  // Port

    return bind(ServerSocket, (struct sockaddr*)&ServerAddress, sizeof(ServerAddress));
}

int ServerListenSocket::SetSocketToListenState()
{
    //listen: places a socket in a state of listening for incoming connection;  accept: permits an incoming connection attempt on a socket;  
    return listen(ServerSocket, MaxPendingConnections);
}

int ServerListenSocket::AcceptConnection()
{
    int IndexOfAvailableSocket = GetIndexOfAvailableSocket();
    if (IndexOfAvailableSocket == -1)
    {
        // Something odd happened here, are you messing up soething with threads clearing up sections of the array? :l
        return SOCKET_ERROR;
    }

    // IMPORTANT NOTE:
    // Keep an explicit reference instaed of object to ensure it doesn't copy and we write directly to the data in the array
    const std::shared_ptr<ClientConnection>& AvailableClientConnection = ClientConnections[IndexOfAvailableSocket];

    SOCKET& IncommingConnection = AvailableClientConnection->ClientSocket;
    SOCKADDR_IN Client_Address;
    int ClientAddressSize = sizeof(Client_Address);

    IncommingConnection = accept(ServerSocket, (struct sockaddr*)&Client_Address, &ClientAddressSize);
    if (IncommingConnection == INVALID_SOCKET)
    {
        return SOCKET_ERROR;
    }

    char addrstr[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &(Client_Address.sin_addr), addrstr, INET_ADDRSTRLEN);

    std::string OutputMessage;
    OutputMessage += "Accepted Connection From:";
    OutputMessage += addrstr;
    LogMessage(OutputMessage.c_str());

    // TODO: Now we need to convert this to use worker object instances
    //RunningJobs.emplace_back(Thread_HandleClientMessageSequence, AvailableClientConnection);

    return NO_ERROR;
}

int ServerListenSocket::ShutdownSocketConnections()
{
    // https://learn.microsoft.com/en-us/windows/win32/api/winsock/nf-winsock-shutdown
    // https://learn.microsoft.com/en-gb/windows/win32/winsock/graceful-shutdown-linger-options-and-socket-closure-2?redirectedfrom=MSDN
    return shutdown(ServerSocket, SD_BOTH);
}

int ServerListenSocket::CloseSocketConnections()
{
    // https://learn.microsoft.com/en-us/windows/win32/api/winsock/nf-winsock-closesocket
    return closesocket(ServerSocket);
}

int ServerListenSocket::TerminateWSA()
{
    // https://learn.microsoft.com/en-us/windows/win32/api/winsock/nf-winsock-wsacleanup
    return WSACleanup();
}
