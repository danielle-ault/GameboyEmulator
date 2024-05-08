// GameBoyEmulator.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
//#include <string>
#include <fstream>
#include <iterator>
#include <vector>
#include <conio.h>
#include <Windows.h>
#include <chrono>
#include <thread>

//#include <glad/glad.h>
//#include <GLFW/glfw3.h>

#include "GameBoyEmulator.h"
#include "Window.h"
#include "DMG.h"
#include "Utils.h"
#include "Tile.h"


int main()
{
	//filepathToROM = "D:/Creative/Programming/c++/GameBoyEmulator/gbassembly/hello world/hello-world-scroll.gb";
	//filepathToROM = "D:/Creative/Programming/c++/GameBoyEmulator/blargg-tests/cpu_instrs.gb";
	//filepathToROM = "D:\\Emulation\\ROMs\\Gameboy\\Pokemon - Blue Version (UE)[!]\\Pokemon Blue.gb";
	//filepathToROM = "D:/Creative/Programming/c++/GameBoyEmulator/gbassembly/test instructions/test_instructions.gb";
	//filepathToROM = Utils::OpenFileDialog("D:/Creative/Programming/c++/GameBoyEmulator/");
	filepathToROM = "D:/Creative/Programming/c++/GameBoyEmulator/blargg-tests/cpu_instrs/individual/06-ld r,r.gb";
	
	//RunTests();
	Utils::DebugViewMode = ViewConsole;

	std::ifstream input(filepathToROM, std::ios::binary);
	std::vector<uint8_t> ROM(std::istreambuf_iterator<char>(input), {});

	DMG cpu_temp = DMG(ROM);
	CPU = &cpu_temp;
	CPU->RAMDisplayStartAddress = RAMDisplayStartAddress;
	CPU->ProgramCounter = 0x100;

	CONSOLE_SCREEN_BUFFER_INFO csbi;
	int columns, rows;

	GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
	columns = csbi.srWindow.Right - csbi.srWindow.Left + 1;
	rows = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;

	CPU->Display.Window = &Window;
	CPU->Display.InitGraphics(Scale);
	CPU->Display.DrawGraphics();
	Window.SwapBuffersAndPollEvents();
	
	size_t i = 0;
	while (!Window.ShouldClose())
	{
		//SimulationPaused = CPU->SimulationPaused;
		//auto start = std::chrono::system_clock::now();

		if (!CPU->SimulationPaused)
		{
			CPU->ProcessNextInstruction(false);
			bool updateDisplay = i % 1000 == 0;
			if (updateDisplay)
			{
				switch (Utils::DebugViewMode)
				{
				case DebugViewMode::ViewCPU:
					CPU->DisplayStateInfo();
					break;
				case DebugViewMode::ViewRAM:
					CPU->DisplayRAMInfo();
					break;
				}
			}
		}

		// basically a breakpoint
		if (CPU->ProgramCounter == 0xC7F9 && CPU->SimulationPaused == false)
		{
			CPU->SimulationPaused = true;
			CPU->DisplayStateInfo();
		}
		
		if (i % 1000 == 0)
			glfwPollEvents();

		i++;
	}
//exit_loop:;

	CPU->Display.Cleanup();
	Window.Close();
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

void ProcessNumInstructions(DMG *cpu, int numInstructions, bool updateDisplay)
{
	for (int i = 0; i < numInstructions; i++)
		cpu->ProcessNextInstruction();

	FirstVisibleInstruction = 0;
	FirstVisibleMemoryOperation = 0;
	cpu->DisplayStateInfo();
}

void GoUpNumInstructions(int numInstructions)
{
	FirstVisibleInstruction -= numInstructions;
	if (FirstVisibleInstruction < 0)
		FirstVisibleInstruction = 0;
	CPU->DisplayStateInfo(FirstVisibleInstruction, FirstVisibleMemoryOperation);
}

void GoDownNumInstructions(int numInstructions)
{
	FirstVisibleInstruction += numInstructions;
	CPU->DisplayStateInfo(FirstVisibleInstruction, FirstVisibleMemoryOperation);
}

void ResizeCallback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

void KeypressCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (action == GLFW_PRESS)
	{
		switch (key)
		{
		case GLFW_KEY_P: CPU->SimulationPaused = !CPU->SimulationPaused; break;
		case GLFW_KEY_ENTER: glfwSetWindowShouldClose(window, true); break;
		case GLFW_KEY_T:
			CPU->Display.DrawTestScreen();
			CPU->Display.DrawGraphics();
			Window.SwapBuffers();
			break;
		case GLFW_KEY_M: SwitchMode(); break;
		case GLFW_KEY_R: Utils::DebugViewMode = ViewRAM; CPU->DisplayRAMInfo(); break;
		case GLFW_KEY_C: Utils::DebugViewMode = ViewCPU; CPU->DisplayStateInfo(); break;
		}
	}

	if (action == GLFW_PRESS && Utils::DebugViewMode == DebugViewMode::ViewCPU)
	{
		switch (key)
		{
		case GLFW_KEY_BACKSPACE: ResetSimulation(); break;
		case GLFW_KEY_SPACE: ProcessNumInstructions(CPU, 1); break;
		case GLFW_KEY_S: ProcessNumInstructions(CPU, 200); break;
		case GLFW_KEY_DOWN: GoDownNumInstructions(1); break;
		case GLFW_KEY_UP: GoUpNumInstructions(1); break;
		case GLFW_KEY_PAGE_DOWN: GoDownNumInstructions(28); break;
		case GLFW_KEY_PAGE_UP: GoUpNumInstructions(28); break;
		case GLFW_KEY_RIGHT:
			FirstVisibleMemoryOperation++;
			CPU->DisplayStateInfo(FirstVisibleInstruction, FirstVisibleMemoryOperation);
			break;
		case GLFW_KEY_LEFT:
			FirstVisibleMemoryOperation--;
			if (FirstVisibleMemoryOperation < 0)
				FirstVisibleMemoryOperation = 0;
			CPU->DisplayStateInfo(FirstVisibleInstruction, FirstVisibleMemoryOperation);
			break;
		}
	}

	if ((action == GLFW_PRESS || action == GLFW_REPEAT) && Utils::DebugViewMode == DebugViewMode::ViewRAM)
	{
		switch (key)
		{
		case GLFW_KEY_DOWN:
			CPU->RAMDisplayStartAddress += CPU->RAMDisplayBytesPerLine;
			CPU->DisplayRAMInfo();
			break;
		case GLFW_KEY_UP:
			CPU->RAMDisplayStartAddress -= CPU->RAMDisplayBytesPerLine;
			if (CPU->RAMDisplayStartAddress < 0)
				CPU->RAMDisplayStartAddress = 0;
			CPU->DisplayRAMInfo();
			break;

		case GLFW_KEY_PAGE_DOWN:
			CPU->RAMDisplayStartAddress += CPU->RAMDisplayBytesPerLine * 0x100;
			CPU->DisplayRAMInfo();
			break;
		case GLFW_KEY_PAGE_UP:
			if (CPU->RAMDisplayStartAddress < CPU->RAMDisplayBytesPerLine * 0x100)
				CPU->RAMDisplayStartAddress = 0;
			else
				CPU->RAMDisplayStartAddress -= CPU->RAMDisplayBytesPerLine * 0x100;
			CPU->DisplayRAMInfo();
			break;
		}
	}
}

void SwitchMode()
{
	switch (Utils::DebugViewMode)
	{
	case ViewCPU: 
		Utils::DebugViewMode = ViewRAM;
		CPU->DisplayRAMInfo();
		break;
	case ViewRAM:
		Utils::DebugViewMode = ViewConsole;
		Utils::ClearConsole();
		break;
	case ViewConsole:
		Utils::DebugViewMode = ViewCPU;
		CPU->DisplayStateInfo();
		break;
	}
}

void ResetSimulation()
{
	DebugViewMode viewMode = Utils::DebugViewMode;

	std::ifstream input(filepathToROM, std::ios::binary);
	std::vector<uint8_t> ROM(std::istreambuf_iterator<char>(input), {});

	DMG cpu_temp = DMG(ROM);
	CPU = &cpu_temp;

	CPU->SimulationPaused = false;

	Utils::DebugViewMode = viewMode;
}

void RunTests()
{
	std::vector<u8> tileBytesRaw = { 0x3c, 0x7e,
									 0x42, 0x42,
									 0x42, 0x42,
									 0x42, 0x42,
									 0x7e, 0x5e,
									 0x7e, 0x0a,
									 0x7c, 0x56,
									 0x38, 0x7c };

	Tile tile = Tile(tileBytesRaw);
}

//std::vector<long> GenerateTestInstructions()
//{
//	std::vector<long> instructions;
//	instructions.push_back(DMG::LD_r_n(Register::H, 0xAB));
//	instructions.push_back(DMG::LD_HL_n(0x50));
//	return instructions;
//}





