#pragma once

class Color
{
public:

	float r = 0.0f;
	float g = 0.0f;
	float b = 0.0f;
	float a = 1.0f;

	Color() : r(1.0f), g(1.0f), b(1.0f), a(1.0f) {}
	Color(float r, float g, float b) : r(r), g(g), b(b), a(1.0f) {}
	Color(int r, int g, int b) 
	  : r((float)r / 255.0f),
		g((float)g / 255.0f),
		b((float)b / 255.0f) {}
};