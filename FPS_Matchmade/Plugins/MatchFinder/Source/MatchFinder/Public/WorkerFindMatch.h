/*
 * @file WorkerFindMatch.h
 * @brief
 * - CPP Only, Works To Find A Match With The MM Server, Currently It Has To Succeed From Begining To End Otherwise
 *   Just Cuts The Connection
 *   TODO: Allow This Worker To Recover From Possible Errors
 *
 *   For Additional Links On How Sockets Work Please Refer To
 *   https://store.algosyntax.com/tutorials/unreal-engine/ue5-multithreading-with-frunnable-and-thread-workflow/
 *
 * @author Nelson Costa
 * Contact: nelson.costa@universidadeeuropeia.pt
 */

#pragma once

#include "MM_EnumLib.h"

/* Runs Off A Chain Of Function Calls For Communication
 * Chain Order Is As Follows
 *
 * 1. Request Login
 * 2. Receive Acknowledgement
 * 3. Request A Gamemode
 * 4. Receive Gamefinding Aknowledgement
 * 5. Receive UE Server Instance IP and Port
 * 
 */
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
	FString GetState() const					{ return State;				}
	FString GetIPnPort() const					{ return ResultIPnPort;		}
	FString GetAdditionalParameters() const		{ return ResultParameters;	}
	EMatchFindingProgress GetProgress() const	{ return MatchFindingProgress; }
	
private: // Internal Functions

	uint32 InitializeConnection();
	uint32 RunCommunicationChain();

	void CleanupSocket();
	
	bool CreateSocketObject(FSocket*& SocketToWriteTo);
	TSharedRef<FInternetAddr> CreateInternetAddressToMatchmakingServer(const TArray<uint8>& ParsedIPv4) const;
	
	bool FormatIPv4StringToNumerics(FString& IP, TArray<uint8>& Output);

	void CleanseIPOfInvalidCharacters(FString& IP);

	bool IsConsideredAValidIP(const TArray<uint8>& IPValues);
	bool HasValidPort( uint16 Port );

	void Sleep();

private: // SocketMessagingChain

	bool SendLoginData() const;
	bool ReceiveLoginReply();

	bool SendGamemodeRequest() const;
	bool ReceiveGamemodeReply() const;

	bool ReceiveGamemodeConnection(FString& OutputIPnPort, FString& OutputParameters) const;
	
private: // Variables

	// IP And Port To Matchmaking Server Overriden In Constructor From MatchFinderSubsystem
	FString MatchmakingServerIP = "127.0.0.1";
	uint16  MatchmakingServerPort = 2000;
	
	class FSocket* SocketToMatchmakingServer = nullptr;

	EMatchFindingProgress MatchFindingProgress;

	FString State;
	FString ResultIPnPort;
	FString ResultParameters;

	FString AuthToken;
};