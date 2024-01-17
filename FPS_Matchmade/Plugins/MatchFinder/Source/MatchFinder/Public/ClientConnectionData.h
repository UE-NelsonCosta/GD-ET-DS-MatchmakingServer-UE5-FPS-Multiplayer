#pragma once

#include "CoreMinimal.h"
#include "ClientConnectionData.generated.h"

USTRUCT(BlueprintType)
struct FClientConnectionData
{
	GENERATED_BODY()

	FClientConnectionData() = default;
	
	FClientConnectionData(FName InClientName, FName InClientLoginToken)
		: ClientName(InClientName)
		, ClientLoginToken(InClientLoginToken)
	{}
	
	FName ClientName;
	FName ClientLoginToken;
};
