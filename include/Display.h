#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "Types.h"
#include "Color.h"
#include "Shader.h"

class Display
{
public:
	static const int SCREEN_WIDTH = 160;
	static const int SCREEN_HEIGHT = 144;
	static const int VERTICAL_BLANKING_PERIOD = 10;

	// Defined in Display.cpp
	static Color COLOR_0;
	static Color COLOR_1;
	static Color COLOR_2;
	static Color COLOR_3;
	static Color COLORS[];

	//TODO: move graphics stuff somewhere else?
	// Graphics Member Variables
	unsigned int VertexArrayID;
	unsigned int VertexBufferID;
	class Shader Shader;
	float* Positions;// [SCREEN_WIDTH * SCREEN_HEIGHT * 3] = { 0.0f };
	Color* Colors;// [SCREEN_WIDTH * SCREEN_HEIGHT] ;
	float* Vertices;// [(sizeof(Colors) / sizeof(Colors[0])) * 3 * 2] = { 0.0f };

	// Graphics Member Functions
	void InitGraphics(int scale);
	void DrawGraphics();
	void InitPixels();
	void DrawTestScreen();
	void Cleanup();

	void DrawNextPixel();
	u8 GetCurrentPixelX();
	u8 GetCurrentPixelY();

//private:
	u8 CurrentPixelX = 0;
	u8 CurrentPixelY = 0;

	void DrawPixel(u8 x, u8 y, float r, float g, float b);
	void DrawPixel(u8 x, u8 y, Color color);
};

