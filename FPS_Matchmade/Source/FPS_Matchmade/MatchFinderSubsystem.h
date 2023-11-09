#pragma once

#include "CoreMinimal.h"
#include "Interfaces/IPv4/IPv4Endpoint.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "MatchFinderSubsystem.generated.h"

UENUM(BlueprintType)
enum class EMatchFinderSubsystemState : uint8
{
	Idle,
	FindingMatch
};

UENUM(BlueprintType)
enum class EMatchFindingProgress : uint8
{
	Idle,
	CreatingConnection,
	LoggingIn,
	RequestingGame,
};

class FWorkerFindMatch;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnConnectingToMatchmakingServerStarted);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnConnectingToMatchmakingServerSucceeded, FString&, Reason);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnConnectingToMatchmakingServerFailed, FString&, Reason);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnGameFound, FString&, IPnPort, FString&, AdditionalParams );
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnGameFindCanceled);

// NOTE: As this contains important variables to serialize such as where to connect to and port, we keep this data saved
//		 in configuration files we can easily tweak, if changes are made in engine, they will be saved out, otherwise
//		 in editor change them in PROJECTDIR\Saved\Config\WindowsEditor\MatchFinderSubsystem.ini
//		 Worth noting this isn't a tracked file on github, so if you dont see it, just run the game once
UCLASS(Config="MatchFinderSubsystem")
class FPS_MATCHMADE_API UMatchFinderSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

	// Break OOP For An Exception
	friend class FWorkerFindMatch;
	
public: // Public Functions

	virtual bool ShouldCreateSubsystem(UObject* Outer) const override;

	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	virtual void Deinitialize() override;
	
	UFUNCTION(BlueprintCallable)
	void RequestGame();

	UFUNCTION(BlueprintCallable)
	void CancelRequest();

	UFUNCTION(BlueprintPure)
	EMatchFinderSubsystemState GetCurrentState() const;

	UFUNCTION(BlueprintPure)
	EMatchFindingProgress GetMatchFindingProgress() const;

public: // Events

	FOnConnectingToMatchmakingServerStarted OnConnectionToMatchmakingServerStartedEvent;
	
	FOnConnectingToMatchmakingServerSucceeded OnConnectionToMatchmakingServerSucceededEvent;
	FOnConnectingToMatchmakingServerFailed OnConnectingToMatchmakingServerFailedEvent;

	FOnGameFound OnGameFoundEvent;

	FOnGameFindCanceled OnGameFindCanceledEvent;
	
private:

	TSharedPtr<FRunnableThread> WorkerThread;
	TSharedPtr<FWorkerFindMatch> WorkerJob;
	
	EMatchFinderSubsystemState MatchFinderState;

	// TODO: If these are ever exposed in the game, please make sure to update the worker
	UPROPERTY(Config)
	FString MatchmakingServerIP = "127.0.0.1";

	UPROPERTY(Config)
	uint16 MatchmakingServerPort = 42069;
};

//https://store.algosyntax.com/tutorials/unreal-engine/ue5-multithreading-with-frunnable-and-thread-workflow/
class FWorkerFindMatch final : public FRunnable
{
public:

	FWorkerFindMatch(UMatchFinderSubsystem* Subsystem)
		: MatchFinderSubsystem(Subsystem)
	{}

	// Called From Caller Thread (This Case Main Thread)
	virtual bool Init() override;
	virtual void Stop() override;

	// Called From Threaded Job
	virtual uint32 Run() override;
	virtual void Exit() override;

	EMatchFindingProgress GetProgress() const;
	
private: // Internal Functions

	bool CreateSocketObject(FSocket*& SocketToWriteTo);
	TSharedRef<FInternetAddr> CreateInternetAddressToMatchmakingServer(const TArray<uint8>& ParsedIPv4);
	
	bool FormatIPv4StringToNumerics(FString& IP, TArray<uint8>& Output);

	void CleanseIPOfInvalidCharacters(FString& IP);

	bool IsConsideredAValidIP(const TArray<uint8>& IPValues);
	bool HasValidPort( uint16 Port );
private: // Variables

	// Create A 
	FSocket* SocketToMatchmakingServer = nullptr;
	
	EMatchFindingProgress MatchfindingProgress;

	UMatchFinderSubsystem* MatchFinderSubsystem;
	
};
