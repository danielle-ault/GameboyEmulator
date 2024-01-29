// GameBoyEmulator.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
//#include <string>
#include <fstream>
#include <iterator>
#include <vector>
#include <conio.h>
#include <Windows.h>

#include "DMG.h"
#include "Utils.h"

//std::string filepathToROM = "D:/Creative/Programming/c++/GameBoyEmulator/gbassembly/hello world/hello-world.gb";//"D:\\Emulation\\ROMs\\Gameboy\\Pokemon - Blue Version (UE)[!]\\Pokemon Blue.gb.test";
std::string filepathToROM = "D:/Creative/Programming/c++/GameBoyEmulator/gbassembly/test instructions/test_instructions.gb";

void TestFlagInputOutput(DMG cpu);
void ConsoleCursorToXY(short x, short y);
void clear_screen(char fill = ' ');
void ProcessNumInstructions(DMG *cpu, int numInstructions);

int main()
{
	std::ifstream input(filepathToROM, std::ios::binary);
	std::vector<uint8_t> ROM(std::istreambuf_iterator<char>(input), {});

	DMG cpu = DMG(ROM);
	cpu.ProgramCounter = 0x100;
	//int programCounter = 0x100;

	//TestFlagInputOutput(cpu);

	CONSOLE_SCREEN_BUFFER_INFO csbi;
	int columns, rows;

	GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
	columns = csbi.srWindow.Right - csbi.srWindow.Left + 1;
	rows = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;


	while (true)
	{
		char input = (char)_getch();

		switch (input)
		{
		case 13: goto exit_loop; // enter key
		case 32: ProcessNumInstructions(&cpu, 1); break; // spacebar
		case 'S':
		case 's': ProcessNumInstructions(&cpu, 10); break; 
		}
	}
exit_loop:;
}

void ProcessNumInstructions(DMG *cpu, int numInstructions)
{
	for (int i = 0; i < numInstructions; i++)
		cpu->ProcessNextInstruction();

	cpu->DisplayStateInfo();
}

//std::vector<long> GenerateTestInstructions()
//{
//	std::vector<long> instructions;
//	instructions.push_back(DMG::LD_r_n(Register::H, 0xAB));
//	instructions.push_back(DMG::LD_HL_n(0x50));
//	return instructions;
//}

void ConsoleCursorToXY(short x, short y)
{
	HANDLE hStdout = GetStdHandle(STD_OUTPUT_HANDLE);
	COORD position = { x, y };

	SetConsoleCursorPosition(hStdout, position);
}

void clear_screen(char fill)
{
	COORD tl = { 0,0 };
	CONSOLE_SCREEN_BUFFER_INFO s;
	HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);
	GetConsoleScreenBufferInfo(console, &s);
	DWORD written, cells = s.dwSize.X * s.dwSize.Y;
	FillConsoleOutputCharacter(console, fill, cells, tl, &written);
	FillConsoleOutputAttribute(console, s.wAttributes, cells, tl, &written);
	SetConsoleCursorPosition(console, tl);
}

void TestFlagInputOutput(DMG cpu)
{
	bool flagZ = true;
	bool flagN = false;
	bool flagH = true;
	bool flagCY = false;

	std::cout << "ZNHCY \n" << flagZ << flagN << flagH << flagCY << "\n";

	cpu.SetFlag(Flag::Z, flagZ);
	cpu.SetFlag(Flag::N, flagN);
	cpu.SetFlag(Flag::H, flagH);
	cpu.SetFlag(Flag::CY, flagCY);

	std::cout << cpu.GetFlag(Flag::Z) << cpu.GetFlag(Flag::N) << cpu.GetFlag(Flag::H) << cpu.GetFlag(Flag::CY) << "\n\n";
}
