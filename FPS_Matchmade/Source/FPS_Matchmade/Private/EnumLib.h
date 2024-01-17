#pragma once

// Note: This could just be a gigantic list of characters
UENUM(BlueprintType)
enum class ECharacterType : uint8
{
	Invalid,
	Manny,
	Quinn,
};

static ECharacterType GetCharacterFromString(const FString& CharacterID)
{
	if(CharacterID.Compare("Manny") == 0)
		return ECharacterType::Manny;

	if(CharacterID.Compare("Quinn") == 0)
		return ECharacterType::Quinn;

	return ECharacterType::Invalid;
}
