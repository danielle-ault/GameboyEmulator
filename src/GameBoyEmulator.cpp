// GameBoyEmulator.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
//#include <string>
#include <fstream>
#include <iterator>
#include <vector>
#include <conio.h>
#include <Windows.h>

//#include <glad/glad.h>
//#include <GLFW/glfw3.h>

#include "Window.h"
#include "DMG.h"
#include "Utils.h"

std::string filepathToROM = "D:/Creative/Programming/c++/GameBoyEmulator/gbassembly/hello world/hello-world.gb";//"D:\\Emulation\\ROMs\\Gameboy\\Pokemon - Blue Version (UE)[!]\\Pokemon Blue.gb.test";
//std::string filepathToROM = "D:/Creative/Programming/c++/GameBoyEmulator/gbassembly/test instructions/test_instructions.gb";

void TestFlagInputOutput(DMG cpu);
void ConsoleCursorToXY(short x, short y);
void clear_screen(char fill = ' ');
void ProcessNumInstructions(DMG *cpu, int numInstructions);

void ResizeCallback(GLFWwindow* window, int width, int height);
void KeypressCallback(GLFWwindow* window, int key, int scancode, int action, int mods);

int Scale = 2;

DMG* CPU;

int main()
{
	Window window(Display::SCREEN_WIDTH * Scale,
		          Display::SCREEN_HEIGHT * Scale,
		          "Gameboy Emulator",
		          ResizeCallback,
		          KeypressCallback);

	std::ifstream input(filepathToROM, std::ios::binary);
	std::vector<uint8_t> ROM(std::istreambuf_iterator<char>(input), {});

	DMG cpu_temp = DMG(ROM);
	CPU = &cpu_temp;
	CPU->ProgramCounter = 0x100;
	//int programCounter = 0x100;

	//TestFlagInputOutput(cpu);

	CONSOLE_SCREEN_BUFFER_INFO csbi;
	int columns, rows;

	GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
	columns = csbi.srWindow.Right - csbi.srWindow.Left + 1;
	rows = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;

	CPU->Display.InitGraphics(Scale);
	
	while (!window.ShouldClose())
	{
		CPU->Display.DrawGraphics();
		window.SwapBuffersAndPollEvents();

		//char input = (char)_getch();

		//switch (input)
		//{
		//case 13: goto exit_loop; // enter key
		//case 32: ProcessNumInstructions(CPU, 1); break; // spacebar
		//case 'S':
		//case 's': ProcessNumInstructions(CPU, 10); break; 
		//}
	}
//exit_loop:;

	CPU->Display.Cleanup();
	window.Close();
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

void ProcessNumInstructions(DMG *cpu, int numInstructions)
{
	for (int i = 0; i < numInstructions; i++)
		cpu->ProcessNextInstruction();

	cpu->DisplayStateInfo();
}

void ResizeCallback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

void KeypressCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_ENTER && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	if (key == GLFW_KEY_SPACE && action == GLFW_PRESS)
		ProcessNumInstructions(CPU, 1);
	
	if (key == GLFW_KEY_S && action == GLFW_PRESS)
		ProcessNumInstructions(CPU, 20);
}

//std::vector<long> GenerateTestInstructions()
//{
//	std::vector<long> instructions;
//	instructions.push_back(DMG::LD_r_n(Register::H, 0xAB));
//	instructions.push_back(DMG::LD_HL_n(0x50));
//	return instructions;
//}





