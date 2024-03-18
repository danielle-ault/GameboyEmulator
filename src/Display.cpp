#include <vector>

#include "Display.h"
#include "Tile.h"
#include "TileMap.h"
#include "Utils.h"

Color Display::COLOR_0 = Color(155 + 20, 188 + 20, 15 + 20);
Color Display::COLOR_1 = Color(139, 172, 15);
Color Display::COLOR_2 = Color(48, 98, 48);
Color Display::COLOR_3 = Color(15, 56, 15);

//Color Display::COLOR_0 = Color(255, 255, 255);
//Color Display::COLOR_1 = Color(170, 170, 170);
//Color Display::COLOR_2 = Color(85, 85, 85);
//Color Display::COLOR_3 = Color(0, 0, 0);

//Color Display::COLORS[] = { COLOR_3, COLOR_2, COLOR_1, COLOR_0 };
Color Display::COLORS[] = { COLOR_0, COLOR_1, COLOR_2, COLOR_3 };

void Display::InitGraphics(int scale)
{
	PixelColors = new Color[SCREEN_WIDTH * SCREEN_HEIGHT];

	//InitPixels();
	//DrawTestScreen();
	DrawTestTiles();

	glGenVertexArrays(1, &VertexArrayID);
	glBindVertexArray(VertexArrayID);

	//GenerateVertexAttributes();

	// Vertex Buffer
	glGenBuffers(1, &VertexBufferID);
	glBindBuffer(GL_ARRAY_BUFFER, VertexBufferID);
	glBufferData(GL_ARRAY_BUFFER,
		         sizeof(Vertices),//SCREEN_WIDTH * SCREEN_HEIGHT * 3 * 2 * sizeof(Vertices[0]), 
		         Vertices, 
		         GL_DYNAMIC_DRAW);

	// Index buffer
	glGenBuffers(1, &IndexBufferID);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IndexBufferID);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(Indices), Indices, GL_STATIC_DRAW);

	// texture stuff
	glGenTextures(1, &TextureID);
	glBindTexture(GL_TEXTURE_2D, TextureID);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	std::vector<float> textureData = GetTextureData(PixelColors);
	glTexImage2D(
		GL_TEXTURE_2D, 
		0, 
		GL_RGB, 
		SCREEN_WIDTH, 
		SCREEN_HEIGHT, 
		0, 
		GL_RGB, 
		GL_FLOAT, 
		textureData.data());

	// position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	// color attribute
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	// texture attribute
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);

	glPointSize(scale);

	Shader = class Shader("shaders/shader.vert", "shaders/shader.frag");
}

void Display::DrawGraphics()
{
	//glClear(GL_COLOR_BUFFER_BIT);

	//GenerateVertexAttributes();

	Shader.Use();

	std::vector<float> textureData = GetTextureData(PixelColors);
	glTexSubImage2D(
		GL_TEXTURE_2D,
		0, 0, 0,
		SCREEN_WIDTH, SCREEN_HEIGHT,
		GL_RGB,
		GL_FLOAT,
		(void*)textureData.data());

	//glTexImage2D(
	//	GL_TEXTURE_2D,
	//	0,
	//	GL_RGB,
	//	SCREEN_WIDTH, 
	//	SCREEN_HEIGHT,
	//	0,
	//	GL_RGB,
	//	GL_FLOAT,
	//	textureData.data());

	//glDrawArrays(GL_TRIANGLES, 0, SCREEN_WIDTH * SCREEN_HEIGHT * 6);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IndexBufferID);
	glBindTexture(GL_TEXTURE_2D, TextureID);
	//glBindVertexArray(VertexArrayID);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, (void*)0);
}

void Display::GenerateVertexAttributes()
{
	int numVertices = 4;// SCREEN_HEIGHT* SCREEN_WIDTH;
	std::vector<float> vertexAttributes(numVertices * 8, 0.0f);
	for (int i = 0; i < numVertices; i++)
	{
		int iPos = i * 3;
		vertexAttributes[(i * 8) + 0] = Positions[iPos + 0];
		vertexAttributes[(i * 8) + 1] = Positions[iPos + 1];
		vertexAttributes[(i * 8) + 2] = Positions[iPos + 2];
		vertexAttributes[(i * 8) + 3] = VertexColors[i].r;
		vertexAttributes[(i * 8) + 4] = VertexColors[i].g;
		vertexAttributes[(i * 8) + 5] = VertexColors[i].b;
		vertexAttributes[(i * 8) + 6] = TexCoords[(i * 2) + 0];
		vertexAttributes[(i * 8) + 7] = TexCoords[(i * 2) + 1];
	}
	std::copy(vertexAttributes.begin(), vertexAttributes.end(), Vertices);
	
	//// Debug Display Vertex Attributes
	//Utils::ClearConsole();
	//for (int i = 0; i < 4 * 8; i += 8)
	//{
	//	std::cout << Vertices[i + 0] << ", ";
	//	std::cout << Vertices[i + 1] << ", ";
	//	std::cout << Vertices[i + 2] << ",   ";
	//	std::cout << Vertices[i + 3] << ", ";
	//	std::cout << Vertices[i + 4] << ", ";
	//	std::cout << Vertices[i + 5] << ",   ";
	//	std::cout << Vertices[i + 6] << ", ";
	//	std::cout << Vertices[i + 7] << ", " << std::endl;
	//}
}

std::vector<float> Display::GetTextureData(Color* colors)
{
	int numColorIndices = SCREEN_WIDTH * SCREEN_HEIGHT * 3;
	std::vector<float> data(numColorIndices, 1);
	for (int i = 0; i < numColorIndices; i += 3)
	{
		data[i + 0] = colors[i / 3].r;
		data[i + 1] = colors[i / 3].g;
		data[i + 2] = colors[i / 3].b;
	}
	return data;
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
	//delete[] Positions;
	//delete[] VertexColors;
	delete[] PixelColors;
	//delete[] Vertices;
	//delete[] Indices;
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
		u16 tileMapAddress = GETBIT(memory[0xFF40], 4) ? 0x9C00 : 0x9800;
		TileMap tileMap = TileMap(tileMapAddress, memory);
		DrawTileMap(&tileMap);

		DrawGraphics();
		Window->SwapBuffers();
		CurrentPixelY = 0;
	}

	// Get Pixel Color from Tile
	
	
	// TODO: the glitchiness is coming from this block of code, I think
	//u8 tileX = CurrentPixelX / 8;
	//u8 tileY = CurrentPixelY / 8;
	//u16 tileIndex = tileY * 32 + tileX;
	//u8 pixelX = CurrentPixelX % (SCREEN_WIDTH / 8);
	//u8 pixelY = CurrentPixelY % (SCREEN_HEIGHT / 8);
	//u8 pixelIndex = pixelY * 8 + pixelX;
	//u8 colorIndex = memory[tileMapAddress + tileIndex + pixelIndex];
	//Color color = COLORS[colorIndex]; //TODO issue is with colorIndex, I think it takes the whole byte instead of just the portion of it
	//if (CurrentPixelY < SCREEN_HEIGHT)
	//	DrawPixel(CurrentPixelX, CurrentPixelY, color);
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
			PixelColors[colorIndex] = COLORS[tile.Pixels[pixelIndex]];
		}
	}
}

void Display::DrawTileMap(TileMap* tileMap)
{
	for (int x = 0; x < SCREEN_WIDTH; x += 8)
	{
		for (int y = 0; y < SCREEN_HEIGHT; y += 8)
		{
			int tileIndex = (y / 8) * 32 + (x / 8);
			DrawTile((tileMap->Tiles)[tileIndex], x, SCREEN_HEIGHT - y - 8);
		}
	}
}

void Display::DrawPixel(u8 x, u8 y, float r, float g, float b)
{
	int i = (y * SCREEN_WIDTH + x);
	PixelColors[i].r = r;
	PixelColors[i].g = g;
	PixelColors[i].b = b;
}

void Display::DrawPixel(u8 x, u8 y, Color color)
{
	int i = (y * SCREEN_WIDTH + x);
	PixelColors[i] = color;
}


