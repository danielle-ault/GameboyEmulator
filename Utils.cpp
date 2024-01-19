#include "Utils.h"

// started from https://stackoverflow.com/questions/22746429/c-decimal-to-binary-converting
// expanded to make output prettier
std::string Utils::GetBinary(long number)
{
	int size = sizeof(number) * 8;
	std::string binary;

	int i = 0;
	while (number != 0)
	{
		binary = (number % 2 == 0 ? "0" : "1") + binary;
		number /= 2;
		i++;

		if (i % 8 == 0)
			binary = " " + binary;
	}

	while (binary.length() < size + 4 - 1)
	{
		binary = "0" + binary;
		i++;

		if (i % 8 == 0 && i != size)
			binary = " " + binary;
	}

	binary = ReplaceSubstring(binary, "00000000 ", "");
	binary = ReplaceSubstring(binary, "00000000 ", "");
	binary = ReplaceSubstring(binary, "00000000 ", "");

	return binary;
}

std::string Utils::ReplaceSubstring(std::string string, std::string toReplace, std::string replaceWith)
{
	if (string.find(toReplace) == -1) return string;

	return string.replace(string.find(toReplace), toReplace.length(), replaceWith);
}