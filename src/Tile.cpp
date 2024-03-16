#include <iostream>

#include "Tile.h"
#include "Utils.h"

Tile::Tile(std::vector<u8> bytes)
{
	if (bytes.size() != 16)
	{
		throw std::invalid_argument("Tile bytes vector must be 16 bytes long.");
	}

	std::copy(bytes.begin(), bytes.end(), InputBytes);

	CombineBytes();
}

Tile::Tile(u8* memory, u16 startAddress)
{
	for (int i = 0; i < 16; i++)
	{
		InputBytes[i] = memory[startAddress + i];
	}

	CombineBytes();
}

void Tile::CombineBytes()
{
	for (int i = 0; i < 8; i++)
	{
		u8 byteLower = InputBytes[i * 2];
		u8 byteHigher = InputBytes[(i * 2) + 1];
		u16 combined = 0;

		for (int bit = 7; bit >= 0; bit--)
		{
			SETBITVALUE(combined, (bit * 2) + 1, GETBIT(byteHigher, bit));
			SETBITVALUE(combined, bit * 2, GETBIT(byteLower, bit));
		}

		OutputBytes[i * 2] = (combined & 0xFF00) >> 8;
		OutputBytes[(i * 2) + 1] = (combined & 0x00FF);
	}
	
	for (int i = 0; i < 16; i++)
	{
		Pixels[i * 4 + 0] = (OutputBytes[i] & 0b1100'0000) >> 6;
		Pixels[i * 4 + 1] = (OutputBytes[i] & 0b0011'0000) >> 4;
		Pixels[i * 4 + 2] = (OutputBytes[i] & 0b0000'1100) >> 2;
		Pixels[i * 4 + 3] = (OutputBytes[i] & 0b0000'0011);
	}
}
