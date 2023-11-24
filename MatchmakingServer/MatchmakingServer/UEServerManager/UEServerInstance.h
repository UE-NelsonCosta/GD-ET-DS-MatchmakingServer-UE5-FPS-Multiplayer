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

	UEServerInstance();

	int GetInstanceID();

	void GetIP(std::string& IP);
	void GetPort(std::string& Port);

	EServerInstanceState GetServerInstanceState();
	void SetServerInstanceState(EServerInstanceState NewState);

private:

	int InstanceID;

	std::string UEServerIP	 = "127.0.0.1";
	std::string UEServerPort = "1337";

	EServerInstanceState ServerInstanceState = EServerInstanceState::NONE;

private: // Static ID Trakcing Nonsense
	static int InstanceIDTracker;

};

