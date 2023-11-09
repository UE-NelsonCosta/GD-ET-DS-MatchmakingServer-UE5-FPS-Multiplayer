#pragma once

#include "Data.h"
#include "ProjectMacros.h"
#include "Utils_Logging.h"

#pragma region InitializationUtils

void InitializeServerData()
{
}

int  InitializeWSAStartup()
{
    // Intializes the WindowSocketApplication (WSA For Short) Just A DLL Process To Help With Everything
    // https://learn.microsoft.com/en-us/windows/win32/api/winsock/nf-winsock-wsastartup
    return WSAStartup(MAKEWORD(2, 0), &ServerData::WSASocketInformation);
}

int  CreateServerSocket()
{
    ServerData::ServerSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    return ServerData::ServerSocket == INVALID_SOCKET ? SOCKET_ERROR : NO_ERROR;
}

int  BindSocketToAddress()
{
    // Clear Out Server Socket Memory Then Fill In The Important Information
    ZeroMemory(&ServerData::ServerAddress, sizeof(ServerData::ServerAddress));
    ServerData::ServerAddress.sin_family = AF_INET;
    ServerData::ServerAddress.sin_addr.s_addr = htonl(INADDR_ANY); // Accepts A Connection Form Any IP
    ServerData::ServerAddress.sin_port = htons(ServerData::ServerSocketPort);  // Port

    return bind(ServerData::ServerSocket, (struct sockaddr*)&ServerData::ServerAddress, sizeof(ServerData::ServerAddress));
}

int  SetSocketToListenState()
{
    //listen: places a socket in a state of listening for incoming connection;  accept: permits an incoming connection attempt on a socket;  
    return listen(ServerData::ServerSocket, MaxPendingConnections);
}

int  InitializeServerSocket()
{
    int ErrorCode = 0;

    ErrorCode = WSAStartup(MAKEWORD(2, 0), &ServerData::WSASocketInformation);
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

int InitializeServerApplication()
{
    int ErrorCode = 0;

    InitializeServerData();

    return InitializeServerSocket();
}

#pragma endregion InitializationUtils

#pragma region SocketTermination

int ShutdownSocketConnections()
{
    // https://learn.microsoft.com/en-us/windows/win32/api/winsock/nf-winsock-shutdown
    // https://learn.microsoft.com/en-gb/windows/win32/winsock/graceful-shutdown-linger-options-and-socket-closure-2?redirectedfrom=MSDN
    return shutdown(ServerData::ServerSocket, SD_BOTH);
}

int CloseSocketConnections()
{
    // https://learn.microsoft.com/en-us/windows/win32/api/winsock/nf-winsock-closesocket
    return closesocket(ServerData::ServerSocket);
}

int TerminateWSA()
{
    // https://learn.microsoft.com/en-us/windows/win32/api/winsock/nf-winsock-wsacleanup
    return WSACleanup();
}

void TerminateThreadJobs()
{
    //for (int i = 0; i < ServerData::Producers.size(); ++i)
    //{
    //    ServerData::Producers[i].join();
    //}
    //
    //for (int i = 0; i < ServerData::Consumers.size(); ++i)
    //{
    //    ServerData::Consumers[i].join();
    //}
}

void CleanupServerData()
{
}

int TerminateServerSocketConnection()
{
    int ErrorCode = 0;

    // Gracefully Terminate The Connection So The Other Side Knows We're Done here
    ErrorCode = ShutdownSocketConnections();
    if (ErrorCode != NO_ERROR)
    {
        LogWSAErrorToConsole();

        return ErrorCode;
    }

    // Initializes The WSA Process So We Can use Sockets
    ErrorCode = CloseSocketConnections();
    if (ErrorCode != NO_ERROR)
    {
        LogWSAErrorToConsole();

        return ErrorCode;
    }

    // Initializes The WSA Process So We Can use Sockets
    ErrorCode = TerminateWSA();
    if (ErrorCode != NO_ERROR)
    {
        LogWSAErrorToConsole();

        return ErrorCode;
    }

    return NO_ERROR;
}

int TerminateServerApplication()
{
    int ErrorCode = TerminateServerSocketConnection();

    TerminateThreadJobs();

    CleanupServerData();

    return ErrorCode;
}

#pragma endregion SocketTermination

#pragma region IP&PortValidation

bool CanStringBeConsideredAnIPv4Address(std::string& IPv4Address)
{
    // IP's have a basic minimum of 7 characters (0.0.0.0) with 3x '.' characters in it
    return IPv4Address.length() >= 7 && std::count(IPv4Address.begin(), IPv4Address.end(), '.') == 3;
}

bool IsValidIPv4Address(std::string& IPAddress)
{
    // Note: Stack Allocate This Temporary Object To Help Us Parse If This Is Valid Or Not, Gets Destroyed When Leaving The Scope
    SOCKADDR_IN sa;
    return inet_pton(AF_INET, IPAddress.c_str(), &(sa.sin_addr)) == 1;
}

bool IsStringAValidIPv4Address(std::string& IPv4Address)
{
    return CanStringBeConsideredAnIPv4Address(IPv4Address) && IsValidIPv4Address(IPv4Address);
}



/*
 * Little function for us to check if the input is a valid IP Address
 * Takes an IP as a string, and tries to convert the result into its binary form to use later
 * https://learn.microsoft.com/en-us/windows/win32/api/ws2tcpip/nf-ws2tcpip-inet_pton
 */

bool CanStringBeConsideredAPort(std::string& IPv4Address)
{
    int value = std::atoi(IPv4Address.c_str());
    return value >= MinimumSupportedSocketPort && value <= MaximumSupportedSocketPort;
}

#pragma endregion IP&PortValidation

void ParseCommandlineArguments(int argc, char* argv[])
{
    LogMessage("Begin Parsing Arguments");

    // Note: This one is always Guaranteed To Exist 
    ServerData::ExecutionPath = argv[0];

    LogMessage("Using Execution Path:");
    LogMessage(ServerData::ExecutionPath.c_str());

    std::string CachedArgument;
    for (int i = 1; i < argc; ++i)
    {
        CachedArgument = argv[i];

        // Let's Look for the IPv4 Address
        if (CanStringBeConsideredAnIPv4Address(CachedArgument))
        {
            if (IsValidIPv4Address(CachedArgument))
            {
                // Override The IPAddress
                ServerData::ServerIPAddress = std::wstring(CachedArgument.begin(), CachedArgument.end());
                LogMessage("Found Override IP:");
                LogMessage(CachedArgument.c_str());
            }
        }

        // Let's Look For The Port
        if (CanStringBeConsideredAPort(CachedArgument))
        {
            ServerData::ServerSocketPort = std::atoi(CachedArgument.c_str());
            LogMessage("Found Override Port:");
            LogMessage(CachedArgument.c_str());
        }
    }

    LogMessage("Completed Parsing Arguments");
}

bool DoesServerDataHaveSpaceForAdditionalConnections()
{
    for (int i = 0; i < ServerData::ClientConnections.size(); ++i)
    {
        if (ServerData::ClientConnections[i].ClientSocket == 0)
            return true;
    }

    return false;
}

int GetIndexOfAvailableSocket()
{
    for (int i = 0; i < ServerData::ClientConnections.size(); ++i)
    {
        if (ServerData::ClientConnections[i].ClientSocket == 0)
            return i;
    }

    return -1;
}

int SendData(SOCKET& SocketToSendTo, const char* DataToSend, int LengthOfMessageInBytes)
{
    // https://learn.microsoft.com/en-us/windows/win32/api/winsock2/nf-winsock2-send
    return send(SocketToSendTo, DataToSend, LengthOfMessageInBytes, 0);
}

int ReceiveData(SOCKET& SocketToWriteReceiveFrom, char* BufferToWriteTo)
{
    return recv(SocketToWriteReceiveFrom, BufferToWriteTo, MessageBufferSize, 0);
}
