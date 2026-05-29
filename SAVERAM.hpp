#pragma once

#define __int8 char
#define __int16 short
#define __int32 int
#define __int64 long long


namespace SAVERAM {

	/* 1023 */
	typedef unsigned int u_int;

	/* 1066 */
	struct SAVERAM_HEADER
	{
		u_int ID;
		u_int Version;
		u_int CheckSum;
	};

	/* 1031 */
	typedef unsigned __int8 u_char;

	/* 1030 */
	typedef unsigned __int16 u_short;

	/* 1050 */
	struct AREA_SET
	{
		u_short Map;
		u_short Battle;
		u_short Event;
	};

	/* 1069 */
	struct YS_BITFLAG_0
	{
		__int8 Flag[32];
	};

	/* 1068 */
	struct YS_PROGRESS_FLAG
	{
		YS_BITFLAG_0 Flag[19];
	};

	/* 1072 */
	struct YS_BITFLAG_1
	{
		__int8 Flag[8];
	};

	/* 1071 */
	struct YS_WORLDPOINT_SAVERAM
	{
		YS_BITFLAG_1 Visit;
	};

	/* 1074 */
	struct YS_DISAREA_SAVERAM
	{
		u_short Status[256][2];
	};

	/* 1075 */
	struct YS_VISITAREA_SAVERAM
	{
		YS_BITFLAG_1 Flag[19];
	};

	/* 1078 */
	struct YS_BITFLAG_2
	{
		__int8 Flag[28];
	};

	/* 1077 */
	struct YS_MENUFLAG
	{
		YS_BITFLAG_2 Flag;
	};

	/* 1081 */
	struct YS_BITFLAG_3
	{
		__int8 Flag[64];
	};

	/* 1080 */
	struct YS_TREASURE_SRAM
	{
		YS_BITFLAG_3 GetFlag;
	};

	/* 1083 */
	struct YS_SAVEFLAG
	{
		YS_BITFLAG_3 Flag;
	};

	/* 1084 */
	struct dk_Clock_SaveRam
	{
		volatile u_int m_playTime;
		volatile u_int m_playTimeColor;
		volatile u_int m_worldPlayTime[19];
	};

	/* 1055 */
	struct YS_BITFLAG
	{
		__int8 Flag[4];
	};

	/* 1086 */
	struct YS_JIGSAW_SAVERAM
	{
		YS_BITFLAG_1 Picture[10];
	};

	/* 1067 */
	struct SAVERAM_SYSTEM
	{
		u_char World;
		u_char Area;
		u_char Entrance;
		u_char endian;
		AREA_SET Set[19][64];
		YS_PROGRESS_FLAG ProgressFlag;
		YS_WORLDPOINT_SAVERAM WorldPoint;
		YS_DISAREA_SAVERAM DisArea;
		YS_VISITAREA_SAVERAM VisitArea;
		YS_MENUFLAG MenuFlag;
		YS_TREASURE_SRAM TreasureFlag;
		u_char BgmSet[19];
		YS_SAVEFLAG SaveFlag;
		u_int Munnies;
		dk_Clock_SaveRam PlayTime;
		int GameLevel;
		YS_BITFLAG Flag;
		YS_JIGSAW_SAVERAM Jigsaw;
	};

	/* 1090 */
	struct YS_ABILITY_EQUIP
	{
		unsigned __int16 IsEquip : 1;
		unsigned __int16 Item : 15;
	};

	/* 1093 */
	struct kn_FriendSaveRam_$5810B05976B5CB56E050144FE554327F_$F28A6CDECBC33BD48A78A8B57B3DF189
	{
		u_char style;
		u_char frequencyLevel[31];
	};

	/* 1092 */
	union kn_FriendSaveRam_$5810B05976B5CB56E050144FE554327F
	{
		u_char buffer[32];
		kn_FriendSaveRam_$5810B05976B5CB56E050144FE554327F_$F28A6CDECBC33BD48A78A8B57B3DF189 _anon_0;
	};

	/* 1091 */
	struct kn_FriendSaveRam
	{
		kn_FriendSaveRam_$5810B05976B5CB56E050144FE554327F _anon_0;
	};

	/* 1089 */
	struct YS_PARTRAM
	{
		u_short Weapon[2];
		u_char Hp;
		u_char MaxHp;
		u_char Mp;
		u_char MaxMp;
		u_char Ap;
		u_char Attack;
		u_char Wisdom;
		u_char Defence;
		u_char MpDrive;
		u_char MpDriveMax;
		u_char LevelUpType;
		u_char Level;
		u_char ArmorMax;
		u_char AccessoryMax;
		u_char ItemMax;
		u_char Padding2[1];
		u_short Armor[8];
		u_short Accessory[8];
		u_short Item[8];
		u_short ItemSlot[8];
		YS_ABILITY_EQUIP Ability[80];
		kn_FriendSaveRam friendSaveRam;
	};

	/* 1094 */
	struct YS_FORMRAM
	{
		u_short Weapon;
		u_char Level;
		u_char AbilityLevel;
		int Exp;
		YS_ABILITY_EQUIP Ability[24];
	};

	/* 1095 */
	union YS_SAVERAM_BATTLE_$AAB7EC95340598ECB7300BBE75D22246
	{
		u_char IsTinkExist;
		u_char GenieForm;
	};

	/* 1096 */
	struct YS_MEMBER
	{
		u_char Member[4];
	};

	/* 1098 */
	struct YS_BITFLAG_4
	{
		__int8 Flag[40];
	};

	/* 1100 */
	struct YS_SAVERAM_BATTLE_$DE895A587A7AC394887209ABA6DCF869
	{
		u_short Count;
		u_short AppearCount;
	};

	/* 1102 */
	struct YS_SCORE
	{
		u_char Type;
		char Padding[3];
		u_int Score;
	};

	/* 1103 */
	struct YS_BITFLAG_5
	{
		__int8 Flag[16];
	};

	/* 1101 */
	struct YS_MINIGAME_SRAM
	{
		YS_SCORE Score[64];
		YS_BITFLAG_5 HelpFlag;
	};

	/* 1105 */
	struct YS_STRUGGLE
	{
		u_char Count[3][3];
	};

	/* 1106 */
	struct YS_BATTLE_REPORT
	{
		u_short KillCount;
		u_short FriendRecovCount;
		u_short ContinueCount;
		u_short FormCount[7];
		u_short SummonCount[4];
	};

	/* 1088 */
	struct YS_SAVERAM_BATTLE
	{
		YS_PARTRAM PartRam[13];
		YS_FORMRAM FormRam[10];
		u_char Form;
		u_char Summon;
		u_char SummonLevel;
		YS_SAVERAM_BATTLE_$AAB7EC95340598ECB7300BBE75D22246 SummonArg;
		u_char Drive;
		u_char DriveCount;
		u_char DriveMax;
		u_char DriveTimeCount;
		float DriveTime;
		float DriveTimeMax;
		YS_MEMBER Member[19];
		u_char Item[320];
		YS_BITFLAG_0 ItemSpecial;
		int Exp;
		int SummonExp;
		u_short KnownBattleItem[8];
		u_short ShortcutCommand[4];
		u_char BonusLevel;
		u_char pad[3];
		YS_BITFLAG_0 BonusLevelFlag;
		YS_BITFLAG_0 BattleLevelFlag;
		YS_BITFLAG DisableMpDrive;
		u_int KillCountHL[96];
		u_int KillCountNB[32];
		u_short RcCount[512];
		u_short LimitCombo[32];
		YS_BITFLAG_4 BackyardItemFlag;
		YS_SAVERAM_BATTLE_$DE895A587A7AC394887209ABA6DCF869 Mickey;
		YS_MINIGAME_SRAM MiniGame;
		YS_STRUGGLE Struggle;
		YS_BATTLE_REPORT BattleReport;
		u_short PushWeapon;
	};

	/* 1109 */
	struct YS_BITFLAG_6
	{
		__int8 Flag[1];
	};

	/* 1108 */
	struct YS_WORLDSTATE
	{
		u_char State;
		u_char Scenario;
		u_char ScenarioState;
		YS_BITFLAG_6 Flag;
	};

	/* 1111 */
	struct YS_WORLDFLAG_SAVERAM
	{
		YS_BITFLAG_0 Flag;
		u_char Update[255];
		u_char UpdateNum;
	};

	/* 1112 */
	struct YS_COURSE
	{
		u_char State;
	};

	/* 1113 */
	struct YS_EPISODE_SAVERAM
	{
		YS_BITFLAG_5 Flag;
		YS_BITFLAG_5 NewFlag;
	};

	/* 1107 */
	struct dk_WM_SAVERAM
	{
		YS_WORLDSTATE WorldState[19];
		YS_WORLDFLAG_SAVERAM WorldFlag;
		YS_COURSE CourseState[32];
		YS_EPISODE_SAVERAM Episode;
		u_char WorldLevel;
		u_char Darkness;
	};

	/* 1115 */
	struct Tz_MenuSramSaveLst
	{
		unsigned __int32 Filler : 23;
		unsigned __int32 ChgWorldName : 1;
		unsigned __int32 FileFace : 8;
	};

	/* 1116 */
	struct Tz_MenuSramConfig
	{
		__int16 DispX;
		__int16 DispY;
		unsigned __int32 Filler : 21;
		unsigned __int32 SummonEffect : 2;
		unsigned __int32 CameraVRev : 1;
		unsigned __int32 CameraHRev : 1;
		unsigned __int32 CommandGra : 1;
		unsigned __int32 RightStick : 1;
		unsigned __int32 Camera : 1;
		unsigned __int32 NaviMode : 1;
		unsigned __int32 SoundMode : 2;
		unsigned __int32 Vibration : 1;
	};

	/* 1118 */
	struct YS_BITFLAG_7
	{
		__int8 Flag[80];
	};

	/* 1120 */
	struct YS_BITFLAG_8
	{
		__int8 Flag[12];
	};

	/* 1122 */
	struct YS_BITFLAG_9
	{
		__int8 Flag[72];
	};

	/* 1124 */
	struct YS_BITFLAG_10
	{
		__int8 Flag[36];
	};

	/* 1117 */
	struct Tz_MenuSramCamp
	{
		YS_BITFLAG_7 StockItemFlg;
		YS_BITFLAG_8 SoraAbilityFlg;
		YS_BITFLAG_9 FriendAbilityFlg;
		YS_BITFLAG_10 FormAbilityFlg;
		YS_BITFLAG TutorialFlg;
		YS_BITFLAG StatusFlg;
		u_short SoraAbilityNum[80];
		u_short FriendAbilityNum[576];
		u_short FormAbilityNum[288];
	};

	/* 1128 */
	struct Tz_MenuSramSMenu
	{
		YS_BITFLAG_1 NewFlg;
	};

	/* 1129 */
	struct Tz_MenuSramShop
	{
		YS_BITFLAG_5 ItemSaw;
		YS_BITFLAG_6 DrawInfoFlg;
	};

	/* 1131 */
	struct Tz_MenuSramSynthesis
	{
		YS_BITFLAG_1 ItemFindFlg;
		YS_BITFLAG_1 ClearConditionFlg;
		YS_BITFLAG_8 RecipeFindFlg;
		YS_BITFLAG_8 ListFindFlg;
		YS_BITFLAG_8 RecipeNewFlg;
		YS_BITFLAG_1 EnableStatFlg;
		u_int MogExp;
		u_int ItemCnt[64];
		u_int ItemTotalCnt[64];
		u_char MogLv;
		YS_BITFLAG_6 EnableTopSelFlg;
	};

	/* 1135 */
	struct YS_BITFLAG_11
	{
		__int8 Flag[24];
	};

	/* 1138 */
	struct YS_BITFLAG_12
	{
		__int8 Flag[2];
	};

	/* 1141 */
	struct Tz_PuzzlePiece
	{
		char PieceId;
		char Rot;
		char Filler[2];
	};

	/* 1132 */
	struct Tz_MenuSramJiminy
	{
		YS_BITFLAG_3 DicFindFlg;
		YS_BITFLAG WorldFindFlg;
		YS_BITFLAG AlbumFindFlg[16];
		YS_BITFLAG BattleReportFindFlg;
		YS_BITFLAG_8 MiniGameFindFlg;
		YS_BITFLAG_11 QuestFindFlg;
		YS_BITFLAG StoryFindFlg[16];
		YS_BITFLAG_3 TreasureFindFlg;
		YS_BITFLAG_5 MapFindFlg;
		YS_BITFLAG AnsemFindFlg;
		YS_BITFLAG MenuItemFindFlg;
		YS_BITFLAG_1 PuzzlePieceFlg[10];
		YS_BITFLAG_1 PuzzleInfoFlg[10];
		YS_BITFLAG_12 PuzzleCompleteFlg;
		YS_BITFLAG_12 StatusFlg;
		Tz_PuzzlePiece PuzzleImageInfo[10][48];
	};

	/* 1114 */
	struct Tz_MenuSram
	{
		Tz_MenuSramSaveLst SaveLst;
		Tz_MenuSramConfig Config;
		Tz_MenuSramCamp Camp;
		Tz_MenuSramSMenu SMenu;
		Tz_MenuSramShop Shop;
		Tz_MenuSramSynthesis Synthesis;
		Tz_MenuSramJiminy Jiminy;
	};

	/* 1143 */
	struct Ts_EDITSAVE_SAVEHEADER
	{
		char Ver;
	};

	/* 1032 */
	typedef char BOOL_1;

	/* 1144 */
	struct Ts_EDITSAVE_USERDATA
	{
		char CoreLevel;
		char KeyMode;
		BOOL_1 EditorStartup;
		char Padding[1];
		char Padding2[134];
		char Config[16];
		BOOL_1 MemoEntry[16];
		BOOL_1 MemoNew[16];
		char PlanStatus[128];
		char temporary[32];
	};

	/* 1064 */
	typedef char BUFFER16;

	/* 1142 */
	struct Ts_EDITSAVE
	{
		Ts_EDITSAVE_SAVEHEADER Header;
		Ts_EDITSAVE_USERDATA UserData;
		BUFFER16 GumiShipPlan[32544];
		BUFFER16 TinyShipPlan[10944];
	};

	/* 1146 */
	struct gb_RECORD
	{
		u_int Score;
		u_short AppearEnemyTotal;
		u_short DestroyedEnemyTotal;
		u_short AppearEnemyRed;
		u_short DestroyedEnemyRed;
		u_short AppearEnemyGold;
		u_short DestroyedEnemyGold;
		u_short AppearBonusItem;
		u_short GetBonusItem;
		u_short DamageFrequency;
		u_short EraseShotFrequency;
		u_char StageNo;
		u_char MissionLv;
		u_char Medal;
		u_char Rank;
		u_char MissionClear;
		u_char Recorded;
		u_char CanPlay;
		u_char Reserve;
		float TreasureComplete;
		u_char Padding[28];
	};

	/* 1147 */
	struct gb_ITEM_SAVERAM
	{
		u_char OwnedNum[256];
	};

	/* 1148 */
	struct gb_TREASURE_SAVERAM
	{
		YS_BITFLAG_7 GetFlag;
	};

	/* 1149 */
	struct gb_SAVEFLAG
	{
		YS_BITFLAG_1 Flag;
	};

	/* 1150 */
	struct ryj_ADVICE_SAVERAM
	{
		YS_BITFLAG_1 UseFlag;
	};

	/* 1145 */
	struct ryj_SAVERAM_GUMIBATTLE
	{
		gb_RECORD Record[54];
		gb_ITEM_SAVERAM Item;
		gb_TREASURE_SAVERAM Treasure;
		gb_SAVEFLAG SaveFlag;
		ryj_ADVICE_SAVERAM Advice;
	};

	/* 1065 */
	struct SAVERAM
	{
		SAVERAM_HEADER Header;
		SAVERAM_SYSTEM System;
		YS_SAVERAM_BATTLE Battle;
		dk_WM_SAVERAM WorldMap;
		Tz_MenuSram Menu;
		Ts_EDITSAVE GumiEdit;
		ryj_SAVERAM_GUMIBATTLE GumiBattle;
	};

}