// Fill out your copyright notice in the Description page of Project Settings.


#include "MatchFinderSubsystem.h"

void UMatchFinderSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	// Example code to let UE5 know if you have other subsystems that need to initialize before this one
	//Collection.InitializeDependency();
}

void UMatchFinderSubsystem::Deinitialize()
{
	Super::Deinitialize();
}

void UMatchFinderSubsystem::RequestGame()
{
}

void UMatchFinderSubsystem::CancelRequest()
{
}

void UMatchFinderSubsystem::GetCurrentRequestState()
{
}
