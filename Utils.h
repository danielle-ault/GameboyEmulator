#pragma once

#include <string>

class Utils
{
public:
	static std::string GetBinary(long number, bool padWithZeros = true);
	static std::string ReplaceSubstring(std::string string, std::string toReplace, std::string replaceWith);
	static std::string GetHexString(u8 number, bool showPaddingZeros = false);
	static std::string GetHexString(u16 number, bool showPaddingZeros = false);

	// from https://stackoverflow.com/a/74869525
	static void gotoxy(int x, int y);
	static void ConsoleTextAtLocation(int x, int y, std::string text);
	static void ConsoleTextAtLocation(int x, int y, u16 text);
	static void ConsoleTextAtLocation(int x, int y, u8 text);
	static void ClearConsole();
	static int GetConsoleWidth();
	static int GetConsoleHeight();

	static void DrawHorizontalLineOnConsole(int startX, int startY, int length);
	static void DrawVerticalLineOnConsole(int startX, int startY, int length);
};

