#pragma once

#include "CoreMinimal.h"
#include "SelectablePlayerCharacterData.generated.h"

USTRUCT(BlueprintType)
struct MATCHFINDER_API FSelectablePlayerCharacterData : public FTableRowBase
{
	GENERATED_BODY()

	// When Doing This Of This Sort You Should Normally Use Soft References To Avoid LOADING ABSOLUTELY EVERYTHING
	// related to this actor, some actors can be massive and if you have to load 100 of them it can chug. So load
	// it when you need it
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(RowType="SelectablePlayerCharacterData"))
	TSoftClassPtr<ACharacter> CharacterSoftClassPointer;
};