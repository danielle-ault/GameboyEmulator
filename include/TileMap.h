#pragma once

#include "Types.h"
#include "Tile.h"

class TileMap
{
public:
	Tile* Tiles[32 * 32];
	
	TileMap(u16 startAddress, u8* Memory, Tile** tileBank1, Tile** tileBank2);
	void UpdateTile(int tileIndex, u8 newTileId, Tile** tileBank1, Tile** tileBank2);

private:
	u16 StartAddress;
	u8 TileIndexes[32 * 32];
};

