#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "MatchFinderSubsystem.generated.h"

UENUM(BlueprintType)
enum class MatchfinderState : uint8
{
	Idle,
	FindingMatch
};

class FWorkerFindMatch :FRunnable
{
	
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnConnectingToMatchmakingServer);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnMatchFound, FString, FString);

UCLASS()
class FPS_MATCHMADE_API UMatchFinderSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public: // Public Functions
	
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	virtual void Deinitialize() override;
	
	UFUNCTION(BlueprintCallable)
	void RequestGame();

	UFUNCTION(BlueprintCallable)
	void CancelRequest();

	UFUNCTION(BlueprintCallable)
	void GetCurrentState();

public: // Events

	
	
private:
	FWorkerFindMatch
	
};
