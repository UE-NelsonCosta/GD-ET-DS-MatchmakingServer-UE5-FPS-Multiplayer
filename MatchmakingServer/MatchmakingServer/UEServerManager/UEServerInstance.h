#pragma once
#include <string>

enum EServerInstanceState
{
	NONE,
	Reserved,
	Running,
	Completed,
	MAX
};

class UEServerInstance
{
public:

	UEServerInstance(std::string OverrideMap = "\"\"");

	int GetInstanceID();

	// Returns Copies So They Dont Meddle With The Information In This Class
	std::string GetIP();
	std::string GetPort();
	std::string GetOverrideMap();

	EServerInstanceState GetServerInstanceState();
	void SetServerInstanceState(EServerInstanceState NewState);

private:

	int InstanceID;

	std::string UEServerIP	 = "127.0.0.1";
	std::string UEServerPort = "1337";

	std::string OverrideMap; 

	EServerInstanceState ServerInstanceState = EServerInstanceState::NONE;

	// Note: Might be worth keeping track of the PID

private: // Static ID Trakcing Nonsense

	static int InstanceIDTracker;

};

