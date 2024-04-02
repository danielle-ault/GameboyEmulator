#include <iostream>
#include <iomanip>
#include <sstream>
#include <Windows.h>

#include "DMG.h"
#include "Utils.h"

DebugViewMode Utils::DebugViewMode = ViewConsole;

// started from https://stackoverflow.com/questions/22746429/c-decimal-to-binary-converting
// expanded to make output prettier
std::string Utils::GetBinary(long number, bool padWithZeros)
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

	if (!padWithZeros) return binary;

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

std::string Utils::GetHexString(u8 number, bool showPaddingZeros)
{
	using namespace std;
	int width = 2;
	stringstream stream;
	
	if (showPaddingZeros)
		stream << setfill('0') << setw(width) << right;
	stream << hex << (u16)number;
	return stream.str();
}

std::string Utils::GetHexString(u16 number, bool showPaddingZeros)
{
	using namespace std;
	int width = 4;
	stringstream stream;
	
	if (showPaddingZeros)
		stream << setfill('0') << setw(width) << right;
	stream << hex << number;
	return stream.str();
}

// from https://stackoverflow.com/a/74869525
void Utils::gotoxy(int x, int y)
{
	// not working consistently on windows. maybe works better on windows
	//std::cout << "\033[" << (y + 1) << ";" << (x + 1) << "H"; 

	// This solution vaguely from https://stackoverflow.com/a/28911818
	HANDLE hConsoleOutput = GetStdHandle(STD_OUTPUT_HANDLE);
	COORD position;
	position.X = x;
	position.Y = y;
	SetConsoleCursorPosition(hConsoleOutput, position);
}

void Utils::ConsoleTextAtLocation(int x, int y, std::string text)
{
	gotoxy(x, y);
	std::cout << text;
}

void Utils::ConsoleTextAtLocation(int x, int y, u16 text)
{
	gotoxy(x, y);
	std::cout << text;
}

void Utils::ConsoleTextAtLocation(int x, int y, u8 text)
{
	gotoxy(x, y);
	std::cout << text;
}

void Utils::ClearConsole()
{
	std::string emptyScreenWidth;
	for (int i = 0; i < GetConsoleWidth(); i++)
		emptyScreenWidth += " ";

	Utils::gotoxy(0, 0);
	for (int i = 0; i < GetConsoleHeight(); i++)
		std::cout << emptyScreenWidth;

	Utils::gotoxy(0, 0);
}

int Utils::GetConsoleWidth()
{
	CONSOLE_SCREEN_BUFFER_INFO csbi;
	GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
	int width = csbi.srWindow.Right - csbi.srWindow.Left + 1;
	return width;
}

int Utils::GetConsoleHeight()
{
	CONSOLE_SCREEN_BUFFER_INFO csbi;
	GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
	int height = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;
	return height;
}

void Utils::DrawHorizontalLineOnConsole(int startX, int startY, int length)
{
	for (int i = 0; i < length; i++)
	{
		gotoxy(startX + i, startY);
		printf("-");
	}
}

void Utils::DrawVerticalLineOnConsole(int startX, int startY, int length)
{
	for (int i = 0; i < length; i++)
	{
		gotoxy(startX, startY + i);
		printf("|");
	}
}

void Utils::DebugPrint(std::string text)
{
	if (DebugViewMode == ViewConsole)
		std::cout << text;
}

void Utils::DebugPrint(std::string text, int var)
{
	if (DebugViewMode == ViewConsole)
		std::cout << text << ": " << var;
}

void Utils::DebugPrintLine(std::string text)
{
	if (DebugViewMode == ViewConsole)
		std::cout << text << std::endl;
}

void Utils::DebugPrintLine(std::string text, int var)
{
	if (DebugViewMode == ViewConsole)
		std::cout << text << ": " << var << std::endl;
}


