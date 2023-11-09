#pragma once
#include "Utils.h"
#include "Data.h"
// This chain should be handled by a thread per client

// First Function In The Chain Listens For A Message To Login From The Client
// If It's Valid We Move Onto Requesting The Game
// If Not We Repeat The Process
void ListenForClientAuthentication(ClientConnection& ClientConnection)
{
	char ReadBuffer[MessageBufferSize];
	ZeroMemory(ReadBuffer, MessageBufferSize);
	int BytesReceived = ReceiveData(ClientConnection.ClientSocket, ReadBuffer);

	std::vector<std::string> ParsedResults;
	ParseMessage(ReadBuffer, ParsedResults);
	//if ()
	//{

	//}
}