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

std::string filepathToROM = "D:\\Emulation\\ROMs\\Gameboy\\Pokemon - Blue Version (UE)[!]\\Pokemon Blue.gb.test2";

void TestFlagInputOutput(DMG cpu);
void clear_screen(char fill = ' ');

int main()
{
	std::ifstream input(filepathToROM, std::ios::binary);
	std::vector<uint8_t> ROM(std::istreambuf_iterator<char>(input), {});

	DMG cpu = DMG(ROM);
	int programCounter = 0x100;

	//TestFlagInputOutput(cpu);


	while (true)
	{
		std::cout << std::hex << programCounter << ": ";
		std::cout << Utils::GetBinary(ROM[programCounter]) << " ";
		std::cout << std::hex << (int)ROM[programCounter] << " ";

		/*std::string instructionString;
		if (cpu.INSTRUCTION_STRINGS.find(ROM[programCounter]) != cpu.INSTRUCTION_STRINGS.end())
			instructionString = cpu.INSTRUCTION_STRINGS.at(ROM[programCounter]);*/

		//std::cout << instructionString << "\n";

		cpu.ProcessInstruction(ROM[programCounter], &programCounter, &ROM);
		//cpu.DisplayRegisters();

		std::cout << "\n";

		if (_getch() == 13) break; // enter key
	}

}

//std::vector<long> GenerateTestInstructions()
//{
//	std::vector<long> instructions;
//	instructions.push_back(DMG::LD_r_n(Register::H, 0xAB));
//	instructions.push_back(DMG::LD_HL_n(0x50));
//	return instructions;
//}

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
