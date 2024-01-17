#pragma once

#include "MM_EnumLib.h"

//https://store.algosyntax.com/tutorials/unreal-engine/ue5-multithreading-with-frunnable-and-thread-workflow/
class FWorkerFindMatch final : public FRunnable
{
public:

	FWorkerFindMatch(const FString& MMServerIP, const uint16 MMServerPort)
		: MatchmakingServerIP(MMServerIP)
		, MatchmakingServerPort(MMServerPort)
		, MatchFindingProgress(EMatchFindingProgress::Idle)
		, State("")
		, ResultIPnPort("")
		, ResultParameters("")
	{}

	// Called From Caller Thread (This Case Main Thread)
	virtual bool Init() override;
	virtual void Stop() override;

	// Called From Threaded Job
	virtual uint32 Run() override;
	virtual void Exit() override;
	
	// Get A Copy Of A Result Or Progress
	EMatchFindingProgress GetProgress() const;

	FString GetState()					{ return State;				}
	FString GetIPnPort()				{ return ResultIPnPort;		}
	FString GetAdditionalParameters()	{ return ResultParameters;	}
	
private: // Internal Functions

	bool CreateSocketObject(FSocket*& SocketToWriteTo);
	TSharedRef<FInternetAddr> CreateInternetAddressToMatchmakingServer(const TArray<uint8>& ParsedIPv4);
	
	bool FormatIPv4StringToNumerics(FString& IP, TArray<uint8>& Output);

	void CleanseIPOfInvalidCharacters(FString& IP);

	bool IsConsideredAValidIP(const TArray<uint8>& IPValues);
	bool HasValidPort( uint16 Port );

private: // SocketMessagingChain

	void SendLoginData();
	bool ReceiveLoginReply();

	void SendGamemodeRequest();
	bool ReceiveGamemodeReply();

	bool ReceiveGamemodeConnection(FString& OutputIPnPort, FString& OutputParameters);
	
private: // Variables

	// IP And Port To Matchmaking Server
	FString MatchmakingServerIP = "127.0.0.1";
	uint16  MatchmakingServerPort = 2000;
	class FSocket* SocketToMatchmakingServer = nullptr;

	EMatchFindingProgress MatchFindingProgress;

	FString State;
	FString ResultIPnPort;
	FString ResultParameters;
};