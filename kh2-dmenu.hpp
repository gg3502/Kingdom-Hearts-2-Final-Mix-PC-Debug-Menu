#pragma once

//#include "dllmain.hpp"
// ANSI Codes
//#include "ansi.hpp"
//#include "helper.hpp"
#include "patterns.hpp"
#include "AREA.hpp"
#include "OBJENRTY_0.hpp"
#include "Unit_Group.hpp"
#include "function_typedefs.hpp"
#include "Entity.hpp"
#include <CinematicCamera.h>


struct SpawnedEntityData {
    YS::OBJ* obj;               // Pointer to the live object, null if deleted
    Vector4 originalPosition;
    float originalRotation;
    OBJENTRY_ID entryId;
    bool isDeleted = false;     // Track if entity was removed during gameplay
};

float GetCurrentAmplitude();

extern YW::SCROLLBAR* g_ScaleSliderX;
extern YW::SCROLLBAR* g_ScaleSliderY;
extern YW::SCROLLBAR* g_ScaleSliderZ;
extern YW::SCROLLBAR* g_ScaleSliderW;
void UpdateScrollbar(YW::SCROLLBAR* slider, float* scaleTarget);

extern YW::SCROLLBAR* g_ScaleSliderPitch;
extern YW::SCROLLBAR* g_ScaleSliderPan;
extern YW::SCROLLBAR* g_ScaleSlider3DPos;
//extern YW::WINDOW* g_AudioPlayBackWindow;
void UpdateAudioParamsFromScrollbars();

extern bool isPreviewSpawned;
extern YS::OBJ* previewObject;
extern Entity* previewEntity;

extern Vector4 FreecamPos;
extern Vector4 FreecamTargetPos;
extern Vector4 AxisCursorPos;
extern OBJENTRY_ID g_SelectedEntityID;
extern char g_SelectedEntityName[64];
extern float FreecamSpeed;
extern int g_SpawnedEntityCount;
extern bool snapToGroundEnabled;
extern bool PartyModeToggle;
extern Fog* gCurrentFog;
extern std::atomic<float> g_currentAmplitude;
extern std::vector<YS::OBJ*> g_spawnedObjects;
extern std::vector<SpawnedEntityData> g_spawnedEntities;

extern void NewObj(OBJENTRY_ID entry_id, int spawn_amount);
extern void NewObj2(OBJENTRY_ID entry_id, int spawn_amount);
extern void NewLevelEditorObj(OBJENTRY_ID entry_id, int spawn_amount);

extern void NewCapture(OBJENTRY_ID entry_id);

void EventPlayerWidget();
extern int g_event_m_frame;
extern bool g_isPlaying;
extern YW::SCROLLBAR* g_FrameSlider;
void UpdateCutscenePlayer();
void UpdateCutsceneSliderManual();
void DebugSliderState();
extern YW::LABEL* g_CurrentFrameLabel;
extern float timeSinceLastSpawn;
extern float previewSpawnCooldown;
void UpdatePreviewSpawnTimer();

extern void UpdateCinematicSlidersFromUI();
extern bool g_cineGameplayEnabled;
extern YS::OBJ* g_player2Obj;

extern void DebugLoop();
extern void CheckRoomLoop();
extern void ReadUnitPointers();
extern AREA ReadSteamAreaPointer(AREA& areaPointer);
extern std::string GetPublicIP();
extern std::string DetectGameVersion();
extern void InitializePointers();
extern uintptr_t SteamPointer;
extern uintptr_t EpicPointer;
extern void NewWorldPosObj(OBJENTRY_ID entry_id, int spawn_amount, Vector4 WorldPos);
extern bool FreeCameraToggle;
extern bool LevelEditorToggle;
extern bool EntityMovementToggle;
extern bool CinematicCameraToggle;
extern bool freecamInitialized;
extern bool g_cineShowSpline;
extern UNIT_GROUP* g_unitGroup;
extern bool isCustomMusicPlaying;
extern bool isPlaying;
extern YS::OBJ* gGlobalObjPtr;
extern YS::OBJ* gGlobalObjPtr1;
extern CinematicCamera g_cinematicCamera;
extern float g_cineTimelineTime;
extern void CreateFreeCamera();
extern void DestroyCinematicCamera();
extern void DestroyFreeCamera();
extern void UpdateFreeCamera();
extern void CreateEntityMovement();
extern void DestroyEntityMovement();
extern void UpdateEntityMovement();
extern DirectX::XMVECTOR myXMVECTOR;


extern uintptr_t cachedGetPlayerAddress;
extern uintptr_t cachedEachObjAddress;
extern uintptr_t cachedGetObjEntryNameAddress;
extern uintptr_t cachedGetOBJENTRYAddress;
extern uintptr_t cachedGetObjBoneMatrixAddress;
extern uintptr_t cachedCalcProjectionMatrixAddress;
extern uintptr_t cachedGetKNCameraAddress;
extern uintptr_t cachedGetSoftPadAddress;
extern uintptr_t cachedGetFuncFrameTimeAddress;

extern uintptr_t cachedMatrixBitwiseNotAddress;
extern uintptr_t cachedAssignMatrixAddress;
extern uintptr_t cachedMatrixMultiplyVectorAddress;
extern uintptr_t cachedNormalizeVector4Address;
extern uintptr_t cachedMultiplyByScalarAddress;
extern uintptr_t cachedNormalizeRadianAddress;
extern uintptr_t cachedAddAssignAddress;
extern uintptr_t cachedInitializeAsIdentityMatrixAddress;
extern uintptr_t cachedApplyMatrixRotationXAddress;
extern uintptr_t cachedApplyMatrixRotationYAddress;
extern uintptr_t cachedAdjustEyeAndAtTargetsAddress;
extern uintptr_t cachedGetHardPadAddress;
extern uintptr_t cachedCopyFromAddress;
extern uintptr_t cachedCancelSubjectiveAddress;
extern uintptr_t cachedGetOBJCameraInstanceAddress;
extern uintptr_t cachedGetOBJControlOFFAddress;
extern uintptr_t cachedGetOBJControlONAddress;
extern uintptr_t cachedSetKNCameraEyeAddress;
extern uintptr_t cachedSetKNCameraAtAddress;
extern uintptr_t cachedStatusPrintfAddress;
extern uintptr_t cachedCommonLayerOFFAddress;
extern uintptr_t cachedCommonLayerONAddress;
extern uintptr_t cachedDBGWINPauseAddress;
extern uintptr_t cachedDBGWINPauseOffAddress;
extern uintptr_t cachedGetKNCameraModeAddress;
extern uintptr_t cachedSetKNCameraModeAddress;
extern uintptr_t cachedGetScreenPosAddress;
extern uintptr_t cachedGetProjectionMatrixAddress;
extern uintptr_t cachedGetMenuOBJFunctionAddress;
extern uintptr_t cachedDecodePointerFunctionAddress;
extern uintptr_t cachedSoundGetLinkNumFunctionAddress;
extern uintptr_t cachedGetEventRestFrameFunctionAddress;
extern uintptr_t cachedGetEventProjectNameFunctionAddress;
extern uintptr_t YSDrawMessageFunctionAddress;


extern FunctionCalls::tGetPlayer GetPlayerFunction;
extern FunctionCalls::tEachObj EachOBJFunction;
extern FunctionCalls::tGetObjEntryName GetOBJEntryNameFunction;
extern FunctionCalls::tGetOBJMenuObj GetMenuOBJFunction;

extern FunctionCalls::tGetOBJBoneMatrix GetOBJBoneMatrixFunction;
extern FunctionCalls::tCalcProjectionMatrix GetCalcProjectionMatrixFunction;
extern FunctionCalls::tknGetCamera GetKNCameraFunction;
extern FunctionCalls::tGetSoftPad GetSoftPadFunction;
extern FunctionCalls::tGetFunctFrameTime GetFuncFrametimeFunction;

extern FunctionCalls::tMatrixBitwiseNot MatrixBitwiseNotFunction;
extern FunctionCalls::tAssignMatrix AssignMatrixFunction;
extern FunctionCalls::tMatrixMultiplyVector MatrixMultiplyVectorFunction;
extern FunctionCalls::tNormalizeVector4 NormalizeVector4Function;
extern FunctionCalls::tMultiplyByScalar MultiplyByScalarFunction;
extern FunctionCalls::tNormalizeRadian NormalizeRadianFunction;
extern FunctionCalls::tAddAssign AddAssignFunction;
extern FunctionCalls::tInitializeAsIdentityMatrix InitializeAsIdentityMatrixFunction;
extern FunctionCalls::tApplyMatrixRotationX ApplyMatrixRotationXFunction;
extern FunctionCalls::tApplyMatrixRotationY ApplyMatrixRotationYFunction;
extern FunctionCalls::tAdjustEyeAndAtTargets AdjustEyeAndAtTargetsFunction;
extern FunctionCalls::tGetHardPad GetHardPadFunction;
extern FunctionCalls::tVector4_Assign CopyFromAddressFunction;
extern FunctionCalls::tObjCameraCancelSubjective CancelSubjectiveFunction;
extern FunctionCalls::tGetOBJCameraInstance GetOBJCameraInstanceFunction;
extern FunctionCalls::tControlEventOff GetOBJControlOFFunction;
extern FunctionCalls::tControlEventOn GetOBJControlONFunction;
extern FunctionCalls::tknCameraSetEye SetKNCameraEyeFunction;
extern FunctionCalls::tknCameraSetAt SetKNCameraAtFunction;
extern FunctionCalls::tStatusPrintf StatusPrintfFunction;
extern FunctionCalls::tCommonLayerOff CommonLayerOFFFunction;
extern FunctionCalls::tCommonLayerOn CommonLayerONFunction;
extern FunctionCalls::tDebugPause DBGWINPauseFunction;
extern FunctionCalls::tDebugPauseOff DBGWINPauseOffFunction;
extern FunctionCalls::tGetOBJENTRY GetOBJENTRYFunction;
extern FunctionCalls::tGetKNCameraMode GetKNCameraModeFunction;
extern FunctionCalls::tSetKNCameraMode SetKNCameraModeFunction;
extern FunctionCalls::tGetScreenPos GetScreenPosFunction;
extern FunctionCalls::tGetProjection GetProjectionMatrixFunction;
extern FunctionCalls::tDecodePointer DecodePointerFunction;
extern FunctionCalls::tSOUNDgetlinknum SoundGetLinkNumFunction;
extern FunctionCalls::tGetEventRestFrame GetEventRestFrameFunction;
extern FunctionCalls::tGetEventProjectName GetEventProjectNameFunction;
extern FunctionCalls::tYSMessageDraw YSDrawMessageFunction;
extern FunctionCalls::tInitObjCamera InitOBJCameraFunction;
extern FunctionCalls::tTaskManagerCreateTask TaskManagerCreateTaskFunction;
extern FunctionCalls::tGetFieldMode GetFieldModeFunction;
extern FunctionCalls::tUpdateObjCamera UpdateObjCameraFunction;
extern FunctionCalls::tSetCameraNumber SetCameraNumberFunction;
extern FunctionCalls::tGetCameraNumber GetCameraNumberFunction;
extern FunctionCalls::tSetOBJCameraParam SetObjCameraParamFunction;
extern FunctionCalls::tPlayerPadToMovement PlayerPadToMovementFunction;
//extern FunctionCalls::tReadPadTask ReadPadTaskFunction;
// Applies or restores NOP patches to disable/enable in-game camera matrix writes
void PatchCameraInstructions(uintptr_t moduleBase, bool enable);

// Updates free camera each frame by overriding the game's view matrix
void UpdateFreeCamera_ViewMatrix(uintptr_t viewMatrixAddr);
void WriteValidViewMatrix(uintptr_t viewMatrixAddr);
extern Vector4 vm_lookat;
extern Vector4 vm_eye;






extern void UpdateFunctionAddresses();