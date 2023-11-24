 #include <UEServerManager/UEServerInstance.h>
 #include <CommandlineParser/CommandlineParameterParser.h>
#include <ProjectMacros.h>

int UEServerInstance::InstanceIDTracker = UEServerPortMin;

UEServerInstance::UEServerInstance()
	: InstanceID(++InstanceIDTracker)
{
	CommandlineParameterParser& CommandlineParameters = CommandlineParameterParser::Instance();
	
	std::string Value;
	CommandlineParameters.GetArgumentWithKey("UEServerIP", Value);

	// TODO: Confirm Validity Of UEServerIP
	UEServerIP = Value;
}

int UEServerInstance::GetInstanceID()
{
	return InstanceID;
}

void UEServerInstance::GetIP(std::string& IP)
{
	IP = UEServerIP;
}

void UEServerInstance::GetPort(std::string& Port)
{
	Port = UEServerPort;
}

EServerInstanceState UEServerInstance::GetServerInstanceState()
{
	return ServerInstanceState;
}

void UEServerInstance::SetServerInstanceState(EServerInstanceState NewState)
{
	ServerInstanceState = NewState;
}