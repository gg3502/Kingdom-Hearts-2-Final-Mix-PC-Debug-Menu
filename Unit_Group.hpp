#pragma once
#include <winsock.h>
#include "fmatrix.hpp"
#include "OBJENRTY_0.hpp"





struct UNIT_DATA_SIGNAL
{
	u_short Signal;
	u_short Arg;
	u_char Action;
	char Padding[3];
};


enum UNIT_DATA_ZONE_OPTION
{
	OPTION_JUMP = 0x0,
	OPTION_GUIDE = 0x1,
	OPTION_GUIDE_TRAIN = 0x2,
	OPTION_TRIGGER = 0x3,
	OPTION_FORMATION = 0x4,
	OPTION_PRIZE = 0x4000,
};

enum UNIT_DATA_ZONE_SHAPE
{
	SHAPE_BOX = 0x0,
	SHAPE_CYLINDER = 0x1,
	SHAPE_INFINITY = 0x2,
};

struct UNIT_DATA_PATH_POINT
{
	float Xpos;
	float Ypos;
	float Zpos;
};

struct UNIT_DATA_RTN_PARAM_POINT
{
	float Xpos;
	float Ypos;
	float Zpos;
};


struct UNIT_DATA_PATH
{
	u_short Serial;
	u_short Num;
	u_char Flag;
	u_char Id;
	u_char reserved[2];
	UNIT_DATA_PATH_POINT Point[];
};

union UNIT_DATA_RTN_PARAM_PARAMS
{
	UNIT_DATA_RTN_PARAM_POINT TargetPos;
	u_int Args[3];
};



struct UNIT_DATA_RTN_PARAM
{
	u_char ID;
	u_char Type;
	u_char Rate;
	u_char DirType;
	UNIT_DATA_RTN_PARAM_PARAMS Params;
};


struct UNIT_DATA_ZONE
{
	UNIT_DATA_ZONE_SHAPE Shape;
	UNIT_DATA_ZONE_OPTION Option;
	float Xpos;
	float Ypos;
	float Zpos;
	float Xsize;
	float Ysize;
	float Zsize;
	float Xrot;
	float Yrot;
	float Zrot;
	u_int Flag;
	u_short Kind;
	u_char OnBgGroup;
	u_char OffBgGroup;
	char Padding[16];
};


struct SLIST_3425
{
	int* Head;
	int* Tail;
};


struct UNIT_DATA_OBJ
{
	OBJENTRY_ID EntryId;
	float Xpos;
	float Ypos;
	float Zpos;
	float Xrot;
	float Yrot;
	float Zrot;
	u_char AppearWay;
	u_char AppearArg;
	u_short Serial;
	int Args[2];
	u_short SysRC;
	u_short AppearWait;
	u_short Command;
	u_short AppearRange;
	u_char Level;
	u_char Medal;
	char Reserve[14];
};




struct UNIT_DATA_GROUP_JUMP_AREA
{
	u_char Area;
	u_char Entrance;
	u_char JumpType;
	u_char pad;
};

struct UNIT_DATA_GROUP_APPEAR_ENEMY
{
	u_short StartWait;
	u_short AppearWait;
	u_short LeaveWait;
	u_char LevelUpPoint;
	u_char RepeatEnd;
	u_char RepeatTop;
	u_char pad;
};

struct UNIT_DATA_GROUP_EVENT_SIGNAL
{
	u_int Signal;
};

struct UNIT_DATA_GROUP_BATTLE
{
	u_int Signal;
};

struct UNIT_DATA_GROUP_TRON
{
	u_int Turn;
};

struct UNIT_DATA_GROUP_TRON_WALL
{
	float AppearDist;
	float LeaveDist;
};


union UNIT_DATA_GROUP_PARAMS
{
	char Params[16];
	UNIT_DATA_GROUP_JUMP_AREA Jump;
	UNIT_DATA_GROUP_APPEAR_ENEMY Enemy;
	UNIT_DATA_GROUP_EVENT_SIGNAL Event;
	UNIT_DATA_GROUP_BATTLE Battle;
	UNIT_DATA_GROUP_TRON Tron;
	UNIT_DATA_GROUP_TRON_WALL TronWall;
};

struct UNIT_DATA_GROUP
{
	u_char Type;
	u_char Flag;
	u_short Id;
	u_short ObjNum;
	u_short ZoneNum;
	u_short PathNum;
	u_short RtnParamNum;
	u_short SignalNum;
	char Reserve[14];
	UNIT_DATA_GROUP_PARAMS Params;
	UNIT_DATA_OBJ ObjData[];
};

struct UNIT_GROUP
{
	u_int UnitName;
	BITFLAG Flag;
	const UNIT_DATA_GROUP* Data;
	SLIST_3425 ZoneList;
	float Wait;
	int EnemyNum;
	int EnemyNumOrig;
	u_char Level;
	const UNIT_DATA_OBJ* ObjData;
	const UNIT_DATA_ZONE* ZoneData;
	const UNIT_DATA_PATH* PathData;
	const UNIT_DATA_RTN_PARAM* RtnParamData;
	const UNIT_DATA_SIGNAL* SignalData;
};

struct UnitEntry {
	char UnitName[4]; //0x0000
	char pad_0004[4];
	void* Pointer; //0x0008
}; //Size: 0x0010