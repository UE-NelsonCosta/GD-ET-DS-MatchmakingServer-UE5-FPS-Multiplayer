#pragma once

#include "Messages.h"
#include <vector>
#include <iostream>

#pragma region MessageParsing

void ParseLoginMessageParameters(const char* ReadBuffer, std::vector<std::string>& ParsedResults)
{
    std::string Message = ReadBuffer;

    // Note: Workaround to avoid the C26451 Warning
    size_t Offset = 1;
    size_t FirstDelimiter = Message.find('|', 0);
    size_t SecondDelimiter = Message.find('|', FirstDelimiter + Offset);

    // Time To Chop Up The Message!
    ParsedResults.push_back(Message.substr(0, FirstDelimiter));
    ParsedResults.push_back(Message.substr(FirstDelimiter + Offset, SecondDelimiter - FirstDelimiter - Offset));
    ParsedResults.push_back(Message.substr(SecondDelimiter + Offset, Message.length() - SecondDelimiter));
}

void ParseRequestGamemode(const char* ReadBuffer, std::vector<std::string>& ParsedResults)
{
    std::string Message = ReadBuffer;

    // Note: Workaround to avoid the C26451 Warning
    size_t Offset = 1;
    size_t FirstDelimiter = Message.find('|', 0);

    // Time To Chop Up The Message!
    ParsedResults.push_back(Message.substr(0, FirstDelimiter));
    ParsedResults.push_back(Message.substr(FirstDelimiter + Offset, Message.length() - FirstDelimiter));
}

EClientMessageType ConvertMessageHeaderToClientMessageType(const char* ReadBuffer)
{
    std::string BufferReaderHelper = ReadBuffer;

    if (BufferReaderHelper.find("LGN") == 0)
    {
        return EClientMessageType::LGN;
    }

    if (BufferReaderHelper.find("CAK") == 0)
    {
        return EClientMessageType::CAK;
    }

    if (BufferReaderHelper.find("RGM") == 0)
    {
        return EClientMessageType::RGM;
    }

    return EClientMessageType::UKN;
}

EClientMessageType ParseMessage(const char* ReadBuffer, std::vector<std::string>& ParsedResults)
{
    EClientMessageType ConvertedMessageType = ConvertMessageHeaderToClientMessageType(ReadBuffer);
    switch (ConvertedMessageType)
    {
    case EClientMessageType::LGN:
    {
        // LGN | Username | Password - Login Request
        ParseLoginMessageParameters(ReadBuffer, ParsedResults);
        return EClientMessageType::LGN;
    }
    case EClientMessageType::CAK:
    {
        // CAK - Server Aknowleding Client Message
        return EClientMessageType::CAK;
    }

    case EClientMessageType::RGM:
    {
        // RGM | GameModeID - Gamemode Request
        ParseRequestGamemode(ReadBuffer, ParsedResults);
        return EClientMessageType::RGM;
    }
    default:
        return EClientMessageType::UKN;
    }
}

#pragma endregion MessageParsing