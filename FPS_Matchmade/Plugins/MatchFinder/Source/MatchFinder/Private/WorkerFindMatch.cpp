#include "WorkerFindMatch.h"

#include "Sockets.h"
#include "SocketSubsystem.h"
#include "Interfaces/IPv4/IPv4Address.h"
#include "Interfaces/IPv4/IPv4Endpoint.h"

#pragma region FWorkerFindMatch

bool FWorkerFindMatch::Init()
{
	MatchFindingProgress = EMatchFindingProgress::CreatingConnection;

	State.Empty();
	ResultIPnPort.Empty();
	ResultParameters.Empty();
	
	
	// Not wanting to do anything here for now, let the main thread go back to whatever it was doing.
	return FRunnable::Init();
}

void FWorkerFindMatch::Stop()
{
	if(SocketToMatchmakingServer && SocketToMatchmakingServer->GetConnectionState() == ESocketConnectionState::SCS_Connected )
	{
		SocketToMatchmakingServer->Shutdown(ESocketShutdownMode::ReadWrite);
		SocketToMatchmakingServer->Close();
		SocketToMatchmakingServer = nullptr;
	}
}

// TODO: Use Exit Codes?
uint32 FWorkerFindMatch::Run()
{
	State = "Connecting To Matchmaking Server";
	MatchFindingProgress = EMatchFindingProgress::CreatingConnection;

	TArray<uint8> ParsedIPv4;
	const bool IsValidIP = FormatIPv4StringToNumerics(MatchmakingServerIP, ParsedIPv4);
	const bool IsValidPort = HasValidPort(MatchmakingServerPort);
	if(!IsValidIP || !IsValidPort)
	{
		State = "Bad IP Given: Please Check Config File";
		MatchFindingProgress = EMatchFindingProgress::ConnectionFailed;

		return 1;
	}
	
	if(!CreateSocketObject(SocketToMatchmakingServer))
	{
		State = "Failed To Create Socket Object";
		MatchFindingProgress = EMatchFindingProgress::ConnectionFailed;

		return 2;
	}
		
	TSharedRef<FInternetAddr> MatchmakingServerAddress = CreateInternetAddressToMatchmakingServer(ParsedIPv4);	
	if (!SocketToMatchmakingServer->Connect(MatchmakingServerAddress.Get()))
	{
		State = "Failed To Connect To MatchmakingServer With Given IP";
		MatchFindingProgress = EMatchFindingProgress::ConnectionFailed;

		return 3;
	}

	// Note: Don't Do This In Real Life! This is Just So The Widget Can Update And Show You A Step By Step
	FPlatformProcess::Sleep(2);

	State = "Connected To Matchmaking Server ... Logging In";
	MatchFindingProgress = EMatchFindingProgress::LoggingIn;


	SendLoginData();
	if(!ReceiveLoginReply())
	{
		State = "Failed To Login Please Try Again Later";
		MatchFindingProgress = EMatchFindingProgress::LoginFailed;

		return 4;
	}
	
	FPlatformProcess::Sleep(2);
	State = "Login Successful! ... Requesting Game!";
	MatchFindingProgress = EMatchFindingProgress::RequestingGame;

	SendGamemodeRequest();
	if(!ReceiveGamemodeReply())
	{
		State = "Failed To Request Gamemode Please Try Again";
		MatchFindingProgress = EMatchFindingProgress::FindingFailed;

		return 5;
	}

	FPlatformProcess::Sleep(2);
	State = "Request Successful! ... Finding A Game!";

	if(!ReceiveGamemodeConnection(ResultIPnPort, ResultParameters))
	{
		State = "Failed To Get Connection Message Please Try Again";
		MatchFindingProgress = EMatchFindingProgress::FindingFailed;

		return 6;
	}
	
	// Note: Don't Do This In Real Life! This is Just So The Widget Can Update And Show You A Step By Step
	FPlatformProcess::Sleep(2);
	State = "Found Gamemode! ... Connecting To Server";
	MatchFindingProgress = EMatchFindingProgress::Complete;


	return 0;
}

void FWorkerFindMatch::Exit()
{
	if(SocketToMatchmakingServer && SocketToMatchmakingServer->GetConnectionState() == ESocketConnectionState::SCS_Connected)
	{
		SocketToMatchmakingServer->Shutdown(ESocketShutdownMode::ReadWrite);
		SocketToMatchmakingServer->Close();
		SocketToMatchmakingServer = nullptr;
	}
}

EMatchFindingProgress FWorkerFindMatch::GetProgress() const
{
	return MatchFindingProgress;
}

bool FWorkerFindMatch::CreateSocketObject(FSocket*& SocketToWriteTo)
{
	SocketToWriteTo = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->CreateSocket(NAME_Stream, "MatchMakingServer", false);
	return SocketToWriteTo != nullptr;
}

TSharedRef<FInternetAddr> FWorkerFindMatch::CreateInternetAddressToMatchmakingServer(const TArray<uint8>& ParsedIPv4)
{
	FIPv4Address IPv4Address(ParsedIPv4[0], ParsedIPv4[1], ParsedIPv4[2], ParsedIPv4[3]);
	FIPv4Endpoint IPv4Endpoint(IPv4Address, MatchmakingServerPort);
	return IPv4Endpoint.ToInternetAddrIPV4();
}

void FWorkerFindMatch::CleanseIPOfInvalidCharacters(FString& IP)
{
	FRegexPattern RegexPattern(TEXT("[^\\d.]"), ERegexPatternFlags::CaseInsensitive);
	FRegexMatcher RegexMatcher(RegexPattern, IP);

	while(RegexMatcher.FindNext())
	{
		int32 AttributeListBegin = RegexMatcher.GetCaptureGroupBeginning(0);
		int32 AttributeListEnd = RegexMatcher.GetCaptureGroupEnding(0);
		IP.RemoveAt(AttributeListBegin, AttributeListEnd - AttributeListBegin);
		RegexMatcher = FRegexMatcher(RegexPattern, IP);
	}
}

bool FWorkerFindMatch::IsConsideredAValidIP(const TArray<uint8>& IPValues)
{
	if(IPValues.Num() != 4)
		return false;

	//for(int i = 0; i < IPValues.Num(); ++i)
	//{
	//	// IP 0 is a default route
	//	// IP 240-255 is generally reserved but usable, for this example assuming not
	//	// IP 224-239 is generally reserved for multicast but possibly usable
	//	if(IPValues[i] == 0 || IPValues[i] >= 224)
	//	{
	//		return false;
	//	}
	//}

	return true;
}

bool FWorkerFindMatch::HasValidPort(uint16 Port)
{
	return Port >= 1024 && Port <= 65535;
}

void FWorkerFindMatch::SendLoginData()
{
	// TODO: Start Sending And Receiving Messages!
	FString Message = "LGN|Nelson|Costa123";
	int32 MessageLength = Message.Len();
	char* Payload = TCHAR_TO_ANSI(*Message); // Ensure The Encoding Is Correct (From UTF-16 to Byte Sized ANSI for C++)

	int32 BytesSent = 0;
	SocketToMatchmakingServer->Send((uint8*)Payload, MessageLength, BytesSent);
}

bool FWorkerFindMatch::ReceiveLoginReply()
{
	// Let's Read Some Data TO Confirm If The Login Was Successful or not
	TArray<uint8> ReadBuffer;
	ReadBuffer.AddZeroed(32);
	
	int32 BytesRead = 0;
	SocketToMatchmakingServer->Recv(ReadBuffer.GetData(), 32, BytesRead);

	FString ServerMessage = ANSI_TO_TCHAR( (char*)ReadBuffer.GetData() ); // Convert To UTF-16 Encoding
	if(ServerMessage.Find(TEXT("LGS")) == 0)
	{
		return true;
	}
	
	return false;
}

void FWorkerFindMatch::SendGamemodeRequest()
{
	// Let's Send The Message Back To Request A Game
	// TODO: This 0 should be something related to some gamemode
	FString Message = "RGM|0";
	int32 MessageLength = Message.Len();
	char* Payload = TCHAR_TO_ANSI(*Message); // Ensure The Encoding Is Correct (UTF-16 to Byte Sized ANSI for C++)

	int32 RequestGameBytesSent = 0;
	SocketToMatchmakingServer->Send((uint8*)Payload, MessageLength, RequestGameBytesSent);
}

bool FWorkerFindMatch::ReceiveGamemodeReply()
{
	TArray<uint8> ReadBuffer;
	ReadBuffer.AddZeroed(32);
	
	int32 BytesRead = 0;
	SocketToMatchmakingServer->Recv(ReadBuffer.GetData(), 32, BytesRead);

	FString ServerMessage = ANSI_TO_TCHAR( (char*)ReadBuffer.GetData() ); // Convert To UTF-16 Encoding
	if(ServerMessage.Find(TEXT("RGS")) == 0)
	{
		return true;
	}
	
	return false;
}

// TODO: Parse Parameters
bool FWorkerFindMatch::ReceiveGamemodeConnection(FString& OutputIPnPort, FString& OutputParameters)
{
	TArray<uint8> ReadBuffer;
	ReadBuffer.AddZeroed(32);
	
	int32 BytesRead = 0;
	SocketToMatchmakingServer->Recv(ReadBuffer.GetData(), 32, BytesRead);

	FString ServerMessage = ANSI_TO_TCHAR( (char*)ReadBuffer.GetData() ); // Convert To UTF-16 Encoding

	FString CommandType;
	FString CommandParams;
	ServerMessage.Split(TEXT("|"), &CommandType, &CommandParams);
	
	if(CommandType.Equals( TEXT("RGC") ) )
	{
		OutputIPnPort = CommandParams;
		
		return true;
	}
	
	return false;
}

bool FWorkerFindMatch::FormatIPv4StringToNumerics(FString& IP, TArray<uint8>& Output)
{
	CleanseIPOfInvalidCharacters(IP);
	
	TArray<FString> SplitString;
	IP.ParseIntoArray( SplitString, TEXT(".") );

	Output.Empty();
	for ( int32 i = 0; i < SplitString.Num(); ++i )
	{
		Output.Add( FCString::Atoi( *SplitString[i] ) );
	}
	
	return IsConsideredAValidIP(Output);
}

#pragma endregion FWorkerFindMatch