#pragma once
/**
* 
* This is here mostly as a piece of documentation so we know and can show how to parse messages
* 
* Uses a | as a delimiter end of a string is delimited with the null terminator character '\n'
* 
* Simple Messages
* UKN						- Unknown/Misunderstood Message
* 
* SAK|Positive/Negative		- Server Aknowleding Client Message
* CAK|Positive/Negative		- Client Aknowleding Server Message
* 
* LGN|Username|Password		- Login Request
* LGS						- Login Result Successful
* LGF						- Login Result Failed
* 
* RGM|GameModeID			- Gamemode Request
* RGS						- Gamemode Request Succeded (Just waiting on players)
* RGF|IsRetrying 			- Gamemode Request Failed
* 
* RGC|IP|Port
* 
**/

enum class EServerMessageType : int
{
	UKN = -1, // Unknown/Bad Message
	SAK = 0, // Server Aknowledgement

	LGS = 3, // Successful Login Message
	LGF = 4, // Unsuccesful Login Message

	RGS = 6, // Successful Request for Gamemode
	RGF = 7, // Failed Request for Gamemode

	RGC = 8, // Requested Gamemode Connect To Message
};

enum class EClientMessageType : int
{
	UKN = -1, // Unknown/Bad Message
	CAK = 1, // Client Aknowledgement

	LGN = 2, // Login Message

	RGM = 5, // Request Gamemode
};