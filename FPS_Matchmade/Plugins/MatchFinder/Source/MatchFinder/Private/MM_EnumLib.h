#pragma once

UENUM(BlueprintType)
enum class EMatchFinderSubsystemState : uint8
{
	Idle,
	FindingMatch
};

UENUM(BlueprintType)
enum class EMatchFindingProgress : uint8
{
	Idle,
	CreatingConnection,
	LoggingIn,
	RequestingGame,
	
	Complete,

	ConnectionFailed,
	LoginFailed,
	FindingFailed,
};