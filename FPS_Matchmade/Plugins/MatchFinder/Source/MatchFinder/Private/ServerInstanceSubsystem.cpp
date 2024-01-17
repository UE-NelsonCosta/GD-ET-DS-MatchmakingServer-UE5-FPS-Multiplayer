//// Fill out your copyright notice in the Description page of Project Settings.

#include "ServerInstanceSubsystem.h"

#include "Sockets.h"
#include "Interfaces/IPv4/IPv4Endpoint.h"
#include "Kismet/GameplayStatics.h"

bool UServerInstanceSubsystem::ShouldCreateSubsystem(UObject* Outer) const
{
#if UE_SERVER || UE_EDITOR // Just so we can debug this in editor
	return true;
#else
	return false;
#endif
}

void UServerInstanceSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	FName Value = "7777"; // Default port, if you use UE from a non batch file this is attributed automatically
	FParse::Value(FCommandLine::Get(), TEXT("Port="), Value);

	ServerID = FCString::Atoi(*Value.ToString());
}

void UServerInstanceSubsystem::Deinitialize()
{
	Super::Deinitialize();
}

// TODO: Error Logging And Handling
bool UServerInstanceSubsystem::RequestClientDataFromMatchmakingServer()
{
	// Do Socket Stuff To Retrieve The Allowed Client List
	// We do a blocking operation as nothing else is allowed to happen until we have this data
	TArray<uint8> ParsedIPv4;
	TArray<FString> SplitString;
	MatchmakingServerIP.ParseIntoArray( SplitString, TEXT(".") );

	ParsedIPv4.Empty();
	for ( int32 i = 0; i < SplitString.Num(); ++i )
	{
		ParsedIPv4.Add( FCString::Atoi( *SplitString[i] ) );
	}
	
	FSocket* SocketToMMSI = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->CreateSocket(NAME_Stream, "MatchMakingServerInstance", false);
	if(!SocketToMMSI)
	{
		// TODO: Log
		return false;
	}

	const FIPv4Address IPv4Address(ParsedIPv4[0], ParsedIPv4[1], ParsedIPv4[2], ParsedIPv4[3]);
	const FIPv4Endpoint IPv4Endpoint(IPv4Address, MatchmakingServerInstancePort);
	const TSharedRef<FInternetAddr> MatchmakingServerAddress = IPv4Endpoint.ToInternetAddrIPV4();
	if (!SocketToMMSI->Connect(MatchmakingServerAddress.Get()))
	{
		// TODO: Log
		return false;
	}

	// Let The Server Know Who's Connecting Using The Server's ID
	const FString Message = FString::FromInt(ServerID);
	const int32 MessageLength = Message.Len();
	char* Payload = TCHAR_TO_ANSI(*Message); // Ensure The Encoding Is Correct (From UTF-16 to Byte Sized ANSI for C++)

	int32 BytesSent = 0;
	SocketToMMSI->Send(reinterpret_cast<uint8*>(Payload), MessageLength, BytesSent);

	
	// Time To Read All Client Data!
	TArray<uint8> ReadBuffer;
	ReadBuffer.AddUninitialized(2048);

	FString CompleteMessage;
	int32 BytesRead = 0;
	while(BytesRead > 0)
	{
		SocketToMMSI->Recv(ReadBuffer.GetData(), 2048, BytesRead);
		CompleteMessage += ANSI_TO_TCHAR( reinterpret_cast<char*>(ReadBuffer.GetData()) );
	}

	TArray<FString> PerClientSplitString;
	CompleteMessage.ParseIntoArray(PerClientSplitString, TEXT("|"));

	for(int i = 0; i < PerClientSplitString.Num(); ++i)
	{
		ExpectedGameClientConnections.Emplace
		(
			*UGameplayStatics::ParseOption(PerClientSplitString[i], "ClientName"),
			*UGameplayStatics::ParseOption(PerClientSplitString[i], "AuthToken")
		);
	}
	
	return true;
}

void UServerInstanceSubsystem::GetClientConnectionData(TArray<FClientConnectionData>& ClientConnectionData) const
{
	ClientConnectionData = ExpectedGameClientConnections;
}
