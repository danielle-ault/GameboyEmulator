#pragma once

#include <string>

// Macros

// GETBIT, SETBIT, and CLRBIT from https://www.chiefdelphi.com/t/extracting-individual-bits-in-c/48028/4
#define GETBIT(var, bit)		(((var) & (1 << (bit))) ? 1 : 0)
#define SETBIT(var, bit)		(var |= (1 << (bit)))
#define CLRBIT(var, bit)		(var &= ~(1 << (bit)))
#define SETBITVALUE(var, bit, value)	((value) ? SETBIT((var), (bit)) : CLRBIT((var), (bit)))	

class Utils
{
public:
	static DebugViewMode DebugViewMode;

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

	static void DebugPrint(std::string text);
	static void DebugPrint(std::string text, int var);
	static void DebugPrintLine(std::string text);
	static void DebugPrintLine(std::string text, int var);
};

