#include "UE5ServerManager.h"
#include <CommandlineParser/CommandlineParameterParser.h>

std::weak_ptr<UEServerInstance> UEServerManager::ReserveServerInstance()
{
    // Note: We Move The Constructed Shared Pointer (This is a way to optimize how we create and pass responsibiltiy to other objects)
    //       This Then Implicitly Converts The Shared Pointer Into A Weak One
    return ServerInstances.emplace_back(std::move(std::make_shared<UEServerInstance>()));
}

bool UEServerManager::RunServer(std::weak_ptr<UEServerInstance> ServerInstance)
{
    std::shared_ptr<UEServerInstance> SharedServerInstance = ServerInstance.lock();
    if(!SharedServerInstance)
        return false;

    // Create A Server Instance Using One Of Our Batch Files
    // First Parameter Is The Map To Load Into (If No Override Specified, Then Just Let The Project Defaults Do It's Thing)
    // Note: Building this the lazy way so I can Explain the "Step by Step Of THe Console Command" However this is a TERRIBLE way to do it due to all the allocations that happen
    std::string ProjectBasePath = "\"";
    CommandlineParameterParser::Instance().GetArgumentWithKey("ProjectBasePath", ProjectBasePath);
    
    std::string BatchFilePath = "\"";
    BatchFilePath += ProjectBasePath;
    BatchFilePath += "LaunchTestUE5Server.bat\"";
    
    std::string LaunchParameter;
    LaunchParameter += "start";                                     // Console Command To Fire And Forget A Process (AKA doesnt get stuck waiting for it to finish)
    LaunchParameter += " ";                                         // White Space
    LaunchParameter += "\"\"";                                      // Title For The CMD Window (Empty In This Case) AKA ""
    LaunchParameter += " /b ";                                      // White Space
    LaunchParameter += BatchFilePath;                               // The Name Of Our Preconfigured Batch File
    LaunchParameter += " ";                                         // White Space
    LaunchParameter += SharedServerInstance.get()->GetOverrideMap();// Parameter One It Expects is An Override Map (\"\" By Default)
    LaunchParameter += " ";                                         // White Space
    LaunchParameter += SharedServerInstance.get()->GetPort();       // Port To Listen In On

    system(LaunchParameter.c_str());

    // Set The UEServerInstance To The Correct State
    SharedServerInstance.get()->SetServerInstanceState(EServerInstanceState::Running);
}
