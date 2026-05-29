#pragma once
#include <winsock.h>

struct TREASURE_TABLE
{
	u_short Id;
	u_short Item;
	u_char Type;
	u_char World;
	u_char Area;
	u_char Num;
	u_short Progress;
	u_short FlagId;
};