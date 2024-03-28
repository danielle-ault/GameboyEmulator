#pragma once

//#include <iostream>
#include <string>
//#include <fstream>
//#include <iterator>
//#include <vector>
//#include <conio.h>
//#include <Windows.h>
//#include <chrono>
//#include <thread>

//#include "Window.h"
#include "DMG.h"
//#include "Utils.h"
//#include "Tile.h"

std::string filepathToROM = "D:/Creative/Programming/c++/GameBoyEmulator/gbassembly/hello world/hello-world-scroll.gb";
//std::string filepathToROM = "D:\\Emulation\\ROMs\\Gameboy\\Pokemon - Blue Version (UE)[!]\\Pokemon Blue.gb";
//std::string filepathToROM = "D:/Creative/Programming/c++/GameBoyEmulator/gbassembly/test instructions/test_instructions.gb";

void TestFlagInputOutput(DMG cpu);
void ConsoleCursorToXY(short x, short y);
void clear_screen(char fill = ' ');
void ProcessNumInstructions(DMG* cpu, int numInstructions, bool updateDisplay = true);
void GoUpNumInstructions(int numInstructions);
void GoDownNumInstructions(int numInstructions);

void ResizeCallback(GLFWwindow* window, int width, int height);
void KeypressCallback(GLFWwindow* window, int key, int scancode, int action, int mods);

void RunTests();

enum DebugViewMode { ViewCPU, ViewRAM };
enum DebugViewMode DebugViewMode = ViewCPU;
u16 RAMDisplayStartAddress = 0xFF40;

int FirstVisibleInstruction = 0;
int FirstVisibleMemoryOperation = 0;
bool SimulationPaused;

int Scale = 3;

DMG* CPU;

class Window Window = class Window(Display::SCREEN_WIDTH * Scale,
	Display::SCREEN_HEIGHT * Scale,
	"Gameboy Emulator",
	ResizeCallback,
	KeypressCallback);