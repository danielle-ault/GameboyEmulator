#include <vector>

#include "Types.h"

#pragma once
class Tile
{	
public:
	u8 Pixels[64] = { 0 };

	Tile() {}
	Tile(std::vector<u8> bytes);
	Tile(u8* memory, u16 startAddress);

	void CombineBytes();

private:
	u8 InputBytes[16] = { 0 };
	u8 OutputBytes[16] = { 0 };

};

