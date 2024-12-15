// Fill out your copyright notice in the Description page of Project Settings.


#include "IngameDeathmatchGamemode.h"

//#include "ServerInstanceSubsystem.h"
#include "SelectablePlayerCharacterData.h"
#include "GameFramework/Character.h"
#include "MatchFinder/Public/ServerInstanceSubsystem.h"
#include "Kismet/GameplayStatics.h"


void AIngameDeathmatchGamemode::BeginPlay()
{
	Super::BeginPlay();

	UWorld* World = GetWorld();
	if(!World)
	{
		// TODO: Log Error
		return;
	}

	// Resolve The Soft Reference object first then load it yourself if it fails
	SelectablePlayerCharacterDataTable = Cast<UDataTable>(DataTablePath.ResolveObject());
	if(!SelectablePlayerCharacterDataTable)
	{
		// Unless you done goofed the asset path this should work
		SelectablePlayerCharacterDataTable = Cast<UDataTable>(DataTablePath.TryLoad());
	}

	// Disabled For Now
	// UGameplayStatics::GetAllActorsWithTag(GetWorld(), "BlueTeamSpawn", BlueTeamSpawns);
	// UGameplayStatics::GetAllActorsWithTag(GetWorld(), "RedTeamSpawn", RedTeamSpawns);

	UServerInstanceSubsystem* ServerInstanceSubsystem = World->GetGameInstance()->GetSubsystem<UServerInstanceSubsystem>();
	if(!ServerInstanceSubsystem)
	{
		// TODO: Log Error
		return;
	}

	ServerInstanceSubsystem->RequestClientDataFromMatchmakingServer();
}

void AIngameDeathmatchGamemode::StartPlay()
{
	Super::StartPlay();

	UWorld* World = GetWorld();
	UGameInstance* GameInstance = GetGameInstance();
	if(!GameInstance || !World)
	{
		return;
	}
	
	//UServerInstanceSubsystem* MatchMadeInstanceSubsystem = GameInstance->GetSubsystem<UServerInstanceSubsystem>();
	//if(MatchMadeInstanceSubsystem)
	//{
	//	if(!ServerInstanceEventDelegate.IsBound())
	//	{
	//		ServerInstanceEventDelegate.BindUFunction(this , "OnReceivedClientConnectionData");
	//	}
	//	MatchMadeInstanceSubsystem->OnReceivedClientConnectionDataEvent.AddUnique(ServerInstanceEventDelegate);
	//	
	//	MatchMadeInstanceSubsystem->RequestClientDataFromMatchmakingServer();
	//}
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

	UE_LOG(LogTemp, Warning, TEXT("Connection Options = %s"), *Options );
	
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

		UE_LOG(LogTemp, Error, TEXT("AuthValue = %s"), *AuthValue );
		UE_LOG(LogTemp, Error, TEXT("Options = %s"), *Options );
		

		// Dump Expected Values
		DumpClientAuthTokens();
	}

	if(!IsClientAuthTokenValid(AuthValue))
	{
		TStringBuilder<256> StringBuilder;
		StringBuilder.Append("Client With Auth Token: ");
		StringBuilder.Append(AuthValue);
		StringBuilder.Append(" - Tried To Connect With Invalid Token!");

		UE_LOG(LogTemp, Error, TEXT("%s"), StringBuilder.ToString() );

		DumpClientAuthTokens();
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

	FString InAuthToken = UGameplayStatics::ParseOption(Options, TEXT("AuthToken")).Left(20);
	if(!IsClientAuthTokenValid(InAuthToken))
	{
		// TODO: Return Some Error Message of Invalid Login Token For This Session
		return nullptr;
	}
	
	return PlayerController;
}

bool AIngameDeathmatchGamemode::IsClientAuthTokenValid(const FString& InAuthToken) const
{
	UServerInstanceSubsystem* ServerInstanceSubsystem = GetWorld()->GetGameInstance()->GetSubsystem<UServerInstanceSubsystem>();
	if(ServerInstanceSubsystem && ServerInstanceSubsystem->HasClientConnectionData())
	{
		TArray<FClientConnectionData> ClientConnectionData;
		ServerInstanceSubsystem->GetClientConnectionData(ClientConnectionData);
		
		for(int i = 0; i < ClientConnectionData.Num(); ++i)
		{
			if(ClientConnectionData[i].ClientLoginToken == InAuthToken)
			{
				return true;
			}
		}	
	}

	return false;
}

void AIngameDeathmatchGamemode::DumpClientAuthTokens() const
{
	UE_LOG(LogTemp, Error, TEXT("Dumping AuthTokens:") );

	UServerInstanceSubsystem* ServerInstanceSubsystem = GetWorld()->GetGameInstance()->GetSubsystem<UServerInstanceSubsystem>();
	if(ServerInstanceSubsystem && ServerInstanceSubsystem->HasClientConnectionData())
	{
		TArray<FClientConnectionData> ClientConnectionData;
		ServerInstanceSubsystem->GetClientConnectionData(ClientConnectionData);
		
		for(int i = 0; i < ClientConnectionData.Num(); ++i)
		{
			UE_LOG(LogTemp, Error, TEXT("%s"), *ClientConnectionData[i].ClientLoginToken.ToString() );
		}
	}
}


// Dont really want to do anything special here
void AIngameDeathmatchGamemode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	// Let's Spawn The Character For The Player
	// It's not implemented yet but this data can be sent across and given to the server to handle as he sees fit
	// For now we just alternate

	static const FName MannyTableID = "Manny";
	static const FName QuinnTableID = "Quinn";

	static int LastSpawnedCharacter = 0;
	
	FSelectablePlayerCharacterData* SelectedCharacterRow = SelectablePlayerCharacterDataTable->FindRow<FSelectablePlayerCharacterData>(LastSpawnedCharacter++ % 2 == 0 ? MannyTableID : QuinnTableID, "");
	if(!SelectedCharacterRow)
	{
		// Log Some Error
		return;
	}

	UClass* CharacterClassToSpawn = SelectedCharacterRow->CharacterSoftClassPointer.LoadSynchronous();

	// This exists as a default implementation, We could get our own find whats in the scene with tags etc.
	// Second parameter is to use a tag to find a specific type of player start such as red or blue teams
	const AActor* SelectedPlayerStart = FindPlayerStart(NewPlayer, "");

	const FVector PlayerLocation = SelectedPlayerStart->GetActorLocation();
	const FRotator PlayerRotation = SelectedPlayerStart->GetActorRotation();
	
	FActorSpawnParameters SpawnParameters;
	SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	ACharacter* SpawnedCharacter = Cast<ACharacter>(GetWorld()->SpawnActor(CharacterClassToSpawn, &PlayerLocation, &PlayerRotation, SpawnParameters));
	if(!SpawnedCharacter)
	{
		// Some Error Message;
		return;
	}

	NewPlayer->Possess(SpawnedCharacter);
	SpawnedCharacter->SetOwner(NewPlayer);
}

ECharacterType AIngameDeathmatchGamemode::ParseCharacterOptionToEnum(const FString& CharacterOption)
{
	//if(CharacterValue)
	return ECharacterType::Invalid;
}

//void AIngameDeathmatchGamemode::OnReceivedClientConnectionData(const TArray<FClientConnectionData>& ExpectedClients)
//{
//	ClientConnectionData = ExpectedClients;
//}
