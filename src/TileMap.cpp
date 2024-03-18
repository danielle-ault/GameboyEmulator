#include "TileMap.h"

TileMap::TileMap(u16 startAddress, u8* memory)
{
	u16 tileDataAddress = 0x9000;
	int tileMapSize = 32 * 32;

	for (int i = 0; i < tileMapSize; i++)
	{
		u16 tileStartAddress = tileDataAddress + memory[startAddress + i] * 16;
		Tiles[i] = Tile(memory, tileStartAddress);
	}
}
