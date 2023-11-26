// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "MatchmadeInstanceSubsystem.generated.h"

/**
 * 
 */
UCLASS()
class FPS_MATCHMADE_API UMatchmadeInstanceSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:

	virtual bool ShouldCreateSubsystem(UObject* Outer) const override;

	/** Implement this for initialization of instances of the system */
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	/** Implement this for deinitialization of instances of the system */
	virtual void Deinitialize() {}

	/** Overridden to check global network context */
	virtual int32 GetFunctionCallspace(UFunction* Function, FFrame* Stack) override;
};
