// Fill out your copyright notice in the Description page of Project Settings.


#include "IngameDeathmatchGamemode.h"

#include "Kismet/GameplayStatics.h"


void AIngameDeathmatchGamemode::StartPlay()
{
	Super::StartPlay();

	UWorld* World = GetWorld();
	UGameInstance* GameInstance = GetGameInstance();
	if(!GameInstance || !World)
	{
		return;
	}
	
	UServerInstanceSubsystem* MatchMadeInstanceSubsystem = GameInstance->GetSubsystem<UServerInstanceSubsystem>();
	if(MatchMadeInstanceSubsystem)
	{
		if(!ServerInstanceEventDelegate.IsBound())
		{
			ServerInstanceEventDelegate.BindUFunction(this , "OnReceivedClientConnectionData");
		}
		MatchMadeInstanceSubsystem->OnReceivedClientConnectionDataEvent.AddUnique(ServerInstanceEventDelegate);
		
		MatchMadeInstanceSubsystem->RequestClientDataFromMatchmakingServer();
	}
}

void AIngameDeathmatchGamemode::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
}

void AIngameDeathmatchGamemode::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
}

// Check All Options To Ensure They Exist And Are Valid!
void AIngameDeathmatchGamemode::PreLogin(const FString& Options, const FString& Address, const FUniqueNetIdRepl& UniqueId, FString& ErrorMessage)
{
	Super::PreLogin(Options, Address, UniqueId, ErrorMessage);

	// Not doing this now but if you'd like additional connection that are just to spectate then you could set the option SpectatorOnly=1
	// That will handle the spectator controller in the next option and return early from here
	
	const FString AuthKey  = TEXT("AuthToken");
	const FString AuthValue = UGameplayStatics::ParseOption( Options, AuthKey );
	if(AuthValue.IsEmpty())
	{
		TStringBuilder<128> StringBuilder;
		StringBuilder.Append("Client Attempted To Connect From IP: ");
		StringBuilder.Append(Address);
		StringBuilder.Append(" - But Had No Authentication Token!");
		
		ErrorMessage += StringBuilder.ToString();
		UE_LOG(LogTemp, Error, TEXT("%s"), StringBuilder.ToString() );
	}
	
	const FString CharacterKey  = TEXT("Character");
	const FString CharacterValue = UGameplayStatics::ParseOption( Options, CharacterKey );
	if(CharacterValue.IsEmpty() || GetCharacterFromString(*CharacterValue) == ECharacterType::Invalid )
	{
		TStringBuilder<128> StringBuilder;
		StringBuilder.Append("None Or Invalid Character Option Passed In!");
		
		ErrorMessage += StringBuilder.ToString();
		UE_LOG(LogTemp, Error, TEXT("%s"), StringBuilder.ToString() );
	}
}

// Check Authentication Tokens This Shall Only Be Possible Once The 
APlayerController* AIngameDeathmatchGamemode::Login(UPlayer* NewPlayer, ENetRole InRemoteRole, const FString& Portal, const FString& Options, const FUniqueNetIdRepl& UniqueId, FString& ErrorMessage)
{
	// Super Uses GameSessions, as this will fail we will handle the controller spawning ourselves
	Super::Login(NewPlayer, InRemoteRole, Portal, Options, UniqueId, ErrorMessage);

	// In this case I'll spawn this normally, if you have a character that requires a specific controller, you could do that logic here
	APlayerController* PlayerController = SpawnPlayerController(InRemoteRole, Options);
	//APlayerStart* 

	PlayerController->SetInitialLocationAndRotation(FVector::ZeroVector, FRotator::ZeroRotator);	

	
	
	// Init player's name
	FString InName = UGameplayStatics::ParseOption(Options, TEXT("Name")).Left(20);
	ChangeName(PlayerController, InName, false);

	return PlayerController;
}

// Dont really want to do anything special here
void AIngameDeathmatchGamemode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	// Spawn Actual Character Etc, it is possible to do most of this in the previous function mind you
}

ECharacterType AIngameDeathmatchGamemode::ParseCharacterOptionToEnum(const FString& CharacterOption)
{
	//if(CharacterValue)
	return ECharacterType::Invalid;
}

void AIngameDeathmatchGamemode::OnReceivedClientConnectionData(const TArray<FClientConnectionData>& ExpectedClients)
{
	ClientConnectionData = ExpectedClients;
}
