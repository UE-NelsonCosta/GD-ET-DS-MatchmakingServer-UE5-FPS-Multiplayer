#include "WorkerFindMatch.h"

#include "Sockets.h"
#include "SocketSubsystem.h"
#include "Interfaces/IPv4/IPv4Address.h"
#include "Interfaces/IPv4/IPv4Endpoint.h"

bool FWorkerFindMatch::Init()
{
	MatchFindingProgress = EMatchFindingProgress::CreatingConnection;

	// Ensure We're Ready To Go Again If We're Reused
	State.Empty();
	ResultIPnPort.Empty();
	ResultParameters.Empty();
	
	// Not wanting to do anything here for now, let the main thread go back to whatever it was doing.
	return FRunnable::Init();
}

void FWorkerFindMatch::Stop()
{
	CleanupSocket();
}

uint32 FWorkerFindMatch::Run()
{
	uint32 ErrorCode = 0;

	ErrorCode = InitializeConnection();
	if(ErrorCode != 0)
		return ErrorCode;

	ErrorCode = RunCommunicationChain();
	if(ErrorCode != 0)
		return ErrorCode;

	return ErrorCode;
}

void FWorkerFindMatch::Exit()
{
	CleanupSocket();
}

uint32 FWorkerFindMatch::InitializeConnection()
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

	return 0;
}

uint32 FWorkerFindMatch::RunCommunicationChain()
{
	Sleep();

	State = "Connected To Matchmaking Server ... Logging In";
	MatchFindingProgress = EMatchFindingProgress::LoggingIn;
	
	if(!SendLoginData())
	{
		State = "Failed To Login Please Try Again Later";
		MatchFindingProgress = EMatchFindingProgress::LoginFailed;

		return 4;
	}
	
	if(!ReceiveLoginReply())
	{
		State = "Failed To Login Please Try Again Later";
		MatchFindingProgress = EMatchFindingProgress::LoginFailed;

		return 4;
	}
	
	Sleep();

	State = "Login Successful! ... Requesting Game!";
	MatchFindingProgress = EMatchFindingProgress::RequestingGame;

	if(!SendGamemodeRequest())
	{
		State = "Failed To Request Gamemode Please Try Again";
		MatchFindingProgress = EMatchFindingProgress::FindingFailed;

		return 5;
	}

	if(!ReceiveGamemodeReply())
	{
		State = "Failed To Request Gamemode Please Try Again";
		MatchFindingProgress = EMatchFindingProgress::FindingFailed;

		return 5;
	}

	Sleep();
	
	State = "Request Successful! ... Finding A Game!";

	if(!ReceiveGamemodeConnection(ResultIPnPort, ResultParameters))
	{
		State = "Failed To Get Connection Message Please Try Again";
		MatchFindingProgress = EMatchFindingProgress::FindingFailed;

		return 6;
	}
	
	Sleep();

	State = "Found Gamemode! ... Connecting To Server";
	MatchFindingProgress = EMatchFindingProgress::Complete;

	return 0;
}

void FWorkerFindMatch::CleanupSocket()
{
	if(SocketToMatchmakingServer && SocketToMatchmakingServer->GetConnectionState() == ESocketConnectionState::SCS_Connected)
	{
		SocketToMatchmakingServer->Shutdown(ESocketShutdownMode::ReadWrite);
		SocketToMatchmakingServer->Close();
		SocketToMatchmakingServer = nullptr;
	}
}

#pragma region MessageChain

bool FWorkerFindMatch::SendLoginData() const
{
	// Some Faux Login Data Example, Not Actually Used ATM
	const FString Message = "LGN|Nelson|Costa123";
	const int32   MessageLength = Message.Len();

	// Ensure The Encoding Is Correct (From UTF-16 to Byte Sized ANSI for C++)
	const uint8* Payload = reinterpret_cast<uint8*>(TCHAR_TO_ANSI(*Message)); 

	// Always Check Your Pointers As Users Can Try Spam The Buttons To Get This To Hit A nullptr
	if(!SocketToMatchmakingServer)
		return false;

	// Send The Payload
	int32 BytesSent = 0;
	SocketToMatchmakingServer->Send(Payload, MessageLength, BytesSent);
		
	return true;
}

bool FWorkerFindMatch::ReceiveLoginReply()
{
	// Buffer To Read Data From
	TArray<uint8> ReadBuffer;
	ReadBuffer.AddZeroed(32);

	// Always Check Your Pointers As Users Can Try Spam The Buttons To Get This To Hit A nullptr
	if(!SocketToMatchmakingServer)
		return false;

	// Do The Actual Reading
	int32 BytesRead = 0;
	SocketToMatchmakingServer->Recv(ReadBuffer.GetData(), 32, BytesRead);

	// Convert And Let's Check That The Message Corresponds To What We Expect
	const FString ServerMessage = ANSI_TO_TCHAR( reinterpret_cast<ANSICHAR*>(ReadBuffer.GetData()) ); // Convert To UTF-16 Encoding

	FString CommandType;
	FString CommandParams;
	ServerMessage.Split(TEXT("|"), &CommandType, &CommandParams);

	if(CommandType.Find(TEXT("LGS")) == 0)
	{
		AuthToken = CommandParams;
		return true;
	}
	
	return false;
}

bool FWorkerFindMatch::SendGamemodeRequest() const
{
	// Some Faux Login Data Example, Not Actually Used ATM
	const FString Message = "RGM|0";
	const int32   MessageLength = Message.Len();

	// Ensure The Encoding Is Correct (From UTF-16 to Byte Sized ANSI for C++)
	const uint8* Payload = reinterpret_cast<uint8*>(TCHAR_TO_ANSI(*Message));

	// Always Check Your Pointers As Users Can Try Spam The Buttons To Get This To Hit A nullptr
	if(!SocketToMatchmakingServer)
		return false;

	// Send The Payload
	int32 RequestGameBytesSent = 0;
	SocketToMatchmakingServer->Send(Payload, MessageLength, RequestGameBytesSent);

	return true;
}

bool FWorkerFindMatch::ReceiveGamemodeReply() const
{
	// Buffer To Read Data From
	TArray<uint8> ReadBuffer;
	ReadBuffer.AddZeroed(32);
	
	// Always Check Your Pointers As Users Can Try Spam The Buttons To Get This To Hit A nullptr
	if(!SocketToMatchmakingServer)
		return false;

	// Do The Actual Reading
	int32 BytesRead = 0;
	SocketToMatchmakingServer->Recv(ReadBuffer.GetData(), 32, BytesRead);

	// Convert And Let's Check That The Message Corresponds To What We Expect
	const FString ServerMessage = ANSI_TO_TCHAR( reinterpret_cast<ANSICHAR*>(ReadBuffer.GetData()) ); // Convert To UTF-16 Encoding
	if(ServerMessage.Find(TEXT("RGS")) == 0)
	{
		return true;
	}
	
	return false;
}

bool FWorkerFindMatch::ReceiveGamemodeConnection(FString& OutputIPnPort, FString& OutputParameters) const
{
	// Buffer To Read Data From
	TArray<uint8> ReadBuffer;
	ReadBuffer.AddZeroed(32);
	
	// Always Check Your Pointers As Users Can Try Spam The Buttons To Get This To Hit A nullptr
	if(!SocketToMatchmakingServer)
		return false;

	// Do The Actual Reading
	int32 BytesRead = 0;
	SocketToMatchmakingServer->Recv(ReadBuffer.GetData(), 32, BytesRead);

	// Convert And Let's Check That The Message Corresponds To What We Expect
	const FString ServerMessage = ANSI_TO_TCHAR( reinterpret_cast<ANSICHAR*>(ReadBuffer.GetData()) );

	// TODO: Support Additional Connection Parameters
	FString CommandType;
	FString CommandParams;
	ServerMessage.Split(TEXT("|"), &CommandType, &CommandParams);
	
	if(CommandType.Equals( TEXT("RGC") ) )
	{
		OutputIPnPort = CommandParams;

		OutputParameters = "AuthToken="+AuthToken;
		
		return true;
	}
	
	return false;
}

#pragma endregion MessageChain

#pragma region Utilities

bool FWorkerFindMatch::CreateSocketObject(FSocket*& SocketToWriteTo)
{
	SocketToWriteTo = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->CreateSocket(NAME_Stream, "MatchMakingServer", false);
	return SocketToWriteTo != nullptr;
}

TSharedRef<FInternetAddr> FWorkerFindMatch::CreateInternetAddressToMatchmakingServer(const TArray<uint8>& ParsedIPv4) const
{
	const FIPv4Address  IPv4Address(ParsedIPv4[0], ParsedIPv4[1], ParsedIPv4[2], ParsedIPv4[3]);
	const FIPv4Endpoint IPv4Endpoint(IPv4Address, MatchmakingServerPort);
	return IPv4Endpoint.ToInternetAddrIPV4();
}

void FWorkerFindMatch::CleanseIPOfInvalidCharacters(FString& IP)
{
	FRegexPattern RegexPattern(TEXT("[^\\d.]"), ERegexPatternFlags::CaseInsensitive);
	FRegexMatcher RegexMatcher(RegexPattern, IP);

	while(RegexMatcher.FindNext())
	{
		const int32 AttributeListBegin = RegexMatcher.GetCaptureGroupBeginning(0);
		const int32 AttributeListEnd = RegexMatcher.GetCaptureGroupEnding(0);
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

bool FWorkerFindMatch::HasValidPort(uint16 Port)
{
	return Port >= 1024 && Port <= 65535;
}

void FWorkerFindMatch::Sleep()
{
	// Note: Don't Do This In Real Life! This is Just So The Widget Can Update And Show You A Step By Step In Editor Only!
#if UE_EDITOR
	FPlatformProcess::Sleep(2);
#endif
}

#pragma endregion Utilities
