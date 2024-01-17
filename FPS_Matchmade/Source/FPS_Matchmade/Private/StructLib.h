// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "EnumLib.h"
#include "StructLib.generated.h"

#pragma region DataTable Structures

USTRUCT(BlueprintType)
struct FCharacterRowList : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly) ECharacterType CharacterType = ECharacterType::Invalid;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly) TSoftClassPtr<ACharacter> SoftCharacterRef = nullptr;
};

#pragma endregion DataTable Structures

#pragma region Gamemode Structures

USTRUCT(BlueprintType)
struct FPlayerConnectionData
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly) FString PlayerName					= "";
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly) FString CharacterID					= "";
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly) int32 PlayerTeamID					= -1;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly) APlayerController* PlayerController	= nullptr;
};

#pragma endregion Gamemode Structures