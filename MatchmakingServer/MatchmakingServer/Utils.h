// SocketIntroduction_Common.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
#pragma once
#include <WinSock2.h>
#include <ws2tcpip.h> 
#include <iostream>
#include "ProjectMacros.h"
#include "Data.h"

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

#pragma region LoggingUtils

int ParseLastWSAErrorToString(std::string& ErrorOutput)
{
    int LastError = WSAGetLastError();

    switch (LastError)
    {
    case WSASYSNOTREADY:
    {
        ErrorOutput = "WSALastError: The underlying network subsystem is not ready for network communication.";
        break;
    }
    case WSAVERNOTSUPPORTED:
    {
        ErrorOutput = "WSALastError: The version of Windows Sockets support requested is not provided by this particular Windows Sockets implementation.";
        break;
    }
    case WSAEINPROGRESS:
    {
        ErrorOutput = "WSALastError: A blocking Windows Sockets 1.1 call is in progress, or the service provider is still processing a callback function.";
        break;
    }
    case WSAEPROCLIM:
    {
        ErrorOutput = "WSALastError: A limit on the number of tasks supported by the Windows Sockets implementation has been reached.";
        break;
    }
    case WSAEFAULT:
    {
        ErrorOutput = "WSALastError: Passed In Parameter Not In Valid Memory - Did you pass in a null pointer?";
        break;
    }
    case WSAEAFNOSUPPORT:
    {
        ErrorOutput = "WSALastError: The specified address family is not supported.For example, an application tried to create a socket for the AF_IRDA address family but an infrared adapterand device driver is not installed on the local computer.";
        break;
    }
    case WSANOTINITIALISED:
    {
        ErrorOutput = "WSALastError: A successful WSAStartup call must occur before using this function.";
        break;
    }
    case WSAENETDOWN:
    {
        ErrorOutput = "WSALastError: The network subsystem or the associated service provider has failed.";
        break;
    }
    case WSAEMFILE:
    {
        ErrorOutput = "WSALastError: No more socket descriptors are available.";
        break;
    }
    case WSAEINVAL:
    {
        ErrorOutput = "WSALastError: An invalid argument was supplied.This error is returned if the af parameter is set to AF_UNSPECand the typeand protocol parameter are unspecified.";
        break;
    }
    case WSAEINVALIDPROVIDER:
    {
        ErrorOutput = "WSALastError: The service provider returned a version other than 2.2.";
        break;
    }
    case WSAEINVALIDPROCTABLE:
    {
        ErrorOutput = "WSALastError: The service provider returned an invalid or incomplete procedure table to the WSPStartup.";
        break;
    }
    case WSAENOBUFS:
    {
        ErrorOutput = "WSALastError: No buffer space is available.The socket cannot be created.";
        break;
    }
    case WSAEPROTONOSUPPORT:
    {
        ErrorOutput = "WSALastError: The specified protocol is not supported.";
        break;
    }
    case WSAEPROTOTYPE:
    {
        ErrorOutput = "WSALastError: The specified protocol is the wrong type for this socket.";
        break;
    }
    case WSAEPROVIDERFAILEDINIT:
    {
        ErrorOutput = "WSALastError: The service provider failed to initialize.This error is returned if a layered service provider(LSP) or namespace provider was improperly installed or the provider fails to operate correctly.";
        break;
    }
    case WSAESOCKTNOSUPPORT:
    {
        ErrorOutput = "WSALastError: Address Family Invalid, Only AF_INET and AFINET6 Supported";
        break;
    }
    case WSAEADDRINUSE:
    {
        ErrorOutput = "WSALastError: The socket's local address is already in use and the socket was not marked to allow address reuse with SO_REUSEADDR. This error usually occurs when executing bind, but could be delayed until the connect function if the bind was to a wildcard address (INADDR_ANY or in6addr_any) for the local IP address. A specific address needs to be implicitly bound by the connect function.";
        break;
    }

    case WSAEINTR:
    {
        ErrorOutput = "WSALastError: The blocking Windows Socket 1.1 call was canceled through WSACancelBlockingCall.";
        break;
    }
    case WSAEALREADY:
    {
        ErrorOutput = "WSALastError: A nonblocking connect call is in progress on the specified socket.\n Note  In order to preserve backward compatibility, this error is reported as WSAEINVAL to Windows Sockets 1.1 applications that link to either Winsock.dll or Wsock32.dll.";
        break;
    }
    case WSAEADDRNOTAVAIL:
    {
        ErrorOutput = "WSALastError: The remote address is not a valid address(such as INADDR_ANY or in6addr_any) .";
        break;
    }
    case WSAECONNREFUSED:
    {
        ErrorOutput = "WSALastError: The attempt to connect was forcefully rejected.";
        break;
    }
    case WSAEISCONN:
    {
        ErrorOutput = "WSALastError: The socket is already connected(connection - oriented sockets only).";
        break;
    }
    case WSAENETUNREACH:
    {
        ErrorOutput = "WSALastError: The network cannot be reached from this host at this time.";
        break;
    }
    case WSAEHOSTUNREACH:
    {
        ErrorOutput = "WSALastError: A socket operation was attempted to an unreachable host.";
        break;
    }
    case WSAENOTSOCK:
    {
        ErrorOutput = "WSALastError: The descriptor specified in the s parameter is not a socket.";
        break;
    }
    case WSAETIMEDOUT:
    {
        ErrorOutput = "WSALastError: An attempt to connect timed out without establishing a connection.";
        break;
    }
    case WSAEWOULDBLOCK:
    {
        ErrorOutput = "WSALastError: The socket is marked as nonblockingand the connection cannot be completed immediately.";
        break;
    }
    case WSAEACCES:
    {
        ErrorOutput = "WSALastError: An attempt to connect a datagram socket to broadcast address failed because setsockopt option SO_BROADCAST is not enabled.";
        break;
    }
    case WSAENOTCONN:
    {
        ErrorOutput = "WSALastError: The socket is not connected.";
        break;
    }
    case WSAENETRESET:
    {
        ErrorOutput = "WSALastError: For a connection - oriented socket, this error indicates that the connection has been broken due to keep - alive activity that detected a failure while the operation was in progress.For a datagram socket, this error indicates that the time to live has expired.";
        break;
    }
    case WSAEOPNOTSUPP:
    {
        ErrorOutput = "WSALastError: MSG_OOB was specified, but the socket is not stream - style such as type SOCK_STREAM, OOB data is not supported in the communication domain associated with this socket, or the socket is unidirectional and supports only send operations.";
        break;
    }
    case WSAESHUTDOWN:
    {
        ErrorOutput = "WSALastError: The socket has been shut down; it is not possible to receive on a socket after shutdown has been invoked with how set to SD_RECEIVE or SD_BOTH.";
        break;
    }
    case WSAEMSGSIZE:
    {
        ErrorOutput = "WSALastError: The message was too large to fit into the specified buffer and was truncated.";
        break;
    }
    case WSAECONNABORTED:
    {
        ErrorOutput = "WSALastError: The virtual circuit was terminated due to a time - out or other failure.The application should close the socket as it is no longer usable.";
        break;
    }
    case WSAECONNRESET:
    {
        ErrorOutput = "WSALastError: The virtual circuit was reset by the remote side executing a hard or abortive close.The application should close the socket as it is no longer usable.On a UDP - datagram socket, this error would indicate that a previous send operation resulted in an ICMP \"Port Unreachable\" message.";
        break;
    }
    }

    return LastError;
}

void LogErrorToConsole(std::string& VerboseErrorOutput)
{
    std::cout << VerboseErrorOutput << std::endl;
}

int LogWSAErrorToConsole()
{
    std::string VerboseErrorOutput;

    int ErrorCode = ParseLastWSAErrorToString(VerboseErrorOutput);

    std::cout << VerboseErrorOutput << std::endl;

    return ErrorCode;
}

void LogMessage(const char* Message)
{
    time_t now = time(0);
    char dt[64];
    ctime_s(dt, 64, &now);

    // TODO: Use A String Builder
    std::cout << "[" << dt << "]: " << Message << std::endl;
}

#pragma endregion LoggingUtils

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

#pragma region ServerUtils

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

#pragma endregion ServerUtils

#pragma region MessageParsing

void ParseLoginMessageParameters(const char* ReadBuffer, std::vector<std::string>& ParsedResults)
{
    std::string Message = ReadBuffer;

    int FirstDelimiter = Message.find('|', 0);
    int SecondDelimiter = Message.find('|', FirstDelimiter + 1);

    // Time To Chop Up The Message!
    ParsedResults.push_back(Message.substr(0, FirstDelimiter));
    ParsedResults.push_back(Message.substr(FirstDelimiter + 1, SecondDelimiter));
    ParsedResults.push_back(Message.substr(SecondDelimiter + 1, Message.length() - SecondDelimiter));
}

bool ParseMessage(const char* ReadBuffer, std::vector<std::string>& ParsedResults)
{
    char MessageHeader[3];
    strncpy_s(MessageHeader, ReadBuffer, 3);

    if (strcmp(MessageHeader, "LGN") == 0)
    {
        // Parse Login Data From Here To Array And Go Back
        // LGN | Username | Password - Login Request
        ParseLoginMessageParameters(ReadBuffer, ParsedResults);
        return true;
    }

    if (strcmp(MessageHeader, "CAK") == 0)
    {
        // Parse Login Data From Here To Array And Go Back
        // CAK - Server Aknowleding Client Message
        return true;
    }

    if (strcmp(MessageHeader, "UKN") == 0)
    {
        // Parse Login Data From Here To Array And Go Back
        // UKN - Unknown / Misunderstood Message
        return true;
    }

    if (strcmp(MessageHeader, "RGN") == 0)
    {
        // Parse Login Data From Here To Array And Go Back
        // RGM | GameModeID - Gamemode Request
        return true;
    }

    return false;
}

#pragma endregion MessageParsing