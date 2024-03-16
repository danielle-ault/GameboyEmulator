#pragma once

#include "DMG.h"

class Test
{
public:
	union
	{
		struct bitfield
		{
			char unused : 1;
			char sw2 : 1;
			char sw1 : 1;
			char state : 2;
			char value : 3;
		};
		char reg;
	};
};

