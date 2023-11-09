#pragma once
/**
* 
* This is here mostly as a piece of documentation so we know and can show how to parse messages
* 
* Uses a | as a delimiter end of a string is delimited with the null terminator character '\n'
* 
* Simple Messages
* SAK						- Server Aknowleding Client Message
* CAK						- Client Aknowleding Server Message
* UKN						- Unknown/Misunderstood Message
* 
* LGN|Username|Password		- Login Request
* LGS						- Login Result Successful
* LGF						- Login Result Failed
* 
* RGM|GameModeID			- Gamemode Request
* RGS|IP|Port				- Gamemode Request Succeded
* RGF|IsRetrying 			- Gamemode Request Failed
* 
**/