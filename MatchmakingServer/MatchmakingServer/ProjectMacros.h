#pragma once

#pragma region MacroConfigurations

// Defines
#define MaxSimultaneousConnections 128
#define MaxPendingConnections 64  
#define MessageBufferSize 64

// We Define The Supported Minimum and Maximum For Sockets
// https://www.ibm.com/docs/en/ztpf/2021?topic=overview-port-numbers
#define MinimumSupportedSocketPort 1024
#define MaximumSupportedSocketPort 10000

// Socket To Listen To Clients
#define ClientListenSocketPort 2000
// Socket To Listen To UE Servers And When They Are Done
#define UEServerListenSocketPort 3000

// Min And Max Ports For UEServerInstances
#define UEServerPortMin 10000
#define UEServerPortMax 65535

#pragma endregion MacroConfigurations

#pragma region MacroValidation

// MacroDataValidation
#if (MinimumSupportedSocketPort < 1024 && MinimumSupportedSocketPort > 65535)
#error MinimumSupportedSocketPort Outside of Supported Range
#endif

#if (MaximumSupportedSocketPort < 1024 && MaximumSupportedSocketPort > 65535)
#error MinimumSupportedSocketPort Outside of Supported Range
#endif

#if MinimumSupportedSocketPort > MaximumSupportedSocketPort
#error Configuration Ill Formed, Please Ensure MinimumSupportedSocketPort < MaximumSupportedSocketPort
#endif

#if ( ClientListenSocketPort < MinimumSupportedSocketPort && ClientListenSocketPort > MaximumSupportedSocketPort )
#error Configuration Ill Formed, Please Ensure ClientListenSocket is In The MinimumSupportedSocketPort and MaximumSupportedSocketPort Macro Range
#endif

#pragma endregion MacroValidation
