#pragma once
#include "YW.hpp"
#include "TASK_177.hpp"
#include "YS.hpp"
#include "AREA.hpp"
#include "fmatrix.hpp"
#include "treasureTable.hpp"
#include "MemoryAllocator.hpp"
#include "MessageDraw.hpp"
#include "BINARC.hpp"
#include "obj_camera.hpp"
#include "kn_camera.hpp"
#include "ProjectionMatrix.hpp"
#include "KN.hpp"
#include "fog.hpp"

namespace FunctionCalls {
	typedef void(__cdecl* InitDebugMenu)();
	typedef void(__fastcall* tYWButton)(YW::BUTTON_3* a, const char* label, int x, int y, int width, int height, void(*press_callback)(unsigned int*), unsigned int* arg);

	typedef void(__fastcall* tSetObjGroundPos)(YS::OBJ* obj);

	typedef YW::YW_WINDOW_39* (__fastcall* tOpenSingletonWindow)(const char* title, int width, int height, u_int option);

	typedef void(__fastcall* tContainerLink)(YW::CONTAINER* a, YW::WIDGET* widget);

	typedef void(__fastcall* tContainerUnLink)(YW::CONTAINER_0* a, YW::WIDGET* widget);

	typedef void(__fastcall* tYWLabel)(YW::LABEL_3* a, const char* label, int x, int y, int width, int height);

	typedef void(__fastcall* tYWLabelNew)(YW::LABEL* a, const char* label, int x, int y, int width, int height);

	typedef void(__fastcall* tDialWidget)(YW::DIAL_3* a, int x, int y, int width, int height, int min, int max, void (*update_callback)(int));

	typedef void(__fastcall* tDrawRectRiseWindow)(YW::RECT* a, const YW::COLOR_0* color);
	typedef void(__fastcall* tDrawRectSinkWindow)(YW::RECT* a, const YW::COLOR_0* color);

	typedef int(__fastcall* tFontGetTextHeight)(const char* str);
	typedef int(__fastcall* tFontGetTextWidth)(const char* str);



	typedef Fog* (__cdecl* tGetMapFogDefaultColors)();

	typedef OBJENTRY_0* (__fastcall* tGetOBJENTRY)(OBJENTRY_ID id);
	typedef void(__fastcall* tReadRequest)(int id, int pri, int bank);
	typedef void(__fastcall* tFlushCacheBuff)(TASK* task);

	typedef YS::OBJ* (__fastcall* tOBJENTRY_APPEAR)(OBJENTRY_ID id, const Vector4 Pos, double rot);

	typedef TASK_0* (__fastcall* tCreateFieldThread)(int id, int priority, TASK_FUNC entry_func, int stack_size);

	typedef TASK_0* (__fastcall* tCreateFieldTask)(int id, int priority, TASK_FUNC entry_func);


	typedef void(__fastcall* tTaskSleep)(TASK* a, int wait);

	typedef void(__fastcall* tTaskSuicide)(TASK* task);

	typedef bool(__fastcall* tIsOBJValid)(YS::OBJ* obj);


	typedef UINT* (__fastcall* tAllocArea)(size_t area);

	typedef int(__fastcall* tDecodePointer)(uint32_t encodedAddress);


	typedef void(__fastcall* tSavekh2CNF)();

	typedef unsigned int* (__fastcall* tAlloc_YW_Memory)(YW::MEMORY* a);

	typedef unsigned int* (__fastcall* tAlloc_YW_MemoryNew)(unsigned int size);

	typedef void(__cdecl* tDebugPause)();
	typedef void(__cdecl* tDebugPauseOff)();

	typedef void(__fastcall* tShowDebugWindow)(YW::WIDGET* a);


	typedef void(__fastcall* tDebugWindowAddSubMenu)(int submenu, const YS::ELEM* elem);

	typedef YW::WINDOW* (__fastcall* tOpendbgWindow)(const char* title, int width, int height, u_int option);

	typedef int(__fastcall* tWidget)(YW::WIDGET* a, int x, int y, int width, int height);


	typedef void(__fastcall* tStatusPrintf)(const char* format, ...);


	typedef void(__fastcall* tSetObjPosition)(YS::OBJ* a, Vector4 pos);

	typedef void(__fastcall* tOBjCameraReturnMode)(obj_camera* camera);

	typedef void(__fastcall* tknModelOBJResetStealth)(kn::ModelObj_9* a, int part);

	typedef void(__fastcall* tknModelOBJSetStealth)(kn::ModelObj_9* a, int part);

	typedef void(__fastcall* tShowOBJPart)(YS::OBJ* obj, int part);

	typedef void(__fastcall* tHideOBJPart)(YS::OBJ* obj, int part);


	typedef YS::OBJ* (__cdecl* tGetPlayer)();

	typedef int(__cdecl* tGetKNCameraMode)();

	typedef void(__fastcall* tSetKNCameraMode)(int mode);

	typedef float(__fastcall* tGetOBJBoneMatrix)(YS::OBJ* obj, int bone, Matrix4x4* mat);


	typedef void(__fastcall* tYWFrame)(YW::FRAME* a, int x, int y, int w, int h, int child_w, int child_h);


	typedef void(__fastcall* tYWListButton)(YW::LIST_BUTTON* a, int x, int y, int w, int h, int current, const char** list, void(*callback)(int, unsigned int*), unsigned int* arg);


	typedef void(__fastcall* tYWToggleButton)(YW::TOGGLE_BUTTON_0* a, const char* label, int x, int y, int w, int h, bool flag, void(*toggle_callback)(bool, unsigned int*), unsigned int* arg);


	typedef void(__fastcall* tOpenInformationWindow)(const char* message);


	typedef void(__fastcall* tCampMenuStart)(int type, int param);


	typedef void(__fastcall* tPlaySystemSEsound)(int SoundEffectNum, int id);


	typedef u_int(__fastcall* tsoundCreate)(int seNum, Vector4 pos);


	typedef u_int(__fastcall* tSTDOBJ_Call_se)(YS::STDOBJ* a, int num, Vector4 pos);


	typedef void(__fastcall* tControlEventOff)(int type);


	typedef void(__fastcall* tControlEventOn)(int type);


	typedef void(__cdecl* tCommonLayerOff)();


	typedef void(__cdecl* tCommonLayerOn)();


	typedef YS::PARTY* (__fastcall* tPartyGet)(int n);


	typedef void(__fastcall* tTargetSetObj)(YS::TARGET* a, YS::OBJ* obj, int group);


	typedef void(__fastcall* tSetCurrentBGM)(YS::BGMSET* bgmset);


	//typedef void(__fastcall* tMapJumpReset)(unsigned int* arg);
	//extern tMapJumpReset MapJumpReset;

	typedef void(__fastcall* tSetMember)(YS::member* member, int n, int member_part);


	typedef YS::member* (__fastcall* tGetMember)(int world);


	typedef void(__fastcall* tMapJump)(AREA area, u_int effectType, int delay, bool is_continue, int recov);


	typedef u_int(__fastcall* tgbVoice)(int id, Vector4 pos);


	typedef YS::OBJ* (__fastcall* tObjClick)(int x, int y);


	typedef const char* (__fastcall* tGetObjEntryName)(YS::OBJ* a);


	//typedef void(__fastcall* tCursorUpdate)(YW_CURSOR* a, double x, double y, u_int button, int w, int h);
	//extern tCursorUpdate CursorUpdate;

	typedef void(__fastcall* tSetOBjRot)(YS::OBJ* a, double rot);


	typedef void(__fastcall* tCreateObjCamera)(YS::OBJ* obj, int type);


	typedef bool(__fastcall* tAttackStrike)(YS::BTLOBJ* owner, int param_id, YS::BTLOBJ* target, Vector4 dir, bool is_force);


	typedef bool(__fastcall* tCanControlObj)(YS::OBJ* a);


	typedef void(__fastcall* tFriendTarget)(YS::OBJ* a, YS::TARGET* target, Vector4 pos, double distance);


	typedef void(__fastcall* tObjActStart)(YS::ACT* a, const char* label);


	//typedef void(__fastcall* tStartMotion)(YS_MOTION_289* a, int id, double blend_time, double start_time);
	//extern tStartMotion StartMotion;

	typedef void(__fastcall* tSetObjSpecialEffect)(YS::STDOBJ* a, int type);


	typedef YS::OBJ_ATTACH* (__fastcall* tAttachOBj)(YS::OBJ* a, YS::OBJ* parent, int bone, int flag);


	typedef void(__fastcall* tOBJAttachCamera)(YS::OBJ* a, kn_FMatrix* matrix);


	typedef void(__fastcall* tModelDrawShape)(kn_FMatrix* matrix, u_int color, int delay);


	typedef OBJENTRY_ID(__fastcall* tOBJgetEntryID)(YS::OBJ* a);


	typedef void(__fastcall* tOBJscatterPrize)(YS::OBJ* a, int id);


	typedef void(__fastcall* tSetCommandItem)(YS::FRIEND* a, int kind, YS::OBJ* target, int num);


	typedef YS::PARTY* (__fastcall* tGetPartyChara)(int index);


	typedef int(__fastcall* tGetYWFontTextWidth)(const char* str);

	typedef void(__fastcall* tOBJtexanmStart)(YS::OBJ* obj, int id);


	typedef void(__fastcall* tSetWindowTitle)(YW::WINDOW* a, const char* title);


	typedef void(__fastcall* tYWList)(YW::YW_LIST_0* a, int x, int y, const char** elem, void(*select_callback)(YW::EVENT, int, unsigned int*), unsigned int* select_callback_arg);


	typedef void(__fastcall* tSpawnPlayer)(YS::PLAYER* a, OBJENTRY_0* entry, int form, YS::PAD* pad, Vector4 pos, double rot);


	typedef YS::PAD* (__fastcall* tGetSoftPad)(int port);


	typedef YS::COMMAND_ELEM* (__fastcall* tCommandElem)(int id);


	typedef int(__fastcall* tGetDialValue)(YW::DIAL_3* a);


	typedef void(__fastcall* tSetDialValue)(YW::DIAL_3* a, int number);


	typedef void(__fastcall* tYWFontPrint)(int x, int y, YW::YW_COLOR_0* color, const char* str);


	typedef void(__fastcall* tConfigWidget)(YW::CONFIG_WIDGET* a);


	typedef void(__fastcall* tConfigWidget2)(YW::CONFIG_WIDGET_2* a);


	typedef void(__fastcall* tGameOver)(YS::GAMEOVER* a, YS::OBJ* target, int mode);


	typedef void(__fastcall* tUnitGroupRandomPosPlayer)(Vector4* pos, OBJENTRY_ID entryid);


	typedef Vector4(__fastcall* tGetObjPos)(YS::OBJ* a);


	typedef void(__fastcall* tOpenTreasureEventInfo)(int articles);


	typedef int(__cdecl* tGetTreasureInfoNum)();


	typedef TREASURE_TABLE* (__fastcall* tTreasureTableGetEvent)(int id);


	typedef void(__fastcall* tGetTreasureTableTreasureBox)(TREASURE_TABLE* a);


	typedef void(__fastcall* tInformationTimer)(int imd, int sqd, int align, int pri, int alertseconds);


	typedef void(__fastcall* tMissionStart)(int id, const char* file);


	typedef void(__cdecl* tActivateMission)();


	typedef void(__fastcall* tRestartTimer)(int second, int id);


	typedef void(__fastcall* tActivateMissionTimer)(YS::MISSION_TIMER* a);


	typedef void(__fastcall* tMission2DActivate)();


	typedef void(__fastcall* tChangeForm)(YS::CHANGEFORM* a, YS::SORA* player, int form, OBJENTRY_ID entry_id);


	typedef void(__fastcall* tChangeFormCapture)(YS::SORA* player, OBJENTRY_ID entry_id);

	typedef void(__fastcall* tChangeFormCaptureEnd)(YS::SORA* player);


	typedef YS::SORA* (__cdecl* tGetSora)();


	typedef size_t(__fastcall* tFile_Read)(const char* filename, unsigned int* addr);


	typedef void(__fastcall* tApplyPlayerParam)(int id);


	typedef void(__fastcall* tSwitchStatus)(int n);


	typedef YS::PAD* (__fastcall* tGetHardPad)(int port);


	


	typedef void(__fastcall* tClearStatusWindow)(YW::STATUS_0* a);


	typedef void(__fastcall* tBattleReportPrintf)(YW::TEST_BATTLEREPORT* a, const char* format, ...);


	typedef char* (__cdecl* tGetEventProjectName)();


	typedef BOOL* (__cdecl* tShowVersion)(bool value);


	typedef void(__fastcall* tRegionInit)(int region);


	typedef bool(__cdecl* tIsFreeAbility)(bool flag);


	typedef void(__cdecl* tSetEventSkipFlag)();


	typedef void(__cdecl* tCacheBuffDestroyAll)();


	typedef YS::SA_EFFECT_WK_0* (__fastcall* tGetEventEffectWK)(int put_id);


	typedef YS::RYJ_EFFECT* (__fastcall* tPaxStart)(YS::RYJ_PAX* a, int id, int flag, int type, int priority, unsigned int* obj);


	typedef void(__fastcall* tReadEventEffectSet)(const char* filename, int put_id);


	typedef MemoryAllocator* (__fastcall* tCreateKnMemoryAllocator)(unsigned int* buffer, size_t buffer_size);


	typedef void(__fastcall* tMessageDraw_MessageDraw)(Message::MESSAGEDRAW_158* a, Message::FONT_TYPE type, const char* message, int size, Message::ALIGN align);


	typedef void(__fastcall* tMessageDraw_Draw)(Message::MESSAGEDRAW_158* a, int x, int y, u_int color);


	typedef void(__fastcall* tRealMessageDraw)(Message::FONT_TYPE type, const char* msg, int x, int y, u_int color, int font_size, Message::ALIGN align);

	typedef void(__fastcall* tYSMessageDraw)(Message::FONT_TYPE type, YS::MESSAGE, int x, int y, u_int color, int font_size, Message::ALIGN align);


	typedef Message::MESSAGE* (__fastcall* tMessageSprintf)(char* buff, const char* format, ...);


	typedef void(__fastcall* tTalkBubble)(const YS::WINDOW_BALLOON* a, int x, int y, int message);

	typedef void(__fastcall* tOBJ2DWindow)(YS::OBJ2D_WINDOW* a, YS::WINDOW* b);


	typedef void(__fastcall* tConfigSaveArea)();


	typedef YS::GAME_MODE* (__cdecl* tGetFieldMode)();


	typedef void(__fastcall* tStartGameMode)(YS::GAME_MODE* a);


	typedef void(__cdecl* tDBGWINsendFont)();


	typedef void(__fastcall* tOpenMapJump)(int n);


	typedef void(__fastcall* tOpenNewWindow)(YW::WINDOW* a, const char* title, int x, int y, int w, int h, u_int option);


	typedef int(__fastcall* tStopBGM)(int bank);


	typedef void(__fastcall* tMatrixUnit)(Matrix4x4* a);


	typedef void(__fastcall* tDrawStuff)(YS::OBJ* a);

	typedef AREA* (__cdecl* tContinueGetArea)(AREA retstr);


	typedef unsigned int* (__fastcall* tAllocDebugMem)(size_t size);


	typedef void(__fastcall* tTestFlagWidget)(YS::TEST_FLAG* a, const char* filename);


	typedef YS::PLAYER* (__fastcall* tAppearPlayer)(OBJENTRY_0* entry, int form, Vector4 pos, double rot);


	typedef void(__fastcall* tFormCaptureEnd)(YS::SORA* player);

	typedef void(__fastcall* tSora)(YS::SORA* sora, OBJENTRY_0* objentry, int form, YS::PAD* pad, Vector4 Position, double rot);

	typedef void(__fastcall* tInitializePlayerCommand)(YS::PLAYER_COMMAND* command, YS::PLAYER* player, YS::PAD* pad);

	typedef void(__fastcall* tInitializeFieldCommand)(YS::FIELD_COMMAND* command, YS::PLAYER* player, YS::PAD* pad);

	typedef void(__fastcall* tPlayerOVRMakeMovement)(YS::PLAYER* player);


	typedef u_int(__cdecl* tVU1DrawClose)();


	typedef void(__cdecl* tVU1DrawFlush)();


	typedef int(__fastcall* tSOUNDgetlinknum)(int type);


	typedef int(__cdecl* tGetEventRestFrame)();


	typedef char* (__fastcall* tGetAreaName)(AREA* area, char* buff);


	typedef YS::OBJ* (__fastcall* tEachObj)(YS::OBJ* obj);

	typedef AREA* (__fastcall* tLocalSetGetArea)(AREA area, int set, int world);


	typedef YS::ENEMY* (__fastcall* tEachEnemy)(YS::ENEMY* enemy);


	typedef void(__fastcall* tYWStatusWindow)(YW::STATUS* a, int x, int y, int w, size_t len);

	typedef void(__fastcall* tYWScrollbar)(YW::SCROLLBAR* a, int x, int y, int width, int height, float min, float max, float thumbwidth, float step);


	typedef ALLOCATOR* (__cdecl* tGetAreaAllocator)();


	typedef void(__fastcall* tPlayEventVoice)(const char* filename, int type);


	typedef YS::OBJ* (__cdecl* tGetOBJMenuObj)();


	typedef void(__fastcall* tPartyHook)(YS::PARTY* a, YS::VM* vm);


	typedef int(__fastcall* tGetCache_BuffPriority)(const char* name);


	typedef int(__fastcall* tGetCache_BuffStatus)(const char* name);


	typedef size_t(__fastcall* tGetMemoryAllocatorSize)(MemoryAllocator* a);


	typedef size_t(__fastcall* tGetMemoryAllocatorUsedSize)(MemoryAllocator* a);


	typedef int(__cdecl* tGetCache_BuffFreesize)();


	typedef unsigned int* (__fastcall* tAllocToCache_Buff)(const char* name, int pri);


	typedef void(__fastcall* tReadCache_BuffRequest)(const char* name, int pri, size_t size);


	typedef void(__fastcall* tPaxInit)(YS::RYJ_PAX* a, unsigned int* data);


	typedef void(__cdecl* tLoadSaveGame)();


	typedef int(__fastcall* tMenuSound_PlaySE)(int type, bool play);


	typedef void(__cdecl* tConfigReflection)();


	typedef void(__fastcall* tVAG_STREAMPlay)(const char* fileName, int volume, int fadeVolume, int time);


	typedef void(__cdecl* tSetTitleShowVersionClear)();


	typedef void(__fastcall* tFadeoutAllSounds)(int fadetime);


	typedef void(__cdecl* tSoftResetStartExit)();


	typedef void(__fastcall* tGetObjDir)(YS::OBJ* a, YS::OBJ* obj);


	typedef void(__fastcall* tYW_WINDOW_set_min_size)(YW::YW_WINDOW_39* a, int w, int h);


	typedef void(__fastcall* tYW_WINDOW_set_max_size)(YW::YW_WINDOW_39* a, int w, int h);


	typedef void(__fastcall* tYS_Config_changeMoveSpeed)(int n, unsigned int* arg);


	typedef void(__fastcall* tFreeYWButtonwidget)(YW::WIDGET_5* a, bool b);


	typedef void(__fastcall* tFreeYWwindowwidget)(YW::WIDGET_5* a, bool b);
	typedef void(__fastcall* tFreeYWbuttonwidget)(YW::WIDGET_5* a, bool b);
	typedef void(__fastcall* tFreeYWlistwidget)(YW::WIDGET_5* a, bool b);
	typedef void(__fastcall* tFreeYWframewidget)(YW::WIDGET_5* a, bool b);


	typedef YS::PARTY* (__fastcall* tSetPartyWeapon)(YS::PARTY* a, OBJENTRY_ID entry_id, int hand);


	typedef int(__fastcall* tParty_Hand_to_bone)(YS::PARTY* a, int hand);


	typedef void(__fastcall* tDetatchObject)(YS::OBJ* a);


	typedef void(__fastcall* tDestroyCacheBuffPri)(int pri);


	typedef void(__fastcall* tDestroyObj)(YS::OBJ* obj);

	typedef void(__fastcall* tHideObjPri)(int pri);


	typedef void(__fastcall* tHideObjPart)(YS::OBJ* a, int part);


	typedef OBJENTRY_ID(__fastcall* tGetWeaponEntry)(OBJENTRY_ID entry_id, int hand);


	//typedef bool(__fastcall* tDoesMotionExist)(YS_MOTION_289* a, int id);
	//extern tDoesMotionExist DoesMotionExist;

	//typedef MOTION_289_QUEUE* (__fastcall* tPushMotionToQueue)(YS_MOTION_289* a, int id, double blend_time, double start_time, void(__fastcall* callback)(OBJ_84*));
	//extern tPushMotionToQueue PushMotionToQueue;

	//typedef int(__fastcall* tMotionIdToNum)(YS_MOTION_289* a, int id);
	//extern tMotionIdToNum MotionIdToNum;

	typedef BINARC* (__fastcall* tGetMotionSetMotion)(YS::MOTIONSET* a, int num);


	typedef BINARC_INFO* (__fastcall* tGetBinArcInfo)(const BINARC* a, u_int type, int n);


	typedef void(__fastcall* tTEST_OBJMENU_TEST_OBJMENU)(YS::TEST_OBJMENU* a, YS::OBJ* obj, int x, int y);


	typedef void(__fastcall* tOBJ_DrawSphere)(YS::OBJ* a, Vector4 Scale, u_int color, double yofs);


	typedef int(__fastcall* tGetObjSerial)(YS::OBJ* a);


	typedef void(__fastcall* tTargetSearch)(YS::TARGET* a, YS::TARGET_TYPE type, YS::OBJ* obj);

	typedef void(__fastcall* tTargetReplace)(YS::OBJ* from, YS::OBJ* to);


	typedef void(__fastcall* tSearchTargetBTLNPC)(YS::TARGET* a, YS::OBJ* obj, double boss_offset);


	typedef const char* (__fastcall* tGetItemMessage)(int item, int icon);


	typedef void(__fastcall* tDrawYWRect)(YW::YW_RECT* a, YW::YW_COLOR_0* color);


	typedef void(__cdecl* tSendFont2Dbgwin)();


	typedef void(__fastcall* tgb_messagedraw)(Message::FONT_TYPE font_type, const char* message, int x, int y, u_int color, int font_size, Message::ALIGN align);


	typedef void(__fastcall* tgmMessageDraw)(Message::gm_message* a, int x, int y);


	typedef void(__fastcall* tStopUnitScript)(int id);


	typedef int(__fastcall* tStartUnitScript)(unsigned int* code);


	typedef void(__fastcall* tFileReadBack)(const char* filename, unsigned int* addr, YS::FILE_READ_CALLBACK callback, unsigned int* arg);


	typedef YS::VM* (__fastcall* tStartVM)(unsigned int* code, YS::OBJ* obj);


	typedef void(__fastcall* tCallVM)(YS::VM* a, int func, YS::BD_VALUE* args, int argc);


	typedef YS::VM_STATUS* (__fastcall* tExecuteVM)(YS::VM* a, YS::VM* current);


	typedef Vector4* (__fastcall* tVector4_Assign)(Vector4* a, Vector4* rhs);


	typedef Vector4* (__fastcall* tfMatrix44_operator)(Vector4* retstr, Matrix4x4* a, Vector4* v);


	typedef Matrix4x4* (__fastcall* tMatrix4x4tOperator)(Matrix4x4* retstr, Matrix4x4* a);


	typedef Matrix4x4* (__fastcall* tAssignMatrix)(Matrix4x4* a, Matrix4x4* rhs);


	typedef float(__fastcall* tNormalizeVector4)(Vector4* a);


	typedef Vector4* (__fastcall* tMultiplyByScalar)(Vector4* a, double s);


	typedef float(__fastcall* tNormalizeRadian)(double rad);


	typedef Vector4* (__fastcall* tAddAssign)(Vector4* a, Vector4* r);

	typedef void(__fastcall* tInitializeAsIdentityMatrix)(Matrix4x4* a);


	typedef Matrix4x4* (__fastcall* tApplyMatrixRotationX)(Matrix4x4* a, double radians);

	typedef Matrix4x4* (__fastcall* tMatrixRotateXYZ)(Matrix4x4* a, float radians);


	typedef Matrix4x4* (__fastcall* tApplyMatrixRotationY)(Matrix4x4* a, double radians);


	typedef void(__fastcall* tAdjustEyeAndAtTargets)(obj_camera* a);


	typedef ProjectionMatrix* (__fastcall* tGetProjection)(bool current);

	typedef void(__fastcall* tCalcProjectionMatrix)(ProjectionMatrix* a);


	typedef kn_camera* (__fastcall* tknGetCamera)(bool current);




	typedef float(__fastcall* tGetProjectionFOV)(screen_projection* a, int fovType);

	typedef Vector4* (__fastcall* tGetScreenPos)(Vector4* position, Vector4 pos);


	typedef void(__fastcall* tknCameraGetPos)(kn_camera* a, Vector4* pos);


	typedef void(__fastcall* tknCameraGetRef)(kn_camera* a, Vector4* ref);

	typedef void(__fastcall* tknCameraSetEye)(kn_camera* a, float x, float y, float z);

	typedef void(__fastcall* tknCameraSetAt)(kn_camera* a, float x, float y, float z);


	typedef void(__fastcall* tObjCameraCancelSubjective)(double time);


	typedef float(__fastcall* tInverseMatrix4x4)(Matrix4x4* a);


	typedef u_int(__fastcall* tSoundCreateIOPvoice)(int bank, int num, Vector4 pos, int minvolume);


	typedef void(__fastcall* tSoundBGMFadeOut)(int fadetime, int mode);


	typedef void(__fastcall* tSoundVoicePlay)(unsigned int* adrs, int size, int vol, int pan);


	typedef void(__fastcall* tClearCacheBuff)(int pri);


	typedef void(__fastcall* tDeleteObject)(YS::OBJ* obj);


	typedef void(__fastcall* tCreateGauge)(int type, YS::OBJ* obj);


	typedef void(__fastcall* tPrintYWtext)(YW::TEXT* a, const char* format, ...);


	typedef void(__fastcall* tYWtextdraw_func)(YW::TEXT* text, YW::RECT* rect, YW::COLOR_0* color);

	typedef void(__fastcall* tDrawRiseWindow)(YW::RECT* rect, YW::COLOR_0* color);

	typedef void(__fastcall* tDrawSinkWindow)(YW::RECT* rect, YW::COLOR_0* color);


	typedef void(__fastcall* tCameraLookAt)(kn_camera* a, Vector4* pos, Vector4* ref, double roll);


	typedef obj_camera* (__cdecl* tGetOBJCameraInstance)();


	typedef const OBJENTRY_0* (__fastcall* tgetOBJENTRY_DATA)(OBJENTRY_ID id);


	typedef void(__fastcall* tDBGWINaddMenuElem)(YS::ELEM* menu, int elem_max, YS::ELEM* elem);

	typedef float(__cdecl* tGetFunctFrameTime)();

	typedef Matrix4x4* (__fastcall* tMatrixBitwiseNot)(Matrix4x4* retstr, const Matrix4x4* const a);

	typedef Vector4* (__fastcall* tMatrixMultiplyVector)(Vector4* retstr, const Matrix4x4* a, const Vector4* v);

	typedef void(__fastcall* tYWWindow)(YW::WINDOW* a, const char* title, int x, int y, int w, int h, u_int option);

	typedef void(__fastcall* tSetoBJWishDir)(YS::OBJ* a, Vector4 dir);

	typedef void(__fastcall* tOpenCharacterLevelUpInformationWindow)(const char* name, const char* message0, const char* message1, int level, __int32 wincolor);

	typedef void(__fastcall* tInitObjCamera)(obj_camera* a);

	typedef TASK_5* (__fastcall* tTaskManagerCreateTask)(TASK_MANAGER_11* a, int id, int priority, TASK_FUNC entry_func);

	typedef void(__fastcall* tUpdateObjCamera)(obj_camera* a, bool always);

	typedef void(__fastcall* tSetCameraNumber)(int number);

	typedef int(__fastcall* tGetCameraNumber)();

	typedef void(__fastcall* tSetOBJCameraParam)(obj_camera* a, int type);

	typedef void(__fastcall* tPlayerPadToMovement)(void* outVec, const void* stickVec);

	typedef void(__fastcall* tReadPadTask)();



	extern InitDebugMenu initDebugMenu;
	extern tYWButton ywButton;
	extern tOpenSingletonWindow openSingletonWindow;
	extern tContainerLink containerLink;
	extern tYWLabel ywLabel;
	extern tDialWidget dialWidget;
	extern tGetOBJENTRY getOBJENTRY;
	extern tReadRequest readRequest;
	extern tFlushCacheBuff flushCacheBuff;
	extern tOBJENTRY_APPEAR OBJENTRY_APPEAR;
	extern tCreateFieldThread createFieldThread;
	extern tTaskSleep taskSleep;
	extern tAllocArea allocArea;
	extern tAlloc_YW_Memory allocYWMemory;
	extern tShowDebugWindow showDebugWindow;
	extern tDebugWindowAddSubMenu debugWindowAddSubMenu;
	extern tOpendbgWindow opendbgWindow;
	extern tWidget widget;
	extern tStatusPrintf statusPrintf;
	extern tSetObjPosition setObjPosition;
	extern tGetPlayer getPlayer;
	extern tYWFrame ywFrame;
	extern tYWListButton ywListButton;
	extern tYWToggleButton ywToggleButton;
	extern tOpenInformationWindow openInformationWindow;
	extern tCampMenuStart campMenuStart;
	extern tPlaySystemSEsound playSystemSEsound;
	extern tsoundCreate soundCreate;
	extern tSTDOBJ_Call_se stdobjCallSe;
	extern tControlEventOff controlEventOff;
	extern tControlEventOn controlEventOn;
	extern tCommonLayerOff commonLayerOff;
	extern tCommonLayerOn commonLayerOn;
	extern tPartyGet partyGet;
	extern tTargetSetObj targetSetObj;
	extern tSetCurrentBGM setCurrentBGM;
	extern tSetMember setMember;
	extern tGetMember getMember;
	extern tMapJump mapJump;
	extern tgbVoice gbVoice;
	extern tObjClick objClick;
	extern tGetObjEntryName getObjEntryName;
	extern tSetOBjRot setObjRot;
	extern tCreateObjCamera createObjCamera;
	extern tAttackStrike attackStrike;
	extern tCanControlObj canControlObj;
	extern tFriendTarget friendTarget;
	extern tObjActStart objActStart;
	extern tSetObjSpecialEffect setObjSpecialEffect;
	extern tAttachOBj attachOBj;
	extern tOBJAttachCamera objAttachCamera;
	extern tModelDrawShape modelDrawShape;
	extern tOBJgetEntryID objgetEntryID;
	extern tOBJscatterPrize objscatterPrize;
	extern tSetCommandItem setCommandItem;
	extern tGetPartyChara getPartyChara;
	extern tGetYWFontTextWidth getYWFontTextWidth;
	extern tSetWindowTitle setWindowTitle;
	extern tYWList ywList;
	extern tSpawnPlayer spawnPlayer;
	extern tGetSoftPad getSoftPad;
	extern tCommandElem commandElem;
	extern tGetDialValue getDialValue;
	extern tSetDialValue setDialValue;
	extern tYWFontPrint ywFontPrint;
	extern tConfigWidget configWidget;
	extern tConfigWidget2 configWidget2;
	extern tGameOver gameOver;
	extern tUnitGroupRandomPosPlayer unitGroupRandomPosPlayer;
	extern tGetObjPos getObjPos;
	extern tOpenTreasureEventInfo openTreasureEventInfo;
	extern tGetTreasureInfoNum getTreasureInfoNum;
	extern tTreasureTableGetEvent treasureTableGetEvent;
	extern tGetTreasureTableTreasureBox getTreasureTableTreasureBox;
	extern tInformationTimer informationTimer;
	extern tMissionStart missionStart;
	extern tActivateMission activateMission;
	extern tRestartTimer restartTimer;
	extern tActivateMissionTimer activateMissionTimer;
	extern tMission2DActivate mission2DActivate;
	extern tChangeForm changeForm;
	extern tChangeFormCapture changeFormCapture;
	extern tGetSora getSora;
	extern tFile_Read fileRead;
	extern tApplyPlayerParam applyPlayerParam;
	extern tSwitchStatus switchStatus;
	extern tGetHardPad getHardPad;
	extern tClearStatusWindow clearStatusWindow;
	extern tBattleReportPrintf battleReportPrintf;
	extern tGetEventProjectName getEventProjectName;
	extern tShowVersion showVersion;
	extern tRegionInit regionInit;
	extern tIsFreeAbility isFreeAbility;
	extern tSetEventSkipFlag setEventSkipFlag;
	extern tCacheBuffDestroyAll cacheBuffDestroyAll;
	extern tGetEventEffectWK getEventEffectWK;
	extern tPaxStart paxStart;
	extern tReadEventEffectSet readEventEffectSet;
	extern tCreateKnMemoryAllocator createKnMemoryAllocator;
	extern tMessageDraw_MessageDraw messageDraw_MessageDraw;
	extern tMessageDraw_Draw messageDraw_Draw;
	extern tRealMessageDraw realMessageDraw;
	extern tMessageSprintf messageSprintf;
	extern tTalkBubble talkBubble;
	extern tOBJ2DWindow obj2DWindow;
	extern tConfigSaveArea configSaveArea;
	extern tGetFieldMode getFieldMode;
	extern tStartGameMode startGameMode;
	extern tDBGWINsendFont dbgwinSendFont;
	extern tOpenMapJump openMapJump;
	extern tOpenNewWindow openNewWindow;
	extern tStopBGM stopBGM;
	extern tMatrixUnit matrixUnit;
	extern tDrawStuff drawStuff;
	extern tAllocDebugMem allocDebugMem;
	extern tTestFlagWidget testFlagWidget;
	extern tAppearPlayer appearPlayer;
	extern tFormCaptureEnd formCaptureEnd;
	extern tVU1DrawClose vu1DrawClose;
	extern tVU1DrawFlush vu1DrawFlush;
	extern tSOUNDgetlinknum soundGetLinkNum;
	extern tGetEventRestFrame getEventRestFrame;
	extern tGetAreaName getAreaName;
	extern tEachObj eachObj;
	extern tEachEnemy eachEnemy;
	extern tYWStatusWindow ywStatusWindow;
	extern tGetAreaAllocator getAreaAllocator;
	extern tPlayEventVoice playEventVoice;
	extern tGetOBJMenuObj getOBJMenuObj;
	extern tPartyHook partyHook;
	extern tGetCache_BuffPriority getCache_BuffPriority;
	extern tGetCache_BuffStatus getCache_BuffStatus;
	extern tGetMemoryAllocatorSize getMemoryAllocatorSize;
	extern tGetMemoryAllocatorUsedSize getMemoryAllocatorUsedSize;
	extern tGetCache_BuffFreesize getCache_BuffFreesize;
	extern tAllocToCache_Buff allocToCache_Buff;
	extern tReadCache_BuffRequest readCache_BuffRequest;
	extern tPaxInit paxInit;
	extern tLoadSaveGame loadSaveGame;
	extern tMenuSound_PlaySE menuSound_PlaySE;
	extern tConfigReflection configReflection;
	extern tVAG_STREAMPlay vagStreamPlay;
	extern tSetTitleShowVersionClear setTitleShowVersionClear;
	extern tFadeoutAllSounds fadeoutAllSounds;
	extern tSoftResetStartExit softResetStartExit;
	extern tGetObjDir getObjDir;
	extern tYW_WINDOW_set_min_size yw_window_set_min_size;
	extern tYW_WINDOW_set_max_size yw_window_set_max_size;
	extern tYS_Config_changeMoveSpeed ys_Config_changeMoveSpeed;
	extern tFreeYWButtonwidget freeYWButtonwidget;
	extern tFreeYWwindowwidget freeYWwindowwidget;
	extern tSetPartyWeapon setPartyWeapon;
	extern tParty_Hand_to_bone party_Hand_to_bone;
	extern tDetatchObject detatchObject;
	extern tDestroyCacheBuffPri destroyCacheBuffPri;
	extern tHideObjPri hideObjPri;
	extern tHideObjPart hideObjPart;
	extern tGetWeaponEntry getWeaponEntry;
	extern tGetMotionSetMotion getMotionSetMotion;
	extern tGetBinArcInfo getBinArcInfo;
	extern tTEST_OBJMENU_TEST_OBJMENU test_objmenu_test_objmenu;
	extern tOBJ_DrawSphere obj_DrawSphere;
	extern tGetObjSerial getObjSerial;
	extern tTargetSearch targetSearch;
	extern tSearchTargetBTLNPC searchTargetBTLNPC;
	extern tGetItemMessage getItemMessage;
	extern tDrawYWRect drawYWRect;
	extern tSendFont2Dbgwin sendFont2Dbgwin;
	extern tgb_messagedraw gb_messagedraw;
	extern tgmMessageDraw gmMessageDraw;
	extern tStopUnitScript stopUnitScript;
	extern tStartUnitScript startUnitScript;
	extern tFileReadBack fileReadBack;
	extern tStartVM startVM;
	extern tCallVM callVM;
	extern tExecuteVM executeVM;
	extern tVector4_Assign vector4_Assign;
	extern tfMatrix44_operator matrix44_operator;
	extern tMatrix4x4tOperator matrix4x4tOperator;
	//extern tMatrix4x4OperatorEquals matrix4x4OperatorEquals;
	extern tNormalizeVector4 normalizeVector4;
	//extern tMultiplyScalarVector4 multiplyScalarVector4;
	//extern tMathFixRadian mathFixRadian;
	//extern tVector4plusequalsOperator vector4plusequalsOperator;
	//extern tMatrixRotateY matrixRotateY;
	//extern tMatrixRotateX matrixRotateX;
	//extern tFixedOBJcamera fixedOBJcamera;
	extern tGetProjection getProjection;
	extern tknGetCamera knGetCamera;
	extern tGetProjectionFOV getProjectionFOV;
	extern tknCameraGetPos knCameraGetPos;
	extern tknCameraGetRef knCameraGetRef;
	extern tObjCameraCancelSubjective objCameraCancelSubjective;
	extern tInverseMatrix4x4 inverseMatrix4x4;
	extern tSoundCreateIOPvoice soundCreateIOPvoice;
	extern tSoundBGMFadeOut soundBGMFadeOut;
	extern tSoundVoicePlay soundVoicePlay;
	extern tClearCacheBuff clearCacheBuff;
	extern tDeleteObject deleteObject;
	extern tCreateGauge createGauge;
	extern tPrintYWtext printYWtext;
	extern tYWtextdraw_func ywtextdraw_func;
	extern tCameraLookAt cameraLookAt;
	extern tGetOBJCameraInstance getOBJCameraInstance;
	extern tgetOBJENTRY_DATA getOBJENTRY_DATA;
	extern tDBGWINaddMenuElem dbgwinAddMenuElem;
	extern tSora Sora;
	extern tInitializePlayerCommand InitializePlayerCommand;
	extern tTargetReplace TargetReplace;
	extern tOBJtexanmStart OBJtexanmStart;
	extern tknModelOBJResetStealth knModelOBJResetStealth;
	extern tYSMessageDraw YSMessageDraw;

}

