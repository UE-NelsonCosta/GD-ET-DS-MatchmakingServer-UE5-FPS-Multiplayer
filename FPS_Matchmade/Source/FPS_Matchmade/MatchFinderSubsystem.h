#pragma once

#include "CoreMinimal.h"
#include "Interfaces/IPv4/IPv4Endpoint.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "MatchFinderSubsystem.generated.h"

// TODO: Move This To A Separate Header File
// Auxiliary Enumerations
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
	
	Complete,

	ConnectionFailed,
	LoginFailed,
	FindingFailed,
};

// Event Declarations
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnGameFindStarted);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnGameFindCanceled);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnGameFound, FString, IPnPort, FString, AdditionalParams );

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnGameFindStateUpdate, FString, State);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMatchFindingFailed, FString, Reason);

// Forward Declarations
class FWorkerFindMatch;

// NOTE: As this contains important variables to serialize such as where to connect to and port, we keep this data saved
//		 in configuration files we can easily tweak, if changes are made in engine, they will be saved out, otherwise
//		 in editor change them in PROJECTDIR\Saved\Config\WindowsEditor\MatchFinderSubsystem.ini
//		 Worth noting this isn't a tracked file on github, so if you dont see it, just run the game once
UCLASS(Config="MatchFinderSubsystem")
class FPS_MATCHMADE_API UMatchFinderSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

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

private: // Private Functions

	UFUNCTION()
	void SlowTickFromTimerCallback();

	TSharedPtr<FWorkerFindMatch> GetOrMakeWorkerJob();

public: // Events
	
	UPROPERTY(BlueprintAssignable)
	FOnGameFindStarted OnGameFindStartedEvent;
	
	UPROPERTY(BlueprintAssignable)
	FOnGameFindCanceled OnGameFindCanceledEvent;

	UPROPERTY(BlueprintAssignable)
	FOnGameFound OnGameFoundEvent;

	UPROPERTY(BlueprintAssignable)
	FOnGameFindStateUpdate OnGameFindStateUpdateEvent;

	UPROPERTY(BlueprintAssignable)
	FOnMatchFindingFailed OnOnMatchFindingFailedEvent;
	
private:

	TSharedPtr<FRunnableThread> WorkerThread;
	TSharedPtr<FWorkerFindMatch> WorkerJob;
	
	EMatchFinderSubsystemState MatchFinderState;

	// TODO: If these are ever exposed in the game, please make sure to update the worker
	UPROPERTY(Config)
	FString MatchmakingServerIP = "127.0.0.1";

	UPROPERTY(Config)
	uint16 MatchmakingServerPort = 2000;

	FTimerHandle SlowTickHandle;
};

//https://store.algosyntax.com/tutorials/unreal-engine/ue5-multithreading-with-frunnable-and-thread-workflow/
class FWorkerFindMatch final : public FRunnable
{
public:

	FWorkerFindMatch(const FString& MMServerIP, const uint16 MMServerPort)
		: MatchmakingServerIP(MMServerIP)
		, MatchmakingServerPort(MMServerPort)
		, MatchFindingProgress(EMatchFindingProgress::Idle)
		, State("")
		, ResultIPnPort("")
		, ResultParameters("")
	{}

	// Called From Caller Thread (This Case Main Thread)
	virtual bool Init() override;
	virtual void Stop() override;

	// Called From Threaded Job
	virtual uint32 Run() override;
	virtual void Exit() override;
	
	// Get A Copy Of A Result Or Progress
	EMatchFindingProgress GetProgress() const;

	FString GetState()					{ return State;				}
	FString GetIPnPort()				{ return ResultIPnPort;		}
	FString GetAdditionalParameters()	{ return ResultParameters;	}
	
private: // Internal Functions

	bool CreateSocketObject(FSocket*& SocketToWriteTo);
	TSharedRef<FInternetAddr> CreateInternetAddressToMatchmakingServer(const TArray<uint8>& ParsedIPv4);
	
	bool FormatIPv4StringToNumerics(FString& IP, TArray<uint8>& Output);

	void CleanseIPOfInvalidCharacters(FString& IP);

	bool IsConsideredAValidIP(const TArray<uint8>& IPValues);
	bool HasValidPort( uint16 Port );

private: // SocketMessagingChain

	void SendLoginData();
	bool ReceiveLoginReply();

	void SendGamemodeRequest();
	bool ReceiveGamemodeReply();

	bool ReceiveGamemodeConnection(FString& OutputIPnPort, FString& OutputParameters);
	
private: // Variables

	// IP And Port To Matchmaking Server
	FString MatchmakingServerIP = "127.0.0.1";
	uint16  MatchmakingServerPort = 2000;
	FSocket* SocketToMatchmakingServer = nullptr;

	EMatchFindingProgress MatchFindingProgress;

	FString State;
	FString ResultIPnPort;
	FString ResultParameters;
};
