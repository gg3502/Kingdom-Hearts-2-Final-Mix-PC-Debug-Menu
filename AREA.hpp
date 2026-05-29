#pragma once
#include <cstdint>

class AREA
{
public:
	uint8_t World; //0x0000
	uint8_t Area; //0x0001
	uint8_t Entrance; //0x0002
	uint8_t undefined;//0x0003
	uint8_t Map; //0x0004
	uint8_t undefined1; //0x0005
	uint8_t Battle; //0x0006
	uint8_t undefined2; //0x0007
	uint8_t Event; //0x0008
};