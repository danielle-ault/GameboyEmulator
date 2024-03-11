#include "Display.h"

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

	DrawPixel(CurrentPixelX, CurrentPixelY);
}

u8 Display::GetCurrentPixelX()
{
	return CurrentPixelX;
}

u8 Display::GetCurrentPixelY()
{
	return CurrentPixelY;
}

void Display::DrawPixel(u8 x, u8 y)
{
	// TODO: implement? somehow?
}
