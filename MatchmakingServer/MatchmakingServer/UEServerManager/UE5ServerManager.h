#pragma once

#include <vector>
#include <memory>
#include <UEServerManager/UEServerInstance.h>
#include <Utils/ASingleton.h>

// Class that requests UE5 Engine Instances and Reserves An Engine For GameSession
class UEServerManager : public ASingleton<UEServerManager>
{
public:

	// TODO: Set State To Reserved
	// Note: This Should Be A 1 to 1 relation with a game session.
	std::weak_ptr<UEServerInstance> ReserveServerInstance();

	// TODO: Set State To Running
	bool RunServer(std::weak_ptr<UEServerInstance> ServerInstance);

	// TODO: Then Set To Completed When Your Server Tells You It's Done
	//		 Then Again Never Use It Again For This Version


private:

	std::vector<std::shared_ptr<UEServerInstance>> ServerInstances;
};

