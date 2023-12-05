// Fill out your copyright notice in the Description page of Project Settings.


#include "IngameDeathmatchGamemode.h"

void AIngameDeathmatchGamemode::BeginPlay()
{
	Super::BeginPlay();
}

void AIngameDeathmatchGamemode::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
}

void AIngameDeathmatchGamemode::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
}

void AIngameDeathmatchGamemode::PreLogin(const FString& Options, const FString& Address,
	const FUniqueNetIdRepl& UniqueId, FString& ErrorMessage)
{
	Super::PreLogin(Options, Address, UniqueId, ErrorMessage);

	// Check The Login Tokens
}

APlayerController* AIngameDeathmatchGamemode::Login(UPlayer* NewPlayer, ENetRole InRemoteRole, const FString& Portal,
	const FString& Options, const FUniqueNetIdRepl& UniqueId, FString& ErrorMessage)
{
	return Super::Login(NewPlayer, InRemoteRole, Portal, Options, UniqueId, ErrorMessage);
}

void AIngameDeathmatchGamemode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);
}
