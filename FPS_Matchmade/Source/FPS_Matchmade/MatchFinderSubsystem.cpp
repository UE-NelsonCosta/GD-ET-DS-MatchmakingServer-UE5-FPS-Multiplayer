// Fill out your copyright notice in the Description page of Project Settings.


#include "MatchFinderSubsystem.h"

#include "Common/TcpSocketBuilder.h"

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
	if(!WorkerJob)
	{
		WorkerJob = MakeShared<FWorkerFindMatch>(this);
	}

	WorkerThread = MakeShareable(FRunnableThread::Create(WorkerJob.Get(), TEXT("MatchFinderThread")));
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

uint32 FWorkerFindMatch::Run()
{
	if(!MatchFinderSubsystem)
		return 1;
	
	MatchFinderSubsystem->OnConnectionToMatchmakingServerStartedEvent.Broadcast();

	TArray<uint8> ParsedIPv4;
	bool IsValidIP = FormatIPv4StringToNumerics(MatchFinderSubsystem->MatchmakingServerIP, ParsedIPv4);
	bool IsValidPort = HasValidPort(MatchFinderSubsystem->MatchmakingServerPort);
	if(!IsValidIP || !IsValidPort)
		return 2;
	
	if(!CreateSocketObject(SocketToMatchmakingServer))
		return 3;

	TSharedRef<FInternetAddr> MatchmakingServerAddress = CreateInternetAddressToMatchmakingServer(ParsedIPv4);	
	if (!SocketToMatchmakingServer->Connect(MatchmakingServerAddress.Get())){}
	//{
		// Handle connection failure
	//	return 1;
	//}





	
	
	FOnConnectingToMatchmakingServerSucceeded OnConnectionToMatchmakingServerSucceededEvent;
	FOnConnectingToMatchmakingServerFailed OnConnectingToMatchmakingServerFailedEvent;

	FOnGameFound OnGameFoundEvent;

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