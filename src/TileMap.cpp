#include "TileMap.h"

TileMap::TileMap(u16 startAddress, u8* memory, Tile** tileBank1, Tile** tileBank2)
{
	StartAddress = startAddress;

	u16 tileDataAddress = 0x9000;
	int tileMapSize = 32 * 32;

	for (int i = 0; i < tileMapSize; i++)
	{
		u8 newTileId = memory[startAddress + i];
		UpdateTile(i, newTileId, tileBank1, tileBank2);
		//if (tileIndex < 128)
		//{
		//	Tiles[i] = &tileBank1[i];
		//}
		//else if (tileIndex >= 128)
		//{
		//	Tiles[i] = &tileBank2[i - 128];
		//}

		//u16 tileStartAddress = tileDataAddress + tileIndex * 16;
		//Tiles[i] = new Tile(memory, tileStartAddress);
	}
}

void TileMap::UpdateTile(int tileIndex, u8 newTileId, Tile** tileBank1, Tile** tileBank2)
{
	if (newTileId < 128)
	{
		Tiles[tileIndex] = tileBank1[newTileId];
	}
	else if (newTileId >= 128)
	{
		Tiles[tileIndex] = tileBank2[newTileId - 128];
	}
}
