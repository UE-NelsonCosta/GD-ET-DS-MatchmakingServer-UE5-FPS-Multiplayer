 #include <UEServerManager/UEServerInstance.h>
 #include <CommandlineParser/CommandlineParameterParser.h>
#include <ProjectMacros.h>
#include <Utils/NetworkingUtilities.h>

int UEServerInstance::InstanceIDTracker = UEServerPortMin;

UEServerInstance::UEServerInstance(std::string OverrideMap)
	: InstanceID(InstanceIDTracker++)
	, OverrideMap(OverrideMap)
{
	CommandlineParameterParser& CommandlineParameters = CommandlineParameterParser::Instance();
	
	// Note: Normally You'd Know What Machine And IP You Can Connect To, However For Presentation Sake We Use This To 
	//		 Override Connections To Make It Easier
	//       Worth noting as well that Ports are dynamically allocated from Min and Max (declared in ProjectMacros.h)
	std::string Value;
	if(CommandlineParameters.GetArgumentWithKey("UEServerIP", Value) && NetworkingUtilities::CanStringBeConsideredAnIPv4Address(Value))
	{
		UEServerIP = Value;
	}

	// Note: For Simplicity Sake, We Just Say That An InstanceID Is The Same As It's Port, Technically This Should Be A
	//		 longlong type so we can have a gajillion different ID's for each game we play. Helps keep track of what games
	//		 did what and who was in them etc for data collection
	UEServerPort = std::to_string(InstanceID);

	ServerInstanceState = EServerInstanceState::Reserved;
}

int UEServerInstance::GetInstanceID()
{
	return InstanceID;
}

std::string UEServerInstance::GetIP()
{
	return UEServerIP;
}

std::string UEServerInstance::GetPort()
{
	return UEServerPort;
}

std::string UEServerInstance::GetOverrideMap()
{
	return OverrideMap;
}

EServerInstanceState UEServerInstance::GetServerInstanceState()
{
	return ServerInstanceState;
}

void UEServerInstance::SetServerInstanceState(EServerInstanceState NewState)
{
	ServerInstanceState = NewState;
}