#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "Types.h"
#include "Color.h"
#include "Shader.h"
#include "Tile.h"
#include "TileMap.h"
#include "Window.h"

struct ObjectSpriteInfo
{
	u8 Y;
	u8 X;
	u8 TileIndex;
	u8 Attributes;
};

const int OBJ_ATTR_PRIORITY_BIT = 0b1000'0000;
const int OBJ_ATTR_Y_FLIP_BIT = 0b0100'0000;
const int OBJ_ATTR_X_FLIP_BIT = 0b0010'0000;
const int OBJ_ATTR_DMG_PALETTE_BIT = 0b0001'0000;
const int OBJ_ATTR_BANK_BIT = 0b0000'1000;
const int OBJ_ATTR_CGB_PALETTE_BITS = 0b0000'0111;

class Display
{
public:
	static const int SCREEN_WIDTH = 160;
	static const int SCREEN_HEIGHT = 144;
	static const int VERTICAL_BLANKING_PERIOD = 10;
	static const int OAM_SCAN_DOTS = 80;
	static const int DRAWING_OAM_PENALTY_DEFAULT = 12;
	static const int MIN_HORIZONTAL_BLANKING_PERIOD = 87;
	static const int MAX_HORIZONTAL_BLANKING_PERIOD = 204;

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
	unsigned int IndexBufferID;
	unsigned int TextureID;
	class Shader Shader;
	
	float Positions[3*4] = {
		-1.0f, -1.0f, 0.0f,
		 1.0f, -1.0f, 0.0f,
		 1.0f,  1.0f, 0.0f,
		-1.0f,  1.0f, 0.0f
	};

	Color VertexColors[4] = {
		Color(255, 255, 255),
		Color(255, 255, 255),
		Color(255, 255, 255),
		Color(255, 255, 255)
	};
	
	
	//float Vertices[4*(3+3+2)];// [(sizeof(Colors) / sizeof(Colors[0])) * 3 * 2] = { 0.0f };
	
	float Vertices[4*8] = {
		// positions          // colors           // texture coords
		 1.0f,  1.0f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f, // top right
		 1.0f, -1.0f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f, // bottom right
		-1.0f, -1.0f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f, // bottom left
		-1.0f,  1.0f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f  // top left 
	};

	unsigned short Indices[6] = {
		0, 2, 1,   0, 3, 2
	};
	
	float TexCoords[8] = {
		0.0f, 0.0f,
		1.0f, 0.0f,
		1.0f, 1.0f,
		0.0f, 1.0f
	};
	
	Color* PixelColors;

	class Window* Window;

	Tile* TileBank1[128] = { NULL };// new Tile * [128];
	Tile* TileBank2[128] = { NULL };// new Tile * [128];
	Tile* TileBank3[128] = { NULL };// new Tile* [128];
	Tile** TileBanks[3] = { TileBank1, TileBank2, TileBank3 };
	u8 CurrentTileBank = 2; // TODO: this is not how the gameboy handles it at all

	TileMap* TileMap1 = NULL;
	TileMap* TileMap2 = NULL;
	TileMap** TileMaps = new TileMap * [2];
	u8 CurrentTileMap = 0; // TODO: this is not how the gameboy handles it, I think

	Display() {}
	Display(u8* memory);

	// Graphics Member Functions
	void InitGraphics(int scale);
	void DrawGraphics();
	void GenerateVertexAttributes();
	std::vector<float> GetTextureData(Color* colors);
	void InitPixels();
	void DrawTestScreen();
	void DrawTestTiles();
	void Cleanup();

	void DrawNextPixel(u8* memory);
	u16 GetCurrentPixelX();
	u8 GetCurrentPixelY();

	void DrawTile(Tile* tile, u8 x, u8 y);
	void DrawTileMap(TileMap* tileMap, u8 scrollX, u8 scrollY);

	void SetColor(u8 index, u8 colorValue);

//private:
	u16 CurrentPixelX = 0;
	u8 CurrentPixelY = 0;

	void DrawPixel(u8 x, u8 y, float r, float g, float b);
	void DrawPixel(u8 x, u8 y, Color color);
};

