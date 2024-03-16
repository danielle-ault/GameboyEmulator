#pragma once

#include "Types.h"
#include "Tile.h"

class TileMap
{
public:
	TileMap(u16 startAddress, u8* Memory);

private:
	u16 startAddress;
	u8 TileIndexes[32 * 32];
};

