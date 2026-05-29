#pragma once
#include <winsock.h>

typedef u_int OBJENTRY_ID;

struct OBJENTRY_0
{
	OBJENTRY_ID Id;
	u_char Type;
	u_char Subtype;
	u_char DrawPriority;
	u_char Skeleton;
	char EntryName[32];
	char MsetFile[32];
	u_short Flag;
	u_char TargetType;
	u_char Padding[1];
	u_short Part;
	u_short WeaponPart;
	float Weight;
	u_char Cost;
	u_char Page;
	u_char ShadowSize;
	char Form;
	u_short ReadId[4];
};