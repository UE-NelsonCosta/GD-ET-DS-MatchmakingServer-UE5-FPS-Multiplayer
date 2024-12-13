//// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "ClientConnectionData.h"
#include "ServerInstanceSubsystem.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnReceivedClientConnectionData, const TArray<FClientConnectionData>&, ExpectedClients );

UCLASS(Config="MatchFinderSubsystem")
class MATCHFINDER_API UServerInstanceSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
	
public: 
	
	virtual bool ShouldCreateSubsystem(UObject* Outer) const override;
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize();

	// Note: This is purposely a blocking call as I want nothing else to happen so that the gamemode cannot reject clients too early
	bool RequestClientDataFromMatchmakingServer();
	
	bool HasClientConnectionData() const { return ExpectedGameClientConnections.Num() > 0; }
	void GetClientConnectionData(TArray<FClientConnectionData>& ClientConnectionData) const;
	
	FOnReceivedClientConnectionData OnReceivedClientConnectionDataEvent;

private:

	uint16 ServerID;
	
	TArray<FClientConnectionData> ExpectedGameClientConnections;

	

	UPROPERTY(Config)
	FString MatchmakingServerIP = "127.0.0.1";

	UPROPERTY(Config)
	uint16  MatchmakingServerInstancePort = 3000;
};
