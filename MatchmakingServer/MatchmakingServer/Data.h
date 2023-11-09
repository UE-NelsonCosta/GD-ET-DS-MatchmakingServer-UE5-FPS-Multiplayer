#pragma once
#include <winsock2.h>  
#include <ws2tcpip.h>  
#include <iostream>  
#include <vector>
#include <thread>

struct ClientConnection
{
	SOCKET ClientSocket;
	std::string Username;
	std::string password;
};

// As we have mutliple projects these names can overlap so we distinguish them using "ClientData" and "ServerData"
namespace ServerData
{
	// Declaring Defaults For Project
	const char* DefaultExecutionPath = "";
	const wchar_t* DefaultServerIPAddress = L"127.0.0.1";
	const int		DefaultServerSocketPort = 42069;

	// Non Const And Overridable Runtime Variables, Using Standard Library Objects To Facilitate Usage
	std::string		ExecutionPath = DefaultExecutionPath;
	std::wstring	ServerIPAddress = DefaultServerIPAddress;
	int				ServerSocketPort = DefaultServerSocketPort;

	SOCKET			ServerSocket = 0;
	SOCKADDR_IN		ServerAddress;
	WSADATA			WSASocketInformation;

	// Macro Dictates How Many We Can Have Already Precreated, Never add to it
	std::vector<ClientConnection>	ClientConnections(MaxSimultaneousConnections);
	std::vector<std::thread> RunningJobs;

	bool IsApplicationRunning = true;
}

