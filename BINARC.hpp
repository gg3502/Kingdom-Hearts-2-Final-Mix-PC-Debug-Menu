#pragma once
#include <winsock.h>
#include "MessageDraw.hpp"


union BINARC_INFO_1
{
	u_int Offset;
	unsigned int* Address;
};

struct BINARC_INFO
{
	u_short Type;
	Message::BITFLAG Flag;
	u_int Tag;
	BINARC_INFO_1 _anon_0;
	u_int Size;
};

struct BINARC
{
	u_char Id[3];
	unsigned __int8 ExtFlg : 4;
	unsigned __int8 Version : 4;
	u_int FileNum;
	u_int Address;
	unsigned __int32 Replace : 30;
	unsigned __int32 Flag : 2;
	BINARC_INFO Info[];
};