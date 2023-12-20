// Fill out your copyright notice in the Description page of Project Settings.


#include "MatchmadeInstanceSubsystem.h"

bool UMatchmadeInstanceSubsystem::ShouldCreateSubsystem(UObject* Outer) const
{
#if UE_SERVER
	return true;
#else
	return false;
#endif
}

void UMatchmadeInstanceSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{

}
