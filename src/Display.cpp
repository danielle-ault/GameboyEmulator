#include <vector>

#include "Display.h"
#include "Tile.h"
#include "TileMap.h"
#include "Utils.h"

Color Display::COLOR_0 = Color(155 + 20, 188 + 20, 15 + 20);
Color Display::COLOR_1 = Color(139, 172, 15);
Color Display::COLOR_2 = Color(48, 98, 48);
Color Display::COLOR_3 = Color(15, 56, 15);
Color Display::COLORS[] = { COLOR_3, COLOR_2, COLOR_1, COLOR_0 };

void Display::InitGraphics(int scale)
{
	Positions = new float[SCREEN_WIDTH * SCREEN_HEIGHT * 3];
	Colors = new Color[SCREEN_WIDTH * SCREEN_HEIGHT];
	Vertices = new float[SCREEN_WIDTH * SCREEN_HEIGHT * 3 * 2];
	//Positions = new float[4 * 3];
	//Colors = new Color[4];
	//Vertices = new float[4 * 3 * 2];

	InitPixels();
	//DrawTestScreen();
	DrawTestTiles();

	glGenVertexArrays(1, &VertexArrayID);
	glBindVertexArray(VertexArrayID);

	// Generate Vertex Attributes
	int numVertices = SCREEN_HEIGHT * SCREEN_WIDTH;
	std::vector<float> vertexAttributes;

	for (int i = 0; i < numVertices; i++)
	{
		int iPos = i * 3;
		vertexAttributes.push_back(Positions[iPos + 0]);
		vertexAttributes.push_back(Positions[iPos + 1]);
		vertexAttributes.push_back(Positions[iPos + 2]);
		vertexAttributes.push_back(Colors[i].r);
		vertexAttributes.push_back(Colors[i].g);
		vertexAttributes.push_back(Colors[i].b);
	}

	std::copy(vertexAttributes.begin(), vertexAttributes.end(), Vertices);

	// Vertex Buffer
	glGenBuffers(1, &VertexBufferID);
	glBindBuffer(GL_ARRAY_BUFFER, VertexBufferID);
	glBufferData(GL_ARRAY_BUFFER, 
		         SCREEN_WIDTH * SCREEN_HEIGHT * 3 * 2 * sizeof(Vertices[0]), 
		         Vertices, 
		         GL_DYNAMIC_DRAW);

	// position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	// color attribute
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	glPointSize(scale);

	Shader = class Shader("shaders/shader.vert", "shaders/shader.frag");
}

void Display::DrawGraphics()
{
	//glClear(GL_COLOR_BUFFER_BIT);

	// Generate Vertex Attributes
	// TODO: make this its own function, it's currently duplicated here and in InitGraphics
	int numVertices = SCREEN_HEIGHT * SCREEN_WIDTH;
	std::vector<float> vertexAttributes;
	for (int i = 0; i < numVertices; i++)
	{
		int iPos = i * 3;
		vertexAttributes.push_back(Positions[iPos + 0]);
		vertexAttributes.push_back(Positions[iPos + 1]);
		vertexAttributes.push_back(Positions[iPos + 2]);
		vertexAttributes.push_back(Colors[i].r);
		vertexAttributes.push_back(Colors[i].g);
		vertexAttributes.push_back(Colors[i].b);
	}
	std::copy(vertexAttributes.begin(), vertexAttributes.end(), Vertices);

	glBindBuffer(GL_ARRAY_BUFFER, VertexBufferID);
	glBufferData(GL_ARRAY_BUFFER, SCREEN_WIDTH * SCREEN_HEIGHT * 3 * 2 * sizeof(Vertices[0]), Vertices, GL_DYNAMIC_DRAW);
	//glBufferSubData(GL_ARRAY_BUFFER, 0, SCREEN_WIDTH * SCREEN_HEIGHT * 3 * 2 * sizeof(Vertices[0]), Vertices);

	//// position attribute
	//glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	//glEnableVertexAttribArray(0);

	//// color attribute
	//glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	//glEnableVertexAttribArray(1);

	Shader.Use();

	glDrawArrays(GL_POINTS, 0, SCREEN_WIDTH * SCREEN_HEIGHT * 6);
}

void Display::InitPixels()
{
	for (int x = 0; x < SCREEN_WIDTH; x++)
	{
		for (int y = 0; y < SCREEN_HEIGHT; y++)
		{
			int i = y * SCREEN_WIDTH + x;
			float offsetX = 1.0f / (float)SCREEN_WIDTH;
			float offsetY = 1.0f / (float)SCREEN_HEIGHT;
			Positions[i * 3 + 0] = ((float)x / (float)SCREEN_WIDTH) * 2 - 1 + offsetX;
			Positions[i * 3 + 1] = ((float)y / (float)SCREEN_HEIGHT) * 2 - 1 + offsetY;
			Positions[i * 3 + 2] = 0.0f;
		}
	}

	//for (int i = 0; i < SCREEN_WIDTH * SCREEN_HEIGHT; i++)
	//{
	//	Colors[i] = Color(0, 0, 0);// COLOR_3;
	//}
}

void Display::DrawTestScreen()
{
	for (int x = 0; x < SCREEN_WIDTH; x++)
	{
		for (int y = 0; y < SCREEN_HEIGHT; y++)
		{
			switch (y % 4)
			{
			case 0: DrawPixel(x, y, COLORS[x % 4]); break;
			case 1: DrawPixel(x, y, COLORS[(x / 2) % 4]); break;
			case 2: DrawPixel(x, y, COLORS[(x / 3) % 4]); break;
			case 3: DrawPixel(x, y, COLORS[(x / 4) % 4]); break;
			}
		}
	}
}

void Display::DrawTestTiles()
{
	std::vector<u8> tileBytesGameboy = { 0x3c, 0x7e,
									     0x42, 0x42,
									     0x42, 0x42,
									     0x42, 0x42,
									     0x7e, 0x5e,
									     0x7e, 0x0a,
									     0x7c, 0x56,
									     0x38, 0x7c };

	std::vector<u8> tileBytes2 = 
		{ 0xFF, 0x00, 0x7e, 0xFF, 0x85, 0x81, 0x89, 0x83,
		  0x93, 0x85, 0xa5, 0x8b, 0xc9, 0x97, 0x7e, 0xff };

	std::vector<u8> tileBlank = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
								  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };

	Tile tileGameboy = Tile(tileBytesGameboy);
	Tile tile2 = Tile(tileBlank);

	for (int x = 0; x < SCREEN_WIDTH; x += 8)
	{
		for (int y = 0; y < SCREEN_HEIGHT; y += 8)
		{
			(x / 8 + y / 8) % 2 == 0 ? DrawTile(tileGameboy, x, y) : DrawTile(tile2, x, y);
		}
	}
}

void Display::Cleanup()
{
	delete[] Positions;
	delete[] Colors;
	delete[] Vertices;
}

void Display::DrawNextPixel(u8* memory)
{
	CurrentPixelX++;

	if (CurrentPixelX == SCREEN_WIDTH)
	{
		CurrentPixelX = 0;
		CurrentPixelY++;
	}

	if (CurrentPixelY == SCREEN_HEIGHT + VERTICAL_BLANKING_PERIOD)
	{
		//DrawGraphics();
		CurrentPixelY = 0;
	}

	// Get Pixel Color from Tile
	
	u16 tileMapAddress = GETBIT(memory[0xFF40], 4) ? 0x9C00 : 0x9800;
	////TileMap tileMap = TileMap(tileMapAddress, memory);

	u8 tileX = CurrentPixelX / 8;
	u8 tileY = CurrentPixelY / 8;
	u16 tileIndex = tileY * 32 + tileX;
	u8 pixelX = CurrentPixelX % (SCREEN_WIDTH / 8);
	u8 pixelY = CurrentPixelY % (SCREEN_HEIGHT / 8);
	u8 pixelIndex = pixelY * 8 + pixelX;
	u8 colorIndex = memory[tileMapAddress + tileIndex + pixelIndex];
	Color color = COLORS[colorIndex];

	if (CurrentPixelY < SCREEN_HEIGHT)
		DrawPixel(CurrentPixelX, CurrentPixelY, color);
}

u8 Display::GetCurrentPixelX()
{
	return CurrentPixelX;
}

u8 Display::GetCurrentPixelY()
{
	return CurrentPixelY;
}

void Display::DrawTile(Tile tile, u8 xPos, u8 yPos)
{
	for (int x = 0; x < 8; x++)
	{
		for (int y = 0; y < 8; y++)
		{
			int colorIndex = (yPos + (7 - y)) * SCREEN_WIDTH + (xPos + x);
			int pixelIndex = y * 8 + x;
			Colors[colorIndex] = COLORS[tile.Pixels[pixelIndex]];
		}
	}
}

void Display::DrawPixel(u8 x, u8 y, float r, float g, float b)
{
	int i = (y * SCREEN_WIDTH + x);
	Colors[i].r = r;
	Colors[i].g = g;
	Colors[i].b = b;
}

void Display::DrawPixel(u8 x, u8 y, Color color)
{
	int i = (y * SCREEN_WIDTH + x);
	Colors[i] = color;
}


