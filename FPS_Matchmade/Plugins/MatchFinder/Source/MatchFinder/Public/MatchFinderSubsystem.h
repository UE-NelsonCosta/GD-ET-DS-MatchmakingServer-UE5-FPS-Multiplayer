/*
 * @file MatchFinderSubsystem.h
 * @brief
 * - Blueprint facing subsystem to request a match made game. Fires a Thread Job to communicate to the
 *   Match Making Server (located in root of project)
 * - Uses events to let the blueprints know what's going on and give feedback to the user
 * - Uses config files to allow you to quickly configure this system
 * - Defaults to loopback IP and the preconfigured match making server port (2000)
 *
 * @author Nelson Costa
 * Contact: nelson.costa@universidadeeuropeia.pt
 */

#pragma once

#include "CoreMinimal.h"
#include "WorkerFindMatch.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "MatchFinderSubsystem.generated.h"

// Event Declarations
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnGameFindStarted);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnGameFindCanceled);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnGameFound, FString, IPnPort, FString, AdditionalParams );

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnGameFindStateUpdate, FString, State);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMatchFindingFailed, FString, Reason);

// NOTE: As this contains important variables to serialize such as where to connect to and port, we keep this data saved
//		 in configuration files we can easily tweak, if changes are made in engine, they will be saved out, otherwise
//		 in editor change them in PROJECTDIR\Saved\Config\WindowsEditor\MatchFinderSubsystem.ini
//		 Worth noting this isn't a tracked file on github, so if you dont see it, just run the game once
UCLASS(Config="MatchFinderSubsystemUser")
class MATCHFINDER_API UMatchFinderSubsystem : public UGameInstanceSubsystem
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

	bool RunJob(const TSharedPtr<FWorkerFindMatch>& Job);

	void RunSlowTimer();
	void ClearSlowTimer();
	
	void ProcessSlowTickEvents() const;

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
	FOnMatchFindingFailed OnMatchFindingFailedEvent;
	
private: // Member Variables

	// Keep track of runnable Thread and it's Job
	TSharedPtr<FRunnableThread> WorkerThread;
	TSharedPtr<FWorkerFindMatch> WorkerJob;

	// This Objects State When Finding A Match Dependant On The Worker Thread State
	EMatchFinderSubsystemState MatchFinderState;

	// Timer Handle For A Slow Tick
	FTimerHandle SlowTickHandle;
	FTimerDelegate TimerDelegate;

private: // Config Variables
	
	UPROPERTY(Config)
	FString MatchmakingServerIP = "127.0.0.1";

	UPROPERTY(Config)
	uint16 MatchmakingServerPort = 2000;
};
