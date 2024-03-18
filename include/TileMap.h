#pragma once

#include "Types.h"
#include "Tile.h"

class TileMap
{
public:
	TileMap(u16 startAddress, u8* Memory);

	Tile Tiles[32 * 32];
private:
	u16 startAddress;
	u8 TileIndexes[32 * 32];
};

