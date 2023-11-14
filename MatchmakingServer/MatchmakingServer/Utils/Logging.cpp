#include "Logging.h"

#include <WinSock2.h>
#include <iomanip>

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
    auto time = std::time(nullptr);
#pragma warning(suppress: 4996)
    std::cout << "[" << std::put_time(std::gmtime(&time), "%F %T") << "]: " << Message << std::endl;
}
