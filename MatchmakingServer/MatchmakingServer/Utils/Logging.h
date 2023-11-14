#pragma once
#include <iostream>

int ParseLastWSAErrorToString(std::string& ErrorOutput);

void LogErrorToConsole(std::string& VerboseErrorOutput);

int LogWSAErrorToConsole();

void LogMessage(const char* Message);
