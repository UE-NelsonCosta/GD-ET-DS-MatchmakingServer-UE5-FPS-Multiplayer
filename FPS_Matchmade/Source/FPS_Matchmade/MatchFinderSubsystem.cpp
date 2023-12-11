// Fill out your copyright notice in the Description page of Project Settings.


#include "MatchFinderSubsystem.h"

#include "Common/TcpSocketBuilder.h"
#include "GenericPlatform/GenericPlatformProcess.h"

#pragma region MatchFinderSubsystem

bool UMatchFinderSubsystem::ShouldCreateSubsystem(UObject* Outer) const
{
	// Dont Run This On The Server For Obvious Reasons
#if UE_SERVER
	return false;
#else
	return true;
#endif
}

void UMatchFinderSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	// Example code to let UE5 know if you have other subsystems that need to initialize before this one
	//Collection.InitializeDependency();

	// Enforces The Serialization Of The Config Variables
}

void UMatchFinderSubsystem::Deinitialize()
{
	Super::Deinitialize();

	SaveConfig();
}

void UMatchFinderSubsystem::RequestGame()
{
	if(WorkerThread.IsValid())
	{
		// TODO: Report Error
	}
	
	if(!WorkerJob)
	{
		WorkerJob = MakeShared<FWorkerFindMatch>(this);
	}

	WorkerThread = MakeShareable(FRunnableThread::Create(WorkerJob.Get(), TEXT("MatchFinderThread")));

	OnConnectionToMatchmakingServerStartedEvent.Broadcast();
	
	// Create A Repeating Timer So We Can Update The UI Over Time
	FTimerDelegate TimerDelegate;
	TimerDelegate.BindUFunction(this, "SlowTickFromTimerCallback");
	GetWorld()->GetTimerManager().SetTimer(SlowTickHandle, TimerDelegate, 0.25f, true);
}

void UMatchFinderSubsystem::CancelRequest()
{
	if(!WorkerThread.IsValid()) 
		return;

	WorkerThread->Kill();
}

EMatchFinderSubsystemState UMatchFinderSubsystem::GetCurrentState() const
{
	return MatchFinderState;
}

EMatchFindingProgress UMatchFinderSubsystem::GetMatchFindingProgress() const
{
	return WorkerJob.IsValid() ? WorkerJob->GetProgress() : EMatchFindingProgress::Idle;
}

void UMatchFinderSubsystem::SlowTickFromTimerCallback()
{
	// Let's Just Update The UI
	// TODO: Time to stop rushing though and give these proper states to poll
	// if logging in is the state
	// MatchFinderSubsystem->OnConnectionToMatchmakingServerSucceededEvent.Broadcast("Success!");

	
	if(WorkerJob->GetProgress() == EMatchFindingProgress::Failed)
	{
		OnConnectingToMatchmakingServerFailedEvent.Broadcast(WorkerJob->GetState());
		GetWorld()->GetTimerManager().ClearTimer(SlowTickHandle);
		return;
	}

	if(WorkerJob->GetProgress() == EMatchFindingProgress::Failed)
	{
		OnGameFoundEvent.Broadcast(WorkerJob->GetIPnPort(), "");
		GetWorld()->GetTimerManager().ClearTimer(SlowTickHandle);
		return;
	}
}

#pragma endregion MatchFinderSubsystem

#pragma region FWorkerFindMatch

bool FWorkerFindMatch::Init()
{
	if(MatchFinderSubsystem)
	{
		MatchFinderSubsystem->MatchFinderState = EMatchFinderSubsystemState::FindingMatch;
	}
	
	// Not wanting to do anything here for now, let the main thread go back to whatever it was doing.
	return FRunnable::Init();
}

void FWorkerFindMatch::Stop()
{
	if(MatchFinderSubsystem)
	{
		MatchFinderSubsystem->MatchFinderState = EMatchFinderSubsystemState::Idle;
		MatchFinderSubsystem->OnGameFindCanceledEvent.Broadcast();
	}
}

// TODO: Use Exit Codes?
uint32 FWorkerFindMatch::Run()
{
	State = "Connecting To Matchmaking Server";
	MatchfindingProgress = EMatchFindingProgress::CreatingConnection;
	
	if(!MatchFinderSubsystem)
	{
		State = "Bad Initalization, Missing Reference To Match Finder Subsystem";
		MatchfindingProgress = EMatchFindingProgress::Failed;

		return 1;
	}
	

	TArray<uint8> ParsedIPv4;
	bool IsValidIP = FormatIPv4StringToNumerics(MatchFinderSubsystem->MatchmakingServerIP, ParsedIPv4);
	bool IsValidPort = HasValidPort(MatchFinderSubsystem->MatchmakingServerPort);
	if(!IsValidIP || !IsValidPort)
	{
		State = "Bad IP Given: Please Check Config File";
		MatchfindingProgress = EMatchFindingProgress::Failed;

		return 2;
	}
	
	if(!CreateSocketObject(SocketToMatchmakingServer))
	{
		State = "Failed To Create Socket Object";
		MatchfindingProgress = EMatchFindingProgress::Failed;

		return 3;
	}
		
	TSharedRef<FInternetAddr> MatchmakingServerAddress = CreateInternetAddressToMatchmakingServer(ParsedIPv4);	
	if (!SocketToMatchmakingServer->Connect(MatchmakingServerAddress.Get()))
	{
		State = "Failed To Connect To MatchmakingServer With Given IP";
		MatchfindingProgress = EMatchFindingProgress::Failed;

		return 4;
	}

	// Note: Don't Do This In Real Life! This is Just So The Widget Can Update And Show You A Step By Step
	FPlatformProcess::Sleep(2);

	State = "Connected To Matchmaking Server ... Logging In";
	MatchfindingProgress = EMatchFindingProgress::LoggingIn;


	SendLoginData();
	if(!ReceiveLoginReply())
	{
		State = "Failed To Login Please Try Again Later";
		MatchfindingProgress = EMatchFindingProgress::Failed;

		return 5;
	}
	
	FPlatformProcess::Sleep(2);
	State = "Login Successful! ... Requesting Game!";
	MatchfindingProgress = EMatchFindingProgress::RequestingGame;

	SendGamemodeRequest();
	if(!ReceiveGamemodeReply())
	{
		State = "Failed To Request Gamemode Please Try Again";
		MatchfindingProgress = EMatchFindingProgress::Failed;

		return 6;
	}

	FPlatformProcess::Sleep(2);
	State = "Request Successful! ... Finding A Game!";

	if(!ReceiveGamemodeConnection(IPnPort))
	{
		State = "Failed To Get Connection Message Please Try Again";
		MatchfindingProgress = EMatchFindingProgress::Failed;

		return 7;
	}
	
	// Note: Don't Do This In Real Life! This is Just So The Widget Can Update And Show You A Step By Step
	FPlatformProcess::Sleep(2);
	State = "Found Gamemode! ... Connecting To Server";
	MatchfindingProgress = EMatchFindingProgress::Completed;


	return 0;
}

void FWorkerFindMatch::Exit()
{
	//SocketToMatchmakingServer->Close();
	
	if(MatchFinderSubsystem)
		MatchFinderSubsystem->MatchFinderState = EMatchFinderSubsystemState::Idle;
}

EMatchFindingProgress FWorkerFindMatch::GetProgress() const
{
	return MatchfindingProgress;
}

bool FWorkerFindMatch::CreateSocketObject(FSocket*& SocketToWriteTo)
{
	SocketToWriteTo = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->CreateSocket(NAME_Stream, "MatchMakingServer", false);
	return SocketToWriteTo != nullptr;
}

TSharedRef<FInternetAddr> FWorkerFindMatch::CreateInternetAddressToMatchmakingServer(const TArray<uint8>& ParsedIPv4)
{
	FIPv4Address IPv4Address(ParsedIPv4[0], ParsedIPv4[1], ParsedIPv4[2], ParsedIPv4[3]);
	FIPv4Endpoint IPv4Endpoint(IPv4Address, MatchFinderSubsystem->MatchmakingServerPort);
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
	ReadBuffer.AddUninitialized(32);
	
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
	ReadBuffer.AddUninitialized(32);
	
	int32 BytesRead = 0;
	SocketToMatchmakingServer->Recv(ReadBuffer.GetData(), 32, BytesRead);

	FString ServerMessage = ANSI_TO_TCHAR( (char*)ReadBuffer.GetData() ); // Convert To UTF-16 Encoding
	if(ServerMessage.Find(TEXT("RGS")) == 0)
	{
		return true;
	}
	
	return false;
}

bool FWorkerFindMatch::ReceiveGamemodeConnection(FString& Output)
{
	TArray<uint8> ReadBuffer;
	ReadBuffer.AddUninitialized(32);
	
	int32 BytesRead = 0;
	SocketToMatchmakingServer->Recv(ReadBuffer.GetData(), 32, BytesRead);

	FString ServerMessage = ANSI_TO_TCHAR( (char*)ReadBuffer.GetData() ); // Convert To UTF-16 Encoding

	FString CommandType;
	FString CommandParams;
	ServerMessage.Split(TEXT("|"), &CommandType, &CommandParams);
	
	if(CommandType.Equals( TEXT("RGC") ) )
	{
		Output = CommandParams;
		
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
