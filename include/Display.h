#pragma once

#include "Types.h"

class Display
{
public:
	void DrawNextPixel();
	u8 GetCurrentPixelX();
	u8 GetCurrentPixelY();

//private:
	const int SCREEN_WIDTH = 160;
	const int SCREEN_HEIGHT = 144;
	const int VERTICAL_BLANKING_PERIOD = 10;

	u8 CurrentPixelX = 0;
	u8 CurrentPixelY = 0;

	void DrawPixel(u8 x, u8 y);
};

