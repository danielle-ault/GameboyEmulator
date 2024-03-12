#include "Display.h"

#include <vector>

Color Display::COLOR_0 = Color(155 + 20, 188 + 20, 15 + 20);
Color Display::COLOR_1 = Color(139, 172, 15);
Color Display::COLOR_2 = Color(48, 98, 48);
Color Display::COLOR_3 = Color(15, 56, 15);
Color Display::COLORS[] = { COLOR_0, COLOR_1, COLOR_2, COLOR_3 };

void Display::InitGraphics(int scale)
{
	Positions = new float[SCREEN_WIDTH * SCREEN_HEIGHT * 3];
	Colors = new Color[SCREEN_WIDTH * SCREEN_HEIGHT];
	Vertices = new float[SCREEN_WIDTH * SCREEN_HEIGHT * 3 * 2];

	InitPixels();
	DrawTestScreen();

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
	glBufferData(GL_ARRAY_BUFFER, SCREEN_WIDTH * SCREEN_HEIGHT * 3 * 2 * sizeof(Vertices[0]), Vertices, GL_STATIC_DRAW);

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
	glClear(GL_COLOR_BUFFER_BIT);

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

	for (int i = 0; i < SCREEN_WIDTH * SCREEN_HEIGHT; i++)
	{
		Colors[i] = COLOR_3;
	}
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

void Display::Cleanup()
{
	delete[] Positions;
	delete[] Colors;
	delete[] Vertices;
}

void Display::DrawNextPixel()
{
	CurrentPixelX++;

	if (CurrentPixelX == SCREEN_WIDTH)
	{
		CurrentPixelX = 0;
		CurrentPixelY++;
	}

	if (CurrentPixelY == SCREEN_HEIGHT + VERTICAL_BLANKING_PERIOD)
	{
		CurrentPixelY = 0;
	}

	//DrawPixel(CurrentPixelX, CurrentPixelY);
}

u8 Display::GetCurrentPixelX()
{
	return CurrentPixelX;
}

u8 Display::GetCurrentPixelY()
{
	return CurrentPixelY;
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


