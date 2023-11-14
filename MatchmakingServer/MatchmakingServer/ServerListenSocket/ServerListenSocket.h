/**
* 
* Lifetime For Server Sockets Should Be Something Of The Sort
*
* Initialization:
* WSAStartup: initiates the Winsock DLL;
* socket: creates a socket that is bound to a transport service provider;  bind: associates a local address with a socket;
* listen: sets the socket to listen mode to let it accept communications;
*
* Lifetime Communication:
* accept: accept a connection
* recv: receives data from a connected socket;
* send: sends data on a connected socket;
*
* Termination:
* closesocket: closes an existing socket;
* WSACleanup: terminates use of the Winsock DLL.
* 
**/

#pragma once

// Dynamically linking of a lib file, you normally do this in your projects settings
#pragma comment(lib, "ws2_32.lib")

#include <WinSock2.h>
#include <vector>
#include "../ProjectMacros.h"
#include <thread>
#include <mutex>
#include "..\ClientConnection\ClientConnection.h";


class ServerListenSocket
{
public:

    ServerListenSocket();

    int  InitializeServerSocket();

    int  RunServerSocket();

    int  TerminateServerSocket();

    int SendData(SOCKET& SocketToSendTo, const char* DataToSend, int LengthOfMessageInBytes)
    {
        // https://learn.microsoft.com/en-us/windows/win32/api/winsock2/nf-winsock2-send
        return send(SocketToSendTo, DataToSend, LengthOfMessageInBytes, 0);
    }

    int ReceiveData(SOCKET& SocketToWriteReceiveFrom, char* BufferToWriteTo)
    {
        return recv(SocketToWriteReceiveFrom, BufferToWriteTo, MessageBufferSize, 0);
    }

private: // Initialization

    void InitializeServerData();
    int  InitializeWSAStartup();
    int  CreateServerSocket();
    int  BindSocketToAddress();
    int  SetSocketToListenState();

private: // Run Server

    // TODO: Should Probably Thread This, And Main Thread Should Unlock The Threads That Are Done Adding Data   
    int AcceptConnection();

private: // Terminations

    int ShutdownSocketConnections();
    int CloseSocketConnections();
    int TerminateWSA();
    void CleanupWorkers();
    void CleanupServerData();





    bool DoesServerDataHaveSpaceForAdditionalConnections()
    {
        for (int i = 0; i < ClientConnections.size(); ++i)
        {
            if (ClientConnections[i]->ClientSocket == 0)
                return true;
        }

        return false;
    }

    int GetIndexOfAvailableSocket()
    {
        for (int i = 0; i < ClientConnections.size(); ++i)
        {
            if (ClientConnections[i]->ClientSocket == 0)
                return i;
        }

        return -1;
    }





private:

    // Declaring Defaults For Project
    const char*     DefaultExecutionPath = "";
    const wchar_t*  DefaultServerIPAddress = L"127.0.0.1";
    const int		DefaultServerSocketPort = 42069;

    // Non Const And Overridable Runtime Variables, Using Standard Library Objects To Facilitate Usage
    std::string		ExecutionPath       = DefaultExecutionPath;
    std::wstring	ServerIPAddress     = DefaultServerIPAddress;
    int				ServerSocketPort    = DefaultServerSocketPort;

    SOCKET			ServerSocket = 0;
    SOCKADDR_IN		ServerAddress;
    WSADATA			WSASocketInformation;

    // Macro Dictates How Many We Can Have Already Precreated, Never add to it
    std::vector<std::shared_ptr<ClientConnection>>	ClientConnections;
    std::vector<std::thread> RunningJobs;

    bool IsApplicationRunning = true;



private:    // Refactored Code

    std::thread AcceptConnectionsWorker;
    std::vector<std::thread> ClientMessageWorkers;
};

