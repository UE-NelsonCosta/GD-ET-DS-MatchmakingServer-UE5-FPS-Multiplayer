// Fill out your copyright notice in the Description page of Project Settings.


#include "MatchFinderSubsystem.h"

bool UMatchFinderSubsystem::ShouldCreateSubsystem(UObject* Outer) const
{
	// Dont Run This On The Server For Obvious Reasons As This Is Client Only
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
	// Collection.InitializeDependency();

	// Just here to force the file to appear if one is not there 
	LoadConfig();

	SaveConfig();
}

void UMatchFinderSubsystem::Deinitialize()
{
	Super::Deinitialize();

	// Force Save Of Config If We Change It
	SaveConfig();
}

void UMatchFinderSubsystem::RequestGame()
{
	// TODO: Thread seems to stay alive? Needs investigation
	if(WorkerThread.IsValid())
	{
		UE_LOG(LogTemp, Warning, TEXT("UMatchFinderSubsystem::RequestGame - Cannot Request A Game Whilst One Is Already being Requested!"));
		
		return;
	}

	const TSharedPtr<FWorkerFindMatch> MatchFinderJob = GetOrMakeWorkerJob();
	if(!MatchFinderJob.IsValid())
	{
		UE_LOG(LogTemp, Error, TEXT("UMatchFinderSubsystem::RequestGame - Invalid MatchFinderJob!"));

		return;
	}

	if(!RunJob(MatchFinderJob))
	{
		UE_LOG(LogTemp, Error, TEXT("UMatchFinderSubsystem::RequestGame - Failed To Create The Thread!"));

		return;
	}
	
	// Set The States And Broadcast The Start!
	MatchFinderState = EMatchFinderSubsystemState::FindingMatch;
	OnGameFindStartedEvent.Broadcast();

	ClearSlowTimer();
	RunSlowTimer();
}

void UMatchFinderSubsystem::CancelRequest()
{
	if(WorkerThread.IsValid())
	{
		// This will call stop on the threaded job
		WorkerThread->Kill(false);
		WorkerThread.Reset();
	}
	
	OnGameFindCanceledEvent.Broadcast();

	MatchFinderState = EMatchFinderSubsystemState::Idle;
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
	if(WorkerJob.IsValid())
	{
		ProcessSlowTickEvents();

		if(	WorkerJob->GetProgress() == EMatchFindingProgress::CreatingConnection ||
			WorkerJob->GetProgress() == EMatchFindingProgress::LoggingIn ||
			WorkerJob->GetProgress() == EMatchFindingProgress::RequestingGame )
			return;
	}

	GetWorld()->GetTimerManager().ClearTimer(SlowTickHandle);

	MatchFinderState = EMatchFinderSubsystemState::Idle;
}

TSharedPtr<FWorkerFindMatch> UMatchFinderSubsystem::GetOrMakeWorkerJob()
{
	if(!WorkerJob)
	{
		WorkerJob = MakeShared<FWorkerFindMatch>(MatchmakingServerIP, MatchmakingServerPort);
	}

	return WorkerJob;
}

bool UMatchFinderSubsystem::RunJob(const TSharedPtr<FWorkerFindMatch>& Job)
{
	if(!Job.IsValid()) return false;
	
	WorkerThread = MakeShareable(FRunnableThread::Create(Job.Get(), TEXT("MatchFinderThread")));

	return WorkerThread.IsValid();
}

void UMatchFinderSubsystem::RunSlowTimer()
{
	if(!TimerDelegate.IsBound())
		TimerDelegate.BindUFunction(this, "SlowTickFromTimerCallback");
	
	if(GetWorld())
		GetWorld()->GetTimerManager().SetTimer(SlowTickHandle, TimerDelegate, 0.5f, true);
}

void UMatchFinderSubsystem::ClearSlowTimer()
{
	if(GetWorld())
		GetWorld()->GetTimerManager().ClearTimer(SlowTickHandle);
}

void UMatchFinderSubsystem::ProcessSlowTickEvents() const
{
	switch(WorkerJob->GetProgress())
	{
	case EMatchFindingProgress::CreatingConnection:
	case EMatchFindingProgress::LoggingIn:
	case EMatchFindingProgress::RequestingGame: 
		{
			OnGameFindStateUpdateEvent.Broadcast(WorkerJob->GetState());

			break;
		}
		
	case EMatchFindingProgress::Complete:
		{
			OnGameFoundEvent.Broadcast(WorkerJob->GetIPnPort(), WorkerJob->GetAdditionalParameters());
			
			break;
		}

	case EMatchFindingProgress::ConnectionFailed:
	case EMatchFindingProgress::LoginFailed:
	case EMatchFindingProgress::FindingFailed:
		{
			OnMatchFindingFailedEvent.Broadcast(WorkerJob->GetState());
			
			break;
		}
	default:
		// Only Idle Would Happen To Land Here On Occasions
		break;
	}
}

//FString ConvertWorkerProgressToString(EMatchFindingProgress State)
//{
//	switch(State)
//	{
//	case EMatchFindingProgress::Idle:				return "Match Finding Not Working";
//	case EMatchFindingProgress::CreatingConnection: return "Connecting To Matchmaking Server";
//	case EMatchFindingProgress::LoggingIn:			return "Logging In To Matchmaking Server";
//	case EMatchFindingProgress::RequestingGame:		return "Request in Game From Matchmaking Server";
//	case EMatchFindingProgress::Complete:			return "Match Finding Complete!";
//	case EMatchFindingProgress::ConnectionFailed:	return "MatchFinder Failed To Connect";
//	case EMatchFindingProgress::LoginFailed:		return "MatchFinder Failed To Login";
//	case EMatchFindingProgress::FindingFailed:		return "MatchFinder Failed To Find A Game";
//	default: return "UMatchFinderSubsystem::ConvertWorkerProgressToString - State Not Setup";
//	}
//}
