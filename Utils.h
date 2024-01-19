#pragma once

#include <string>

class Utils
{
public:
	static std::string GetBinary(long number);
	static std::string ReplaceSubstring(std::string string, std::string toReplace, std::string replaceWith);
};

