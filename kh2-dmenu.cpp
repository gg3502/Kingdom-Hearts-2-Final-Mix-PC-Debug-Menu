#include <stdint.h>
// snprintf_s
#include <stdio.h>
// memset
#include <string.h>
// va args
#include <stdarg.h>
// always_assert
#include "assert.hpp"

#include "helper.hpp"
#include "dllmain.hpp"
// ANSI Codes
#include "ansi.hpp"

//#include "kh2-dmenu.hpp"
#include <vector>
#include <iostream>
#include <Windows.h>
#include <direct.h>
#include <corecrt_io.h>
#include "YW.hpp"
//#include "patterns.hpp"
//#include "helper.hpp"
#ifdef max
#undef max
#endif
#include "function_typedefs.hpp"
#include "function_ptr.hpp"
#include <mmsystem.h>
#pragma comment(lib, "winmm.lib")
//#include "function_ptr.hpp"
#include "kh2-dmenu.hpp"
#include "P2PServer.hpp"
#include <thread>
#include <winhttp.h>
#include <map>
#include "inipp.h"
#include "Unit_Group.hpp"
#include "FloatEditor.hpp"
#include <random>
#include "Entity.hpp"
#include "KN.hpp"
#include "FileSelection.h"
#include <filesystem>
#include "MenuRects.hpp"
#include "pa.hpp"
#include "ChaosManager.hpp"
#include "TwitchChat.hpp"
#include "TwitchConfig.hpp"
ChaosManager chaosManager;
TwitchConfig twitchConfig;
#include "KHSCII.hpp"
#include <DirectXMath.h>
#include "CinematicCamera.h"

using namespace DirectX;

#ifdef min
#undef min
#endif
#include "hook_directx12.hpp"

#define MINIAUDIO_IMPLEMENTATION
#include "miniaudio.h"
#include <EffectManager.hpp>
#include <VoteManager.hpp>
#include <imgui.h>
#include <Fonts.hpp>
#include "fog.hpp"
//#include <d3d12.h>
//#include "ImguiTextureManager.h"




Vector4 vm_eye = { 0,0,0,1 };
Vector4 vm_lookat = { 0,0,0,1 };

static float yaw = 0.0f;
static float pitch = 0.0f;

bool freecamInitialized = false;

static const char** PngTexturesList = nullptr;      // null-terminated list for YWList
static std::vector<std::wstring> g_PngFiles;        // full paths
static bool g_PngListLoaded = false;

std::wstring g_SelectedPngFullPath;   // full path to active texture
std::string  g_SelectedPngName;       // filename for display/logging
int          g_SelectedPngIndex = -1; // optional

//// Optional: keep selection stable between rebuilds
//static int g_SelectedPngIndex = 0;

YS::OBJ* g_player2Obj = nullptr;

// Global cinematic camera instance
CinematicCamera g_cinematicCamera;

//float g_cineSpeed = 1.0f;   // affects how fast we move along keyframes
float g_cineTimeScale = 1.0f;   // optional: global time scale
int   g_cineSelectedKF = -1;     // index of “current” keyframe for menu ops


// Simple timeline cursor (what “time” the next keyframe will be at)
float g_cineTimelineTime = 0.0f;

int g_SpawnedEntityCount = 0;

std::vector<YS::OBJ*> g_spawnedObjects;

// UI list that YWList consumes
//static const char** PngTexturesList = nullptr;

// Full paths for each entry in the UI list
//static std::vector<std::wstring> g_PngFiles;





YS::OBJ* previewObject = nullptr;
Entity* previewEntity = nullptr;
float previewBlinkTimer = 0.0f;
bool previewVisible = true;
OBJENTRY_ID previewEntityId = -1;  // Currently previewed ID
bool isSpawningPreview = false;
bool isPreviewSpawned = false;


std::vector<SpawnedEntityData> g_spawnedEntities;

float g_SpawnRotation = 0.0f;

float previewSpawnCooldown = 0.3f;
float timeSinceLastSpawn = 999.0f;

static uintptr_t g_GameSpeedAddress = 0;


std::vector<const char*> AllActStrings;

static bool originalFogSaved = false;
static Fog originalFog;
Fog* gCurrentFog = nullptr;

bool FreecamInitialized = false;


int OBJColorR = 128;
int OBJColorG = 128;
int OBJColorB = 128;
int OBJColorA = 128;

OBJENTRY_ID g_SelectedEntityID = (OBJENTRY_ID)-1;
char g_SelectedEntityName[64] = "None";

Vector4 AxisCursorPos = { 0.0f, 0.0f, 0.0f, 0.0f };

std::atomic<float> g_currentAmplitude{ 0.0f };


ma_device g_device;

MenuUI g_MenuUI;

uintptr_t moduleBase1 = (uintptr_t)GetModuleHandle(L"KINGDOM HEARTS II FINAL MIX.exe");

namespace fs = std::filesystem;

#pragma comment(lib, "Winhttp.lib")

YW::FLOAT_EDITOR* editor = nullptr;

std::vector<std::wstring> playlist;

const char** playlistList;

std::vector<std::wstring> g_CineCamFiles;   // full paths
const char** CinematicCamerasList = nullptr;
bool g_CineListLoaded = false;
bool g_CineSettingsOpen = false;
YW::YW_WINDOW_39* g_CineSettingsWindow = nullptr;
bool g_cineAutoPlayOnce = false;
bool g_cineGameplayEnabled = false;

void CinematicCameraSettingsMenuCallback(unsigned int* arg);

FileSelectionWidget* g_FileSelectionWidget = nullptr;

//std::vector<const char*> PlayerList;

const char* PlayerList[1024];



const char* ZakoList[1024];
size_t zakoCount = 0; 
constexpr int maxZakoCount = 1024;

constexpr int maxEntryCount = 1024;

struct EntryMenuContext {
    const char* entries[maxEntryCount + 1]; // +1 for null terminator
    size_t count = 0;
};

YW::WINDOW* PaEditerWindow = nullptr;

bool snapToGroundEnabled = false;

std::vector<std::string> bdxfileNames;
const char** bdxfileNamesArray = new const char* [bdxfileNames.size() + 1];

EntryMenuContext npcMenu;
EntryMenuContext bossMenu;
EntryMenuContext zakoMenu;
EntryMenuContext weaponMenu;
EntryMenuContext enemyWeaponMenu;
EntryMenuContext savepointMenu;
EntryMenuContext fieldObjMenu;
EntryMenuContext battleNpcMenu;
EntryMenuContext treasureBoxMenu;
EntryMenuContext submenuMenu;
EntryMenuContext largeBossMenu;
EntryMenuContext gimmickObjMenu;
EntryMenuContext memoMenu;
EntryMenuContext rtnMenu;
EntryMenuContext minigameMenu;
EntryMenuContext worldMapMenu;
EntryMenuContext prizeBoxMenu;
EntryMenuContext summonMenu;
EntryMenuContext shopPointMenu;
EntryMenuContext largeZakoMenu;
EntryMenuContext massEffectMenu;
EntryMenuContext enemyObjMenu;
EntryMenuContext jigsawMenu;

const char* List[1024];

const char** PlayerListList = nullptr;

int WidgetSpawnAmountValue = 1;

size_t playerCount = 0;
size_t previousPlayerCount = 0;

bool hostlistLoaded = false;

std::vector<std::wstring> hostlist;

std::vector<std::wstring> ipAddresses;
std::vector<std::wstring> usernames;

const char** hostlistList;

UNIT_GROUP* g_unitGroup = nullptr;

YW::YW_WINDOW_39* OBJENTRYMenu = nullptr;

//size_t loopingOptionIndex = LOOPING_OPTION_INDEX;

bool isLooping = false; // You can set this variable as needed

bool isPlaying = false;

ma_engine g_engine;              // global audio engine
ma_sound g_sound;                // current playing sound
bool g_soundInitialized = false;

YW::SCROLLBAR* g_ScaleSliderPitch;
YW::SCROLLBAR* g_ScaleSliderPan;
YW::SCROLLBAR* g_ScaleSlider3DPos;

float g_currentPan = 0.0f;
float g_currentPitch = 1.0f;      // your own pitch variable (because miniaudio doesn't track pitch)
float g_current3DPos = 0.0f;      // if 1D for simplicity or get x/y/z from ma_sound_get_position
//YW::WINDOW* g_AudioPlayBackWindow;

ma_decoder g_decoder;
bool g_decoderInitialized = false;

bool g_cineShowSpline = true;


bool playlistLoaded = false;

int Entry1CallbackID = 1;
int Entry1CallbackSpawnAmount = 1;
int Entry2CallbackID = 1;
int Entry2CallbackSpawnAmount = 1;
int Entry3CallbackID = 1;
int Entry3CallbackSpawnAmount = 1;
int Entry4CallbackID = 1;
int Entry4CallbackSpawnAmount = 1;
int Entry5CallbackID = 1;
int Entry5CallbackSpawnAmount = 1;
int PlayerEntryCallBackID = 1;

int IdRequest1 = 0;
int obj_spawnamount1 = 0;
int IdRequest2 = 0;
int obj_spawnamount2 = 0;
int IdRequest3 = 0;
int obj_spawnamount3 = 0;
int IdRequest4 = 0;
int obj_spawnamount4 = 0;
int IdRequest5 = 0;
int obj_spawnamount5 = 0;
int PlayerIdRequest = 0;

bool IsEntry1Enabled = false;
bool IsEntry2Enabled = false;
bool IsEntry3Enabled = false;
bool IsEntry4Enabled = false;
bool IsEntry5Enabled = false;
bool IsPlayerEntryEnabled = false;
bool IsPlayerFormOut = false;

int OBJMaxHP = 0;
int OBJCurrentHP = 0;
int OBJDefense = 0;
int OBJAttack = 0;

int gObjectScale = 0;
int gObjectTexanmID = 1;

struct SpawnParams {
    OBJENTRY_ID objId;
    int spawnAmount;
};

YW::SCROLLBAR* g_ScaleSliderX;
YW::SCROLLBAR* g_ScaleSliderY;
YW::SCROLLBAR* g_ScaleSliderZ;
YW::SCROLLBAR* g_ScaleSliderW;

YW::SCROLLBAR* g_CineSlider;
YW::SCROLLBAR* g_TimeScaleSlider;
YW::SCROLLBAR* g_TimelineScaleSlider;

YW::SCROLLBAR* g_FrameSlider;
bool g_isPlaying = false;
float g_frameTimer = 0.0f;
const float FRAME_DURATION = 1.0f / 30.0f;  // for 30 FPS

YW::LABEL* g_CurrentFrameLabel;

float g_event_m_time = 1.0f;
int g_event_m_frame = *(int*)(moduleBase1 + 0xB64F98);
int g_event_end_frame = *(int*)(moduleBase1 + 0xB64FB4);

BYTE original_addss[4];    // Length must match instruction size
BYTE original_movss[8];
BYTE original_mov_frame[6];

uintptr_t TestCodeAddress = moduleBase1 + 0x26AE2BB;
unsigned int* DebugInfo2 = reinterpret_cast<unsigned int*>(TestCodeAddress);

//YW::WINDOW* InputMenu = nullptr;
unsigned int* SpawnAmountDialMem;

YW::YW_WINDOW_39* InputMenu = nullptr;

YW::YW_WINDOW_39* ActorInputMenu = nullptr;

YW::YW_WINDOW_39* YesNoMenu = nullptr;


//std::wstring currentDir = L"DebugMenu\\music"; // Set the directory where your music files are stored
std::wstring currentSoundFile;
size_t currentSongIndex = 0;
bool isCustomMusicPlaying = false;

std::wstring currentPath = L"DebugMenu\\music";  // Dynamic path
//std::vector<std::wstring> playlist;
std::vector<bool> isDirectoryList;

const size_t LOOPING_OPTION_INDEX = std::numeric_limits<size_t>::max();
size_t loopingOptionIndex = LOOPING_OPTION_INDEX;
OBJENTRY_ID myObjEntryId = 0;
// Define the function pointer type
using CallbackType = void (*)(OBJENTRY_ID);
// Declare a global variable of the function pointer type
CallbackType globalCallback = nullptr;
int IdRequest1i = 0;
int obj_spawnamount1i = 0;
int pri = 0xA8C; // A8C
int bank = 30; // 30
u_char entry_type = 0;
Vector4 ObjSpawnWorldPos = { 0.0f, 0.0f, 0.0f, 0.0f };
uintptr_t SteamPointer;
uintptr_t EpicPointer;
std::string gameVersion = DetectGameVersion();
YS::OBJ* gGlobalObjPtr = nullptr;
YS::OBJ* gGlobalObjPtr1 = nullptr;
bool FreeCameraToggle = false;
bool LevelEditorToggle = false;
bool EntityMovementToggle = false;
bool CinematicCameraToggle = false;

bool PartyModeToggle = false;

const char* TypeStrings[25] = { "PLAYER","FRIEND","NPC","BOSS","ZAKO","WEAPON","E_WEAPON","SAVEPOINT","FIELDOBJ","BTLNPC","TREASURE_BOX","SUBMENU","LARGEBOSS","GIMMICKOBJ","MEMO","RTN","MINIGAME","WORLDMAP","PRIZEBOX","SUMMON","SHOPPOINT","L_ZAKO","MASSEFFECT","ENEMYOBJ","JIGSAW" };

//typedef YS::OBJ* (__cdecl* tGetPlayer)();
//tGetPlayer GetPlayer = (tGetPlayer)(moduleBase + 0x3B3B40);

typedef void(__cdecl* createmulticamera_t)();
createmulticamera_t createmulticamera = (createmulticamera_t)(moduleBase1 + 0x162C50);


//const char** TypeElem = new const char* [25];


/*void AddPlayerEntries() {
    PlayerList.clear();  // Clear the previous entries
    PlayerList.push_back("AL14_PLAYER");
    PlayerList.push_back("DEAD_BOSS");
    PlayerList.push_back("F_NM180");
    PlayerList.push_back("LAST_ATTACKER");
    PlayerList.push_back("LAST_GIMMICK");
    PlayerList.push_back("LAST_HITMARK");
    PlayerList.push_back("M_EX950_CARD");
    PlayerList.push_back("M_EX950_DICE");
    PlayerList.push_back("PLAYER");
    PlayerList.push_back("PO06_PLAYER");
    PlayerList.push_back("P_EX100");
    PlayerList.push_back("M_EX950_DICE");
    PlayerList.push_back("P_EX100_AL_CARPET");
    PlayerList.push_back("P_EX100_BTLF");
    PlayerList.push_back("P_EX100_HTLF");
    PlayerList.push_back("P_EX100_KH1F");
    PlayerList.push_back("P_EX100_MAGF");
    PlayerList.push_back("P_EX100_NM");
    PlayerList.push_back("P_EX100_NM_BTLF");
    PlayerList.push_back("P_EX100_NM_HTLF");
    PlayerList.push_back("P_EX100_NM_KH1F");
    PlayerList.push_back("P_EX100_NM_MAGF");
    PlayerList.push_back("P_EX100_NM_TRIF");
    PlayerList.push_back("P_EX100_NM_ULTF");
    PlayerList.push_back("P_EX100_SIDECAR");
    PlayerList.push_back("P_EX100_TR");
    PlayerList.push_back("P_EX100_TRIF");
    PlayerList.push_back("P_EX100_TR_BTLF");
    PlayerList.push_back("P_EX100_TR_BTLF");
    PlayerList.push_back("P_EX100_TR_HTLF");
    PlayerList.push_back("P_EX100_TR_KH1F");
    PlayerList.push_back("P_EX100_TR_LIGHTCYCLE");
    PlayerList.push_back("P_EX100_TR_MAGF");
    PlayerList.push_back("P_EX100_TR_TRIF");
    PlayerList.push_back("P_EX100_TR_ULTF");
    PlayerList.push_back("P_EX100_ULTF");
    PlayerList.push_back("P_EX100_WI");
    PlayerList.push_back("P_EX100_WI_BTLF");
    PlayerList.push_back("P_EX100_WI_HTLF");
    PlayerList.push_back("P_EX100_WI_KH1F");
    PlayerList.push_back("P_EX100_WI_MAGF");
    PlayerList.push_back("P_EX100_WI_TRIF");
    PlayerList.push_back("P_EX100_WI_ULTF");
    PlayerList.push_back("P_EX100_XM");
    PlayerList.push_back("P_EX100_XM_BTLF");
    PlayerList.push_back("P_EX100_XM_HTLF");
    PlayerList.push_back("P_EX100_XM_KH1F");
    PlayerList.push_back("P_EX100_XM_MAGF");
    PlayerList.push_back("P_EX100_XM_TRIF");
    PlayerList.push_back("P_EX100_XM_ULTF");
    PlayerList.push_back("P_EX110");
    PlayerList.push_back("P_EX110_BTLF");
    PlayerList.push_back("P_EX120");
    PlayerList.push_back("P_EX200");
    PlayerList.push_back("P_EX120");
    PlayerList.push_back("P_EX200");
    PlayerList.push_back("P_EX210");
    PlayerList.push_back("P_EX220");
    PlayerList.push_back("P_LK100");
    PlayerList.push_back("P_LM100");

}*/

static void FreeCStringList(const char**& list)
{
    if (!list) return;
    for (size_t i = 0; list[i] != nullptr; ++i)
        delete[] list[i];
    delete[] list;
    list = nullptr;
}



struct CameraPatch
{
    uintptr_t address;
    std::vector<uint8_t> originalBytes;
    std::vector<uint8_t> patchBytes;
};

CameraPatch camPatches[] = {
    { 0x1657FB, {}, { 0x90, 0x90, 0x90, 0x90, 0x90 } },
    { 0x165837, {}, { 0x90, 0x90, 0x90, 0x90, 0x90 } },
    { 0x125E01, {}, {
        0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90,
        0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90,
        0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90,
        0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90,
        0x90, 0x90, 0x90
    } }
};

void PatchCameraInstructions(uintptr_t moduleBase, bool enable)
{
    DWORD oldProtect;
    for (auto& patch : camPatches)
    {
        uintptr_t address = moduleBase + patch.address;
        size_t patchSize = patch.patchBytes.size();

        if (enable)
        {
            // Save original bytes before patching
            patch.originalBytes.resize(patchSize);
            memcpy(patch.originalBytes.data(), reinterpret_cast<void*>(address), patchSize);

            VirtualProtect(reinterpret_cast<void*>(address), patchSize, PAGE_EXECUTE_READWRITE, &oldProtect);
            memcpy(reinterpret_cast<void*>(address), patch.patchBytes.data(), patchSize);
            VirtualProtect(reinterpret_cast<void*>(address), patchSize, oldProtect, &oldProtect);
        }
        else
        {
            // Restore original bytes
            if (!patch.originalBytes.empty())
            {
                VirtualProtect(reinterpret_cast<void*>(address), patch.originalBytes.size(), PAGE_EXECUTE_READWRITE, &oldProtect);
                memcpy(reinterpret_cast<void*>(address), patch.originalBytes.data(), patch.originalBytes.size());
                VirtualProtect(reinterpret_cast<void*>(address), patch.originalBytes.size(), oldProtect, &oldProtect);
            }
        }
    }
}

void AddPlayerEntries() {
    PlayerList[0] = "AL14_PLAYER";
    PlayerList[1] = "DEAD_BOSS";
    PlayerList[2] = "F_NM180";
    PlayerList[3] = "LAST_ATTACKER";
    PlayerList[4] = "LAST_GIMMICK";
    PlayerList[5] = "LAST_HITMARK";
    PlayerList[6] = "M_EX950_CARD";
    PlayerList[7] = "M_EX950_DICE";
    PlayerList[8] = "PLAYER";
    PlayerList[9] = "PO06_PLAYER";
    PlayerList[10] = "P_EX100";
    PlayerList[11] = "P_EX100_AL_CARPET";
    PlayerList[12] = "P_EX100_BTLF";
    PlayerList[13] = "P_EX100_HTLF";
    PlayerList[14] = "P_EX100_KH1F";
    PlayerList[15] = "P_EX100_MAGF";
    PlayerList[16] = "P_EX100_NM";
    PlayerList[17] = "P_EX100_NM_BTLF";
    PlayerList[18] = "P_EX100_NM_HTLF";
    PlayerList[19] = "P_EX100_NM_KH1F";
    PlayerList[20] = "P_EX100_NM_MAGF";
    PlayerList[21] = "P_EX100_NM_TRIF";
    PlayerList[22] = "P_EX100_NM_ULTF";
    PlayerList[23] = "P_EX100_SIDECAR";
    PlayerList[24] = "P_EX100_TR";
    PlayerList[25] = "P_EX100_TRIF";
    PlayerList[26] = "P_EX100_TR_BTLF";
    PlayerList[27] = "P_EX100_TR_HTLF";
    PlayerList[28] = "P_EX100_TR_KH1F";
    PlayerList[29] = "P_EX100_TR_LIGHTCYCLE";
    PlayerList[30] = "P_EX100_TR_MAGF";
    PlayerList[31] = "P_EX100_TR_TRIF";
    PlayerList[32] = "P_EX100_TR_ULTF";
    PlayerList[33] = "P_EX100_ULTF";
    PlayerList[34] = "P_EX100_WI";
    PlayerList[35] = "P_EX100_WI_BTLF";
    PlayerList[36] = "P_EX100_WI_HTLF";
    PlayerList[37] = "P_EX100_WI_KH1F";
    PlayerList[38] = "P_EX100_WI_MAGF";
    PlayerList[39] = "P_EX100_WI_TRIF";
    PlayerList[40] = "P_EX100_WI_ULTF";
    PlayerList[41] = "P_EX100_XM";
    PlayerList[42] = "P_EX100_XM_BTLF";
    PlayerList[43] = "P_EX100_XM_HTLF";
    PlayerList[44] = "P_EX100_XM_KH1F";
    PlayerList[45] = "P_EX100_XM_MAGF";
    PlayerList[46] = "P_EX100_XM_TRIF";
    PlayerList[47] = "P_EX100_XM_ULTF";
    PlayerList[48] = "P_EX110";
    PlayerList[49] = "P_EX110_BTLF";
    PlayerList[50] = "P_EX120"; 
    PlayerList[51] = "P_EX200";
    PlayerList[52] = "P_EX210";
    PlayerList[53] = "P_EX220";
    PlayerList[54] = "P_LK100";
    PlayerList[55] = "P_LM100";
}

unsigned int PlayerList_values[] = { 0x5E9, 0x68F, 0x3C8, 0x386, 0x6BE, 0x4B4, 0x601, 0x602, 0x236, 0x35B, 0x54, 0x4FF, 0x55, 0x59, 0x95D, 0x56, 0x2B5, 0x3E6, 0x3EA, 0x95E, 0x3E7, 0x3E8, 0x3E9, 0x840, 0x656, 0x57, 0x669, 0x671, 0x960, 0x5F5, 0x66B, 0x66D, 0x66F, 0x58, 0x657, 0x66A, 0x672, 0x961, 0x66C, 0x66E, 0x670, 0x955, 0x956, 0x95A, 0x95F, 0x957, 0x958, 0x959, 0x5A, 0x323, 0x6C1, 0x5B, 0x869, 0x318, 0x28A, 0x3BE };



enum YS_OBJENTRY_TYPE {
    TYPE_PLAYER_9 = 0x0,
    TYPE_FRIEND_4 = 0x1,
    TYPE_NPC = 0x2,
    TYPE_BOSS_0 = 0x3,
    TYPE_ZAKO = 0x4,
    TYPE_WEAPON_0 = 0x5,
    TYPE_E_WEAPON = 0x6,
    TYPE_SAVEPOINT_0 = 0x7,
    TYPE_FIELDOBJ = 0x8,
    TYPE_BTLNPC_0 = 0x9,
    TYPE_TREASURE_BOX = 0xA,
    TYPE_SUBMENU_0 = 0xB,
    TYPE_LARGEBOSS = 0xC,
    TYPE_GIMMICKOBJ = 0xD,
    TYPE_MEMO = 0xE,
    TYPE_RTN = 0xF,
    TYPE_MINIGAME_3 = 0x10,
    TYPE_WORLDMAP_1 = 0x11,
    TYPE_PRIZEBOX = 0x12,
    TYPE_SUMMON_2 = 0x13,
    TYPE_SHOPPOINT = 0x14,
    TYPE_L_ZAKO = 0x15,
    TYPE_MASSEFFECT = 0x16,
    TYPE_ENEMYOBJ = 0x17,
    TYPE_JIGSAW = 0x18,
    TYPE_MAX_41 = 0x19,
    TYPE_LEVELEDITOR_OBJ = 0x20
};

char* PartsSelectorLabels[33] = {
    (char*)"  00", (char*)"  01", (char*)"  02", (char*)"  03", (char*)"  04",
    (char*)"  05", (char*)"  06", (char*)"  07", (char*)"  08", (char*)"  09",
    (char*)"  10", (char*)"  11", (char*)"  12", (char*)"  13", (char*)"  14",
    (char*)"  15", (char*)"  16", (char*)"  17", (char*)"  18", (char*)"  19",
    (char*)"  20", (char*)"  21", (char*)"  22", (char*)"  23", (char*)"  24",
    (char*)"  25", (char*)"  26", (char*)"  27", (char*)"  28", (char*)"  29",
    (char*)"  30", (char*)"  31", nullptr 
};

char* PartsSelectorLabelsSelected[33] = {
    (char*)"X 01", (char*)"X 02", (char*)"X 03", (char*)"X 04", (char*)"X 05",
    (char*)"X 06", (char*)"X 07", (char*)"X 08", (char*)"X 09", (char*)"X 10",
    (char*)"X 11", (char*)"X 12", (char*)"X 13", (char*)"X 14", (char*)"X 15",
    (char*)"X 16", (char*)"X 17", (char*)"X 18", (char*)"X 19", (char*)"X 20",
    (char*)"X 21", (char*)"X 22", (char*)"X 23", (char*)"X 24", (char*)"X 25",
    (char*)"X 26", (char*)"X 27", (char*)"X 28", (char*)"X 29", (char*)"X 30",
    (char*)"X 31", nullptr // nullptr to mark the end if needed
};

Vector4 FreecamPos = {};
Vector4 FreecamTargetPos = {};
Vector4 CinematicCamPos = {};
Vector4 CinematicCamTargetPos = {};


float FreecamYaw = 0.0f;
float FreecamPitch = 0.0f;
float CinematicCamYaw = 0.0f;
float CinematicCamPitch = 0.0f;

DirectX::XMVECTOR myXMVECTOR = DirectX::XMLoadFloat4(reinterpret_cast<const DirectX::XMFLOAT4*>(&FreecamPos));

bool isPaused = false;
float FreecamSpeed = 136.0f;
float CinematicCamSpeed = 136.0f;


typedef void(__fastcall* DestroyFunc)(void* thisPtr);
typedef void(__fastcall* DrawFunc)(void* thisPtr, const YW::RECT* rect, const YW::COLOR_0* color);
typedef void(__fastcall* FocusFunc)(void* thisPtr, const YW::EVENT* event, int x, int y);

struct WidgetVTable {
    DestroyFunc destroy_func;
    DrawFunc draw_func;
    FocusFunc focus_func;
};


void FileSelectionFinalizerFunction(YW::WIDGET* widget) {
    g_FileSelectionWidget = nullptr;
}

bool CanSpawnPreview() {
    return timeSinceLastSpawn >= previewSpawnCooldown;
}

void ResetPreviewSpawnCooldown() {
    timeSinceLastSpawn = 0.0f;
}

void InitAudioEngine() {
    ma_result result = ma_engine_init(NULL, &g_engine);
    if (result != MA_SUCCESS) {
        wprintf(L"Failed to init miniaudio engine\n");
    }
}

void ShutdownAudioEngine() {
    if (g_soundInitialized) {
        ma_sound_uninit(&g_sound);
        g_soundInitialized = false;
    }
    ma_engine_uninit(&g_engine);
}

void SoundDataCallback(ma_device* pDevice, void* pOutput, const void* pInput, ma_uint32 frameCount)
{

    


    float* outputF32 = (float*)pOutput;
    ma_uint64 framesRead = 0;

    // Read decoded audio frames from decoder
    ma_result result = ma_decoder_read_pcm_frames(&g_decoder, outputF32, frameCount, &framesRead);

    //printf("[AUDIO] framesRead=%llu\n", framesRead);

    if (result != MA_SUCCESS) {
        // If decoding fails, output silence
        memset(outputF32, 0, frameCount * pDevice->playback.channels * sizeof(float));
        g_currentAmplitude = 0.0f;
        return;
    }

    // If we read less frames than requested, zero the rest
    if (framesRead < frameCount) {
        memset(outputF32 + framesRead * pDevice->playback.channels, 0,
            (frameCount - framesRead) * pDevice->playback.channels * sizeof(float));
    }

    float amplitude = 0.0f;
    ma_uint32 sampleCount = (ma_uint32)(framesRead * pDevice->playback.channels);

    for (ma_uint32 i = 0; i < sampleCount; i++)
    {
        float s = outputF32[i];
        amplitude += s * s;
    }

    amplitude = sqrtf(amplitude / sampleCount);

    g_currentAmplitude.store(amplitude, std::memory_order_relaxed);
}


void InitAudioDevice()
{
    // Configure the device for playback using decoder's format and channels
    ma_device_config deviceConfig = ma_device_config_init(ma_device_type_playback);
    deviceConfig.playback.format = g_decoder.outputFormat;
    deviceConfig.playback.channels = g_decoder.outputChannels;
    deviceConfig.sampleRate = g_decoder.outputSampleRate;
    deviceConfig.dataCallback = SoundDataCallback;
    deviceConfig.pUserData = nullptr;

    if (ma_device_init(NULL, &deviceConfig, &g_device) != MA_SUCCESS) {
        // Handle error initializing device
        return;
    }

    if (ma_device_start(&g_device) != MA_SUCCESS) {
        // Handle error starting device
        ma_device_uninit(&g_device);
        return;
    }
}




uintptr_t cachedGetPlayerAddress = 0;
uintptr_t cachedEachObjAddress = 0;
uintptr_t cachedGetObjEntryNameAddress = 0;
uintptr_t cachedGetOBJENTRYAddress = 0;
uintptr_t cachedGetObjBoneMatrixAddress = 0;
uintptr_t cachedCalcProjectionMatrixAddress = 0;
uintptr_t cachedGetKNCameraAddress = 0;
uintptr_t cachedGetSoftPadAddress = 0;
uintptr_t cachedGetFuncFrameTimeAddress = 0;

uintptr_t cachedMatrixBitwiseNotAddress = 0;
uintptr_t cachedAssignMatrixAddress = 0;
uintptr_t cachedMatrixMultiplyVectorAddress = 0;
uintptr_t cachedNormalizeVector4Address = 0;
uintptr_t cachedMultiplyByScalarAddress = 0;
uintptr_t cachedNormalizeRadianAddress = 0;
uintptr_t cachedAddAssignAddress = 0;
uintptr_t cachedInitializeAsIdentityMatrixAddress = 0;
uintptr_t cachedApplyMatrixRotationXAddress = 0;
uintptr_t cachedApplyMatrixRotationYAddress = 0;
uintptr_t cachedAdjustEyeAndAtTargetsAddress = 0;
uintptr_t cachedGetHardPadAddress = 0;
uintptr_t cachedCopyFromAddress = 0;
uintptr_t cachedCancelSubjectiveAddress = 0;
uintptr_t cachedGetOBJCameraInstanceAddress = 0;
uintptr_t cachedGetOBJControlOFFAddress = 0;
uintptr_t cachedGetOBJControlONAddress = 0;
uintptr_t cachedSetKNCameraEyeAddress = 0;
uintptr_t cachedSetKNCameraAtAddress = 0;
uintptr_t cachedStatusPrintfAddress = 0;
uintptr_t cachedCommonLayerOFFAddress = 0;
uintptr_t cachedCommonLayerONAddress = 0;
uintptr_t cachedDBGWINPauseAddress = 0;
uintptr_t cachedDBGWINPauseOffAddress = 0;
uintptr_t cachedGetKNCameraModeAddress = 0;
uintptr_t cachedSetKNCameraModeAddress = 0;
uintptr_t cachedGetScreenPosAddress = 0;
uintptr_t cachedGetProjectionMatrixAddress = 0;
uintptr_t cachedGetMenuOBJFunctionAddress = 0;
uintptr_t cachedDecodePointerFunctionAddress = 0;
uintptr_t cachedOBJWishDirFunctionAddress = 0;
uintptr_t cachedVector4AssignFunctionAddress = 0;
uintptr_t cachedMatrixRotateXYZFunctionAddress = 0;
uintptr_t cachedSoundGetLinkNumFunctionAddress = 0;
uintptr_t cachedGetEventRestFrameFunctionAddress = 0;
uintptr_t cachedGetEventProjectNameFunctionAddress = 0;
uintptr_t cachedGetDialValueFunctionAddress = 0;
uintptr_t cachedYWFontPrintFunctionAddress = 0;
uintptr_t cachedUnlinkContainerFunctionAddress = 0;
uintptr_t cachedDrawRectRiseWindowFunctionAddress = 0;
uintptr_t cachedDrawRectSinkWindowFunctionAddress = 0;
uintptr_t cachedWidgetFunctionAddress = 0;
uintptr_t cachedFontGetTextHeightFunctionAddress = 0;
uintptr_t cachedFontGetTextWidthFunctionAddress = 0;
uintptr_t YSDrawMessageFunctionAddress = 0;
uintptr_t cachedDestroyobjFunctionAddress = 0;
uintptr_t cachedRealDestroyobjFunctionAddress = 0;
uintptr_t cachedGetAreaNameFunctionAddress = 0;
uintptr_t cachedInitObjCameraFunctionAddress = 0;
uintptr_t cachedCreateTaskManagertaskFunctionAddress = 0;
uintptr_t cachedGetFieldModeFunctionAddress = 0;
uintptr_t cachedUpdateObjCameraFunctionAddress = 0;
uintptr_t cachedSetCameraNumberFunctionAddress = 0;
uintptr_t cachedGetCameraNumberFunctionAddress = 0;
uintptr_t cachedObjCameraSetParamFunctionAddress = 0;
uintptr_t cachedPlayerPadToMovementFunctionAddress = 0;
uintptr_t cachedReadPadTaskFunctionAddress = 0;



// Define the function pointers
FunctionCalls::tGetAreaName GetAreaNameFunction = 0;
FunctionCalls::tGetPlayer GetPlayerFunction = 0;
FunctionCalls::tEachObj EachOBJFunction = 0;
FunctionCalls::tGetObjEntryName GetOBJEntryNameFunction = 0;
FunctionCalls::tGetOBJMenuObj GetMenuOBJFunction = 0;

FunctionCalls::tGetOBJBoneMatrix GetOBJBoneMatrixFunction = 0;
FunctionCalls::tCalcProjectionMatrix GetCalcProjectionMatrixFunction = 0;
FunctionCalls::tknGetCamera GetKNCameraFunction = 0;
FunctionCalls::tGetSoftPad GetSoftPadFunction = 0;
FunctionCalls::tGetFunctFrameTime GetFuncFrametimeFunction = 0;

FunctionCalls::tMatrixBitwiseNot MatrixBitwiseNotFunction = 0;
FunctionCalls::tAssignMatrix AssignMatrixFunction = 0;
FunctionCalls::tMatrixMultiplyVector MatrixMultiplyVectorFunction = 0;
FunctionCalls::tNormalizeVector4 NormalizeVector4Function = 0;
FunctionCalls::tMultiplyByScalar MultiplyByScalarFunction = 0;
FunctionCalls::tNormalizeRadian NormalizeRadianFunction = 0;
FunctionCalls::tAddAssign AddAssignFunction = 0;
FunctionCalls::tInitializeAsIdentityMatrix InitializeAsIdentityMatrixFunction = 0;
FunctionCalls::tApplyMatrixRotationX ApplyMatrixRotationXFunction = 0;
FunctionCalls::tApplyMatrixRotationY ApplyMatrixRotationYFunction = 0;
FunctionCalls::tAdjustEyeAndAtTargets AdjustEyeAndAtTargetsFunction = 0;
FunctionCalls::tGetHardPad GetHardPadFunction = 0;
FunctionCalls::tVector4_Assign CopyFromAddressFunction = 0;
FunctionCalls::tObjCameraCancelSubjective CancelSubjectiveFunction = 0;
FunctionCalls::tGetOBJCameraInstance GetOBJCameraInstanceFunction = 0;
FunctionCalls::tControlEventOff GetOBJControlOFFunction = 0;
FunctionCalls::tControlEventOn GetOBJControlONFunction = 0;
FunctionCalls::tknCameraSetEye SetKNCameraEyeFunction = 0;
FunctionCalls::tknCameraSetAt SetKNCameraAtFunction = 0;
FunctionCalls::tStatusPrintf StatusPrintfFunction = 0;
FunctionCalls::tCommonLayerOff CommonLayerOFFFunction = 0;
FunctionCalls::tCommonLayerOn CommonLayerONFunction = 0;
FunctionCalls::tDebugPause DBGWINPauseFunction = 0;
FunctionCalls::tDebugPauseOff DBGWINPauseOffFunction = 0;
FunctionCalls::tGetOBJENTRY GetOBJENTRYFunction = 0;
FunctionCalls::tGetKNCameraMode GetKNCameraModeFunction = 0;
FunctionCalls::tSetKNCameraMode SetKNCameraModeFunction = 0;
FunctionCalls::tGetScreenPos GetScreenPosFunction = 0;
FunctionCalls::tGetProjection GetProjectionMatrixFunction = 0;
FunctionCalls::tDecodePointer DecodePointerFunction = 0;
FunctionCalls::tSetoBJWishDir OBJWishDirFunction = 0;
FunctionCalls::tVector4_Assign Vector4AssignFunction = 0;
FunctionCalls::tMatrixRotateXYZ MatrixRotateXYZFunction = 0;
FunctionCalls::tSOUNDgetlinknum SoundGetLinkNumFunction = 0;
FunctionCalls::tGetEventRestFrame GetEventRestFrameFunction = 0;
FunctionCalls::tGetEventProjectName GetEventProjectNameFunction = 0;
FunctionCalls::tGetDialValue GetDialValueFunction = 0;
FunctionCalls::tYWFontPrint YWFontPrintFunction = 0;
FunctionCalls::tContainerUnLink UnLinkContainerFunction = 0;
FunctionCalls::tDrawRectRiseWindow DrawRectRiseWindowFunction = 0;
FunctionCalls::tDrawRectSinkWindow DrawRectSinkWindowFunction = 0;
FunctionCalls::tWidget WidgetFunction = 0;
FunctionCalls::tFontGetTextHeight FontGetTextHeightFunction = 0;
FunctionCalls::tFontGetTextWidth FontGetTextWidthFunction = 0;
FunctionCalls::tYSMessageDraw YSDrawMessageFunction = 0;
FunctionCalls::tDestroyObj DestroyobjFunction = 0;
FunctionCalls::tDestroyObj RealDestroyobjFunction = 0;
FunctionCalls::tInitObjCamera InitOBJCameraFunction = 0;
FunctionCalls::tTaskManagerCreateTask TaskManagerCreateTaskFunction = 0;
FunctionCalls::tGetFieldMode GetFieldModeFunction = 0;
FunctionCalls::tUpdateObjCamera UpdateObjCameraFunction = 0;
FunctionCalls::tSetCameraNumber SetCameraNumberFunction = 0;
FunctionCalls::tGetCameraNumber GetCameraNumberFunction = 0;
FunctionCalls::tSetOBJCameraParam SetObjCameraParamFunction = 0;
FunctionCalls::tPlayerPadToMovement PlayerPadToMovementFunction = 0;
FunctionCalls::tReadPadTask ReadPadTaskFunction = 0;




void UpdateFunctionAddresses() {
    InitializePointers();
    if (cachedGetPlayerAddress == 0) {
        cachedGetPlayerAddress = ReadLEA32(Patterns::pattern_for_GetPlayer, L"Get Player", 0, 1, 5);
        if (cachedGetPlayerAddress) {
            GetPlayerFunction = reinterpret_cast<FunctionCalls::tGetPlayer>(cachedGetPlayerAddress);
        }
        else {
            LOG(L"[ERROR] Failed to find Get Player address.\n");
        }
    }

    if (cachedEachObjAddress == 0) {
        cachedEachObjAddress = ReadLEA32(Patterns::pattern_for_EachOBJ, L"Each OBJ", 0, 1, 5);
        if (cachedEachObjAddress) {
            EachOBJFunction = reinterpret_cast<FunctionCalls::tEachObj>(cachedEachObjAddress);
        }
        else {
            LOG(L"[ERROR] Failed to find Each OBJ address.\n");
        }
    }

    if (cachedGetObjEntryNameAddress == 0) {
        cachedGetObjEntryNameAddress = ReadLEA32(Patterns::pattern_for_GetObjEntryName, L"Get OBJ EntryName", 0, 1, 5);
        if (cachedGetObjEntryNameAddress) {
            GetOBJEntryNameFunction = reinterpret_cast<FunctionCalls::tGetObjEntryName>(cachedGetObjEntryNameAddress);
        }
        else {
            LOG(L"[ERROR] Failed to find Get OBJ EntryName address.\n");
        }
    }

    if (cachedGetObjBoneMatrixAddress == 0) {
        cachedGetObjBoneMatrixAddress = ReadLEA32(Patterns::pattern_for_GetOBJBoneMatrix, L"Get OBJ Bone Matrix", 0, 1, 5);
        if (cachedGetObjBoneMatrixAddress) {
            GetOBJBoneMatrixFunction = reinterpret_cast<FunctionCalls::tGetOBJBoneMatrix>(cachedGetObjBoneMatrixAddress);
        }
        else {
            LOG(L"[ERROR] Failed to find Get OBJ Bone Matrix address.\n");
        }
    }
    if (cachedGetKNCameraAddress == 0) {
        cachedGetKNCameraAddress = ReadLEA32(Patterns::pattern_for_GetKNCamera, L"Get KN Camera", 0, 1, 5);
        if (cachedGetKNCameraAddress) {
            GetKNCameraFunction = reinterpret_cast<FunctionCalls::tknGetCamera>(cachedGetKNCameraAddress);
        }
        else {
            LOG(L"[ERROR] Failed to find Get KN Camera address.\n");
        }
    }
    if (cachedGetSoftPadAddress == 0) {
        cachedGetSoftPadAddress = ReadLEA32(Patterns::pattern_for_GetSoftPad, L"Get Soft PAD", 0, 1, 5);
        if (cachedGetSoftPadAddress) {
            GetSoftPadFunction = reinterpret_cast<FunctionCalls::tGetSoftPad>(cachedGetSoftPadAddress);
        }
        else {
            LOG(L"[ERROR] Failed to find Get SoftPad address.\n");
        }
        //uintptr_t GetSoftPADFunctionAddress = ReadLEA32(Patterns::pattern_for_GetSoftPad, L"Get Soft PAD", 0, 1, 5);
    }
    if (cachedGetFuncFrameTimeAddress == 0) {
        cachedGetFuncFrameTimeAddress = ReadLEA32(Patterns::pattern_for_GetFuncFrameTime, L"Get Func Frametime", 0, 1, 5);
        if (cachedGetFuncFrameTimeAddress) {
            GetFuncFrametimeFunction = reinterpret_cast<FunctionCalls::tGetFunctFrameTime>(cachedGetFuncFrameTimeAddress);
        }
        else {
            LOG(L"[ERROR] Failed to find Get Func FrameTime address.\n");
        }
    }
    if (cachedMatrixBitwiseNotAddress == 0) {
        cachedMatrixBitwiseNotAddress = ReadLEA32(Patterns::pattern_for_MatrixBitwiseNot, L"Matrix Bitwise Not", 0, 1, 5);
        if (cachedMatrixBitwiseNotAddress) {
            MatrixBitwiseNotFunction = reinterpret_cast<FunctionCalls::tMatrixBitwiseNot>(cachedMatrixBitwiseNotAddress);
        }
        else {
            LOG(L"[ERROR] Failed to find Matrix Bitwise Not address.\n");
        }
    }

    if (cachedAssignMatrixAddress == 0) {
        cachedAssignMatrixAddress = ReadLEA32(Patterns::pattern_for_AssignMatrix, L"Assign Matrix", 0, 1, 5);
        if (cachedAssignMatrixAddress) {
            AssignMatrixFunction = reinterpret_cast<FunctionCalls::tAssignMatrix>(cachedAssignMatrixAddress);
        }
        else {
            LOG(L"[ERROR] Failed to find Assign Matrix address.\n");
        }
    }

    if (cachedMatrixMultiplyVectorAddress == 0) {
        cachedMatrixMultiplyVectorAddress = ReadLEA32(Patterns::pattern_for_MatrixMultiplyVector, L"Matrix Multiply Vector", 0, 1, 5);
        if (cachedMatrixMultiplyVectorAddress) {
            MatrixMultiplyVectorFunction = reinterpret_cast<FunctionCalls::tMatrixMultiplyVector>(cachedMatrixMultiplyVectorAddress);
        }
        else {
            LOG(L"[ERROR] Failed to find Matrix Multiply Vector address.\n");
        }
    }

    if (cachedNormalizeVector4Address == 0) {
        cachedNormalizeVector4Address = ReadLEA32(Patterns::pattern_for_NormalizeVector4, L"Normalize Vector4", 0, 1, 5);
        if (cachedNormalizeVector4Address) {
            NormalizeVector4Function = reinterpret_cast<FunctionCalls::tNormalizeVector4>(cachedNormalizeVector4Address);
        }
        else {
            LOG(L"[ERROR] Failed to find Normalize Vector4 address.\n");
        }
    }

    if (cachedMultiplyByScalarAddress == 0) {
        cachedMultiplyByScalarAddress = ReadLEA32(Patterns::pattern_for_MultiplyByScalar, L"Multiply By Scalar", 0, 1, 5);
        if (cachedMultiplyByScalarAddress) {
            MultiplyByScalarFunction = reinterpret_cast<FunctionCalls::tMultiplyByScalar>(cachedMultiplyByScalarAddress);
        }
        else {
            LOG(L"[ERROR] Failed to find Multiply By Scalar address.\n");
        }
    }

    if (cachedNormalizeRadianAddress == 0) {
        cachedNormalizeRadianAddress = ReadLEA32(Patterns::pattern_for_NormalizeRadian, L"Normalize Radian", 0, 1, 5);
        if (cachedNormalizeRadianAddress) {
            NormalizeRadianFunction = reinterpret_cast<FunctionCalls::tNormalizeRadian>(cachedNormalizeRadianAddress);
        }
        else {
            LOG(L"[ERROR] Failed to find Normalize Radian address.\n");
        }
    }

    if (cachedAddAssignAddress == 0) {
        cachedAddAssignAddress = ReadLEA32(Patterns::pattern_for_AddAssign, L"Add Assign", 0, 1, 5);
        if (cachedAddAssignAddress) {
            AddAssignFunction = reinterpret_cast<FunctionCalls::tAddAssign>(cachedAddAssignAddress);
        }
        else {
            LOG(L"[ERROR] Failed to find Add Assign address.\n");
        }
    }

    if (cachedInitializeAsIdentityMatrixAddress == 0) {
        cachedInitializeAsIdentityMatrixAddress = ReadLEA32(Patterns::pattern_for_InitializeAsIdentityMatrix, L"Initialize As Identity Matrix", 0, 1, 5);
        if (cachedInitializeAsIdentityMatrixAddress) {
            InitializeAsIdentityMatrixFunction = reinterpret_cast<FunctionCalls::tInitializeAsIdentityMatrix>(cachedInitializeAsIdentityMatrixAddress);
        }
        else {
            LOG(L"[ERROR] Failed to find Initialize As Identity Matrix address.\n");
        }
    }

    if (cachedApplyMatrixRotationXAddress == 0) {
        cachedApplyMatrixRotationXAddress = ReadLEA32(Patterns::pattern_for_ApplyMatrixRotationX, L"Apply Matrix Rotation X", 0, 1, 5);
        if (cachedApplyMatrixRotationXAddress) {
            ApplyMatrixRotationXFunction = reinterpret_cast<FunctionCalls::tApplyMatrixRotationX>(cachedApplyMatrixRotationXAddress);
        }
        else {
            LOG(L"[ERROR] Failed to find Apply Matrix Rotation X address.\n");
        }
    }

    if (cachedApplyMatrixRotationYAddress == 0) {
        cachedApplyMatrixRotationYAddress = ReadLEA32(Patterns::pattern_for_ApplyMatrixRotationY, L"Apply Matrix Rotation Y", 0, 1, 5);
        if (cachedApplyMatrixRotationYAddress) {
            ApplyMatrixRotationYFunction = reinterpret_cast<FunctionCalls::tApplyMatrixRotationY>(cachedApplyMatrixRotationYAddress);
        }
        else {
            LOG(L"[ERROR] Failed to find Apply Matrix Rotation Y address.\n");
        }
    }

    if (cachedAdjustEyeAndAtTargetsAddress == 0) {
        cachedAdjustEyeAndAtTargetsAddress = ReadLEA32(Patterns::pattern_for_AdjustEyeAndAtTargets, L"Adjust Eye and At Targets", 0, 1, 5);
        if (cachedAdjustEyeAndAtTargetsAddress) {
            AdjustEyeAndAtTargetsFunction = reinterpret_cast<FunctionCalls::tAdjustEyeAndAtTargets>(cachedAdjustEyeAndAtTargetsAddress);
        }
        else {
            LOG(L"[ERROR] Failed to find Adjust Eye and At Targets address.\n");
        }
    }
    if (cachedGetHardPadAddress == 0) {
        cachedGetHardPadAddress = ReadLEA32(Patterns::pattern_for_GetHardPad, L"Get Hard PAD", 0, 1, 5);
        if (cachedGetHardPadAddress) {
            GetHardPadFunction = reinterpret_cast<FunctionCalls::tGetHardPad>(cachedGetHardPadAddress);
        }
        else {
            LOG(L"[ERROR] Failed to find Get HardPad address.\n");
        }

    }
    if (cachedCancelSubjectiveAddress == 0) {
        cachedCancelSubjectiveAddress = ReadLEA32(Patterns::pattern_for_CancelSubjectiveOBJCamera, L"CancelSubjectiveOBJCamera", 0, 1, 5);
        if (cachedCancelSubjectiveAddress) {
            CancelSubjectiveFunction = reinterpret_cast<FunctionCalls::tObjCameraCancelSubjective>(cachedCancelSubjectiveAddress);
        }
        else {
            LOG(L"[ERROR] Failed to find Cancel Subjective address.\n");
        }
    }
    if (cachedGetOBJCameraInstanceAddress == 0) {
        cachedGetOBJCameraInstanceAddress = ReadLEA32(Patterns::pattern_for_GetOBJCameraInstance, L"Get OBJ Camera instance", 0, 1, 5);
        if (cachedGetOBJCameraInstanceAddress) {
            GetOBJCameraInstanceFunction = reinterpret_cast<FunctionCalls::tGetOBJCameraInstance>(cachedGetOBJCameraInstanceAddress);
        }
        else {
            LOG(L"[ERROR] Failed to find Get OBJ Camera instance address.\n");
        }
    }
    if (cachedGetOBJControlOFFAddress == 0) {
        cachedGetOBJControlOFFAddress = ReadLEA32(Patterns::pattern_for_ControlEventOff, L"Get OBJControlOFF", 0, 1, 5);
        if (cachedGetOBJControlOFFAddress) {
            GetOBJControlOFFunction = reinterpret_cast<FunctionCalls::tControlEventOff>(cachedGetOBJControlOFFAddress);
        }
        else {
            LOG(L"[ERROR] Failed to find Get OBJControlOFF address.\n");
        }
    }
    if (cachedGetOBJControlONAddress == 0) {
        cachedGetOBJControlONAddress = ReadLEA32(Patterns::pattern_for_ControlEventOn, L"Get OBJControlON", 0, 1, 5);
        if (cachedGetOBJControlONAddress) {
            GetOBJControlONFunction = reinterpret_cast<FunctionCalls::tControlEventOn>(cachedGetOBJControlONAddress);
        }
        else {
            LOG(L"[ERROR] Failed to find Get OBJControlON address.\n");
        }
    }
    if (cachedSetKNCameraEyeAddress == 0) {
        cachedSetKNCameraEyeAddress = ReadLEA32(Patterns::pattern_for_SetKNCameraEye, L"Set KN Camera Eye", 0, 1, 5);
        if (cachedSetKNCameraEyeAddress) {
            SetKNCameraEyeFunction = reinterpret_cast<FunctionCalls::tknCameraSetEye>(cachedSetKNCameraEyeAddress);
        }
        else {
            LOG(L"[ERROR] Failed to find Set KN Camera Eye address.\n");
        }
    }
    if (cachedSetKNCameraAtAddress == 0) {
        cachedSetKNCameraAtAddress = ReadLEA32(Patterns::pattern_for_SetKNCameraAt, L"Set KN Camera At", 0, 1, 5);
        if (cachedSetKNCameraAtAddress) {
            SetKNCameraAtFunction = reinterpret_cast<FunctionCalls::tknCameraSetAt>(cachedSetKNCameraAtAddress);
        }
        else {
            LOG(L"[ERROR] Failed to find Set KN Camera At address.\n");
        }
    }
    if (cachedStatusPrintfAddress == 0) {
        cachedStatusPrintfAddress = FindAndPrintPatternW(Patterns::pattern_for_StatusPrintf, L"StatusPrintf", 0);
        if (cachedStatusPrintfAddress) {
            StatusPrintfFunction = reinterpret_cast<FunctionCalls::tStatusPrintf>(cachedStatusPrintfAddress);
        }
        else {
            LOG(L"[ERROR] Failed to find StatusPrintf address.\n");
        }
    }
    if (cachedCommonLayerONAddress == 0) {
        cachedCommonLayerONAddress = ReadLEA32(Patterns::pattern_for_CommonLayerOn, L"Set Common Layer ON", 0, 1, 5);
        if (cachedCommonLayerONAddress) {
            CommonLayerONFunction = reinterpret_cast<FunctionCalls::tCommonLayerOn>(cachedCommonLayerONAddress);
        }
        else {
            LOG(L"[ERROR] Failed to find Common Layer ON address.\n");
        }
    }
    if (cachedCommonLayerOFFAddress == 0) {
        cachedCommonLayerOFFAddress = ReadLEA32(Patterns::pattern_for_CommonLayerOff, L"Set Common Layer OFF", 0, 1, 5);
        if (cachedCommonLayerOFFAddress) {
            CommonLayerOFFFunction = reinterpret_cast<FunctionCalls::tCommonLayerOff>(cachedCommonLayerOFFAddress);
        }
        else {
            LOG(L"[ERROR] Failed to find Common Layer OFF address.\n");
        }
    }
    if (cachedDBGWINPauseAddress == 0) {
        cachedDBGWINPauseAddress = FindAndPrintPatternW(Patterns::pattern_for_DBGWINPause, L"debug pause", 0);
        if (cachedDBGWINPauseAddress) {
            DBGWINPauseFunction = reinterpret_cast<FunctionCalls::tDebugPause>(cachedDBGWINPauseAddress);
        }
        else {
            LOG(L"[ERROR] Failed to find Debug Pause address.\n");
        }
    }
    if (cachedDBGWINPauseOffAddress == 0) {
        cachedDBGWINPauseOffAddress = FindAndPrintPatternW(Patterns::pattern_for_DBGWINPauseOff, L"debug pause OFF", 0);
        if (cachedDBGWINPauseOffAddress) {
            DBGWINPauseOffFunction = reinterpret_cast<FunctionCalls::tDebugPauseOff>(cachedDBGWINPauseOffAddress);
        }
        else {
            LOG(L"[ERROR] Failed to find Debug Pause Off address.\n");
        }
    }
    if (cachedGetOBJENTRYAddress == 0) {
        cachedGetOBJENTRYAddress = ReadLEA32(Patterns::pattern_for_GetOBJENTRY, L"Get OBJENTRY", 0, 1, 5);
        if (cachedGetOBJENTRYAddress) {
            GetOBJENTRYFunction = reinterpret_cast<FunctionCalls::tGetOBJENTRY>(cachedGetOBJENTRYAddress);
        }
        else {
            LOG(L"[ERROR] Failed to find Get OBJENTRY address.\n");
        }
    }
    if (cachedGetKNCameraModeAddress == 0) {
        cachedGetKNCameraModeAddress = ReadLEA32(Patterns::pattern_for_GetKNCameraMode, L"Get KN Camera Mode", 0, 1, 5);
        if (cachedGetKNCameraModeAddress) {
            GetKNCameraModeFunction = reinterpret_cast<FunctionCalls::tGetKNCameraMode>(cachedGetKNCameraModeAddress);
        }
        else {
            LOG(L"[ERROR] Failed to find Get KN Camera Mode address.\n");
        }
    }
    if (cachedSetKNCameraModeAddress == 0) {
        cachedSetKNCameraModeAddress = ReadLEA32(Patterns::pattern_for_SetKNCameraMode, L"Set KN Camera Mode", 0, 1, 5);
        if (cachedSetKNCameraModeAddress) {
            SetKNCameraModeFunction = reinterpret_cast<FunctionCalls::tSetKNCameraMode>(cachedSetKNCameraModeAddress);
        }
        else {
            LOG(L"[ERROR] Failed to find Set KN Camera Mode address.\n");
        }
    }
    if (cachedGetScreenPosAddress == 0) {
        cachedGetScreenPosAddress = FindAndPrintPatternW(Patterns::pattern_for_GetScreenpos, L"Get Screen Pos", 0);
        if (cachedGetScreenPosAddress) {
            GetScreenPosFunction = reinterpret_cast<FunctionCalls::tGetScreenPos>(cachedGetScreenPosAddress);
        }
        else {
            LOG(L"[ERROR] Failed to find Get Screen Pos address.\n");
        }
    }
    if (cachedGetProjectionMatrixAddress == 0) {
        cachedGetProjectionMatrixAddress = ReadLEA32(Patterns::pattern_for_GetProjectionMatrix, L"Get Projection Matrix", 0, 1, 5);
        if (cachedGetProjectionMatrixAddress) {
            GetProjectionMatrixFunction = reinterpret_cast<FunctionCalls::tGetProjection>(cachedGetProjectionMatrixAddress);
        }
        else {
            LOG(L"[ERROR] Failed to find Get Projection Matrix address.\n");
        }
    }
    if (cachedGetMenuOBJFunctionAddress == 0) {
        cachedGetMenuOBJFunctionAddress = FindAndPrintPatternW(Patterns::pattern_for_GetOBJMenuObj, L"Get OBJ Menu OBJ", 0);
        if (cachedGetMenuOBJFunctionAddress) {
            GetMenuOBJFunction = reinterpret_cast<FunctionCalls::tGetOBJMenuObj>(cachedGetMenuOBJFunctionAddress);
        }
        else {
            LOG(L"[ERROR] Failed to find Get OBJ Menu OBJ address.\n");
        }
        
    }

    if (cachedDecodePointerFunctionAddress == 0) {
        cachedDecodePointerFunctionAddress = FindAndPrintPatternW(Patterns::pattern_for_DecodePointer, L"Decode pointer", 0);
        if (cachedDecodePointerFunctionAddress) {
            DecodePointerFunction = reinterpret_cast<FunctionCalls::tDecodePointer>(cachedDecodePointerFunctionAddress);
        }
        else {
            LOG(L"[ERROR] Failed to find DecodePointerFunction address.\n");
        }

    }

    if (cachedOBJWishDirFunctionAddress == 0) {
       cachedOBJWishDirFunctionAddress = ReadLEA32(Patterns::pattern_for_ObjWishDir, L"OBJ Wish Dir", 0, 1, 5);
       if (cachedOBJWishDirFunctionAddress) {
          OBJWishDirFunction = reinterpret_cast<FunctionCalls::tSetoBJWishDir>(cachedOBJWishDirFunctionAddress);
       }
       else {
           LOG(L"[ERROR] Failed to find OBJWishDirFunction address.\n");
       }

    }

    if (cachedVector4AssignFunctionAddress == 0) {
       cachedVector4AssignFunctionAddress = FindAndPrintPatternW(Patterns::pattern_for_OperatorEquals, L"Vector4 Assign", 0);
       if (cachedVector4AssignFunctionAddress) {
          Vector4AssignFunction = reinterpret_cast<FunctionCalls::tVector4_Assign>(cachedVector4AssignFunctionAddress);
       }
       else {
           LOG(L"[ERROR] Failed to find Vector4AssignFunction address.\n");
       }
    }

    if (cachedMatrixRotateXYZFunctionAddress == 0) {
        cachedMatrixRotateXYZFunctionAddress = ReadLEA32(Patterns::pattern_for_MatrixRotateXYZ, L"Rotate Matrix XYZ", 0, 1, 5);
        if (cachedMatrixRotateXYZFunctionAddress) {
            MatrixRotateXYZFunction = reinterpret_cast<FunctionCalls::tMatrixRotateXYZ>(cachedMatrixRotateXYZFunctionAddress);
        }
        else {
            LOG(L"[ERROR] Failed to find MatrixRotateXYZFunction address.\n");
        }
    }

    if (cachedSoundGetLinkNumFunctionAddress == 0) {
        cachedSoundGetLinkNumFunctionAddress = ReadLEA32(Patterns::pattern_for_SoundGetLinkNum, L"Get Sound Link num", 0, 1, 5);
        if (cachedSoundGetLinkNumFunctionAddress) {
            SoundGetLinkNumFunction = reinterpret_cast<FunctionCalls::tSOUNDgetlinknum>(cachedSoundGetLinkNumFunctionAddress);
        }
        else {
            LOG(L"[ERROR] Failed to find SoundGetLinkNumFunction address.\n");
        }
    }

    if (cachedGetEventRestFrameFunctionAddress == 0) {
        cachedGetEventRestFrameFunctionAddress = FindAndPrintPatternW(Patterns::pattern_for_GetEventRestFrame, L"Get Event Rest Frame", 0);
        if (cachedGetEventRestFrameFunctionAddress) {
            GetEventRestFrameFunction = reinterpret_cast<FunctionCalls::tGetEventRestFrame>(cachedGetEventRestFrameFunctionAddress);
        }
        else {
            LOG(L"[ERROR] Failed to find GetEventRestFrameFunction address.\n");
        }
    }

    if (cachedGetEventProjectNameFunctionAddress == 0) {
        cachedGetEventProjectNameFunctionAddress = ReadLEA32(Patterns::pattern_for_GetEventRestFrame, L"Get Event Project Name", 0, 1, 5);
        if (cachedGetEventProjectNameFunctionAddress) {
            GetEventProjectNameFunction = reinterpret_cast<FunctionCalls::tGetEventProjectName>(cachedGetEventProjectNameFunctionAddress);
        }
        else {
            LOG(L"[ERROR] Failed to find GetEventProjectNameFunction address.\n");
        }
    }

    if (cachedGetDialValueFunctionAddress == 0) {
        cachedGetDialValueFunctionAddress = ReadLEA32(Patterns::pattern_for_GetDialValue, L"Get Dial Value", 0, 1, 5);
        if (cachedGetDialValueFunctionAddress) {
            GetDialValueFunction = reinterpret_cast<FunctionCalls::tGetDialValue>(cachedGetDialValueFunctionAddress);
        }
        else {
            LOG(L"[ERROR] Failed to find GetDialValueFunction address.\n");
        }
    }

    if (cachedYWFontPrintFunctionAddress == 0) {
        cachedYWFontPrintFunctionAddress = ReadLEA32(Patterns::pattern_for_YWFontPrint, L"YW Font Print", 0, 1, 5);
        if (cachedYWFontPrintFunctionAddress) {
            YWFontPrintFunction = reinterpret_cast<FunctionCalls::tYWFontPrint>(cachedYWFontPrintFunctionAddress);
        }
        else {
            LOG(L"[ERROR] Failed to find YWFontPrintFunction address.\n");
        }
    }

    if (cachedUnlinkContainerFunctionAddress == 0) {
        cachedUnlinkContainerFunctionAddress = FindAndPrintPatternW(Patterns::pattern_for_GetEventRestFrame, L"Unlink Container", 0);
        if (cachedUnlinkContainerFunctionAddress) {
            UnLinkContainerFunction = reinterpret_cast<FunctionCalls::tContainerUnLink>(cachedUnlinkContainerFunctionAddress);
        }
        else {
            LOG(L"[ERROR] Failed to find UnLinkContainerFunction address.\n");
        }
    }

    if (cachedDrawRectRiseWindowFunctionAddress == 0) {
        cachedDrawRectRiseWindowFunctionAddress = ReadLEA32(Patterns::pattern_for_DrawRECTRiseWindow, L"Draw Rect Rise Window", 0, 1, 5);
        if (cachedDrawRectRiseWindowFunctionAddress) {
            DrawRectRiseWindowFunction = reinterpret_cast<FunctionCalls::tDrawRectRiseWindow>(cachedDrawRectRiseWindowFunctionAddress);
        }
        else {
            LOG(L"[ERROR] Failed to find DrawRectRiseWindowFunction address.\n");
        }
    }

    if (cachedDrawRectSinkWindowFunctionAddress == 0) {
        cachedDrawRectSinkWindowFunctionAddress = ReadLEA32(Patterns::pattern_for_DrawRECTSinkWindow, L"Draw Rect Sink Window", 0, 1, 5);
        if (cachedDrawRectSinkWindowFunctionAddress) {
            DrawRectSinkWindowFunction = reinterpret_cast<FunctionCalls::tDrawRectSinkWindow>(cachedDrawRectSinkWindowFunctionAddress);
        }
        else {
            LOG(L"[ERROR] Failed to find DrawRectRiseWindowFunction address.\n");
        }
    }

    if (cachedWidgetFunctionAddress == 0) {
        cachedWidgetFunctionAddress = ReadLEA32(Patterns::pattern_for_Widget, L"Widget Function", 0, 1, 5);
        if (cachedWidgetFunctionAddress) {
            WidgetFunction = reinterpret_cast<FunctionCalls::tWidget>(cachedWidgetFunctionAddress);
        }
        else {
            LOG(L"[ERROR] Failed to find cachedWidgetFunctionAddress address.\n");
        }
    }

    if (cachedFontGetTextHeightFunctionAddress == 0) {
        cachedFontGetTextHeightFunctionAddress = ReadLEA32(Patterns::pattern_for_FontGetTextHeight, L"Font Get text Height", 0, 1, 5);
        if (cachedFontGetTextHeightFunctionAddress) {
            FontGetTextHeightFunction = reinterpret_cast<FunctionCalls::tFontGetTextHeight>(cachedFontGetTextHeightFunctionAddress);
        }
        else {
            LOG(L"[ERROR] Failed to find cachedFontGetTextHeightFunctionAddress address.\n");
        }
    }

    if (cachedFontGetTextWidthFunctionAddress == 0) {
        cachedFontGetTextWidthFunctionAddress = ReadLEA32(Patterns::pattern_for_FontGetTextWidth, L"Font Get text Width", 0, 1, 5);
        if (cachedFontGetTextWidthFunctionAddress) {
            FontGetTextWidthFunction = reinterpret_cast<FunctionCalls::tFontGetTextWidth>(cachedFontGetTextWidthFunctionAddress);
        }
        else {
            LOG(L"[ERROR] Failed to find cachedFontGetTextWidthFunctionAddress address.\n");
        }
    }
            
    if (YSDrawMessageFunctionAddress == 0) {
        YSDrawMessageFunctionAddress = ReadLEA32(Patterns::pattern_for_YSMessageDraw, L"YS MESSAGE DRAW", 0, 1, 5);
        if (YSDrawMessageFunctionAddress) {
            YSDrawMessageFunction = reinterpret_cast<FunctionCalls::tYSMessageDraw>(YSDrawMessageFunctionAddress);
        }
        else {
            LOG(L"[ERROR] Failed to find YSDrawMessageFunctionAddress address.\n");
        }
    }

    if (cachedDestroyobjFunctionAddress == 0) {
        cachedDestroyobjFunctionAddress = ReadLEA32(Patterns::pattern_for_DestroyObj, L"Destroy Obj", 0, 1, 5);
        if (cachedDestroyobjFunctionAddress) {
            DestroyobjFunction = reinterpret_cast<FunctionCalls::tDestroyObj>(cachedDestroyobjFunctionAddress);
        }
        else {
            LOG(L"[ERROR] Failed to find cachedDestroyobjFunctionAddress.\n");
        }
        
    }

    if (cachedRealDestroyobjFunctionAddress == 0) {
        cachedRealDestroyobjFunctionAddress = ReadLEA32(Patterns::pattern_for_RealDestroyObj, L"Real Destroy Obj", 0,1,5);
        if (cachedRealDestroyobjFunctionAddress) {
            RealDestroyobjFunction = reinterpret_cast<FunctionCalls::tDestroyObj>(cachedRealDestroyobjFunctionAddress);
        }
        else {
            LOG(L"[ERROR] Failed to find cachedRealDestroyobjFunctionAddress.\n");
        }

    }

    if (cachedGetAreaNameFunctionAddress == 0) {
        cachedGetAreaNameFunctionAddress = ReadLEA32(Patterns::pattern_for_GetAreaName, L"Get Area Name", 0, 1, 5);
        if (cachedGetAreaNameFunctionAddress) {
            GetAreaNameFunction = reinterpret_cast<FunctionCalls::tGetAreaName>(cachedGetAreaNameFunctionAddress);
        }
        else {
            LOG(L"[ERROR] Failed to find cachedGetAreaNameFunctionAddress.\n");
        }
    }

    if (cachedInitObjCameraFunctionAddress == 0) {
        cachedInitObjCameraFunctionAddress = FindAndPrintPatternW(Patterns::pattern_for_InitObjCamera, L"Init OBJ Camera", 0);
        if (cachedInitObjCameraFunctionAddress) {
            InitOBJCameraFunction = reinterpret_cast<FunctionCalls::tInitObjCamera>(cachedInitObjCameraFunctionAddress);
        }
        else {
            LOG(L"[ERROR] Failed to find cachedGetAreaNameFunctionAddress.\n");
        }
    }


    if (cachedCreateTaskManagertaskFunctionAddress == 0) {
        cachedCreateTaskManagertaskFunctionAddress = ReadLEA32(Patterns::pattern_for_TaskManagerCreateTask, L"Task Manager Create Task", 0, 1, 5);
        if (cachedCreateTaskManagertaskFunctionAddress) {
            TaskManagerCreateTaskFunction = reinterpret_cast<FunctionCalls::tTaskManagerCreateTask>(cachedCreateTaskManagertaskFunctionAddress);
        }
        else {
            LOG(L"[ERROR] Failed to find cachedGetAreaNameFunctionAddress.\n");
        }
    }

    if (cachedGetFieldModeFunctionAddress == 0) {
        cachedGetFieldModeFunctionAddress = ReadLEA32(Patterns::pattern_for_GetFieldMode, L"Get Field Mode", 0, 1, 5);
        if (cachedGetFieldModeFunctionAddress) {
            GetFieldModeFunction = reinterpret_cast<FunctionCalls::tGetFieldMode>(cachedGetFieldModeFunctionAddress);
        }
        else {
            LOG(L"[ERROR] Failed to find cachedGetAreaNameFunctionAddress.\n");
        }
    }

    if (cachedUpdateObjCameraFunctionAddress == 0) {
        cachedUpdateObjCameraFunctionAddress = FindAndPrintPatternW(Patterns::pattern_for_UpdateObjCamera, L"Update OBJ Camera", 0);
        if (cachedUpdateObjCameraFunctionAddress) {
            UpdateObjCameraFunction = reinterpret_cast<FunctionCalls::tUpdateObjCamera>(cachedUpdateObjCameraFunctionAddress);
        }
        else {
            LOG(L"[ERROR] Failed to find cachedGetAreaNameFunctionAddress.\n");
        }
    }
    
    if (cachedSetCameraNumberFunctionAddress == 0) {
        cachedSetCameraNumberFunctionAddress = ReadLEA32(Patterns::pattern_for_SetCameraNumber, L"Set Camera Number", 0, 1, 5);
        if (cachedSetCameraNumberFunctionAddress) {
            SetCameraNumberFunction = reinterpret_cast<FunctionCalls::tSetCameraNumber>(cachedSetCameraNumberFunctionAddress);
        }
        else {
            LOG(L"[ERROR] Failed to find cachedGetAreaNameFunctionAddress.\n");
        }
    }

    if (cachedGetCameraNumberFunctionAddress == 0) {
        cachedGetCameraNumberFunctionAddress = ReadLEA32(Patterns::pattern_for_GetCameraNumber, L"Get Camera Number", 0, 1, 5);
        if (cachedGetCameraNumberFunctionAddress) {
            GetCameraNumberFunction = reinterpret_cast<FunctionCalls::tGetCameraNumber>(cachedGetCameraNumberFunctionAddress);
        }
        else {
            LOG(L"[ERROR] Failed to find cachedGetAreaNameFunctionAddress.\n");
        }
    }

    if (cachedObjCameraSetParamFunctionAddress == 0) {
        cachedObjCameraSetParamFunctionAddress = ReadLEA32(Patterns::pattern_for_SetObjCameraParam, L"Set OBJ Camera Param", 0, 1, 5);
        if (cachedObjCameraSetParamFunctionAddress) {
            SetObjCameraParamFunction = reinterpret_cast<FunctionCalls::tSetOBJCameraParam>(cachedObjCameraSetParamFunctionAddress);
        }
        else {
            LOG(L"[ERROR] Failed to find cachedGetAreaNameFunctionAddress.\n");
        }
    }

    if (cachedPlayerPadToMovementFunctionAddress == 0) {
        cachedPlayerPadToMovementFunctionAddress = ReadLEA32(Patterns::pattern_for_SetPlayerPadToMovement, L"Set Player PAD To Movement", 0, 1, 5);
        if (cachedPlayerPadToMovementFunctionAddress) {
            PlayerPadToMovementFunction = reinterpret_cast<FunctionCalls::tPlayerPadToMovement>(cachedPlayerPadToMovementFunctionAddress);
        }
        else {
            LOG(L"[ERROR] Failed to find cachedGetAreaNameFunctionAddress.\n");
        }
    }

    
    
}


YS::OBJ* GetCachedMenuOBJ()
{
    YS::OBJ* current = GetMenuOBJFunction();  // game’s function

    if (current != nullptr)
    {
        // Only update when we get a real pointer
        gGlobalObjPtr = current;
    }

    // When current == nullptr, we keep the last non-null value
    return gGlobalObjPtr;
}

void init_pa_editer_window(int wx, int wy) {
    uintptr_t SetYWwindowTitleFunctionAddress = FindAndPrintPatternW(Patterns::pattern_for_SetYWwindowTitle, L"Set YW Window Title", 0);
    FunctionCalls::tSetWindowTitle SetYWwindowTitleFunction = reinterpret_cast<FunctionCalls::tSetWindowTitle>(SetYWwindowTitleFunctionAddress);

    uintptr_t SetDialValueFunctionAddress = FindAndPrintPatternW(Patterns::pattern_for_SetDialValue, L"Set DialValue", 0);

    FunctionCalls::tSetDialValue SetDialValueFunction = reinterpret_cast<FunctionCalls::tSetDialValue>(SetDialValueFunctionAddress);

    uintptr_t YWLabelFunctionAddress = ReadLEA32(Patterns::pattern_for_YWLabel, L"YWlabel", 0, 1, 5);

    FunctionCalls::tYWLabel YWLabelFunction = reinterpret_cast<FunctionCalls::tYWLabel>(YWLabelFunctionAddress);

    FunctionCalls::tYWLabelNew YWLabelFunctionNew = reinterpret_cast<FunctionCalls::tYWLabelNew>(YWLabelFunctionAddress);

    uintptr_t OpenSingletonWindowFunctionAddress = ReadLEA32(Patterns::pattern_for_OpenSingletonWindow, L"OpenSingletonWindow", 0, 1, 5);

    FunctionCalls::tOpenSingletonWindow OpenSingletonWindowFunction = reinterpret_cast<FunctionCalls::tOpenSingletonWindow>(OpenSingletonWindowFunctionAddress);

    //uintptr_t YWLabelFunctionAddress = ReadLEA32(Patterns::pattern_for_YWLabel, L"YWlabel", 0, 1, 5);

    //FunctionCalls::tYWLabel YWLabelFunction = reinterpret_cast<FunctionCalls::tYWLabel>(YWLabelFunctionAddress);

    uintptr_t ShowDialWidgetFunctionAddress = ReadLEA32(Patterns::pattern_for_DialWidget, L"Dial Widget", 0, 1, 5);
    FunctionCalls::tDialWidget ShowDialWidgetFunction = reinterpret_cast<FunctionCalls::tDialWidget>(ShowDialWidgetFunctionAddress);

    uintptr_t GetOBJEntrynameFunctionAddress = ReadLEA32(Patterns::pattern_for_GetObjEntryName, L"Get obj Entryname", 0, 1, 5);
    FunctionCalls::tGetObjEntryName GetOBJEntrynameFunction = reinterpret_cast<FunctionCalls::tGetObjEntryName>(GetOBJEntrynameFunctionAddress);

    uintptr_t ShowDebugWindowFunctionAddress = ReadLEA32(Patterns::pattern_for_ShowDebugWindow, L"ShowDebugWindow", 0, 1, 5);
    FunctionCalls::tShowDebugWindow ShowDebugWindowFunction = reinterpret_cast<FunctionCalls::tShowDebugWindow>(ShowDebugWindowFunctionAddress);

    uintptr_t AllocYWMemoryFunctionAddress = ReadLEA32(Patterns::pattern_for_Alloc_YW_Memory, L"AllocYWMemory", 0, 1, 5);

    FunctionCalls::tAlloc_YW_Memory AllocYWMemoryFunction = reinterpret_cast<FunctionCalls::tAlloc_YW_Memory>(AllocYWMemoryFunctionAddress);

    FunctionCalls::tAlloc_YW_MemoryNew AllocYWMemoryFunctionNew = reinterpret_cast<FunctionCalls::tAlloc_YW_MemoryNew>(AllocYWMemoryFunctionAddress);

    uintptr_t ContainerLinkFunctionAddress = FindAndPrintPatternW(Patterns::pattern_for_ContainerLink, L"ContainerLink", 0);

    FunctionCalls::tContainerLink ContainerLinkFunction = reinterpret_cast<FunctionCalls::tContainerLink>(ContainerLinkFunctionAddress);

    uintptr_t YWScrollbarFunctionAddress = FindAndPrintPatternW(Patterns::pattern_for_YWScrollbar, L"YWScrollbar", 0);

    FunctionCalls::tYWScrollbar YWScrollbarFunction = reinterpret_cast<FunctionCalls::tYWScrollbar>(YWScrollbarFunctionAddress);

    uintptr_t YWButtonFunctionAddress = ReadLEA32(Patterns::pattern_for_YWButton, L"YWButton", 0, 1, 5);
    FunctionCalls::tYWButton YWButtonFunction = reinterpret_cast<FunctionCalls::tYWButton>(YWButtonFunctionAddress);

    uintptr_t YWListButtonFunctionAddress = ReadLEA32(Patterns::pattern_for_YWListButton, L"YW LIST BUTTON", 0, 1, 5);
    FunctionCalls::tYWListButton YWListButtonFunction = reinterpret_cast<FunctionCalls::tYWListButton>(YWListButtonFunctionAddress);

    //SetYWwindowTitleFunction((YW::WINDOW*)this_param, "=== .pa editer ===");

    PaEditerWindow->Rect.X = wx;
    PaEditerWindow->Rect.Y = wy;
    PaEditerWindow->Rect.W = 230;
    PaEditerWindow->Rect.H = 180;

    const char* ListLabels[] = { "test", nullptr };

    LOG(L"Now im inside the widgets creation\n");

    YW::WIDGET* dirlabelWidget = 0;
    unsigned int* dirlabel = AllocYWMemoryFunction((YW::MEMORY*)0x200);

    YWLabelFunction((YW::LABEL_3*)dirlabel, "dir:", 5, 5, 0, 0);
    dirlabelWidget = (YW::WIDGET*)dirlabel;

    ContainerLinkFunction(PaEditerWindow, dirlabelWidget);

    YW::WIDGET* ListButtonWidget = 0;

    unsigned int* ListButton = AllocYWMemoryFunction((YW::MEMORY*)0x200);

    YWListButtonFunction((YW::LIST_BUTTON*)ListButton, 47, 5, 100, 12, 0, ListLabels, 0,0);
    ListButtonWidget = (YW::WIDGET*)ListButton;

    ContainerLinkFunction(PaEditerWindow, ListButtonWidget);
    //pa::SectionDir = ListButtonWidget

    YW::WIDGET* ButtonWidget = 0;
    unsigned int* Button = AllocYWMemoryFunction((YW::MEMORY*)0x200);

    YWButtonFunction((YW::BUTTON_3*)Button, "load", 165, 19, 60, 12, 0, 0);
    ButtonWidget = (YW::WIDGET*)Button;

    ContainerLinkFunction(PaEditerWindow, ButtonWidget);

    YW::WIDGET* nolabelWidget = 0;
    unsigned int* nolabel = AllocYWMemoryFunction((YW::MEMORY*)0x200);

    YWLabelFunction((YW::LABEL_3*)nolabel, "no:", 5, 33, 0, 0);
    nolabelWidget = (YW::WIDGET*)nolabel;

    ContainerLinkFunction(PaEditerWindow, nolabelWidget);

    YW::WIDGET* leftArrowButtonWidget = 0;
    unsigned int* leftArrowButton = AllocYWMemoryFunction((YW::MEMORY*)0x200);

    YWButtonFunction((YW::BUTTON_3*)leftArrowButton, "<", 39, 33, 10, 12, 0, 0);
    leftArrowButtonWidget = (YW::WIDGET*)leftArrowButton;

    ContainerLinkFunction(PaEditerWindow, leftArrowButtonWidget);

    YW::WIDGET* rightArrowButtonWidget = 0;
    unsigned int* rightArrowButton = AllocYWMemoryFunction((YW::MEMORY*)0x200);

    YWButtonFunction((YW::BUTTON_3*)rightArrowButton, ">", 93, 33, 10, 12, 0, 0);
    rightArrowButtonWidget = (YW::WIDGET*)rightArrowButton;

    ContainerLinkFunction(PaEditerWindow, rightArrowButtonWidget);

    YW::WIDGET* CreateButtonWidget = 0;
    unsigned int* CreateButton = AllocYWMemoryFunction((YW::MEMORY*)0x200);

    YWButtonFunction((YW::BUTTON_3*)CreateButton, "create", 113, 33, 70, 12, 0, 0);
    CreateButtonWidget = (YW::WIDGET*)CreateButton;

    ContainerLinkFunction(PaEditerWindow, CreateButtonWidget);

    YW::WIDGET* AllButtonWidget = 0;
    unsigned int* AllButton = AllocYWMemoryFunction((YW::MEMORY*)0x200);

    YWButtonFunction((YW::BUTTON_3*)AllButton, "all", 193, 33, 30, 12, 0, 0);
    AllButtonWidget = (YW::WIDGET*)AllButton;

    ContainerLinkFunction(PaEditerWindow, AllButtonWidget);

    YW::WIDGET* SeperatorlabelWidget = 0;
    unsigned int* Seperatorlabel = AllocYWMemoryFunction((YW::MEMORY*)0x200);

    YWLabelFunction((YW::LABEL_3*)Seperatorlabel, "------------------------", 5, 47, 0, 0);
    SeperatorlabelWidget = (YW::WIDGET*)nolabel;

    ContainerLinkFunction(PaEditerWindow, SeperatorlabelWidget);

    YW::WIDGET* EffectlabelWidget = 0;
    unsigned int* Effectlabel = AllocYWMemoryFunction((YW::MEMORY*)0x200);

    YWLabelFunction((YW::LABEL_3*)Effectlabel, "effect:", 5, 61, 0, 0);
    EffectlabelWidget = (YW::WIDGET*)Effectlabel;

    ContainerLinkFunction(PaEditerWindow, EffectlabelWidget);


    YW::WIDGET* leftArrowButtonWidget2 = 0;
    unsigned int* leftArrowButton2 = AllocYWMemoryFunction((YW::MEMORY*)0x200);

    YWButtonFunction((YW::BUTTON_3*)leftArrowButton2, "<", 71, 61, 10, 12, 0, 0);
    leftArrowButtonWidget2 = (YW::WIDGET*)leftArrowButton2;

    ContainerLinkFunction(PaEditerWindow, leftArrowButtonWidget2);

    YW::WIDGET* rightArrowButtonWidget2 = 0;
    unsigned int* rightArrowButton2 = AllocYWMemoryFunction((YW::MEMORY*)0x200);

    YWButtonFunction((YW::BUTTON_3*)rightArrowButton2, ">", 117, 61, 10, 12, 0, 0);
    rightArrowButtonWidget2 = (YW::WIDGET*)rightArrowButton2;

    ContainerLinkFunction(PaEditerWindow, rightArrowButtonWidget2);

    YW::WIDGET* ctgButtonWidget = 0;
    unsigned int* ctgButton = AllocYWMemoryFunction((YW::MEMORY*)0x200);

    YWButtonFunction((YW::BUTTON_3*)ctgButton, "ctg", 129, 61, 40, 12, 0, 0);
    ctgButtonWidget = (YW::WIDGET*)ctgButton;

    ContainerLinkFunction(PaEditerWindow, ctgButtonWidget);

    YW::WIDGET* bindButtonWidget = 0;
    unsigned int* bindButton = AllocYWMemoryFunction((YW::MEMORY*)0x200);

    YWButtonFunction((YW::BUTTON_3*)bindButton, "bind", 171, 61, 40, 12, 0, 0);
    bindButtonWidget = (YW::WIDGET*)bindButton;

    ContainerLinkFunction(PaEditerWindow, bindButtonWidget);

    YW::WIDGET* oneButtonWidget = 0;
    unsigned int* oneButton = AllocYWMemoryFunction((YW::MEMORY*)0x200);

    YWButtonFunction((YW::BUTTON_3*)oneButton, "1", 213, 61, 10, 12, 0, 0);
    oneButtonWidget = (YW::WIDGET*)oneButton;

    ContainerLinkFunction(PaEditerWindow, oneButtonWidget);

    YW::WIDGET* idlabelWidget = 0;
    unsigned int* idlabel = AllocYWMemoryFunction((YW::MEMORY*)0x200);

    YWLabelFunction((YW::LABEL_3*)idlabel, "id:", 5, 75, 0, 0);
    idlabelWidget = (YW::WIDGET*)idlabel;

    ContainerLinkFunction(PaEditerWindow, idlabelWidget);


    YW::WIDGET* playButtonWidget = 0;
    unsigned int* playButton = AllocYWMemoryFunction((YW::MEMORY*)0x200);

    YWButtonFunction((YW::BUTTON_3*)playButton, "play", 165, 113, 60, 12, 0, 0);
    playButtonWidget = (YW::WIDGET*)playButton;

    ContainerLinkFunction(PaEditerWindow, playButtonWidget);


    YW::WIDGET* pauseButtonWidget = 0;
    unsigned int* pauseButton = AllocYWMemoryFunction((YW::MEMORY*)0x200);

    YWButtonFunction((YW::BUTTON_3*)pauseButton, "pause", 165, 127, 60, 12, 0, 0);
    pauseButtonWidget = (YW::WIDGET*)pauseButton;

    ContainerLinkFunction(PaEditerWindow, pauseButtonWidget);


    YW::WIDGET* stopButtonWidget = 0;
    unsigned int* stopButton = AllocYWMemoryFunction((YW::MEMORY*)0x200);

    YWButtonFunction((YW::BUTTON_3*)stopButton, "stop", 165, 141, 60, 12, 0, 0);
    stopButtonWidget = (YW::WIDGET*)stopButton;

    ContainerLinkFunction(PaEditerWindow, stopButtonWidget);


    YW::WIDGET* savepaButtonWidget = 0;
    unsigned int* savepaButton = AllocYWMemoryFunction((YW::MEMORY*)0x200);

    YWButtonFunction((YW::BUTTON_3*)savepaButton, "save pa", 5, 155, 80, 12, 0, 0);
    //savepaButtonWidget = (YW::WIDGET*)savepaButton;

    ContainerLinkFunction(PaEditerWindow, savepaButtonWidget);


    YW::WIDGET* sclButtonWidget = 0;
    unsigned int* sclButton = AllocYWMemoryFunction((YW::MEMORY*)0x200);

    YWButtonFunction((YW::BUTTON_3*)sclButton, "scl[]", 165, 155, 60, 12, 0, 0);
    //sclButtonWidget = (YW::WIDGET*)sclButton;

    ContainerLinkFunction(PaEditerWindow, (YW::WIDGET*)sclButton);


    ShowDebugWindowFunction(PaEditerWindow);

    LOG(L"Called the showdebugwindowFunction\n");

}

void init_ps_editer_window(int wx, int wy) {

}


void PAediterWindow(int wx, int wy, int mode) {
    uintptr_t YWWindowFunctionAddress = ReadLEA32(Patterns::pattern_for_YWwindow, L"YW Window", 0, 1, 5);
    FunctionCalls::tYWWindow YWWindowFunction = reinterpret_cast<FunctionCalls::tYWWindow>(YWWindowFunctionAddress);
    
    LOG(L"now im here\n");

    YWWindowFunction((YW::WINDOW*)PaEditerWindow, "=== .pa editer ===", wx, wy, 230, 180, 2);

    //PaEditerWindow = this_param;
    int EditerMode = mode;

    if (EditerMode > 0)
    {
        if (EditerMode <= 1)
            init_ps_editer_window(wx, wy);
    }
    else if (EditerMode > -1)
    {
        LOG(L"just before calling pa_editer\n");
        init_pa_editer_window(wx, wy);
    }

}


void InitPAEditorUI(int x, int y, int mode) {

    uintptr_t AllocYWMemoryFunctionAddress = ReadLEA32(Patterns::pattern_for_Alloc_YW_Memory, L"AllocYWMemory", 0, 1, 5);
    auto AllocYWMemoryFunction = reinterpret_cast<FunctionCalls::tAlloc_YW_Memory>(AllocYWMemoryFunctionAddress);
    
    //if (PaEditerWindow) return nullptr;

    // Use the game's memory allocator instead of new
    PaEditerWindow = (YW::WINDOW*)AllocYWMemoryFunction((YW::MEMORY*)0x200);
    //if (!mem) return nullptr;

    //pa::PaEditerWindow = new (mem) pa::PA_EDITER_WINDOW(x, y, mode);

    LOG(L"got this far in InitPAEditorUI\n");
    PAediterWindow( x, y, mode);

    
}




void UpdateScrollbar(YW::SCROLLBAR* slider, float* scaleTarget)
{
    static float lastValue = -1.0f;
    static int lastState = -1;

    

    //YS::OBJ* obj = GetCachedMenuOBJ();

    //if (!obj) {
    //    return;
    //}

    if (!slider || !scaleTarget || !gGlobalObjPtr1)
        return;

    if (slider->State == 2) // dragging
    {
        *scaleTarget = slider->Value;

        if (slider->Value != lastValue || slider->State != lastState) {
            LOG(L"Dragging slider, Value: %f, scale: %f\n", slider->Value, *scaleTarget);
            lastValue = slider->Value;
            lastState = slider->State;
        }
    }
    else {
        lastState = slider->State;
        lastValue = slider->Value;
    }
}




void DrawYWText(const char* string, YW::RECT* rect, YW::YW_COLOR_0* color) {
    YWFontPrintFunction(rect->X + 4, rect->Y + 4, color, string);
}


struct YWTextBuffer {
    static constexpr size_t MaxLen = 1024;
    char buffer[MaxLen];
    char* pt;

    YWTextBuffer() {
        clear();
    }

    // Custom printf into buffer
    void printf(const char* fmt, ...) {
        va_list args;
        va_start(args, fmt);
        int written = vsnprintf(pt, buffer + MaxLen - pt, fmt, args);
        va_end(args);

        if (written < 0 || pt + written >= buffer + MaxLen) {
            fprintf(stderr, "[YWTextBuffer] Overflow detected!\n");
            std::abort(); // Or your custom error handling
        }
        else {
            pt += written;
        }
    }

    // Clear buffer
    void clear() {
        pt = buffer;
        buffer[0] = '\0';
    }

    // Draw using external function
    void draw(YW::RECT* rect, YW::YW_COLOR_0* color) {
        DrawYWText(buffer, rect, color);
    }

    const char* get() const {
        return buffer;
    }
};

void SearchFilesAndFolders(
    const std::wstring& path,
    const std::wstring& extension,
    std::vector<std::wstring>& entries,
    std::vector<bool>& isDirectoryList
) {
    entries.clear();
    isDirectoryList.clear();

    std::wstring searchPath = path + L"\\*";
    struct _wfinddata_t fileinfo;
    intptr_t handle = _wfindfirst(searchPath.c_str(), &fileinfo);

    std::vector<std::wstring> folders;
    std::vector<std::wstring> files;

    if (handle != -1) {
        do {
            std::wstring filename = fileinfo.name;
            if (filename == L"." || filename == L"..") continue;

            if (fileinfo.attrib & _A_SUBDIR) {
                folders.push_back(filename + L"/");
            }
            else if (filename.size() >= extension.size() &&
                filename.substr(filename.size() - extension.size()) == extension) {
                files.push_back(filename);
            }
        } while (_wfindnext(handle, &fileinfo) == 0);
        _findclose(handle);
    }

    // ".." to go back
    if (path != L"root") {
        entries.push_back(L"..\\");
        isDirectoryList.push_back(true);
    }

    for (const auto& folder : folders) {
        entries.push_back(folder);
        isDirectoryList.push_back(true);
    }

    for (const auto& file : files) {
        entries.push_back(file);
        isDirectoryList.push_back(false);
    }
}





void select_callback(YW::EVENT event, int index, unsigned int* arg) {
    auto* widget = (FileSelectionWidget*)arg;

    if (!widget || !widget->Callback || !widget->FileList)
        return;

    int selectedIndex = widget->Page * 100 + index;

    if (selectedIndex < 0 || selectedIndex >= widget->FileListNum)
        return;

    const char* selectedChar = widget->FileList[selectedIndex];
    std::wstring selected(selectedChar, selectedChar + strlen(selectedChar));

    if (widget->IsDirectoryList[selectedIndex]) {
        if (selected == L"..\\" || selected == L"../") {
            // Go up one directory
            size_t pos = widget->CurrentFolder.find_last_of(L"\\/");
            if (pos != std::wstring::npos) {
                widget->CurrentFolder = widget->CurrentFolder.substr(0, pos);
            }
        }
        else {
            // Go into the folder
            selected.pop_back(); // remove trailing slash
            widget->CurrentFolder += L"\\" + selected;
        }

        // Refresh the menu with the new path
        widget->Open(widget->CurrentFolder);
    }
    else {
        // It’s a file — return the full path
        std::wstring fullPath = widget->CurrentFolder + L"\\" + selected;
        std::string fullPathStr(fullPath.begin(), fullPath.end());
        widget->Callback(fullPathStr.c_str());
    }
}

void SetFileSelectionList(FileSelectionWidget* widget);


void button_callback(unsigned int* arg) {
    FileSelectionWidget* widget = (FileSelectionWidget*)arg;

    if (widget->PageNum > widget->Page + 1) {
        widget->Page += 1;
    }
    else {
        widget->Page = 0;
    }

    SetFileSelectionList(widget);
}


void SetFileSelectionList(FileSelectionWidget* widget) {

    uintptr_t OpenSingletonWindowFunctionAddress = ReadLEA32(Patterns::pattern_for_OpenSingletonWindow, L"OpenSingletonWindow", 0, 1, 5);
    uintptr_t AllocYWMemoryFunctionAddress = ReadLEA32(Patterns::pattern_for_Alloc_YW_Memory, L"AllocYWMemory", 0, 1, 5);
    uintptr_t ContainerLinkFunctionAddress = FindAndPrintPatternW(Patterns::pattern_for_ContainerLink, L"ContainerLink", 0);
    uintptr_t YWFrameFunctionAddress = ReadLEA32(Patterns::pattern_for_YWFrame, L"YWFrame", 0, 1, 5);
    uintptr_t WListFunctionAddress = ReadLEA32(Patterns::pattern_for_YWList, L"YWList", 0, 1, 5);
    uintptr_t ShowDebugWindowFunctionAddress = ReadLEA32(Patterns::pattern_for_ShowDebugWindow, L"ShowDebugWindow", 0, 1, 5);
    uintptr_t YWwindowMinSizeFunctionAddress = FindAndPrintPatternW(Patterns::pattern_for_YWwindow_minSize, L"YWwindow Min Size", 0);
    uintptr_t YWwindowMaxSizeFunctionAddress = FindAndPrintPatternW(Patterns::pattern_for_YWwindow_maxSize, L"YWwindow Max Size", 0);
    uintptr_t GetOBJENTRYFunctionAddress = ReadLEA32(Patterns::pattern_for_GetOBJENTRY, L"Get OBJENTRY", 0, 1, 5);
    uintptr_t YWButtonFunctionAddress = ReadLEA32(Patterns::pattern_for_YWButton, L"YWButton", 0, 1, 5);
    uintptr_t FreeYWwidgetFunctionAddress = FindAndPrintPatternW(Patterns::pattern_for_FreeYWwindowWidget, L"FREE YW WINDOW WIDGET", 0);
    uintptr_t FreeYWbuttonFunctionAddress = FindAndPrintPatternW(Patterns::pattern_for_FreeYWbuttonWidget, L"FREE YW BUTTON WIDGET", 0);
    uintptr_t FreeYWlistFunctionAddress = FindAndPrintPatternW(Patterns::pattern_for_FreeYWlistWidget, L"FREE YW LIST WIDGET", 0);
    uintptr_t FreeYWframeFunctionAddress = FindAndPrintPatternW(Patterns::pattern_for_FreeYWframeWidget, L"FREE YW FRAME WIDGET", 0);

    
    

    auto OpenSingletonWindowFunction = reinterpret_cast<FunctionCalls::tOpenSingletonWindow>(OpenSingletonWindowFunctionAddress);
    auto AllocYWMemoryFunction = reinterpret_cast<FunctionCalls::tAlloc_YW_Memory>(AllocYWMemoryFunctionAddress);
    auto ContainerLinkFunction = reinterpret_cast<FunctionCalls::tContainerLink>(ContainerLinkFunctionAddress);
    auto YWFrameFunction = reinterpret_cast<FunctionCalls::tYWFrame>(YWFrameFunctionAddress);
    auto YWListFunction = reinterpret_cast<FunctionCalls::tYWList>(WListFunctionAddress);
    auto ShowDebugWindowFunction = reinterpret_cast<FunctionCalls::tShowDebugWindow>(ShowDebugWindowFunctionAddress);
    auto YWwindowMinSizeFunction = reinterpret_cast<FunctionCalls::tYW_WINDOW_set_min_size>(YWwindowMinSizeFunctionAddress);
    auto YWwindowMaxSizeFunction = reinterpret_cast<FunctionCalls::tYW_WINDOW_set_max_size>(YWwindowMaxSizeFunctionAddress);
    auto GetOBJENTRYFunction = reinterpret_cast<FunctionCalls::tGetOBJENTRY>(GetOBJENTRYFunctionAddress);
    FunctionCalls::tYWButton YWButtonFunction = reinterpret_cast<FunctionCalls::tYWButton>(YWButtonFunctionAddress);
    FunctionCalls::tFreeYWwindowwidget FreeYWwidgetFunction = reinterpret_cast<FunctionCalls::tFreeYWwindowwidget>(FreeYWwidgetFunctionAddress);
    FunctionCalls::tFreeYWbuttonwidget FreeYWbuttonFunction = reinterpret_cast<FunctionCalls::tFreeYWbuttonwidget>(FreeYWbuttonFunctionAddress);
    FunctionCalls::tFreeYWlistwidget FreeYWlistFunction = reinterpret_cast<FunctionCalls::tFreeYWlistwidget>(FreeYWlistFunctionAddress);
    FunctionCalls::tFreeYWframewidget FreeYWframeFunction = reinterpret_cast<FunctionCalls::tFreeYWframewidget>(FreeYWframeFunctionAddress);
    

    const int ITEMS_PER_PAGE = 100;
    int start = widget->Page * ITEMS_PER_PAGE;
    int count = widget->FileListNum - start;


    if (count > ITEMS_PER_PAGE)
        count = ITEMS_PER_PAGE;

    // Fill DrawFileList
    for (int i = 0; i < count; ++i) {
        widget->DrawFileList[i] = widget->FileList[start + i];
    }
    widget->DrawFileList[count] = nullptr;

    // Cleanup existing widgets
    if (widget->Button) {
        FreeYWbuttonFunction(reinterpret_cast<YW::WIDGET_5*>(widget->Button), true);
        widget->Button = nullptr;
    }
    if (widget->List) {
        FreeYWlistFunction(reinterpret_cast<YW::WIDGET_5*>(widget->List), true);
        widget->List = nullptr;
    }
    if (widget->Frame) {
        FreeYWframeFunction(reinterpret_cast<YW::WIDGET_5*>(widget->Frame), true);
        widget->Frame = nullptr;
    }

    // Create List
    
    auto* listMem = AllocYWMemoryFunction((YW::MEMORY*)0x200);
    if (listMem) {
        YWListFunction(
            (YW::YW_LIST_0*)listMem,
            0, 0,
            widget->DrawFileList,
            select_callback,
            (unsigned int*)widget
        );
        widget->List = (YW::YW_LIST_0*)listMem;
    }

    int offsetY = 0;
    int listHeight = widget->List->Rect.H;
    int totalHeight = listHeight;

    // Page controls
    if (widget->PageNum > 1) {
        // Create page button
        auto* buttonMem = AllocYWMemoryFunction((YW::MEMORY*)0x200);
        if (buttonMem) {
            YWButtonFunction((YW::BUTTON_3*)buttonMem, ">", widget->List->Rect.W - 30, 0, 30, 12, button_callback, (unsigned int*)widget);
            widget->Button = (YW::BUTTON_3*)buttonMem;
            ContainerLinkFunction(widget, (YW::WIDGET*)widget->Button);
        }

        YWTextBuffer text;
        text.printf("Page: %d/%d", widget->Page + 1, widget->PageNum);

        YW::RECT textRect = widget->Rect;
        textRect.X = 0;
        textRect.Y = 0;
        textRect.W = widget->Rect.W;  // keep width the same or adjust as needed
        textRect.H = 12;              // height for the text line

        text.draw(&textRect, widget->Color[0]);
    }

    // Create frame and link list
    auto* frameMem = AllocYWMemoryFunction((YW::MEMORY*)0x200);
    if (frameMem) {
        YWFrameFunction((YW::FRAME*)frameMem, 0, offsetY, 0, 0, 0, listHeight);
        widget->Frame = (YW::FRAME*)frameMem;
        ContainerLinkFunction(widget->Frame, (YW::WIDGET*)widget->List);
        ContainerLinkFunction(widget, widget->Frame);
    }

    // Update window rect
    int fullW = widget->List->Rect.W + 10;
    int fullH = (widget->Rect.H < totalHeight) ? widget->Rect.H : totalHeight;
    widget->Rect.W = fullW;
    widget->Rect.H = fullH;

    YWwindowMinSizeFunction(widget, fullW, 0);
    YWwindowMaxSizeFunction(widget, fullW, totalHeight);
}



void FileSelectionMenuOpen(FileSelectionWidget** widgetPtr, const std::wstring& folderPath, const std::wstring& fileExtension, int x, int y, int h, void (*callback)(const char*))
{

    uintptr_t OpenSingletonWindowFunctionAddress = ReadLEA32(Patterns::pattern_for_OpenSingletonWindow, L"OpenSingletonWindow", 0, 1, 5);
    uintptr_t AllocYWMemoryFunctionAddress = ReadLEA32(Patterns::pattern_for_Alloc_YW_Memory, L"AllocYWMemory", 0, 1, 5);
    uintptr_t ContainerLinkFunctionAddress = FindAndPrintPatternW(Patterns::pattern_for_ContainerLink, L"ContainerLink", 0);
    uintptr_t YWFrameFunctionAddress = ReadLEA32(Patterns::pattern_for_YWFrame, L"YWFrame", 0, 1, 5);
    uintptr_t WListFunctionAddress = ReadLEA32(Patterns::pattern_for_YWList, L"YWList", 0, 1, 5);
    uintptr_t ShowDebugWindowFunctionAddress = ReadLEA32(Patterns::pattern_for_ShowDebugWindow, L"ShowDebugWindow", 0, 1, 5);
    uintptr_t YWwindowMinSizeFunctionAddress = FindAndPrintPatternW(Patterns::pattern_for_YWwindow_minSize, L"YWwindow Min Size", 0);
    uintptr_t YWwindowMaxSizeFunctionAddress = FindAndPrintPatternW(Patterns::pattern_for_YWwindow_maxSize, L"YWwindow Max Size", 0);
    uintptr_t GetOBJENTRYFunctionAddress = ReadLEA32(Patterns::pattern_for_GetOBJENTRY, L"Get OBJENTRY", 0, 1, 5);
    uintptr_t YWWindowFunctionAddress = ReadLEA32(Patterns::pattern_for_YWwindow, L"YW Window", 0, 1, 5);

    auto OpenSingletonWindowFunction = reinterpret_cast<FunctionCalls::tOpenSingletonWindow>(OpenSingletonWindowFunctionAddress);
    auto AllocYWMemoryFunction = reinterpret_cast<FunctionCalls::tAlloc_YW_Memory>(AllocYWMemoryFunctionAddress);
    auto ContainerLinkFunction = reinterpret_cast<FunctionCalls::tContainerLink>(ContainerLinkFunctionAddress);
    auto YWFrameFunction = reinterpret_cast<FunctionCalls::tYWFrame>(YWFrameFunctionAddress);
    auto YWListFunction = reinterpret_cast<FunctionCalls::tYWList>(WListFunctionAddress);
    auto ShowDebugWindowFunction = reinterpret_cast<FunctionCalls::tShowDebugWindow>(ShowDebugWindowFunctionAddress);
    auto YWwindowMinSizeFunction = reinterpret_cast<FunctionCalls::tYW_WINDOW_set_min_size>(YWwindowMinSizeFunctionAddress);
    auto YWwindowMaxSizeFunction = reinterpret_cast<FunctionCalls::tYW_WINDOW_set_max_size>(YWwindowMaxSizeFunctionAddress);
    auto GetOBJENTRYFunction = reinterpret_cast<FunctionCalls::tGetOBJENTRY>(GetOBJENTRYFunctionAddress);
    FunctionCalls::tYWWindow YWWindowFunction = reinterpret_cast<FunctionCalls::tYWWindow>(YWWindowFunctionAddress);
    


    // Allocate memory for the widget state
    *widgetPtr = (FileSelectionWidget*)AllocYWMemoryFunction((YW::MEMORY*)sizeof(FileSelectionWidget));
    FileSelectionWidget* widget = *widgetPtr;


    widget->CurrentFolder = folderPath;
    widget->Callback = callback;
    widget->Page = 0;

    // Allocate file buffer and file list
    widget->Buff = (char*)AllocYWMemoryFunction((YW::MEMORY*)0x2000);
    widget->FileList = (const char**)AllocYWMemoryFunction((YW::MEMORY*)0x800); // 512 entries
    widget->DrawFileList = (const char**)AllocYWMemoryFunction((YW::MEMORY*)0x194); // 100 entries max page

    int count = 0;
    char* bufferPtr = widget->Buff;
    size_t bufferSize = 0x2000;

    for (const auto& entry : fs::directory_iterator(folderPath)) {
        if (count >= 512) break;

        std::wstring wname = entry.path().filename().wstring();
        std::string name(wname.begin(), wname.end());

        if (entry.is_directory() || entry.path().extension() == fileExtension) {
            if (entry.is_directory()) {
                name += "/";  // Mark folders with trailing slash
            }

            size_t nameLen = name.length() + 1;  // Include null terminator
            if (nameLen > bufferSize) {
                break;  // Not enough space in buffer
            }

            // Copy name into buffer
            strcpy(bufferPtr, name.c_str());

            // Point FileList entry to buffer location
            widget->FileList[count] = bufferPtr;

            // Move buffer pointer forward
            bufferPtr += nameLen;
            bufferSize -= nameLen;

            count++;
        }
    }

    widget->FileListNum = count;
    widget->PageNum = (count + 99) / 100; // 100 files per page

    // Initialize DrawFileList with first page
    int drawCount = (count > 100) ? 100 : count;
    for (int i = 0; i < drawCount; ++i) {
        widget->DrawFileList[i] = widget->FileList[i];
    }

    SetFileSelectionList(widget);

    // Create YW Frame
    unsigned int* frameMem = AllocYWMemoryFunction((YW::MEMORY*)0x200);
    YWFrameFunction((YW::FRAME*)frameMem, 0, 0, 0, 0, 0, widget->List->Rect.H);
    widget->Frame = (YW::FRAME*)frameMem;

    // Link list to frame
    ContainerLinkFunction(widget->Frame, (YW::WIDGET*)widget->List);

    std::string title(folderPath.begin(), folderPath.end());

    YWWindowFunction((YW::WINDOW*)widget, title.c_str(), x, y, 0, h, 0);
    ContainerLinkFunction(widget, widget->Frame);

    // Set min/max size
    YWwindowMaxSizeFunction(widget, widget->List->Rect.W, widget->List->Rect.H);
    YWwindowMinSizeFunction(widget, widget->List->Rect.W, 0);

    

    // Show the window
    ShowDebugWindowFunction(widget);
}



void* MyCustomWidgetVTable[];

void MyFocusFunc(YW::BUTTON* thisPtr, const YW::EVENT* event, int x, int y)
{
    if (!event) {
        // No event — likely a hover or blur
        return;
    }

    if (event) {
        LOG("MyFocusFunc: this=%p, event=0x%p, flags=0x%X, x=%d, y=%d\n", thisPtr, event, event->Flag, x, y);
    }
    else {
        LOG("MyFocusFunc: this=%p, event=NULL, x=%d, y=%d\n", thisPtr, x, y);
    }

    LOG("event->Flag = 0x%X\n", event->Flag);

    if ((event->Flag & 0x20) != 0) // TRIGGER_1 — left click
    {
        if (thisPtr->PressCallback)
            thisPtr->PressCallback(thisPtr->PressCallbackArg);

        thisPtr->SinkTime = 5;
    }
}

void MyDestroyFunc(void* thisPtr)
{
    YW::BUTTON* btn = (YW::BUTTON*)thisPtr;

    // Reset vtable pointer to base widget vtable (replace with actual base vtable address)
    btn->_vptr_ = reinterpret_cast<const int*>(MyCustomWidgetVTable);

    // Call finalizer callback if any
    if (btn->Finalizer)
        //btn->Finalizer();

    // Unlink from parent container if present
        if (btn->Parent)
            //YW::CONTAINER::unlink(btn->Parent, (YW::WIDGET*)btn);
            UnLinkContainerFunction(btn->Parent, (YW::WIDGET*)btn);

    // Free resources you allocated, e.g. label string
    //if (btn->Label)
   // {
    //    free(btn->Label);
     //   btn->Label = nullptr;
   // }

    // Do NOT free the widget memory here — that is done by caller if needed
}

void MyDrawFunc(YW::BUTTON* thisPtr, YW::RECT* rect, YW::COLOR_0* color) {
    if (thisPtr->SinkTime <= 0) {
        DrawRectRiseWindowFunction(rect, color);
    }
    else {
        DrawRectSinkWindowFunction(rect, color);
        --thisPtr->SinkTime;
    }
    /*YWFontPrintFunction(rect->X + (rect->W - thisPtr->LabelWidth) / 2,
        rect->Y + (rect->H - thisPtr->LabelHeight) / 2,
        color + 1,
        thisPtr->Label);*/
}



void __fastcall nullsub_234(void* thisPtr) {}
void __fastcall nullsub_1491(void* thisPtr) {}

void* MyCustomWidgetVTable[] = {
    (void*)&MyDestroyFunc,
    (void*)&MyDrawFunc,
    (void*)&nullsub_234,
    (void*)&MyFocusFunc,
    (void*)&nullsub_1491
};


void CreateMyCustomButton(
    YW::BUTTON* thisPtr,
    const char* label,
    int x, int y, int w, int h,
    void (*press_callback)(unsigned int*),
    unsigned int* callback_arg)
{

    uintptr_t AllocYWMemoryFunctionAddress = ReadLEA32(Patterns::pattern_for_Alloc_YW_Memory, L"AllocYWMemory", 0, 1, 5);

    uintptr_t ContainerLinkFunctionAddress = FindAndPrintPatternW(Patterns::pattern_for_ContainerLink, L"ContainerLink", 0);

    FunctionCalls::tAlloc_YW_Memory AllocYWMemoryFunction = reinterpret_cast<FunctionCalls::tAlloc_YW_Memory>(AllocYWMemoryFunctionAddress);

    FunctionCalls::tContainerLink ContainerLinkFunction = reinterpret_cast<FunctionCalls::tContainerLink>(ContainerLinkFunctionAddress);

    static const YW::COLOR_0 GamePalette_ButtonNormal = { 128, 128, 128, 128 };
    static const YW::COLOR_0 GamePalette_ButtonPressed = { 192, 192, 192, 128 };


    // Allocate memory (via game allocator or malloc)
   // auto* btn = (YW::BUTTON*)AllocYWMemoryFunction((YW::MEMORY*)sizeof(YW::BUTTON));
   // if (!btn)
      //  return nullptr;

    //memset(btn, 0, sizeof(YW::BUTTON));

    // Call base widget constructor (or replicate it manually)
    //YWWidgetConstructorFunction((YW::WIDGET*)btn, x, y, w, h); // <- if available
    // Or emulate like this:
    // btn->Rect = { x, y, w, h };
    // btn->DrawRect = btn->Rect;
    // btn->Frame = btn->Rect;
    // btn->Offset = 0;
    // btn->Parent = nullptr; btn->Next = nullptr;
    // btn->Finalizer = nullptr;

    // Assign your custom vtable
    //*(void**)btn = MyCustomWidgetVTable;

    WidgetFunction((YW::WIDGET*)thisPtr, x, y, w, h);

    // Duplicate label (safe copy)
    thisPtr->Label = _strdup(label);  // or strdup depending on platform
    thisPtr->PressCallback = press_callback;
    thisPtr->PressCallbackArg = callback_arg;
    thisPtr->SinkTime = 0;
    thisPtr->_vptr_ = reinterpret_cast<const int*>(MyCustomWidgetVTable);
    thisPtr->LabelWidth = FontGetTextHeightFunction(label);
    thisPtr->LabelHeight = FontGetTextWidthFunction(label);
    thisPtr->Color[0] = &GamePalette_ButtonNormal;   // From game
    thisPtr->Color[1] = &GamePalette_ButtonPressed;  // From game

    
}



void SetOBJColorR(int R) {

    

    

    
        //gGlobalObjPtr = GetMenuOBJFunction();
        const size_t offset = 0x04D8; // Offset to Color_R member
        uint8_t* offsetPtr = reinterpret_cast<uint8_t*>(gGlobalObjPtr1) + offset;

        // Set specific values to the members at the preset offset
        offsetPtr[0] = R;
        OBJColorR = R;
    
}

void SetOBJColorG(int G) {

    

    

    
        const size_t offset = 0x04D8; // Offset to Color_R member
        uint8_t* offsetPtr = reinterpret_cast<uint8_t*>(gGlobalObjPtr1) + offset;

        // Set specific values to the members at the preset offset
        offsetPtr[1] = G;
        OBJColorG = G;
    
}

void SetOBJColorB(int B) {


   

    
        const size_t offset = 0x04D8; // Offset to Color_R member
        uint8_t* offsetPtr = reinterpret_cast<uint8_t*>(gGlobalObjPtr1) + offset;

        // Set specific values to the members at the preset offset
        offsetPtr[2] = B;
        OBJColorB = B;
    
}

void SetOBJColorA(int A) {


    

    
        const size_t offset = 0x04D8; // Offset to Color_R member
        uint8_t* offsetPtr = reinterpret_cast<uint8_t*>(gGlobalObjPtr1) + offset;

        // Set specific values to the members at the preset offset
        offsetPtr[3] = A;
        OBJColorA = A;
    
}

void ChangeOBJColors(unsigned int* arg) {

    uintptr_t OpenSingletonWindowFunctionAddress = ReadLEA32(Patterns::pattern_for_OpenSingletonWindow, L"OpenSingletonWindow", 0, 1, 5);

    uintptr_t AllocYWMemoryFunctionAddress = ReadLEA32(Patterns::pattern_for_Alloc_YW_Memory, L"AllocYWMemory", 0, 1, 5);

    uintptr_t WListFunctionAddress = ReadLEA32(Patterns::pattern_for_YWList, L"YWList", 0, 1, 5);


    uintptr_t YWFrameFunctionAddress = ReadLEA32(Patterns::pattern_for_YWFrame, L"YWFrame", 0, 1, 5);


    uintptr_t ContainerLinkFunctionAddress = FindAndPrintPatternW(Patterns::pattern_for_ContainerLink, L"ContainerLink", 0);//muuta tohon FindAndPrintPatternW 0


    uintptr_t YWwindowMinSizeFunctionAddress = FindAndPrintPatternW(Patterns::pattern_for_YWwindow_minSize, L"YWwindow Min Size", 0);// muuta tohon FindAndPrintPatternW 0


    uintptr_t YWwindowMaxSizeFunctionAddress = FindAndPrintPatternW(Patterns::pattern_for_YWwindow_maxSize, L"YWwindow Max Size", 0);


    uintptr_t ShowDebugWindowFunctionAddress = ReadLEA32(Patterns::pattern_for_ShowDebugWindow, L"ShowDebugWindow", 0, 1, 5);

    uintptr_t YWLabelFunctionAddress = ReadLEA32(Patterns::pattern_for_YWLabel, L"YWlabel", 0, 1, 5);

    uintptr_t DialWidgetFunctionAddress = ReadLEA32(Patterns::pattern_for_DialWidget, L"DialWidget", 0, 1, 5);

    uintptr_t SetDialValueFunctionAddress = FindAndPrintPatternW(Patterns::pattern_for_SetDialValue, L"Set DialValue", 0);





    FunctionCalls::tAlloc_YW_Memory AllocYWMemoryFunction = reinterpret_cast<FunctionCalls::tAlloc_YW_Memory>(AllocYWMemoryFunctionAddress);
    FunctionCalls::tYWList YWListFunction = reinterpret_cast<FunctionCalls::tYWList>(WListFunctionAddress);
    FunctionCalls::tYWFrame YWFrameFunction = reinterpret_cast<FunctionCalls::tYWFrame>(YWFrameFunctionAddress);
    FunctionCalls::tContainerLink ContainerLinkFunction = reinterpret_cast<FunctionCalls::tContainerLink>(ContainerLinkFunctionAddress);
    FunctionCalls::tYW_WINDOW_set_min_size YWwindowMinSizeFunction = reinterpret_cast<FunctionCalls::tYW_WINDOW_set_min_size>(YWwindowMinSizeFunctionAddress);
    FunctionCalls::tYW_WINDOW_set_max_size YWwindowMaxSizeFunction = reinterpret_cast<FunctionCalls::tYW_WINDOW_set_max_size>(YWwindowMaxSizeFunctionAddress);
    FunctionCalls::tShowDebugWindow ShowDebugWindowFunction = reinterpret_cast<FunctionCalls::tShowDebugWindow>(ShowDebugWindowFunctionAddress);

    FunctionCalls::tOpenSingletonWindow OpenSingletonWindowFunction = reinterpret_cast<FunctionCalls::tOpenSingletonWindow>(OpenSingletonWindowFunctionAddress);
    FunctionCalls::tYWLabel YWLabelFunction = reinterpret_cast<FunctionCalls::tYWLabel>(YWLabelFunctionAddress);
    FunctionCalls::tDialWidget DialWidgetFunction = reinterpret_cast<FunctionCalls::tDialWidget>(DialWidgetFunctionAddress);
    FunctionCalls::tSetDialValue SetDialValueFunction = reinterpret_cast<FunctionCalls::tSetDialValue>(SetDialValueFunctionAddress);
    

    
    YW::YW_WINDOW_39* Menu = OpenSingletonWindowFunction("Change OBJ colors", 136, 116, 0);
    // auto SlidebarAlloc = Alloc_YW_Memory((YW_MEMORY*)0x200);
     //YW_SLIDEBAR((YW_SLIDEBAR_0*)SlidebarAlloc, 5, 10, 96, 16, 0.0, 255.0, 10.0, 0);
    // ContainerLink((YW_CONTAINER_5*)Menu, (YW_WIDGET_5*)SlidebarAlloc);
    auto allocYWMemDialR = AllocYWMemoryFunction((YW::MEMORY*)0x200);
    auto allocYWMemDialG = AllocYWMemoryFunction((YW::MEMORY*)0x200);
    auto allocYWMemDialB = AllocYWMemoryFunction((YW::MEMORY*)0x200);
    auto allocYWMemDialA = AllocYWMemoryFunction((YW::MEMORY*)0x200);

    auto allocYWMemLabelR = AllocYWMemoryFunction((YW::MEMORY*)0x200);
    auto allocYWMemLabelG = AllocYWMemoryFunction((YW::MEMORY*)0x200);
    auto allocYWMemLabelB = AllocYWMemoryFunction((YW::MEMORY*)0x200);
    auto allocYWMemLabelA = AllocYWMemoryFunction((YW::MEMORY*)0x200);

    YWLabelFunction((YW::LABEL_3*)allocYWMemLabelR, "R:", 5, 5, 80, 12);
    YWLabelFunction((YW::LABEL_3*)allocYWMemLabelG, "G:", 5, 25, 80, 12);
    YWLabelFunction((YW::LABEL_3*)allocYWMemLabelB, "B:", 5, 45, 80, 12);
    YWLabelFunction((YW::LABEL_3*)allocYWMemLabelA, "A:", 5, 65, 80, 12);

    DialWidgetFunction((YW::DIAL_3*)allocYWMemDialR, 30, 5, 96, 16, 0, 255, SetOBJColorR);
    DialWidgetFunction((YW::DIAL_3*)allocYWMemDialG, 30, 25, 96, 16, 0, 255, SetOBJColorG);
    DialWidgetFunction((YW::DIAL_3*)allocYWMemDialB, 30, 45, 96, 16, 0, 255, SetOBJColorB);
    DialWidgetFunction((YW::DIAL_3*)allocYWMemDialA, 30, 65, 96, 16, 0, 255, SetOBJColorA);

    SetDialValueFunction((YW::DIAL_3*)allocYWMemDialR, OBJColorR);
    SetDialValueFunction((YW::DIAL_3*)allocYWMemDialG, OBJColorG);
    SetDialValueFunction((YW::DIAL_3*)allocYWMemDialB, OBJColorB);
    SetDialValueFunction((YW::DIAL_3*)allocYWMemDialA, OBJColorA);



    ContainerLinkFunction((YW::CONTAINER*)Menu, (YW::WIDGET*)allocYWMemDialR);
    ContainerLinkFunction((YW::CONTAINER*)Menu, (YW::WIDGET*)allocYWMemDialG);
    ContainerLinkFunction((YW::CONTAINER*)Menu, (YW::WIDGET*)allocYWMemDialB);
    ContainerLinkFunction((YW::CONTAINER*)Menu, (YW::WIDGET*)allocYWMemDialA);

    ContainerLinkFunction((YW::CONTAINER*)Menu, (YW::WIDGET*)allocYWMemLabelR);
    ContainerLinkFunction((YW::CONTAINER*)Menu, (YW::WIDGET*)allocYWMemLabelG);
    ContainerLinkFunction((YW::CONTAINER*)Menu, (YW::WIDGET*)allocYWMemLabelB);
    ContainerLinkFunction((YW::CONTAINER*)Menu, (YW::WIDGET*)allocYWMemLabelA);

    ShowDebugWindowFunction((YW::WIDGET*)Menu);
}


void ClearPlayerList(const char* PlayerList[], size_t size) {
    for (size_t i = 0; i < size; ++i) {
        PlayerList[i] = nullptr;  // Reset the pointer to nullptr
    }
}

int compareEntries(const void* a, const void* b) {
    // Cast the input to the correct type
    const OBJENTRY_0* entryA = *(const OBJENTRY_0**)a;
    const OBJENTRY_0* entryB = *(const OBJENTRY_0**)b;

    // Perform string comparison using EntryName
    return strcmp(entryA->EntryName, entryB->EntryName);
}






void ProcessOBJENTRYData(int type, const char* PlayerList[], size_t& playerCount) {
    // Ensure to clear the PlayerList before starting
    //ClearPlayerList(PlayerList, 1024);

    // Get the address of the function that retrieves OBJENTRY data
    uintptr_t GetOBJENTRYFunctionAddress = ReadLEA32(Patterns::pattern_for_GetOBJENTRY, L"Get OBJENTRY", 0, 1, 5);
    FunctionCalls::tGetOBJENTRY GetOBJENTRYFunction = reinterpret_cast<FunctionCalls::tGetOBJENTRY>(GetOBJENTRYFunctionAddress);

    // Reset playerCount to 0
    //playerCount = 0;

    // Loop through all OBJENTRY_IDs (from 0 to 2561)
    for (OBJENTRY_ID id = 0; id <= 2561; ++id) {
        OBJENTRY_0* entry = GetOBJENTRYFunction(id);  // Get entry by ID

        // Ensure valid entry and we haven't exceeded max players
        if (entry != nullptr && playerCount < 100) {
            // Filter by type, only store entries that match the given type
            if (entry->Type == type) {
                // Add entry to the PlayerList
                PlayerList[playerCount++] = entry->EntryName;
            }
        }
    }

    // Sort the PlayerList
    //qsort(PlayerList, playerCount, sizeof(const char*), compareEntries);

    // Check if no valid players were found and handle gracefully
    if (playerCount == 0) {
        const char* errorMessage = "No entries found for this type\n";
        PlayerList[0] = errorMessage;  // Assign error message to the first entry in the PlayerList
        LOG(L"No valid players found for type: %d\n", type);
    }
    else {
        LOG(L"OBJENTRY loop finished. Total players added: %zu\n", playerCount);  // Debug log
    }

    // Debugging: Check the contents of PlayerList
    for (size_t i = 0; i < playerCount; ++i) {
        if (PlayerList[i] != nullptr) {
            LOG(L"Player %zu: %s\n", i, PlayerList[i]);
        }
        else {
            LOG(L"Player %zu: NULL entry\n", i);
        }
    }
}

void PrintOBJENTRYNamesByType(int type) {
    // Resolve the function pointer to GetOBJENTRY
    uintptr_t GetOBJENTRYFunctionAddress = ReadLEA32(Patterns::pattern_for_GetOBJENTRY, L"Get OBJENTRY", 0, 1, 5);
    FunctionCalls::tGetOBJENTRY GetOBJENTRYFunction = reinterpret_cast<FunctionCalls::tGetOBJENTRY>(GetOBJENTRYFunctionAddress);

    size_t matchCount = 0;

    for (OBJENTRY_ID id = 0; id <= 2561; ++id) {
        OBJENTRY_0* entry = GetOBJENTRYFunction(id);
        if (entry && entry->Type == type) {
            LOG(L"Match %zu: ID %u - Name: %S\n", matchCount++, id, entry->EntryName);
        }
    }

    if (matchCount == 0) {
        LOG(L"No entries found for type: %d\n", type);
    }
    else {
        LOG(L"Total entries found for type %d: %zu\n", type, matchCount);
    }
}






/*void PopulatePlayerListList(const char*** PlayerListList, size_t* size) {
    *size = PlayerList.size();  // Get the number of entries
    *PlayerListList = new const char* [*size];  // Allocate memory

    // Copy pointers from PlayerList to PlayerListList
    for (size_t i = 0; i < *size; ++i) {
        (*PlayerListList)[i] = PlayerList[i];
    }
}*/

void FreePlayerListList(const char** PlayerListList, size_t playerCount) {
    // Set each pointer to nullptr
    for (size_t i = 0; i < playerCount; ++i) {
        PlayerListList[i] = nullptr;  // Set each pointer to nullptr
    }
    //PlayerListList = nullptr;

    // Optionally, you can delete the array if it's no longer needed
    // delete[] PlayerListList;  // Uncomment this line if you want to free the memory
}

YS::PAD* CustomGet(int playerIndex) {
    uintptr_t GetSoftPADFunctionAddress = ReadLEA32(Patterns::pattern_for_GetSoftPad, L"Get Soft PAD", 0, 1, 5);
    FunctionCalls::tGetSoftPad GetSoftPADFunction = reinterpret_cast<FunctionCalls::tGetSoftPad>(GetSoftPADFunctionAddress);


    
        if (playerIndex == 0) {
            return GetSoftPADFunction(0); // For Player 1
        }
        else {
            return GetSoftPADFunction(1); // For Player 2
        }
    
}

void HookSoftPad() {
    // Define your patch pattern for the original YS::SOFTPAD::Get function
    const wchar_t* patchPattern = Patterns::pattern_for_GetSoftPad; // Replace with actual bytes
    size_t patchSize = sizeof(patchPattern); // Size of the pattern
    const wchar_t* patchName = L"Hook YS::SOFTPAD::Get";

    // Determine the address to patch
    uint64_t returnAddress = 0;

    // Call your WritePatchPattern_Hook function to perform the patching
    WritePatchPattern_Hook(patchPattern, patchSize, patchName, 0, (void*)CustomGet, &returnAddress);
}

void OBJMenuAddMenu(YS::ELEM* elem) {
    InitializePointers();
    uintptr_t DBGMenuAddMenuElemFunctionAddress = FindAndPrintPatternW(Patterns::pattern_for_AddDebugMenuElem, L"DebugMenu Add Menu Elem", 0);
    FunctionCalls::tDBGWINaddMenuElem DBGMenuAddMenuElemFunction = reinterpret_cast<FunctionCalls::tDBGWINaddMenuElem>(DBGMenuAddMenuElemFunctionAddress);
    uintptr_t YS_MenuElemAddress = 0;
    
    if (gameVersion == "Steam") {
        YS_MenuElemAddress = (SteamPointer + 0x1FEC068);

    }
    else if (gameVersion == "Epic") {
        YS_MenuElemAddress = (EpicPointer + 0x1FEC068);
    }
     
    YS::ELEM* menu = reinterpret_cast<YS::ELEM*>(YS_MenuElemAddress);

    
    //LOG("YS_MenuElemAddress: 0x%p", reinterpret_cast<void*>(YS_MenuElemAddress));

    DBGMenuAddMenuElemFunction(menu, 32, elem);
}

bool IsModuleLoaded(const char* moduleName) {
    return GetModuleHandleA(moduleName) != NULL;
}

std::string DetectGameVersion() {
    if (IsModuleLoaded("steam_api64.dll")) {
        //LOG("Steam Version");
        return "Steam";
        
    }
    else if (IsModuleLoaded("EOSSDK-Win64-Shipping.dll")) {
        //LOG("EPIC VERSION");
        return "Epic";
        
    }
    else {
        //LOG("UNKNOWN VERSION");
        return "Unknown";
    }
}

void InitializePointers() {
    if (gameVersion == "Steam") {
        SteamPointer = reinterpret_cast<uintptr_t>(baseModule) + 0xB0D598;
        // Use SteamPointer for the Steam version
    }
    else if (gameVersion == "Epic") {
        EpicPointer = reinterpret_cast<uintptr_t>(baseModule) + 0xB0D018;
        // Use EpicPointer for the Epic version
    }
}

void NewObjThread(TASK* task) {
    //printf("NewObjThread started\n");
    //Vector4 CameraPos = (*(Vector4*)(moduleBase + 0x716A28));
    uintptr_t GetOBJENTRYFunctionAddress = ReadLEA32(Patterns::pattern_for_GetOBJENTRY, L"Get OBJENTRY", 0, 1, 5);
    uintptr_t ReadRequestFunctionAddress = ReadLEA32(Patterns::pattern_for_ReadRequest, L"Read Request", 0, 1, 5);
    uintptr_t FlushCacheBuffFunctionAddress = ReadLEA32(Patterns::pattern_for_FlushCacheBuff, L"Flush Cachebuff", 0, 1, 5);
    uintptr_t OBJENTRYAppearFunctionAddress = ReadLEA32(Patterns::pattern_for_OBJENTRY_APPEAR, L"OBJENTRY APPEAR", 0, 1, 5);
    uintptr_t RandomPosPlayerFunctionAddress = FindAndPrintPatternW(Patterns::pattern_for_RandomPosPlayer, L"Random Pos Player", 0);
    FunctionCalls::tGetOBJENTRY GetOBJENTRYFunction = reinterpret_cast<FunctionCalls::tGetOBJENTRY>(GetOBJENTRYFunctionAddress);
    FunctionCalls::tReadRequest ReadRequestFunction = reinterpret_cast<FunctionCalls::tReadRequest>(ReadRequestFunctionAddress);
    FunctionCalls::tFlushCacheBuff FlushCacheBuffFunction = reinterpret_cast<FunctionCalls::tFlushCacheBuff>(FlushCacheBuffFunctionAddress);
    FunctionCalls::tOBJENTRY_APPEAR OBJENTRYAppearFunction = reinterpret_cast<FunctionCalls::tOBJENTRY_APPEAR>(OBJENTRYAppearFunctionAddress);
    FunctionCalls::tUnitGroupRandomPosPlayer RandomPosPlayerFunction = reinterpret_cast<FunctionCalls::tUnitGroupRandomPosPlayer>(RandomPosPlayerFunctionAddress);
    
    //uintptr_t SteamPointer = reinterpret_cast<uintptr_t>(baseModule) + 0xB0D598;

   // Vector4 CameraPos = (*(Vector4*)(SteamPointer - 0x3F4920));



    //LOG("Camera Positions: %1.0f, %1.0f, %1.0f, %1.0f",
     //   CameraPos.x, CameraPos.y, CameraPos.z, CameraPos.w);
    //Vector4 CameraRot = (*(Vector4*)(moduleBase + 0x716A18));// replace with steam version address
    //float angle = atan2f(CameraRot.x, -CameraRot.y);
    //double rotation = angle * 180.0 / 3.14159265358979323846 + 180.0;
    auto entry = GetOBJENTRYFunction(myObjEntryId);
    auto request = (unsigned int)myObjEntryId;
    int v1 = 0;
    Vector4 randomPosition = { 0.0f, 0.0f, 0.0f, 1.0f };

    if (entry->Type == 22) {

        if (entry->Subtype == 14 || (v1 = 0, entry->Subtype == 15)) {
            request = (unsigned int)myObjEntryId | 0x40000000LL;
            v1 = 1;
            myObjEntryId |= 0x40000000LL;
        }
    }

    ReadRequestFunction(request, pri, bank);
    FlushCacheBuffFunction(task);
    int incNum = 0;

    Entity* player = GetPlayer1();

    if (player) {

        RandomPosPlayerFunction(&randomPosition, request);

    }
    else {
        randomPosition = { 0.0f, 0.0f, 0.0f, 1.0f };
    }

        if (obj_spawnamount1 > 0) {
            //double rotation = 0.0;
            do {
                //auto player = gGetPlayer();
                g_player2Obj = OBJENTRYAppearFunction(myObjEntryId, randomPosition, 0.0);
                ++incNum;
            } while (incNum < obj_spawnamount1);
        }
    
    //Cache_BuffDump();
    // size_t allocatorSize = GetMemoryAllocatorSize((MemoryAllocator*)GetAreaAllocator());
    // size_t allocatorUsedSize = GetMemoryAllocatorUsedSize((MemoryAllocator*)GetAreaAllocator());
    // double allocatorSizeMB = static_cast<double>(allocatorSize) / (1024 * 1024);
    // double allocatorUsedSizeMB = static_cast<double>(allocatorUsedSize) / (1024 * 1024);
     //double remainingSizeMB = static_cast<double>(allocatorSize - allocatorUsedSize) / (1024 * 1024);
     //DebugPrint("Remaining Size: %.3f MB\n", remainingSizeMB);
     //printf("Used Memory: %.3f MB / Max Memory: %.3f MB\n", allocatorUsedSizeMB, allocatorSizeMB);

 //printf("NewObjThread ended\n");
}

void NewLevelEditorObjThread(TASK* task) {
    uintptr_t GetOBJENTRYFunctionAddress = ReadLEA32(Patterns::pattern_for_GetOBJENTRY, L"Get OBJENTRY", 0, 1, 5);
    uintptr_t ReadRequestFunctionAddress = ReadLEA32(Patterns::pattern_for_ReadRequest, L"Read Request", 0, 1, 5);
    uintptr_t FlushCacheBuffFunctionAddress = ReadLEA32(Patterns::pattern_for_FlushCacheBuff, L"Flush Cachebuff", 0, 1, 5);
    uintptr_t OBJENTRYAppearFunctionAddress = ReadLEA32(Patterns::pattern_for_OBJENTRY_APPEAR, L"OBJENTRY APPEAR", 0, 1, 5);
    uintptr_t SetOBJGroundPositionFunctionAddress = ReadLEA32(Patterns::pattern_for_SetObjGroundPos, L"Set obj ground position", 0, 1, 5);

    auto GetOBJENTRYFunction = reinterpret_cast<FunctionCalls::tGetOBJENTRY>(GetOBJENTRYFunctionAddress);
    auto ReadRequestFunction = reinterpret_cast<FunctionCalls::tReadRequest>(ReadRequestFunctionAddress);
    auto FlushCacheBuffFunction = reinterpret_cast<FunctionCalls::tFlushCacheBuff>(FlushCacheBuffFunctionAddress);
    auto OBJENTRYAppearFunction = reinterpret_cast<FunctionCalls::tOBJENTRY_APPEAR>(OBJENTRYAppearFunctionAddress);
    auto SetOBJGroundPositionFunction = reinterpret_cast<FunctionCalls::tSetObjGroundPos>(SetOBJGroundPositionFunctionAddress);

    const OBJENTRY_0* entry = GetOBJENTRYFunction(myObjEntryId);
    unsigned int request = static_cast<unsigned int>(myObjEntryId);
    int v1 = 0;

    if (entry->Type == 22) {
        if (entry->Subtype == 14 || entry->Subtype == 15) {
            request = myObjEntryId | 0x40000000;
            v1 = 1;
            myObjEntryId |= 0x40000000;
        }
    }

    ReadRequestFunction(request, pri, bank);
    FlushCacheBuffFunction(task);

    int incNum = 0;

    if (obj_spawnamount1 > 0) {
        do {
            YS::OBJ* SpawnObject = OBJENTRYAppearFunction(myObjEntryId, AxisCursorPos, 0.0);

            if (snapToGroundEnabled) {
                SetOBJGroundPositionFunction(SpawnObject);
            }

            if (SpawnObject) {
                g_spawnedObjects.push_back(SpawnObject);
                ++g_SpawnedEntityCount;
                ++incNum;

                printf("[DEBUG] Spawned normal object with ID: %d\n", myObjEntryId);
            }
        } while (incNum < obj_spawnamount1);
    }
}


void NewLevelEditorPreviewThread(TASK* task) {
    uintptr_t GetOBJENTRYFunctionAddress = ReadLEA32(Patterns::pattern_for_GetOBJENTRY, L"Get OBJENTRY", 0, 1, 5);
    uintptr_t ReadRequestFunctionAddress = ReadLEA32(Patterns::pattern_for_ReadRequest, L"Read Request", 0, 1, 5);
    uintptr_t FlushCacheBuffFunctionAddress = ReadLEA32(Patterns::pattern_for_FlushCacheBuff, L"Flush Cachebuff", 0, 1, 5);
    uintptr_t OBJENTRYAppearFunctionAddress = ReadLEA32(Patterns::pattern_for_OBJENTRY_APPEAR, L"OBJENTRY APPEAR", 0, 1, 5);
    uintptr_t TaskSuicideFunctionAddress = FindAndPrintPatternW(Patterns::pattern_for_TaskSuicide, L"Task Suicide", 0);
    FunctionCalls::tTaskSuicide TaskSuicideFunction = reinterpret_cast<FunctionCalls::tTaskSuicide>(TaskSuicideFunctionAddress);

    auto GetOBJENTRYFunction = reinterpret_cast<FunctionCalls::tGetOBJENTRY>(GetOBJENTRYFunctionAddress);
    auto ReadRequestFunction = reinterpret_cast<FunctionCalls::tReadRequest>(ReadRequestFunctionAddress);
    auto FlushCacheBuffFunction = reinterpret_cast<FunctionCalls::tFlushCacheBuff>(FlushCacheBuffFunctionAddress);
    auto OBJENTRYAppearFunction = reinterpret_cast<FunctionCalls::tOBJENTRY_APPEAR>(OBJENTRYAppearFunctionAddress);

    const OBJENTRY_0* entry = GetOBJENTRYFunction(previewEntityId);
    unsigned int request = static_cast<unsigned int>(previewEntityId);

    if (entry->Type == 22 && (entry->Subtype == 14 || entry->Subtype == 15)) {
        request |= 0x40000000;
        previewEntityId |= 0x40000000;
    }

    ReadRequestFunction(request, pri, bank);
    FlushCacheBuffFunction(task);

    // Spawn preview object at cursor
    YS::OBJ* obj = OBJENTRYAppearFunction(request, AxisCursorPos, 0.0);

    if (obj) {
        previewObject = obj;
        previewEntity = ToEntity(previewObject);
        isPreviewSpawned = true;

        TaskSuicideFunction(task);

        printf("[DEBUG] Spawned preview object with ID: 0x%X\n", request);
    }
    else {
        TaskSuicideFunction(task);
        printf("[DEBUG] Failed to spawn preview object for ID: %d\n", request);
    }
}




void NewObjThread2(TASK* task) {
    //printf("NewObjThread started\n");
    //Vector4 CameraPos = (*(Vector4*)(moduleBase + 0x716A28));
    uintptr_t GetOBJENTRYFunctionAddress = ReadLEA32(Patterns::pattern_for_GetOBJENTRY, L"Get OBJENTRY", 0, 1, 5);
    uintptr_t ReadRequestFunctionAddress = ReadLEA32(Patterns::pattern_for_ReadRequest, L"Read Request", 0, 1, 5);
    uintptr_t FlushCacheBuffFunctionAddress = ReadLEA32(Patterns::pattern_for_FlushCacheBuff, L"Flush Cachebuff", 0, 1, 5);
    uintptr_t OBJENTRYAppearFunctionAddress = ReadLEA32(Patterns::pattern_for_OBJENTRY_APPEAR, L"OBJENTRY APPEAR", 0, 1, 5);
    uintptr_t RandomPosPlayerFunctionAddress = FindAndPrintPatternW(Patterns::pattern_for_RandomPosPlayer, L"Random Pos Player", 0);
    FunctionCalls::tUnitGroupRandomPosPlayer RandomPosPlayerFunction = reinterpret_cast<FunctionCalls::tUnitGroupRandomPosPlayer>(RandomPosPlayerFunctionAddress);
    FunctionCalls::tGetOBJENTRY GetOBJENTRYFunction = reinterpret_cast<FunctionCalls::tGetOBJENTRY>(GetOBJENTRYFunctionAddress);
    FunctionCalls::tReadRequest ReadRequestFunction = reinterpret_cast<FunctionCalls::tReadRequest>(ReadRequestFunctionAddress);
    FunctionCalls::tFlushCacheBuff FlushCacheBuffFunction = reinterpret_cast<FunctionCalls::tFlushCacheBuff>(FlushCacheBuffFunctionAddress);
    FunctionCalls::tOBJENTRY_APPEAR OBJENTRYAppearFunction = reinterpret_cast<FunctionCalls::tOBJENTRY_APPEAR>(OBJENTRYAppearFunctionAddress);
    

    //uintptr_t SteamPointer = reinterpret_cast<uintptr_t>(baseModule) + 0xB0D598;

   // Vector4 CameraPos = (*(Vector4*)(SteamPointer - 0x3F4920));



    //LOG("Camera Positions: %1.0f, %1.0f, %1.0f, %1.0f",
     //   CameraPos.x, CameraPos.y, CameraPos.z, CameraPos.w);
    //Vector4 CameraRot = (*(Vector4*)(moduleBase + 0x716A18));// replace with steam version address
    //float angle = atan2f(CameraRot.x, -CameraRot.y);
    //double rotation = angle * 180.0 / 3.14159265358979323846 + 180.0;
    auto entry = GetOBJENTRYFunction(IdRequest2);
    auto request = (unsigned int)IdRequest2;
    int v1 = 0;
    Vector4 randomPosition = { 0.0f, 0.0f, 0.0f, 0.0f };

    if (entry->Type == 22) {

        if (entry->Subtype == 14 || (v1 = 0, entry->Subtype == 15)) {
            request = (unsigned int)IdRequest2 | 0x40000000LL;
            v1 = 1;
            IdRequest2 |= 0x40000000LL;
        }
    }

    ReadRequestFunction(request, pri, bank);
    FlushCacheBuffFunction(task);
    int incNum = 0;

    Entity* player = GetPlayer1();

    if (player) {

        RandomPosPlayerFunction(&randomPosition, request);

    }
    else {
        randomPosition = { 0.0f, 0.0f, 0.0f, 1.0f };
    }

    if (obj_spawnamount2 > 0) {
        //double rotation = 0.0;
        do {
            //auto player = gGetPlayer();
            auto SpawnObject = OBJENTRYAppearFunction(IdRequest2, randomPosition, 0.0);
            ++incNum;
        } while (incNum < obj_spawnamount2);
    }
    //Cache_BuffDump();
    // size_t allocatorSize = GetMemoryAllocatorSize((MemoryAllocator*)GetAreaAllocator());
    // size_t allocatorUsedSize = GetMemoryAllocatorUsedSize((MemoryAllocator*)GetAreaAllocator());
    // double allocatorSizeMB = static_cast<double>(allocatorSize) / (1024 * 1024);
    // double allocatorUsedSizeMB = static_cast<double>(allocatorUsedSize) / (1024 * 1024);
     //double remainingSizeMB = static_cast<double>(allocatorSize - allocatorUsedSize) / (1024 * 1024);
     //DebugPrint("Remaining Size: %.3f MB\n", remainingSizeMB);
     //printf("Used Memory: %.3f MB / Max Memory: %.3f MB\n", allocatorUsedSizeMB, allocatorSizeMB);

 //printf("NewObjThread ended\n");
}

void NewObjThread3(TASK* task) {
    //printf("NewObjThread started\n");
    //Vector4 CameraPos = (*(Vector4*)(moduleBase + 0x716A28));
    uintptr_t GetOBJENTRYFunctionAddress = ReadLEA32(Patterns::pattern_for_GetOBJENTRY, L"Get OBJENTRY", 0, 1, 5);
    uintptr_t ReadRequestFunctionAddress = ReadLEA32(Patterns::pattern_for_ReadRequest, L"Read Request", 0, 1, 5);
    uintptr_t FlushCacheBuffFunctionAddress = ReadLEA32(Patterns::pattern_for_FlushCacheBuff, L"Flush Cachebuff", 0, 1, 5);
    uintptr_t OBJENTRYAppearFunctionAddress = ReadLEA32(Patterns::pattern_for_OBJENTRY_APPEAR, L"OBJENTRY APPEAR", 0, 1, 5);
    uintptr_t RandomPosPlayerFunctionAddress = FindAndPrintPatternW(Patterns::pattern_for_RandomPosPlayer, L"Random Pos Player", 0);
    FunctionCalls::tGetOBJENTRY GetOBJENTRYFunction = reinterpret_cast<FunctionCalls::tGetOBJENTRY>(GetOBJENTRYFunctionAddress);
    FunctionCalls::tReadRequest ReadRequestFunction = reinterpret_cast<FunctionCalls::tReadRequest>(ReadRequestFunctionAddress);
    FunctionCalls::tFlushCacheBuff FlushCacheBuffFunction = reinterpret_cast<FunctionCalls::tFlushCacheBuff>(FlushCacheBuffFunctionAddress);
    FunctionCalls::tOBJENTRY_APPEAR OBJENTRYAppearFunction = reinterpret_cast<FunctionCalls::tOBJENTRY_APPEAR>(OBJENTRYAppearFunctionAddress);
    FunctionCalls::tUnitGroupRandomPosPlayer RandomPosPlayerFunction = reinterpret_cast<FunctionCalls::tUnitGroupRandomPosPlayer>(RandomPosPlayerFunctionAddress);

    //uintptr_t SteamPointer = reinterpret_cast<uintptr_t>(baseModule) + 0xB0D598;

   // Vector4 CameraPos = (*(Vector4*)(SteamPointer - 0x3F4920));



    //LOG("Camera Positions: %1.0f, %1.0f, %1.0f, %1.0f",
     //   CameraPos.x, CameraPos.y, CameraPos.z, CameraPos.w);
    //Vector4 CameraRot = (*(Vector4*)(moduleBase + 0x716A18));// replace with steam version address
    //float angle = atan2f(CameraRot.x, -CameraRot.y);
    //double rotation = angle * 180.0 / 3.14159265358979323846 + 180.0;
    auto entry = GetOBJENTRYFunction(IdRequest3);
    auto request = (unsigned int)IdRequest3;
    int v1 = 0;
    Vector4 randomPosition = { 0.0f, 0.0f, 0.0f, 0.0f };

    if (entry->Type == 22) {

        if (entry->Subtype == 14 || (v1 = 0, entry->Subtype == 15)) {
            request = (unsigned int)IdRequest3 | 0x40000000LL;
            v1 = 1;
            IdRequest3 |= 0x40000000LL;
        }
    }

    ReadRequestFunction(request, pri, bank);
    FlushCacheBuffFunction(task);
    int incNum = 0;

    Entity* player = GetPlayer1();

    if (player) {

        RandomPosPlayerFunction(&randomPosition, request);

    }
    else {
        randomPosition = { 0.0f, 0.0f, 0.0f, 1.0f };
    }

    if (obj_spawnamount3 > 0) {
        //double rotation = 0.0;
        do {
            //auto player = gGetPlayer();
            auto SpawnObject = OBJENTRYAppearFunction(IdRequest3, randomPosition, 0.0);
            ++incNum;
        } while (incNum < obj_spawnamount3);
    }
    //Cache_BuffDump();
    // size_t allocatorSize = GetMemoryAllocatorSize((MemoryAllocator*)GetAreaAllocator());
    // size_t allocatorUsedSize = GetMemoryAllocatorUsedSize((MemoryAllocator*)GetAreaAllocator());
    // double allocatorSizeMB = static_cast<double>(allocatorSize) / (1024 * 1024);
    // double allocatorUsedSizeMB = static_cast<double>(allocatorUsedSize) / (1024 * 1024);
     //double remainingSizeMB = static_cast<double>(allocatorSize - allocatorUsedSize) / (1024 * 1024);
     //DebugPrint("Remaining Size: %.3f MB\n", remainingSizeMB);
     //printf("Used Memory: %.3f MB / Max Memory: %.3f MB\n", allocatorUsedSizeMB, allocatorSizeMB);

 //printf("NewObjThread ended\n");
}

void NewObjThread4(TASK* task) {
    //printf("NewObjThread started\n");
    //Vector4 CameraPos = (*(Vector4*)(moduleBase + 0x716A28));
    uintptr_t GetOBJENTRYFunctionAddress = ReadLEA32(Patterns::pattern_for_GetOBJENTRY, L"Get OBJENTRY", 0, 1, 5);
    uintptr_t ReadRequestFunctionAddress = ReadLEA32(Patterns::pattern_for_ReadRequest, L"Read Request", 0, 1, 5);
    uintptr_t FlushCacheBuffFunctionAddress = ReadLEA32(Patterns::pattern_for_FlushCacheBuff, L"Flush Cachebuff", 0, 1, 5);
    uintptr_t OBJENTRYAppearFunctionAddress = ReadLEA32(Patterns::pattern_for_OBJENTRY_APPEAR, L"OBJENTRY APPEAR", 0, 1, 5);
    uintptr_t RandomPosPlayerFunctionAddress = FindAndPrintPatternW(Patterns::pattern_for_RandomPosPlayer, L"Random Pos Player", 0);
    FunctionCalls::tGetOBJENTRY GetOBJENTRYFunction = reinterpret_cast<FunctionCalls::tGetOBJENTRY>(GetOBJENTRYFunctionAddress);
    FunctionCalls::tReadRequest ReadRequestFunction = reinterpret_cast<FunctionCalls::tReadRequest>(ReadRequestFunctionAddress);
    FunctionCalls::tFlushCacheBuff FlushCacheBuffFunction = reinterpret_cast<FunctionCalls::tFlushCacheBuff>(FlushCacheBuffFunctionAddress);
    FunctionCalls::tOBJENTRY_APPEAR OBJENTRYAppearFunction = reinterpret_cast<FunctionCalls::tOBJENTRY_APPEAR>(OBJENTRYAppearFunctionAddress);
    FunctionCalls::tUnitGroupRandomPosPlayer RandomPosPlayerFunction = reinterpret_cast<FunctionCalls::tUnitGroupRandomPosPlayer>(RandomPosPlayerFunctionAddress);

    //uintptr_t SteamPointer = reinterpret_cast<uintptr_t>(baseModule) + 0xB0D598;

   // Vector4 CameraPos = (*(Vector4*)(SteamPointer - 0x3F4920));



    //LOG("Camera Positions: %1.0f, %1.0f, %1.0f, %1.0f",
    //LOG("Camera Positions: %1.0f, %1.0f, %1.0f, %1.0f",
     //   CameraPos.x, CameraPos.y, CameraPos.z, CameraPos.w);
    //Vector4 CameraRot = (*(Vector4*)(moduleBase + 0x716A18));// replace with steam version address
    //float angle = atan2f(CameraRot.x, -CameraRot.y);
    //double rotation = angle * 180.0 / 3.14159265358979323846 + 180.0;
    auto entry = GetOBJENTRYFunction(IdRequest4);
    auto request = (unsigned int)IdRequest4;
    int v1 = 0;
    Vector4 randomPosition = { 0.0f, 0.0f, 0.0f, 0.0f };

    if (entry->Type == 22) {

        if (entry->Subtype == 14 || (v1 = 0, entry->Subtype == 15)) {
            request = (unsigned int)IdRequest4 | 0x40000000LL;
            v1 = 1;
            IdRequest4 |= 0x40000000LL;
        }
    }

    ReadRequestFunction(request, pri, bank);
    FlushCacheBuffFunction(task);
    int incNum = 0;

    Entity* player = GetPlayer1();

    if (player) {

        RandomPosPlayerFunction(&randomPosition, request);

    }
    else {
        randomPosition = { 0.0f, 0.0f, 0.0f, 1.0f };
    }

    if (obj_spawnamount4 > 0) {
        //double rotation = 0.0;
        do {
            //auto player = gGetPlayer();
            auto SpawnObject = OBJENTRYAppearFunction(IdRequest4, randomPosition, 0.0);
            ++incNum;
        } while (incNum < obj_spawnamount4);
    }
    //Cache_BuffDump();
    // size_t allocatorSize = GetMemoryAllocatorSize((MemoryAllocator*)GetAreaAllocator());
    // size_t allocatorUsedSize = GetMemoryAllocatorUsedSize((MemoryAllocator*)GetAreaAllocator());
    // double allocatorSizeMB = static_cast<double>(allocatorSize) / (1024 * 1024);
    // double allocatorUsedSizeMB = static_cast<double>(allocatorUsedSize) / (1024 * 1024);
     //double remainingSizeMB = static_cast<double>(allocatorSize - allocatorUsedSize) / (1024 * 1024);
     //DebugPrint("Remaining Size: %.3f MB\n", remainingSizeMB);
     //printf("Used Memory: %.3f MB / Max Memory: %.3f MB\n", allocatorUsedSizeMB, allocatorSizeMB);

 //printf("NewObjThread ended\n");
}

void NewObjThread5(TASK* task) {
    //printf("NewObjThread started\n");
    //Vector4 CameraPos = (*(Vector4*)(moduleBase + 0x716A28));
    uintptr_t GetOBJENTRYFunctionAddress = ReadLEA32(Patterns::pattern_for_GetOBJENTRY, L"Get OBJENTRY", 0, 1, 5);
    uintptr_t ReadRequestFunctionAddress = ReadLEA32(Patterns::pattern_for_ReadRequest, L"Read Request", 0, 1, 5);
    uintptr_t FlushCacheBuffFunctionAddress = ReadLEA32(Patterns::pattern_for_FlushCacheBuff, L"Flush Cachebuff", 0, 1, 5);
    uintptr_t OBJENTRYAppearFunctionAddress = ReadLEA32(Patterns::pattern_for_OBJENTRY_APPEAR, L"OBJENTRY APPEAR", 0, 1, 5);
    uintptr_t RandomPosPlayerFunctionAddress = FindAndPrintPatternW(Patterns::pattern_for_RandomPosPlayer, L"Random Pos Player", 0);
    FunctionCalls::tGetOBJENTRY GetOBJENTRYFunction = reinterpret_cast<FunctionCalls::tGetOBJENTRY>(GetOBJENTRYFunctionAddress);
    FunctionCalls::tReadRequest ReadRequestFunction = reinterpret_cast<FunctionCalls::tReadRequest>(ReadRequestFunctionAddress);
    FunctionCalls::tFlushCacheBuff FlushCacheBuffFunction = reinterpret_cast<FunctionCalls::tFlushCacheBuff>(FlushCacheBuffFunctionAddress);
    FunctionCalls::tOBJENTRY_APPEAR OBJENTRYAppearFunction = reinterpret_cast<FunctionCalls::tOBJENTRY_APPEAR>(OBJENTRYAppearFunctionAddress);
    FunctionCalls::tUnitGroupRandomPosPlayer RandomPosPlayerFunction = reinterpret_cast<FunctionCalls::tUnitGroupRandomPosPlayer>(RandomPosPlayerFunctionAddress);

    //uintptr_t SteamPointer = reinterpret_cast<uintptr_t>(baseModule) + 0xB0D598;

   // Vector4 CameraPos = (*(Vector4*)(SteamPointer - 0x3F4920));



    //LOG("Camera Positions: %1.0f, %1.0f, %1.0f, %1.0f",
     //   CameraPos.x, CameraPos.y, CameraPos.z, CameraPos.w);
    //Vector4 CameraRot = (*(Vector4*)(moduleBase + 0x716A18));// replace with steam version address
    //float angle = atan2f(CameraRot.x, -CameraRot.y);
    //double rotation = angle * 180.0 / 3.14159265358979323846 + 180.0;
    auto entry = GetOBJENTRYFunction(IdRequest5);
    auto request = (unsigned int)IdRequest5;
    int v1 = 0;
    Vector4 randomPosition = { 0.0f, 0.0f, 0.0f, 0.0f };

    if (entry->Type == 22) {

        if (entry->Subtype == 14 || (v1 = 0, entry->Subtype == 15)) {
            request = (unsigned int)IdRequest5 | 0x40000000LL;
            v1 = 1;
            IdRequest5 |= 0x40000000LL;
        }
    }

    ReadRequestFunction(request, pri, bank);
    FlushCacheBuffFunction(task);
    int incNum = 0;

    Entity* player = GetPlayer1();

    if (player) {

        RandomPosPlayerFunction(&randomPosition, request);

    }
    else {
        randomPosition = { 0.0f, 0.0f, 0.0f, 1.0f };
    }

    if (obj_spawnamount5 > 0) {
        //double rotation = 0.0;
        do {
            //auto player = gGetPlayer();
            auto SpawnObject = OBJENTRYAppearFunction(IdRequest5, randomPosition, 0.0);
            ++incNum;
        } while (incNum < obj_spawnamount5);
    }
    //Cache_BuffDump();
    // size_t allocatorSize = GetMemoryAllocatorSize((MemoryAllocator*)GetAreaAllocator());
    // size_t allocatorUsedSize = GetMemoryAllocatorUsedSize((MemoryAllocator*)GetAreaAllocator());
    // double allocatorSizeMB = static_cast<double>(allocatorSize) / (1024 * 1024);
    // double allocatorUsedSizeMB = static_cast<double>(allocatorUsedSize) / (1024 * 1024);
     //double remainingSizeMB = static_cast<double>(allocatorSize - allocatorUsedSize) / (1024 * 1024);
     //DebugPrint("Remaining Size: %.3f MB\n", remainingSizeMB);
     //printf("Used Memory: %.3f MB / Max Memory: %.3f MB\n", allocatorUsedSizeMB, allocatorSizeMB);

 //printf("NewObjThread ended\n");
}

void SafeChangeForm(YS::SORA* sora, int targetFormId) {

    uintptr_t GetOBJENTRYFunctionAddress = ReadLEA32(Patterns::pattern_for_GetOBJENTRY, L"Get OBJENTRY", 0, 1, 5);
    uintptr_t ReadRequestFunctionAddress = ReadLEA32(Patterns::pattern_for_ReadRequest, L"Read Request", 0, 1, 5);
    uintptr_t FlushCacheBuffFunctionAddress = ReadLEA32(Patterns::pattern_for_FlushCacheBuff, L"Flush Cachebuff", 0, 1, 5);
    uintptr_t OBJENTRYAppearFunctionAddress = ReadLEA32(Patterns::pattern_for_OBJENTRY_APPEAR, L"OBJENTRY APPEAR", 0, 1, 5);
    uintptr_t ChangeFormCaptureFunctionAddress = FindAndPrintPatternW(Patterns::pattern_for_ChangeFormCapture, L"Change Form Capture", 0);
    uintptr_t ChangeFormCaptureEndFunctionAddress = FindAndPrintPatternW(Patterns::pattern_for_ChangeFormCaptureEnd, L"Change Form Capture End", 0);

    uintptr_t GetPlayerFunctionAddress = ReadLEA32(Patterns::pattern_for_GetPlayer, L"Get Player", 0, 1, 5);
    uintptr_t GetSoraFunctionAddress = ReadLEA32(Patterns::pattern_for_GetSora, L"Get Sora", 0, 1, 5);
    uintptr_t ChangeFormFunctionAddress = ReadLEA32(Patterns::pattern_for_ChangeForm, L"Get Form", 0, 1, 5);
    uintptr_t AllocAREAFunctionAddress = ReadLEA32(Patterns::pattern_for_AllocArea, L"ALLOC AREA", 0, 1, 5);


    FunctionCalls::tGetOBJENTRY GetOBJENTRYFunction = reinterpret_cast<FunctionCalls::tGetOBJENTRY>(GetOBJENTRYFunctionAddress);
    FunctionCalls::tReadRequest ReadRequestFunction = reinterpret_cast<FunctionCalls::tReadRequest>(ReadRequestFunctionAddress);
    FunctionCalls::tFlushCacheBuff FlushCacheBuffFunction = reinterpret_cast<FunctionCalls::tFlushCacheBuff>(FlushCacheBuffFunctionAddress);
    FunctionCalls::tOBJENTRY_APPEAR OBJENTRYAppearFunction = reinterpret_cast<FunctionCalls::tOBJENTRY_APPEAR>(OBJENTRYAppearFunctionAddress);
    FunctionCalls::tChangeFormCapture ChangeFormCaptureFunction = reinterpret_cast<FunctionCalls::tChangeFormCapture>(ChangeFormCaptureFunctionAddress);
    FunctionCalls::tGetPlayer GetPlayerFunction = reinterpret_cast<FunctionCalls::tGetPlayer>(GetPlayerFunctionAddress);
    FunctionCalls::tGetSora GetSoraFunction = reinterpret_cast<FunctionCalls::tGetSora>(GetSoraFunctionAddress);
    FunctionCalls::tChangeForm ChangeFormFunction = reinterpret_cast<FunctionCalls::tChangeForm>(ChangeFormFunctionAddress);
    FunctionCalls::tAllocArea AllocAREAFunction = reinterpret_cast<FunctionCalls::tAllocArea>(AllocAREAFunctionAddress);
    FunctionCalls::tChangeFormCaptureEnd ChangeFormCaptureEndFunction = reinterpret_cast<FunctionCalls::tChangeFormCaptureEnd>(ChangeFormCaptureEndFunctionAddress);

    if (!sora) return;

    int* currentFormPtr = (int*)((uintptr_t)sora + 0xDE0);

    if (*currentFormPtr == 12) {
        // Sora is in Drive form, end it safely
        wprintf(L"Drive form detected. Calling CaptureEnd.\n");

        ChangeFormCaptureEndFunction(sora);

        // Wait until Sora exits Drive form
        while (*currentFormPtr == 12) {
            Sleep(10); // allow time for transition
        }
    }

    // Now change to the desired form
    wprintf(L"Changing to target form: 0x%X\n", targetFormId);
    UINT* area = AllocAREAFunction(0x38);
    if (area) {
        ChangeFormFunction((YS::CHANGEFORM*)area, sora, 12, targetFormId);
    }
}


void NewCaptureThread(TASK* task) {
    uintptr_t GetOBJENTRYFunctionAddress = ReadLEA32(Patterns::pattern_for_GetOBJENTRY, L"Get OBJENTRY", 0, 1, 5);
    uintptr_t ReadRequestFunctionAddress = ReadLEA32(Patterns::pattern_for_ReadRequest, L"Read Request", 0, 1, 5);
    uintptr_t FlushCacheBuffFunctionAddress = ReadLEA32(Patterns::pattern_for_FlushCacheBuff, L"Flush Cachebuff", 0, 1, 5);
    uintptr_t OBJENTRYAppearFunctionAddress = ReadLEA32(Patterns::pattern_for_OBJENTRY_APPEAR, L"OBJENTRY APPEAR", 0, 1, 5);
    uintptr_t ChangeFormCaptureFunctionAddress = FindAndPrintPatternW(Patterns::pattern_for_ChangeFormCapture, L"Change Form Capture", 0);
    uintptr_t ChangeFormCaptureEndFunctionAddress = FindAndPrintPatternW(Patterns::pattern_for_ChangeFormCaptureEnd, L"Change Form Capture End", 0);
    
    uintptr_t GetPlayerFunctionAddress = ReadLEA32(Patterns::pattern_for_GetPlayer, L"Get Player", 0, 1, 5);
    uintptr_t GetSoraFunctionAddress = ReadLEA32(Patterns::pattern_for_GetSora, L"Get Sora", 0, 1, 5);
    uintptr_t ChangeFormFunctionAddress = ReadLEA32(Patterns::pattern_for_ChangeForm, L"Get Form", 0, 1, 5);
    uintptr_t AllocAREAFunctionAddress = ReadLEA32(Patterns::pattern_for_AllocArea, L"ALLOC AREA", 0, 1, 5);
    
    
    FunctionCalls::tGetOBJENTRY GetOBJENTRYFunction = reinterpret_cast<FunctionCalls::tGetOBJENTRY>(GetOBJENTRYFunctionAddress);
    FunctionCalls::tReadRequest ReadRequestFunction = reinterpret_cast<FunctionCalls::tReadRequest>(ReadRequestFunctionAddress);
    FunctionCalls::tFlushCacheBuff FlushCacheBuffFunction = reinterpret_cast<FunctionCalls::tFlushCacheBuff>(FlushCacheBuffFunctionAddress);
    FunctionCalls::tOBJENTRY_APPEAR OBJENTRYAppearFunction = reinterpret_cast<FunctionCalls::tOBJENTRY_APPEAR>(OBJENTRYAppearFunctionAddress);
    FunctionCalls::tChangeFormCapture ChangeFormCaptureFunction = reinterpret_cast<FunctionCalls::tChangeFormCapture>(ChangeFormCaptureFunctionAddress);
    FunctionCalls::tGetPlayer GetPlayerFunction = reinterpret_cast<FunctionCalls::tGetPlayer>(GetPlayerFunctionAddress);
    FunctionCalls::tGetSora GetSoraFunction = reinterpret_cast<FunctionCalls::tGetSora>(GetSoraFunctionAddress);
    FunctionCalls::tChangeForm ChangeFormFunction = reinterpret_cast<FunctionCalls::tChangeForm>(ChangeFormFunctionAddress);
    FunctionCalls::tAllocArea AllocAREAFunction = reinterpret_cast<FunctionCalls::tAllocArea>(AllocAREAFunctionAddress);
    FunctionCalls::tChangeFormCaptureEnd ChangeFormCaptureEndFunction = reinterpret_cast<FunctionCalls::tChangeFormCaptureEnd>(ChangeFormCaptureEndFunctionAddress);
    

    YS::SORA* sora = GetSoraFunction();
    if (!sora) {
        wprintf(L"Failed to retrieve Sora pointer.\n");
        return;
    }

    ReadRequestFunction(myObjEntryId, pri, 4); //0x1388
    FlushCacheBuffFunction(task);

    //ChangeFormCaptureFunction((YS::SORA*)GetSoraFunction, myObjEntryId);
    //UINT* AllocChangeFormStart = AllocAREAFunction(0x38);
   // UINT* AllocChangeFormEnd = AllocAREAFunction(0x38);

    //ChangeFormFunction((YS::CHANGEFORM*)AllocChangeFormEnd, GetSoraFunction(), 0, 0x54);

    //ChangeFormCaptureEndFunction(GetSoraFunction());
    SafeChangeForm(sora, myObjEntryId);

    //ChangeFormFunction((YS::CHANGEFORM*)AllocChangeFormStart, GetSoraFunction(), 12, myObjEntryId);

}

void LeaveAllThread(TASK* task) {
    YS::OBJ* v2;
    YS::OBJ* v3;

    uintptr_t EachOBJFunctionAddress = ReadLEA32(Patterns::pattern_for_EachOBJ, L"Each OBJ", 0, 1, 5);
    uintptr_t IsOBJValidFunctionAddress = ReadLEA32(Patterns::pattern_for_IsObjValid, L"Is OBJ Valid?", 0, 1, 5);
    uintptr_t TaskSleepFunctionAddress = ReadLEA32(Patterns::pattern_for_TaskSleep, L"Task Sleep", 0, 1, 5);
    uintptr_t DestroyCacheBuffFunctionAddress = FindAndPrintPatternW(Patterns::pattern_for_DestroyCacheBuff, L"Destroy Cache_Buff", 0);
    uintptr_t GetOBJENTRYFunctionAddress = ReadLEA32(Patterns::pattern_for_GetOBJENTRY, L"Get OBJENTRY", 0, 1, 5);
    uintptr_t CreateFieldThreadFunctionAddress = ReadLEA32(Patterns::pattern_for_CreateFieldThread, L"CreateField Thread", 0, 1, 5);

    FunctionCalls::tEachObj EachOBJFunction = reinterpret_cast<FunctionCalls::tEachObj>(EachOBJFunctionAddress);
    FunctionCalls::tIsOBJValid IsOBJValidFunction = reinterpret_cast<FunctionCalls::tIsOBJValid>(IsOBJValidFunctionAddress);
    FunctionCalls::tTaskSleep TaskSleepFunction = reinterpret_cast<FunctionCalls::tTaskSleep>(TaskSleepFunctionAddress);
    FunctionCalls::tDestroyCacheBuffPri DestroyCacheBuffFunction = reinterpret_cast<FunctionCalls::tDestroyCacheBuffPri>(DestroyCacheBuffFunctionAddress);
    FunctionCalls::tGetOBJENTRY GetOBJENTRYFunction = reinterpret_cast<FunctionCalls::tGetOBJENTRY>(GetOBJENTRYFunctionAddress);
    FunctionCalls::tCreateFieldThread CreateFieldThreadFunction = reinterpret_cast<FunctionCalls::tCreateFieldThread>(CreateFieldThreadFunctionAddress);

    v2 = EachOBJFunction(0);
    if (v2) {
        do {
            v3 = EachOBJFunction(v2);
            while ((unsigned int)IsOBJValidFunction(v2))
                TaskSleepFunction(task, 0);

            v2 = v3;
        }

        while (v3);
    }
    TaskSleepFunction(task, 2);
    DestroyCacheBuffFunction(pri);

}

void ReloadThread(TASK* task) {
    YS::OBJ* v2;
    YS::OBJ* v3;
    int v11;
    const OBJENTRY_0* v12;
    char v13;

    uintptr_t EachOBJFunctionAddress = ReadLEA32(Patterns::pattern_for_EachOBJ, L"Each OBJ", 0, 1, 5);
    uintptr_t IsOBJValidFunctionAddress = ReadLEA32(Patterns::pattern_for_IsObjValid, L"Is OBJ Valid?", 0, 1, 5);
    uintptr_t TaskSleepFunctionAddress = ReadLEA32(Patterns::pattern_for_TaskSleep, L"Task Sleep", 0, 1, 5);
    uintptr_t DestroyCacheBuffFunctionAddress = FindAndPrintPatternW(Patterns::pattern_for_DestroyCacheBuff, L"Destroy Cache_Buff", 0);
    uintptr_t GetOBJENTRYFunctionAddress = ReadLEA32(Patterns::pattern_for_GetOBJENTRY, L"Get OBJENTRY", 0, 1, 5);
    uintptr_t CreateFieldThreadFunctionAddress = ReadLEA32(Patterns::pattern_for_CreateFieldThread, L"CreateField Thread", 0, 1, 5);
    
    FunctionCalls::tEachObj EachOBJFunction = reinterpret_cast<FunctionCalls::tEachObj>(EachOBJFunctionAddress);
    FunctionCalls::tIsOBJValid IsOBJValidFunction = reinterpret_cast<FunctionCalls::tIsOBJValid>(IsOBJValidFunctionAddress);
    FunctionCalls::tTaskSleep TaskSleepFunction = reinterpret_cast<FunctionCalls::tTaskSleep>(TaskSleepFunctionAddress);
    FunctionCalls::tDestroyCacheBuffPri DestroyCacheBuffFunction = reinterpret_cast<FunctionCalls::tDestroyCacheBuffPri>(DestroyCacheBuffFunctionAddress);
    FunctionCalls::tGetOBJENTRY GetOBJENTRYFunction = reinterpret_cast<FunctionCalls::tGetOBJENTRY>(GetOBJENTRYFunctionAddress);
    FunctionCalls::tCreateFieldThread CreateFieldThreadFunction = reinterpret_cast<FunctionCalls::tCreateFieldThread>(CreateFieldThreadFunctionAddress);


    v2 = EachOBJFunction(0);
    if (v2) {
        do {
            v3 = EachOBJFunction(v2);
            while ((unsigned int)IsOBJValidFunction(v2))
                TaskSleepFunction(task, 0);

           v2 = v3;
        }

     while (v3);
    }
    TaskSleepFunction(task, 2);
    DestroyCacheBuffFunction(pri);

    v11 = myObjEntryId;
    v12 = GetOBJENTRYFunction(myObjEntryId);
    if (v12) {
        v13 = v12->Type;
        myObjEntryId = v11;
        entry_type = v13;
        obj_spawnamount1 = 1;
        CreateFieldThreadFunction(1, 1000, (TASK_FUNC)&NewObjThread, 0x2000);

    }

}

void NewObj(OBJENTRY_ID entry_id, int spawn_amount) {
    uintptr_t GetOBJENTRYFunctionAddress = ReadLEA32(Patterns::pattern_for_GetOBJENTRY, L"Get OBJENTRY", 0, 1, 5);
    uintptr_t CreateFieldThreadFunctionAddress = ReadLEA32(Patterns::pattern_for_CreateFieldThread, L"CreateField Thread", 0, 1, 5);
    FunctionCalls::tGetOBJENTRY GetOBJENTRYFunction = reinterpret_cast<FunctionCalls::tGetOBJENTRY>(GetOBJENTRYFunctionAddress);
    FunctionCalls::tCreateFieldThread CreateFieldThreadFunction = reinterpret_cast<FunctionCalls::tCreateFieldThread>(CreateFieldThreadFunctionAddress);
    
    auto entry = GetOBJENTRYFunction(entry_id);
    if ((DWORD)entry) {
        entry_type = entry->Type;
        myObjEntryId = entry_id;
        obj_spawnamount1 = spawn_amount;

        CreateFieldThreadFunction(0, 1000, (TASK_FUNC)&NewObjThread, 0x2000);

    }

}

void NewLevelEditorObj(OBJENTRY_ID entry_id, int spawn_amount) {
    uintptr_t GetOBJENTRYFunctionAddress = ReadLEA32(Patterns::pattern_for_GetOBJENTRY, L"Get OBJENTRY", 0, 1, 5);
    uintptr_t CreateFieldThreadFunctionAddress = ReadLEA32(Patterns::pattern_for_CreateFieldThread, L"CreateField Thread", 0, 1, 5);
    FunctionCalls::tGetOBJENTRY GetOBJENTRYFunction = reinterpret_cast<FunctionCalls::tGetOBJENTRY>(GetOBJENTRYFunctionAddress);
    FunctionCalls::tCreateFieldThread CreateFieldThreadFunction = reinterpret_cast<FunctionCalls::tCreateFieldThread>(CreateFieldThreadFunctionAddress);

    auto entry = GetOBJENTRYFunction(entry_id);
    if ((DWORD)entry) {
        entry_type = entry->Type;
        myObjEntryId = entry_id;
        obj_spawnamount1 = spawn_amount;

        CreateFieldThreadFunction(0, 1000, (TASK_FUNC)&NewLevelEditorObjThread, 0x2000);

    }

}

void NewWorldPosObjThread(TASK* task) {
    //printf("NewObjThread started\n");
    //Vector4 CameraPos = (*(Vector4*)(moduleBase + 0x716A28));
    uintptr_t GetOBJENTRYFunctionAddress = ReadLEA32(Patterns::pattern_for_GetOBJENTRY, L"Get OBJENTRY", 0, 1, 5);
    uintptr_t ReadRequestFunctionAddress = ReadLEA32(Patterns::pattern_for_ReadRequest, L"Read Request", 0, 1, 5);
    uintptr_t FlushCacheBuffFunctionAddress = ReadLEA32(Patterns::pattern_for_FlushCacheBuff, L"Flush Cachebuff", 0, 1, 5);
    uintptr_t OBJENTRYAppearFunctionAddress = ReadLEA32(Patterns::pattern_for_OBJENTRY_APPEAR, L"OBJENTRY APPEAR", 0, 1, 5);
    uintptr_t RandomPosPlayerFunctionAddress = FindAndPrintPatternW(Patterns::pattern_for_RandomPosPlayer, L"Random Pos Player", 0);
    FunctionCalls::tGetOBJENTRY GetOBJENTRYFunction = reinterpret_cast<FunctionCalls::tGetOBJENTRY>(GetOBJENTRYFunctionAddress);
    FunctionCalls::tReadRequest ReadRequestFunction = reinterpret_cast<FunctionCalls::tReadRequest>(ReadRequestFunctionAddress);
    FunctionCalls::tFlushCacheBuff FlushCacheBuffFunction = reinterpret_cast<FunctionCalls::tFlushCacheBuff>(FlushCacheBuffFunctionAddress);
    FunctionCalls::tOBJENTRY_APPEAR OBJENTRYAppearFunction = reinterpret_cast<FunctionCalls::tOBJENTRY_APPEAR>(OBJENTRYAppearFunctionAddress);
    FunctionCalls::tUnitGroupRandomPosPlayer RandomPosPlayerFunction = reinterpret_cast<FunctionCalls::tUnitGroupRandomPosPlayer>(RandomPosPlayerFunctionAddress);

    //uintptr_t SteamPointer = reinterpret_cast<uintptr_t>(baseModule) + 0xB0D598;

   // Vector4 CameraPos = (*(Vector4*)(SteamPointer - 0x3F4920));



    //LOG("Camera Positions: %1.0f, %1.0f, %1.0f, %1.0f",
     //   CameraPos.x, CameraPos.y, CameraPos.z, CameraPos.w);
    //Vector4 CameraRot = (*(Vector4*)(moduleBase + 0x716A18));// replace with steam version address
    //float angle = atan2f(CameraRot.x, -CameraRot.y);
    //double rotation = angle * 180.0 / 3.14159265358979323846 + 180.0;
    auto entry = GetOBJENTRYFunction(myObjEntryId);
    auto request = (unsigned int)myObjEntryId;
    int v1 = 0;
   // Vector4 randomPosition = { 0.0f, 0.0f, 0.0f, 0.0f };

    if (entry->Type == 22) {

        if (entry->Subtype == 14 || (v1 = 0, entry->Subtype == 15)) {
            request = (unsigned int)myObjEntryId | 0x40000000LL;
            v1 = 1;
            myObjEntryId |= 0x40000000LL;
        }
    }

    ReadRequestFunction(request, pri, bank);
    FlushCacheBuffFunction(task);
    int incNum = 0;

    //RandomPosPlayerFunction(&randomPosition, request);

    if (obj_spawnamount1 > 0) {
        //double rotation = 0.0;
        do {
            //auto player = gGetPlayer();
            auto SpawnObject = OBJENTRYAppearFunction(myObjEntryId, ObjSpawnWorldPos, 0.0);
            ++incNum;
        } while (incNum < obj_spawnamount1);
    }
    //Cache_BuffDump();
    // size_t allocatorSize = GetMemoryAllocatorSize((MemoryAllocator*)GetAreaAllocator());
    // size_t allocatorUsedSize = GetMemoryAllocatorUsedSize((MemoryAllocator*)GetAreaAllocator());
    // double allocatorSizeMB = static_cast<double>(allocatorSize) / (1024 * 1024);
    // double allocatorUsedSizeMB = static_cast<double>(allocatorUsedSize) / (1024 * 1024);
     //double remainingSizeMB = static_cast<double>(allocatorSize - allocatorUsedSize) / (1024 * 1024);
     //DebugPrint("Remaining Size: %.3f MB\n", remainingSizeMB);
     //printf("Used Memory: %.3f MB / Max Memory: %.3f MB\n", allocatorUsedSizeMB, allocatorSizeMB);

 //printf("NewObjThread ended\n");
}

void NewWorldPosObj(OBJENTRY_ID entry_id, int spawn_amount,Vector4 WorldPos) {
    uintptr_t GetOBJENTRYFunctionAddress = ReadLEA32(Patterns::pattern_for_GetOBJENTRY, L"Get OBJENTRY", 0, 1, 5);
    uintptr_t CreateFieldThreadFunctionAddress = ReadLEA32(Patterns::pattern_for_CreateFieldThread, L"CreateField Thread", 0, 1, 5);
    FunctionCalls::tGetOBJENTRY GetOBJENTRYFunction = reinterpret_cast<FunctionCalls::tGetOBJENTRY>(GetOBJENTRYFunctionAddress);
    FunctionCalls::tCreateFieldThread CreateFieldThreadFunction = reinterpret_cast<FunctionCalls::tCreateFieldThread>(CreateFieldThreadFunctionAddress);

    auto entry = GetOBJENTRYFunction(entry_id);
    if ((DWORD)entry) {
        entry_type = entry->Type;
        myObjEntryId = entry_id;
        obj_spawnamount1 = spawn_amount;
        ObjSpawnWorldPos = WorldPos;

        CreateFieldThreadFunction(0, 1000, (TASK_FUNC)&NewWorldPosObjThread, 0x2000);

    }

}

void NewObj1(OBJENTRY_ID entry_id) {
    uintptr_t GetOBJENTRYFunctionAddress = ReadLEA32(Patterns::pattern_for_GetOBJENTRY, L"Get OBJENTRY", 0, 1, 5);
    uintptr_t CreateFieldThreadFunctionAddress = ReadLEA32(Patterns::pattern_for_CreateFieldThread, L"CreateField Thread", 0, 1, 5);
    FunctionCalls::tGetOBJENTRY GetOBJENTRYFunction = reinterpret_cast<FunctionCalls::tGetOBJENTRY>(GetOBJENTRYFunctionAddress);
    FunctionCalls::tCreateFieldThread CreateFieldThreadFunction = reinterpret_cast<FunctionCalls::tCreateFieldThread>(CreateFieldThreadFunctionAddress);

    auto entry = GetOBJENTRYFunction(entry_id);
    if ((DWORD)entry) {
        entry_type = entry->Type;
        myObjEntryId = entry_id;
        obj_spawnamount1 = 1;

        CreateFieldThreadFunction(0, 1000, (TASK_FUNC)&NewObjThread, 0x2000);

    }

}

void NewObj2(OBJENTRY_ID entry_id, int spawn_amount) {

    uintptr_t GetOBJENTRYFunctionAddress = ReadLEA32(Patterns::pattern_for_GetOBJENTRY, L"Get OBJENTRY", 0, 1, 5);
    uintptr_t CreateFieldThreadFunctionAddress = ReadLEA32(Patterns::pattern_for_CreateFieldThread, L"CreateField Thread", 0, 1, 5);
    FunctionCalls::tGetOBJENTRY GetOBJENTRYFunction = reinterpret_cast<FunctionCalls::tGetOBJENTRY>(GetOBJENTRYFunctionAddress);
    FunctionCalls::tCreateFieldThread CreateFieldThreadFunction = reinterpret_cast<FunctionCalls::tCreateFieldThread>(CreateFieldThreadFunctionAddress);

    auto entry = GetOBJENTRYFunction(entry_id);
    if ((DWORD)entry) {
        u_char entry_type = entry->Type;
        IdRequest2 = entry_id;
        obj_spawnamount2 = spawn_amount;

        CreateFieldThreadFunction(0, 1000, (TASK_FUNC)&NewObjThread2, 0x2000);

    }

}

void NewObj3(OBJENTRY_ID entry_id, int spawn_amount) {

    uintptr_t GetOBJENTRYFunctionAddress = ReadLEA32(Patterns::pattern_for_GetOBJENTRY, L"Get OBJENTRY", 0, 1, 5);
    uintptr_t CreateFieldThreadFunctionAddress = ReadLEA32(Patterns::pattern_for_CreateFieldThread, L"CreateField Thread", 0, 1, 5);
    FunctionCalls::tGetOBJENTRY GetOBJENTRYFunction = reinterpret_cast<FunctionCalls::tGetOBJENTRY>(GetOBJENTRYFunctionAddress);
    FunctionCalls::tCreateFieldThread CreateFieldThreadFunction = reinterpret_cast<FunctionCalls::tCreateFieldThread>(CreateFieldThreadFunctionAddress);

    auto entry = GetOBJENTRYFunction(entry_id);
    if ((DWORD)entry) {
        u_char entry_type = entry->Type;
        IdRequest3 = entry_id;
        obj_spawnamount3 = spawn_amount;

        CreateFieldThreadFunction(0, 1000, (TASK_FUNC)&NewObjThread3, 0x2000);

    }

}

void NewObj4(OBJENTRY_ID entry_id, int spawn_amount) {

    uintptr_t GetOBJENTRYFunctionAddress = ReadLEA32(Patterns::pattern_for_GetOBJENTRY, L"Get OBJENTRY", 0, 1, 5);
    uintptr_t CreateFieldThreadFunctionAddress = ReadLEA32(Patterns::pattern_for_CreateFieldThread, L"CreateField Thread", 0, 1, 5);
    FunctionCalls::tGetOBJENTRY GetOBJENTRYFunction = reinterpret_cast<FunctionCalls::tGetOBJENTRY>(GetOBJENTRYFunctionAddress);
    FunctionCalls::tCreateFieldThread CreateFieldThreadFunction = reinterpret_cast<FunctionCalls::tCreateFieldThread>(CreateFieldThreadFunctionAddress);

    auto entry = GetOBJENTRYFunction(entry_id);
    if ((DWORD)entry) {
        u_char entry_type = entry->Type;
        IdRequest4 = entry_id;
        obj_spawnamount4 = spawn_amount;

        CreateFieldThreadFunction(0, 1000, (TASK_FUNC)&NewObjThread4, 0x2000);

    }

}

void NewObj5(OBJENTRY_ID entry_id, int spawn_amount) {

    uintptr_t GetOBJENTRYFunctionAddress = ReadLEA32(Patterns::pattern_for_GetOBJENTRY, L"Get OBJENTRY", 0, 1, 5);
    uintptr_t CreateFieldThreadFunctionAddress = ReadLEA32(Patterns::pattern_for_CreateFieldThread, L"CreateField Thread", 0, 1, 5);
    FunctionCalls::tGetOBJENTRY GetOBJENTRYFunction = reinterpret_cast<FunctionCalls::tGetOBJENTRY>(GetOBJENTRYFunctionAddress);
    FunctionCalls::tCreateFieldThread CreateFieldThreadFunction = reinterpret_cast<FunctionCalls::tCreateFieldThread>(CreateFieldThreadFunctionAddress);

    auto entry = GetOBJENTRYFunction(entry_id);
    if ((DWORD)entry) {
        u_char entry_type = entry->Type;
        IdRequest5 = entry_id;
        obj_spawnamount5 = spawn_amount;

        CreateFieldThreadFunction(0, 1000, (TASK_FUNC)&NewObjThread5, 0x2000);

    }

}

void NewObj3i(OBJENTRY_ID entry_id) {
    uintptr_t GetOBJENTRYFunctionAddress = ReadLEA32(Patterns::pattern_for_GetOBJENTRY, L"Get OBJENTRY", 0, 1, 5);
    uintptr_t CreateFieldThreadFunctionAddress = ReadLEA32(Patterns::pattern_for_CreateFieldThread, L"CreateField Thread", 0, 1, 5);
    FunctionCalls::tGetOBJENTRY GetOBJENTRYFunction = reinterpret_cast<FunctionCalls::tGetOBJENTRY>(GetOBJENTRYFunctionAddress);
    FunctionCalls::tCreateFieldThread CreateFieldThreadFunction = reinterpret_cast<FunctionCalls::tCreateFieldThread>(CreateFieldThreadFunctionAddress);

    auto entry = GetOBJENTRYFunction(entry_id);
    if ((DWORD)entry) {
        entry_type = entry->Type;
        myObjEntryId = entry_id;
        obj_spawnamount1 = 3;

        CreateFieldThreadFunction(0, 1000, (TASK_FUNC)&NewObjThread, 0x2000);

    }

}

void NewObj5i(OBJENTRY_ID entry_id) {
    uintptr_t GetOBJENTRYFunctionAddress = ReadLEA32(Patterns::pattern_for_GetOBJENTRY, L"Get OBJENTRY", 0, 1, 5);
    uintptr_t CreateFieldThreadFunctionAddress = ReadLEA32(Patterns::pattern_for_CreateFieldThread, L"CreateField Thread", 0, 1, 5);
    FunctionCalls::tGetOBJENTRY GetOBJENTRYFunction = reinterpret_cast<FunctionCalls::tGetOBJENTRY>(GetOBJENTRYFunctionAddress);
    FunctionCalls::tCreateFieldThread CreateFieldThreadFunction = reinterpret_cast<FunctionCalls::tCreateFieldThread>(CreateFieldThreadFunctionAddress);

    auto entry = GetOBJENTRYFunction(entry_id);
    if ((DWORD)entry) {
        entry_type = entry->Type;
        myObjEntryId = entry_id;
        obj_spawnamount1 = 5;

        CreateFieldThreadFunction(0, 1000, (TASK_FUNC)&NewObjThread, 0x2000);

    }

}

void NewObj10(OBJENTRY_ID entry_id) {
    uintptr_t GetOBJENTRYFunctionAddress = ReadLEA32(Patterns::pattern_for_GetOBJENTRY, L"Get OBJENTRY", 0, 1, 5);
    uintptr_t CreateFieldThreadFunctionAddress = ReadLEA32(Patterns::pattern_for_CreateFieldThread, L"CreateField Thread", 0, 1, 5);
    FunctionCalls::tGetOBJENTRY GetOBJENTRYFunction = reinterpret_cast<FunctionCalls::tGetOBJENTRY>(GetOBJENTRYFunctionAddress);
    FunctionCalls::tCreateFieldThread CreateFieldThreadFunction = reinterpret_cast<FunctionCalls::tCreateFieldThread>(CreateFieldThreadFunctionAddress);

    auto entry = GetOBJENTRYFunction(entry_id);
    if ((DWORD)entry) {
        entry_type = entry->Type;
        myObjEntryId = entry_id;
        obj_spawnamount1 = 10;

        CreateFieldThreadFunction(0, 1000, (TASK_FUNC)&NewObjThread, 0x2000);

    }

}

void NewObj20(OBJENTRY_ID entry_id) {
    uintptr_t GetOBJENTRYFunctionAddress = ReadLEA32(Patterns::pattern_for_GetOBJENTRY, L"Get OBJENTRY", 0, 1, 5);
    uintptr_t CreateFieldThreadFunctionAddress = ReadLEA32(Patterns::pattern_for_CreateFieldThread, L"CreateField Thread", 0, 1, 5);
    FunctionCalls::tGetOBJENTRY GetOBJENTRYFunction = reinterpret_cast<FunctionCalls::tGetOBJENTRY>(GetOBJENTRYFunctionAddress);
    FunctionCalls::tCreateFieldThread CreateFieldThreadFunction = reinterpret_cast<FunctionCalls::tCreateFieldThread>(CreateFieldThreadFunctionAddress);

    auto entry = GetOBJENTRYFunction(entry_id);
    if ((DWORD)entry) {
        entry_type = entry->Type;
        myObjEntryId = entry_id;
        obj_spawnamount1 = 20;

        CreateFieldThreadFunction(0, 1000, (TASK_FUNC)&NewObjThread, 0x2000);

    }

}

void NewObj30(OBJENTRY_ID entry_id) {
    uintptr_t GetOBJENTRYFunctionAddress = ReadLEA32(Patterns::pattern_for_GetOBJENTRY, L"Get OBJENTRY", 0, 1, 5);
    uintptr_t CreateFieldThreadFunctionAddress = ReadLEA32(Patterns::pattern_for_CreateFieldThread, L"CreateField Thread", 0, 1, 5);
    FunctionCalls::tGetOBJENTRY GetOBJENTRYFunction = reinterpret_cast<FunctionCalls::tGetOBJENTRY>(GetOBJENTRYFunctionAddress);
    FunctionCalls::tCreateFieldThread CreateFieldThreadFunction = reinterpret_cast<FunctionCalls::tCreateFieldThread>(CreateFieldThreadFunctionAddress);

    auto entry = GetOBJENTRYFunction(entry_id);
    if ((DWORD)entry) {
        entry_type = entry->Type;
        myObjEntryId = entry_id;
        obj_spawnamount1 = 30;

        CreateFieldThreadFunction(0, 1000, (TASK_FUNC)&NewObjThread, 0x2000);

    }

}

void NewObj50(OBJENTRY_ID entry_id) {
    uintptr_t GetOBJENTRYFunctionAddress = ReadLEA32(Patterns::pattern_for_GetOBJENTRY, L"Get OBJENTRY", 0, 1, 5);
    uintptr_t CreateFieldThreadFunctionAddress = ReadLEA32(Patterns::pattern_for_CreateFieldThread, L"CreateField Thread", 0, 1, 5);
    FunctionCalls::tGetOBJENTRY GetOBJENTRYFunction = reinterpret_cast<FunctionCalls::tGetOBJENTRY>(GetOBJENTRYFunctionAddress);
    FunctionCalls::tCreateFieldThread CreateFieldThreadFunction = reinterpret_cast<FunctionCalls::tCreateFieldThread>(CreateFieldThreadFunctionAddress);

    auto entry = GetOBJENTRYFunction(entry_id);
    if ((DWORD)entry) {
        entry_type = entry->Type;
        myObjEntryId = entry_id;
        obj_spawnamount1 = 50;

        CreateFieldThreadFunction(0, 1000, (TASK_FUNC)&NewObjThread, 0x2000);

    }

}

void NewActor(OBJENTRY_ID entry_id, int spawn_amount) {
    OBJENTRY_ID v1;
    const OBJENTRY_0* v2;
    char v3;
    

    uintptr_t GetOBJENTRYFunctionAddress = ReadLEA32(Patterns::pattern_for_GetOBJENTRY, L"Get OBJENTRY", 0, 1, 5);
    uintptr_t CreateFieldThreadFunctionAddress = ReadLEA32(Patterns::pattern_for_CreateFieldThread, L"CreateField Thread", 0, 1, 5);
    FunctionCalls::tGetOBJENTRY GetOBJENTRYFunction = reinterpret_cast<FunctionCalls::tGetOBJENTRY>(GetOBJENTRYFunctionAddress);
    FunctionCalls::tCreateFieldThread CreateFieldThreadFunction = reinterpret_cast<FunctionCalls::tCreateFieldThread>(CreateFieldThreadFunctionAddress);


    v1 = entry_id | 0x40000000;
    v2 = GetOBJENTRYFunction(entry_id | 0x40000000);
    if (v2) {
        v3 = v2->Type;
        myObjEntryId = v1;
        entry_type = v3;
        obj_spawnamount1 = 1;
        bank = 30;
        CreateFieldThreadFunction(0, 1000, (TASK_FUNC)&NewObjThread, 0x2000);
    }

}

void NewLevelEditorPreviewActor(OBJENTRY_ID entry_id, int spawn_amount) {
    OBJENTRY_ID v1;
    const OBJENTRY_0* v2;
    char v3;

    

    uintptr_t GetOBJENTRYFunctionAddress = ReadLEA32(Patterns::pattern_for_GetOBJENTRY, L"Get OBJENTRY", 0, 1, 5);
    uintptr_t CreateFieldThreadFunctionAddress = ReadLEA32(Patterns::pattern_for_CreateFieldThread, L"CreateField Thread", 0, 1, 5);
    uintptr_t CreateFieldTaskFunctionAddress = ReadLEA32(Patterns::pattern_for_CreateFieldTask, L"CreateField Task", 0, 1, 5);
    FunctionCalls::tGetOBJENTRY GetOBJENTRYFunction = reinterpret_cast<FunctionCalls::tGetOBJENTRY>(GetOBJENTRYFunctionAddress);
    FunctionCalls::tCreateFieldThread CreateFieldThreadFunction = reinterpret_cast<FunctionCalls::tCreateFieldThread>(CreateFieldThreadFunctionAddress);
    FunctionCalls::tCreateFieldTask CreateFieldTaskFunction = reinterpret_cast<FunctionCalls::tCreateFieldTask>(CreateFieldTaskFunctionAddress);
    


    previewEntityId = entry_id;
    isPreviewSpawned = false;
    previewObject = nullptr;
    previewEntity = nullptr;

   // v1 = entry_id | 0x40000000;
    v2 = GetOBJENTRYFunction(entry_id);
    if (v2) {
        
        v3 = v2->Type;
        myObjEntryId = entry_id;
        entry_type = v2->Type;
        obj_spawnamount1 = spawn_amount;
        
        bank = 30;
        CreateFieldThreadFunction(0, 1000, (TASK_FUNC)&NewLevelEditorPreviewThread, 0x2000);
        //CreateFieldTaskFunction(0, 1000, (TASK_FUNC)&NewLevelEditorPreviewThread);
    }

}


void NewFriend(OBJENTRY_ID entry_id) {
    uintptr_t GetOBJENTRYFunctionAddress = ReadLEA32(Patterns::pattern_for_GetOBJENTRY, L"Get OBJENTRY", 0, 1, 5);
    uintptr_t CreateFieldThreadFunctionAddress = ReadLEA32(Patterns::pattern_for_CreateFieldThread, L"CreateField Thread", 0, 1, 5);
    FunctionCalls::tGetOBJENTRY GetOBJENTRYFunction = reinterpret_cast<FunctionCalls::tGetOBJENTRY>(GetOBJENTRYFunctionAddress);
    FunctionCalls::tCreateFieldThread CreateFieldThreadFunction = reinterpret_cast<FunctionCalls::tCreateFieldThread>(CreateFieldThreadFunctionAddress);

    auto entry = GetOBJENTRYFunction(entry_id);
    if ((DWORD)entry) {
        entry_type = entry->Type;
        myObjEntryId = entry_id;
        obj_spawnamount1 = 1;
        pri = 0x96A;
        bank = 0x05;
        CreateFieldThreadFunction(0, 1000, (TASK_FUNC)&NewObjThread, 0x2000);

    }

}

void NewCapture(OBJENTRY_ID entry_id) {
    uintptr_t GetOBJENTRYFunctionAddress = ReadLEA32(Patterns::pattern_for_GetOBJENTRY, L"Get OBJENTRY", 0, 1, 5);
    uintptr_t CreateFieldThreadFunctionAddress = ReadLEA32(Patterns::pattern_for_CreateFieldThread, L"CreateField Thread", 0, 1, 5);
    FunctionCalls::tGetOBJENTRY GetOBJENTRYFunction = reinterpret_cast<FunctionCalls::tGetOBJENTRY>(GetOBJENTRYFunctionAddress);
    FunctionCalls::tCreateFieldThread CreateFieldThreadFunction = reinterpret_cast<FunctionCalls::tCreateFieldThread>(CreateFieldThreadFunctionAddress);
    uintptr_t ChangeFormCaptureEndFunctionAddress = FindAndPrintPatternW(Patterns::pattern_for_ChangeFormCaptureEnd, L"Change Form Capture End", 0);
    FunctionCalls::tChangeFormCaptureEnd ChangeFormCaptureEndFunction = reinterpret_cast<FunctionCalls::tChangeFormCaptureEnd>(ChangeFormCaptureEndFunctionAddress);
    uintptr_t GetSoraFunctionAddress = ReadLEA32(Patterns::pattern_for_GetSora, L"Get Sora", 0, 1, 5);
    FunctionCalls::tGetSora GetSoraFunction = reinterpret_cast<FunctionCalls::tGetSora>(GetSoraFunctionAddress);


    /*YS::SORA* sora = GetSoraFunction();
    if (!sora) {
        wprintf(L"Failed to retrieve Sora pointer.\n");
        return;
    }

    int* currentFormPtr = (int*)((uintptr_t)sora + 0xDE0);

    if (*currentFormPtr == 12) {

        ChangeFormCaptureEndFunction(sora);
    }*/

    auto entry = GetOBJENTRYFunction(entry_id);
    if ((DWORD)entry) {
        entry_type = entry->Type;
        myObjEntryId = entry_id;
        obj_spawnamount1 = 1;
        //pri = 0x96A;
        //bank = 0x05;
        CreateFieldThreadFunction(1, 1000, (TASK_FUNC)&NewCaptureThread, 0x2000);

    }

}

void NewSoraThread(TASK_0* task) {
    //printf("NewObjThread started\n");
    //Vector4 CameraPos = (*(Vector4*)(moduleBase + 0x716A28));
    unsigned int* alloc;
    OBJENTRY_ID v8;

    uintptr_t GetOBJENTRYFunctionAddress = ReadLEA32(Patterns::pattern_for_GetOBJENTRY, L"Get OBJENTRY", 0, 1, 5);
    uintptr_t ReadRequestFunctionAddress = ReadLEA32(Patterns::pattern_for_ReadRequest, L"Read Request", 0, 1, 5);
    uintptr_t FlushCacheBuffFunctionAddress = ReadLEA32(Patterns::pattern_for_FlushCacheBuff, L"Flush Cachebuff", 0, 1, 5);
    uintptr_t OBJENTRYAppearFunctionAddress = ReadLEA32(Patterns::pattern_for_OBJENTRY_APPEAR, L"OBJENTRY APPEAR", 0, 1, 5);
    uintptr_t NewSoraFunctionAddress = ReadLEA32(Patterns::pattern_for_SORA, L"new sora", 0, 1, 5);
    uintptr_t AllocAREAFunctionAddress = ReadLEA32(Patterns::pattern_for_AllocArea, L"ALLOC AREA", 0, 1, 5);
    uintptr_t GetSoftPADFunctionAddress = ReadLEA32(Patterns::pattern_for_GetSoftPad, L"Get Soft PAD", 0, 1, 5);
    uintptr_t NewPLAYERFunctionAddress = ReadLEA32(Patterns::pattern_for_PLAYER, L"new player", 0, 1, 5);
    uintptr_t RandomPosPlayerFunctionAddress = FindAndPrintPatternW(Patterns::pattern_for_RandomPosPlayer, L"Random Pos Player", 0);
    
    
    FunctionCalls::tSora NewSoraFunction = reinterpret_cast<FunctionCalls::tSora>(NewSoraFunctionAddress);
    FunctionCalls::tGetOBJENTRY GetOBJENTRYFunction = reinterpret_cast<FunctionCalls::tGetOBJENTRY>(GetOBJENTRYFunctionAddress);
    FunctionCalls::tReadRequest ReadRequestFunction = reinterpret_cast<FunctionCalls::tReadRequest>(ReadRequestFunctionAddress);
    FunctionCalls::tFlushCacheBuff FlushCacheBuffFunction = reinterpret_cast<FunctionCalls::tFlushCacheBuff>(FlushCacheBuffFunctionAddress);
    FunctionCalls::tOBJENTRY_APPEAR OBJENTRYAppearFunction = reinterpret_cast<FunctionCalls::tOBJENTRY_APPEAR>(OBJENTRYAppearFunctionAddress);
    FunctionCalls::tAllocArea AllocAREAFunction = reinterpret_cast<FunctionCalls::tAllocArea>(AllocAREAFunctionAddress);
    FunctionCalls::tGetSoftPad GetSoftPADFunction = reinterpret_cast<FunctionCalls::tGetSoftPad>(GetSoftPADFunctionAddress);
    FunctionCalls::tSpawnPlayer NewPLAYERFunction = reinterpret_cast<FunctionCalls::tSpawnPlayer>(NewPLAYERFunctionAddress);
    FunctionCalls::tUnitGroupRandomPosPlayer RandomPosPlayerFunction = reinterpret_cast<FunctionCalls::tUnitGroupRandomPosPlayer>(RandomPosPlayerFunctionAddress);

    //DestroyobjFunction(GetPlayerFunction());
    //RealDestroyobjFunction(GetPlayerFunction());

    
    

    uintptr_t SteamPointer = reinterpret_cast<uintptr_t>(baseModule) + 0xB0D598;

    Vector4 CameraPos = (*(Vector4*)(SteamPointer - 0x3F4920));

    Vector4 randomPosition = { 0.0f, 0.0f, 0.0f, 0.0f };

    

    
    //Vector4 CameraRot = (*(Vector4*)(moduleBase + 0x716A18));// replace with steam version address
    //float angle = atan2f(CameraRot.x, -CameraRot.y);
    //double rotation = angle * 180.0 / 3.14159265358979323846 + 180.0;
    //OBJENTRY_ID entry_id = task->Args[0].i;
    auto entry = GetOBJENTRYFunction(task->Args[0].i);
    //auto request = (unsigned int)entry_id;

    ReadRequestFunction(task->Args[0].i, pri, 4);
    FlushCacheBuffFunction((TASK*)task);
    int incNum = 0;

    //RandomPosPlayerFunction(&randomPosition, request);

    LOG("Random Position: %1.0f, %1.0f, %1.0f, %1.0f",
        randomPosition.x, randomPosition.y, randomPosition.z, randomPosition.w);

    if (obj_spawnamount1 > 0) {
        //double rotation = 0.0;
        do {
            //auto player = gGetPlayer();
            //auto SpawnObject = OBJENTRYAppearFunction(myObjEntryId, CameraPos, 0.0);
            //OBJENTRYAppearFunction()
           //NewSoraFunction();
            alloc = AllocAREAFunction(0xE60);
            //v8 = entry->I
            auto pad = GetSoftPADFunction(0);
            NewPLAYERFunction((YS::PLAYER*)alloc,entry,11, pad, randomPosition,0.0);
            ++incNum;
        } while (incNum < obj_spawnamount1);
    }
    //Cache_BuffDump();
    // size_t allocatorSize = GetMemoryAllocatorSize((MemoryAllocator*)GetAreaAllocator());
    // size_t allocatorUsedSize = GetMemoryAllocatorUsedSize((MemoryAllocator*)GetAreaAllocator());
    // double allocatorSizeMB = static_cast<double>(allocatorSize) / (1024 * 1024);
    // double allocatorUsedSizeMB = static_cast<double>(allocatorUsedSize) / (1024 * 1024);
     //double remainingSizeMB = static_cast<double>(allocatorSize - allocatorUsedSize) / (1024 * 1024);
     //DebugPrint("Remaining Size: %.3f MB\n", remainingSizeMB);
     //printf("Used Memory: %.3f MB / Max Memory: %.3f MB\n", allocatorUsedSizeMB, allocatorSizeMB);

 //printf("NewObjThread ended\n");
}

void NewSora(OBJENTRY_ID entry_id) {
    uintptr_t GetOBJENTRYFunctionAddress = ReadLEA32(Patterns::pattern_for_GetOBJENTRY, L"Get OBJENTRY", 0, 1, 5);
    uintptr_t CreateFieldThreadFunctionAddress = ReadLEA32(Patterns::pattern_for_CreateFieldThread, L"CreateField Thread", 0, 1, 5);
    FunctionCalls::tGetOBJENTRY GetOBJENTRYFunction = reinterpret_cast<FunctionCalls::tGetOBJENTRY>(GetOBJENTRYFunctionAddress);
    FunctionCalls::tCreateFieldThread CreateFieldThreadFunction = reinterpret_cast<FunctionCalls::tCreateFieldThread>(CreateFieldThreadFunctionAddress);

    auto entry = GetOBJENTRYFunction(entry_id);
    if ((DWORD)entry) {
        entry_type = entry->Type;
        //myObjEntryId = entry_id;
        obj_spawnamount1 = 1;
        //LOG("Calling new sora Thread with entry id: 0x%x", myObjEntryId);
        //CreateFieldThreadFunction(1, 1000, (TASK_FUNC)&NewSoraThread, 0x2000);
        TASK_0* task = CreateFieldThreadFunction(1, 1000, (TASK_FUNC)&NewSoraThread, 0x2000);

        // Write to offset 0x40 (64 bytes) as a DWORD
        if (task)
            task->Args[0].i = entry_id;
    }
}

std::string GetPublicIP() {
    std::string publicIP = "";

    // Specify the target URL (e.g., api.ipify.org)
    HINTERNET hSession = WinHttpOpen(L"A WinHTTP Example Program/1.0",
        WINHTTP_ACCESS_TYPE_DEFAULT_PROXY,
        WINHTTP_NO_PROXY_NAME,
        WINHTTP_NO_PROXY_BYPASS, 0);

    if (hSession) {
        HINTERNET hConnect = WinHttpConnect(hSession, L"api.ipify.org",
            INTERNET_DEFAULT_HTTP_PORT, 0);

        if (hConnect) {
            HINTERNET hRequest = WinHttpOpenRequest(hConnect, L"GET", NULL, NULL, WINHTTP_NO_REFERER,
                WINHTTP_DEFAULT_ACCEPT_TYPES, 0);

            if (hRequest) {
                BOOL bResults = WinHttpSendRequest(hRequest, WINHTTP_NO_ADDITIONAL_HEADERS, 0,
                    WINHTTP_NO_REQUEST_DATA, 0, 0, 0);

                if (bResults) {
                    bResults = WinHttpReceiveResponse(hRequest, NULL);

                    if (bResults) {
                        DWORD dwSize = 0;
                        WinHttpQueryDataAvailable(hRequest, &dwSize);

                        if (dwSize > 0) {
                            char* buffer = new char[dwSize + 1];
                            DWORD dwDownloaded = 0;
                            ZeroMemory(buffer, dwSize + 1);

                            WinHttpReadData(hRequest, (LPVOID)buffer, dwSize, &dwDownloaded);

                            publicIP = buffer;
                            delete[] buffer;
                        }
                    }
                }
                WinHttpCloseHandle(hRequest);
            }
            WinHttpCloseHandle(hConnect);
        }
        WinHttpCloseHandle(hSession);
    }

    return publicIP;
}


   
    

void ChatRect(YW::CHAT* chat, int x, int y, int w, size_t len) {

    uintptr_t YWwidgetFunctionAddress = ReadLEA32(Patterns::pattern_for_YWwidget, L"YW Widget", 0, 1, 5);
    FunctionCalls::tWidget YWwidgetFunction = reinterpret_cast<FunctionCalls::tWidget>(YWwidgetFunctionAddress);

    YWwidgetFunction(chat, x, y, w, 0);

    u_int v7;
    char* v8;
    
    YW::YW_COLOR_0* color = new YW::YW_COLOR_0;


    color->R = 0x60;
    color->G = 0x60;
    color->B = 0x60;
    color->A = 0x80;

    uintptr_t SteamPointer = reinterpret_cast<uintptr_t>(baseModule) + 0xB0D598;
    const int* areaBase = reinterpret_cast<const int*>(SteamPointer - 0x50C568);

    chat->_vptr_ = areaBase;

    v7 = chat->Flag.Flag;
    chat->Len = len;
    chat->Flag.Flag = v7 | 8;

    v8 = new char[len];
    chat->Text = v8;
    chat->Pt = v8;
    *v8 = 0;
    chat->Color[0] = color;
    chat->Color[1] = color;

}



void YW::LABEL::SetText(const char* newText) {
    if (Label) delete[] Label;
    Label = _strdup(newText);
    this->LabelHeight = FontGetTextHeightFunction(newText);
    this->LabelWidth = FontGetTextWidthFunction(newText);
}

void DebugSliderState() {
    if (!g_FrameSlider) return;

    static float lastValue = -1.0f;
    static int lastState = -1;

    if (g_FrameSlider->Value != lastValue || g_FrameSlider->State != lastState) {
        LOG(L"Slider Debug — State: %d, Value: %.2f\n", g_FrameSlider->State, g_FrameSlider->Value);
        lastValue = g_FrameSlider->Value;
        lastState = g_FrameSlider->State;
    }
}

void RestoreFrameCTRLInstructions()
{
    BYTE* addr1 = (BYTE*)(moduleBase1 + 0x02CFB28);
    BYTE* addr2 = (BYTE*)(moduleBase1 + 0x02CFB31);
    BYTE* addr3 = (BYTE*)(moduleBase1 + 0x02CFB6B);

    DWORD oldProtect;
    VirtualProtect(addr1, sizeof(original_addss), PAGE_EXECUTE_READWRITE, &oldProtect);
    memcpy(addr1, original_addss, sizeof(original_addss));
    VirtualProtect(addr1, sizeof(original_addss), oldProtect, &oldProtect);

    VirtualProtect(addr2, sizeof(original_movss), PAGE_EXECUTE_READWRITE, &oldProtect);
    memcpy(addr2, original_movss, sizeof(original_movss));
    VirtualProtect(addr2, sizeof(original_movss), oldProtect, &oldProtect);

    VirtualProtect(addr3, sizeof(original_mov_frame), PAGE_EXECUTE_READWRITE, &oldProtect);
    memcpy(addr3, original_mov_frame, sizeof(original_mov_frame));
    VirtualProtect(addr3, sizeof(original_mov_frame), oldProtect, &oldProtect);
}


void NopFrameCTRLInstructions()
{
    BYTE* addr1 = (BYTE*)(moduleBase1 + 0x02CFB28); // addss xmm0, xmm6
    BYTE* addr2 = (BYTE*)(moduleBase1 + 0x02CFB31); // movss dword_140B64F84, xmm0
    BYTE* addr3 = (BYTE*)(moduleBase1 + 0x02CFB6B); // mov dword_140B64F98, eax

    // Save original bytes
    memcpy(original_addss, addr1, sizeof(original_addss));
    memcpy(original_movss, addr2, sizeof(original_movss));
    memcpy(original_mov_frame, addr3, sizeof(original_mov_frame));

    DWORD oldProtect;
    VirtualProtect(addr1, 4, PAGE_EXECUTE_READWRITE, &oldProtect);
    memset(addr1, 0x90, 4);
    VirtualProtect(addr1, 4, oldProtect, &oldProtect);

    VirtualProtect(addr2, 8, PAGE_EXECUTE_READWRITE, &oldProtect);
    memset(addr2, 0x90, 8);
    VirtualProtect(addr2, 8, oldProtect, &oldProtect);

    VirtualProtect(addr3, 6, PAGE_EXECUTE_READWRITE, &oldProtect);
    memset(addr3, 0x90, 6);
    VirtualProtect(addr3, 6, oldProtect, &oldProtect);
}

void UpdateCutscenePlayer() {
    if (!g_isPlaying) return;

    // Real pointer to game memory
    int* g_event_m_frame_ptr = reinterpret_cast<int*>(moduleBase1 + 0xB64F98);

    // Advance time (your own logic)
    g_event_m_time += 1.0f; // Same as dk::Vsync::s_frameTime
    int newFrame = static_cast<int>(g_event_m_time / 2.0f);

    // Clamp frame to end
    if (newFrame >= g_event_end_frame) {
        newFrame = g_event_end_frame;
        g_isPlaying = false;
    }

    // Write frame directly to game memory
    *g_event_m_frame_ptr = newFrame;

    // Sync slider UI with actual in-game frame
    if (g_FrameSlider)
        g_FrameSlider->Value = static_cast<float>(*g_event_m_frame_ptr);
}

void UpdateCutsceneSliderManual() {
    if (!g_isPlaying && g_FrameSlider) {
        int newFrame = static_cast<int>(g_FrameSlider->Value);

        static int* g_event_m_frame_ptr = (int*)(moduleBase1 + 0xB64F98);

        if (newFrame != *g_event_m_frame_ptr) {
            *g_event_m_frame_ptr = newFrame;
            g_event_m_time = static_cast<float>(newFrame * 2.0f);

            //LOG(L"Dragging frame slider, Frame: %d (written to game memory)\n", newFrame);
        }
    }
}

void PlayButtonCallback(unsigned int* arg) {
    RestoreFrameCTRLInstructions();
    //g_event_m_time = static_cast<float>(g_event_m_frame * 2); // match game logic
    g_isPlaying = true;

}

void PauseButtonCallback(unsigned int* arg) {
    NopFrameCTRLInstructions();
   // g_event_m_time = static_cast<float>(g_event_m_frame * 2); // match game logic
    g_isPlaying = false;

}


void EventPlayerWidget() {
    uintptr_t YWWindowFunctionAddress = ReadLEA32(Patterns::pattern_for_YWwindow, L"YW Window", 0, 1, 5);
    FunctionCalls::tYWWindow YWWindowFunction = reinterpret_cast<FunctionCalls::tYWWindow>(YWWindowFunctionAddress);
    uintptr_t SetDialValueFunctionAddress = FindAndPrintPatternW(Patterns::pattern_for_SetDialValue, L"Set DialValue", 0);

    FunctionCalls::tSetDialValue SetDialValueFunction = reinterpret_cast<FunctionCalls::tSetDialValue>(SetDialValueFunctionAddress);

    uintptr_t YWLabelFunctionAddress = ReadLEA32(Patterns::pattern_for_YWLabel, L"YWlabel", 0, 1, 5);

    FunctionCalls::tYWLabel YWLabelFunction = reinterpret_cast<FunctionCalls::tYWLabel>(YWLabelFunctionAddress);

    FunctionCalls::tYWLabelNew YWLabelFunctionNew = reinterpret_cast<FunctionCalls::tYWLabelNew>(YWLabelFunctionAddress);

    uintptr_t OpenSingletonWindowFunctionAddress = ReadLEA32(Patterns::pattern_for_OpenSingletonWindow, L"OpenSingletonWindow", 0, 1, 5);

    FunctionCalls::tOpenSingletonWindow OpenSingletonWindowFunction = reinterpret_cast<FunctionCalls::tOpenSingletonWindow>(OpenSingletonWindowFunctionAddress);

    //uintptr_t YWLabelFunctionAddress = ReadLEA32(Patterns::pattern_for_YWLabel, L"YWlabel", 0, 1, 5);

    //FunctionCalls::tYWLabel YWLabelFunction = reinterpret_cast<FunctionCalls::tYWLabel>(YWLabelFunctionAddress);

    uintptr_t ShowDialWidgetFunctionAddress = ReadLEA32(Patterns::pattern_for_DialWidget, L"Dial Widget", 0, 1, 5);
    FunctionCalls::tDialWidget ShowDialWidgetFunction = reinterpret_cast<FunctionCalls::tDialWidget>(ShowDialWidgetFunctionAddress);


    uintptr_t GetOBJEntrynameFunctionAddress = ReadLEA32(Patterns::pattern_for_GetObjEntryName, L"Get obj Entryname", 0, 1, 5);
    FunctionCalls::tGetObjEntryName GetOBJEntrynameFunction = reinterpret_cast<FunctionCalls::tGetObjEntryName>(GetOBJEntrynameFunctionAddress);



    uintptr_t ShowDebugWindowFunctionAddress = ReadLEA32(Patterns::pattern_for_ShowDebugWindow, L"ShowDebugWindow", 0, 1, 5);
    FunctionCalls::tShowDebugWindow ShowDebugWindowFunction = reinterpret_cast<FunctionCalls::tShowDebugWindow>(ShowDebugWindowFunctionAddress);


    uintptr_t AllocYWMemoryFunctionAddress = ReadLEA32(Patterns::pattern_for_Alloc_YW_Memory, L"AllocYWMemory", 0, 1, 5);

    FunctionCalls::tAlloc_YW_Memory AllocYWMemoryFunction = reinterpret_cast<FunctionCalls::tAlloc_YW_Memory>(AllocYWMemoryFunctionAddress);

    FunctionCalls::tAlloc_YW_MemoryNew AllocYWMemoryFunctionNew = reinterpret_cast<FunctionCalls::tAlloc_YW_MemoryNew>(AllocYWMemoryFunctionAddress);


    uintptr_t ContainerLinkFunctionAddress = FindAndPrintPatternW(Patterns::pattern_for_ContainerLink, L"ContainerLink", 0);

    FunctionCalls::tContainerLink ContainerLinkFunction = reinterpret_cast<FunctionCalls::tContainerLink>(ContainerLinkFunctionAddress);

    uintptr_t YWScrollbarFunctionAddress = FindAndPrintPatternW(Patterns::pattern_for_YWScrollbar, L"YWScrollbar", 0);

    FunctionCalls::tYWScrollbar YWScrollbarFunction = reinterpret_cast<FunctionCalls::tYWScrollbar>(YWScrollbarFunctionAddress);

    uintptr_t YWButtonFunctionAddress = ReadLEA32(Patterns::pattern_for_YWButton, L"YWButton", 0, 1, 5);
    FunctionCalls::tYWButton YWButtonFunction = reinterpret_cast<FunctionCalls::tYWButton>(YWButtonFunctionAddress);




    //auto* Menu = OpenSingletonWindowFunction(windowTitle.c_str(), 280, 156, 0);
    //g_CurrentFrameLabel = (YW::LABEL*)AllocYWMemoryFunction((YW::MEMORY*)0x200);
    auto allocYWMemPlayButton = AllocYWMemoryFunction((YW::MEMORY*)0x200);
    auto allocYWMemPauseButton = AllocYWMemoryFunction((YW::MEMORY*)0x200);
    
    //auto allocYWMemLabelScaleW = AllocYWMemoryFunction((YW::MEMORY*)0x200);
   // auto allocYWMemDialScale = AllocYWMemoryFunction((YW::MEMORY*)0x200);
    auto Menu = AllocYWMemoryFunction((YW::MEMORY*)0x200);

    YWWindowFunction((YW::WINDOW*)Menu, "Event Player", 0, 0, 300, 220, 2);

    g_FrameSlider = (YW::SCROLLBAR*)AllocYWMemoryFunctionNew(0xA0);
    //g_ScaleSliderW = (YW::SCROLLBAR*)AllocYWMemoryFunctionNew(0xA0);

    int margin = 5;
    int rowHeight = 25;
    int buttonWidth = 60;
    int buttonHeight = 20;
    int labelWidth = 90;
    int sliderWidth = 500;
    int spacing = 10;
    float scrollbarMin = 0.0f;
    float scrollbarMax = (g_event_end_frame > 0) ? static_cast<float>(g_event_end_frame) : 100.0f;
    float thumbWidth = std::max(10.0f, (scrollbarMax - scrollbarMin) / 40.0f); // visual balance
    //float step = 1.0f;
    // Y positions (aligned)
    int labelY = margin;
    int sliderY = labelY + 1; // slight nudge for visual alignment if needed
    int buttonsY = labelY + rowHeight + spacing;

    // X positions
    int labelX = margin;
    int sliderX = labelX + labelWidth + spacing;

    char buffer[64];
    sprintf(buffer, "Current Frame: %d", g_event_m_frame);  // or snprintf for safety

    // Label: "Current Frame:"
    //YWLabelFunctionNew((YW::LABEL*)g_CurrentFrameLabel, buffer, labelX, labelY, labelWidth, rowHeight);

    

    // Scrollbar (beside label)
    //float scrollbarMin = 0.0f;
   // float scrollbarMax = static_cast<float>(g_event_end_frame);
    //float thumbWidth = 1.0f;
    float step = 1.0f;

    YWScrollbarFunction(g_FrameSlider, 5, 30, sliderWidth, rowHeight,
        scrollbarMin, scrollbarMax, thumbWidth, step);

    // Reassign vtable if necessary
    *reinterpret_cast<void***>(g_FrameSlider) = reinterpret_cast<void**>(0x140600F48);

    // Buttons (side-by-side below label/slider)
    int playButtonX = margin;
    int pauseButtonX = playButtonX + buttonWidth + spacing;

    YWButtonFunction((YW::BUTTON_3*)allocYWMemPlayButton, "Play", playButtonX, 60, buttonWidth, buttonHeight, PlayButtonCallback, nullptr);
    YWButtonFunction((YW::BUTTON_3*)allocYWMemPauseButton, "Pause", pauseButtonX, 60, buttonWidth, buttonHeight, PauseButtonCallback, nullptr);

    //ShowDialWidgetFunction((YW::DIAL_3*)allocYWMemDialScale, 140, 5, 56, 16, 0, 8, ObjectScaleCallBack);

   // SetDialValueFunction((YW::DIAL_3*)allocYWMemDialScale, gObjectScale);

    //ContainerLinkFunction((YW::CONTAINER*)Menu, (YW::WIDGET*)g_CurrentFrameLabel);
    ContainerLinkFunction((YW::CONTAINER*)Menu, (YW::WIDGET*)allocYWMemPlayButton);
    ContainerLinkFunction((YW::CONTAINER*)Menu, (YW::WIDGET*)allocYWMemPauseButton);

    //ContainerLinkFunction((YW::CONTAINER*)Menu, (YW::WIDGET*)allocYWMemDialScale);

    ContainerLinkFunction((YW::CONTAINER*)Menu, (YW::WIDGET*)g_FrameSlider);

    ShowDebugWindowFunction((YW::WIDGET*)Menu);

}
    

AREA ReadSteamAreaPointer(AREA& areaPointer) {
    uintptr_t SteamPointer = reinterpret_cast<uintptr_t>(baseModule) + 0xB0D598;
    uintptr_t areaBase = SteamPointer - 0x3F6590;

    // Read the AREA structure from the memory
    areaPointer = *reinterpret_cast<AREA*>(areaBase);

    // Log the values using the LOG macro
    /*LOG(L"Client's Area Info: World: %u, Area: %u, Entrance: %u, Map: %u, Battle: %u, Event: %u\n",
        static_cast<unsigned>(areaPointer.World),
        static_cast<unsigned>(areaPointer.Area),
        static_cast<unsigned>(areaPointer.Entrance),
        static_cast<unsigned>(areaPointer.Map),
        static_cast<unsigned>(areaPointer.Battle),
        static_cast<unsigned>(areaPointer.Event));*/

    return areaPointer;
}

AREA* GetCurrentArea()
{
    uintptr_t CurrentAreaAddress = 0;

    if (DetectGameVersion() == "Steam") {
        CurrentAreaAddress = SteamPointer = reinterpret_cast<uintptr_t>(baseModule) + 0x717008;
        
    }
    else if (DetectGameVersion() == "Epic") {
        CurrentAreaAddress = EpicPointer = reinterpret_cast<uintptr_t>(baseModule) + 0x717008;
    }
    else {
        return nullptr;
    }

    // Print the full address
    //StatusPrintfFunction("GetCurrentArea() -> Address: 0x%p\n", (void*)CurrentAreaAddress);

    return reinterpret_cast<AREA*>(CurrentAreaAddress);
}

void InitGameSpeedAddress()
{
    if (g_GameSpeedAddress != 0)
        return; // already set

    uintptr_t base = reinterpret_cast<uintptr_t>(baseModule);

    if (DetectGameVersion() == "Steam")
    {
        g_GameSpeedAddress = base + 0x717424;
    }
    else if (DetectGameVersion() == "Epic")
    {
        g_GameSpeedAddress = base + 0x717424;
    }
}


float ReadGameSpeed()
{
    InitGameSpeedAddress();
    if (!g_GameSpeedAddress)
        return 1.0f; // fallback

    return *reinterpret_cast<float*>(g_GameSpeedAddress);
}

void WriteGameSpeed(float value)
{
    InitGameSpeedAddress();
    if (!g_GameSpeedAddress)
        return;

    *reinterpret_cast<float*>(g_GameSpeedAddress) = value;
}





void InitDecoder(const wchar_t* soundFileName) {
    if (g_decoderInitialized) {
        ma_decoder_uninit(&g_decoder);
        g_decoderInitialized = false;
    }

    ma_decoder_config config = ma_decoder_config_init(ma_format_f32, 2, 48000); // force float, stereo
    ma_result result = ma_decoder_init_file_w(soundFileName, &config, &g_decoder);

    if (result == MA_SUCCESS) {
        g_decoderInitialized = true;
    }
    else {
        wprintf(L"Failed to init decoder for amplitude\n");
    }
}



    std::string WStringToString(const std::wstring& wstr) {
        return std::string(wstr.begin(), wstr.end());
        
    }

    std::wstring GetLastFolderName(const std::wstring& path) {
        size_t pos = path.find_last_of(L"\\/");
        if (pos != std::wstring::npos && pos + 1 < path.length()) {
            return path.substr(pos + 1);
        }
        return path;
    }

    int EstimateTextWidth(const std::string& text, int averageCharWidth = 8) {
        return static_cast<int>(text.length()) * averageCharWidth;
    }


    void RefreshList() {
        // Update the text of the looping option based on the current state
        const char* loopingOption = isLooping ? "Loop Music: On" : "Loop Music: Off";

        // Check if the looping option already exists in the list
        bool loopingOptionFound = false;
        for (size_t i = 0; i < playlist.size(); ++i) {
            if (strcmp(playlistList[i], loopingOption) == 0) {
                loopingOptionFound = true;
                break;
            }
        }

        // If the looping option doesn't exist, add it to the end of the list
        if (!loopingOptionFound) {
            playlistList[playlist.size()] = loopingOption;
        }
    }

    void SearchAndAddAudioFilesAndFolders(const std::wstring& path, std::vector<std::wstring>& playlist, std::vector<bool>& isDirectoryList) {
        playlist.clear();
        isDirectoryList.clear();

        std::wstring searchPath = path + L"\\*";

        struct _wfinddata_t fileinfo;
        intptr_t handle = _wfindfirst(searchPath.c_str(), &fileinfo);

        std::vector<std::wstring> folders;
        std::vector<std::wstring> files;

        auto hasSupportedExtension = [](const std::wstring& filename) -> bool {
            size_t dotPos = filename.find_last_of(L'.');
            if (dotPos == std::wstring::npos) return false;

            std::wstring ext = filename.substr(dotPos);
            std::transform(ext.begin(), ext.end(), ext.begin(), ::towlower);

            return (ext == L".wav" || ext == L".mp3" || ext == L".ogg" || ext == L".flac");
            };

        if (handle != -1) {
            do {
                std::wstring filename = fileinfo.name;
                if (filename == L"." || filename == L"..") continue;

                if (fileinfo.attrib & _A_SUBDIR) {
                    folders.push_back(filename + L"/");
                }
                else if (hasSupportedExtension(filename)) {
                    files.push_back(filename);
                }
            } while (_wfindnext(handle, &fileinfo) == 0);
            _findclose(handle);
        }

        // Add parent directory navigation if not in root
        const std::wstring rootPath = L"DebugMenu\\music";
        if (path.length() > rootPath.length() && path.substr(0, rootPath.length()) == rootPath) {
            playlist.push_back(L"..\\");
            isDirectoryList.push_back(true);
        }

        // Add folders first
        for (const auto& folder : folders) {
            playlist.push_back(folder);
            isDirectoryList.push_back(true);
        }

        // Then add audio files
        for (const auto& file : files) {
            playlist.push_back(file);
            isDirectoryList.push_back(false);
        }
    }


    /*void PlaySoundWithTrackingLoop(const wchar_t* soundFileName) {
        if (soundFileName != nullptr) {
            currentSoundFile = soundFileName;
            //StopBGM(1);
            wprintf(L"Playing sound (looped): %s\n", soundFileName);
            PlaySound(soundFileName, NULL, SND_FILENAME | SND_ASYNC | SND_LOOP); // Add SND_LOOP
        }
    }*/

    void UpdateAudioParamsFromScrollbars() {
        if (!g_soundInitialized) return;

        if (g_ScaleSliderPitch) {
            float pitch = g_ScaleSliderPitch->Value;
            ma_sound_set_pitch(&g_sound, pitch);
        }

        if (g_ScaleSliderPan) {
            float pan = g_ScaleSliderPan->Value;
            ma_sound_set_pan(&g_sound, pan);
        }

        if (g_ScaleSlider3DPos) {
            float posValue = g_ScaleSlider3DPos->Value;
            ma_sound_set_spatialization_enabled(&g_sound, MA_TRUE);
            ma_sound_set_position(&g_sound, posValue, 0.0f, 0.0f);  // Example: move on X axis only
        }
    }

    inline bool IsValidWidget(void* w)
    {
        return w && *reinterpret_cast<void**>(w);
    }

    void ResetSlidersCallback(unsigned int* arg) {
        // Reset slider values to defaults
       
            float g_DefaultPan = 0.0f;      // default pan value
            float g_DefaultPitch = 1.0f;    // default pitch value
            float g_Default3DPos = 0.0f;    // default 3D position value

            if (IsValidWidget(g_ScaleSliderPan))
                g_ScaleSliderPan->Value = 0.0f;

            if (IsValidWidget(g_ScaleSliderPitch))
                g_ScaleSliderPitch->Value = 1.0f;

            if (IsValidWidget(g_ScaleSlider3DPos))
                g_ScaleSlider3DPos->Value = 0.0f;

            // If you need to force UI redraw/update here, call that function or send a message
            // For example, if you have a function to refresh the widget, call it here.

            // Optionally, update the underlying audio parameters here too if needed
        
    }

    


    void AudioPlayBackWidget() {
        uintptr_t YWWindowFunctionAddress = ReadLEA32(Patterns::pattern_for_YWwindow, L"YW Window", 0, 1, 5);
        FunctionCalls::tYWWindow YWWindowFunction = reinterpret_cast<FunctionCalls::tYWWindow>(YWWindowFunctionAddress);
        uintptr_t SetDialValueFunctionAddress = FindAndPrintPatternW(Patterns::pattern_for_SetDialValue, L"Set DialValue", 0);

        FunctionCalls::tSetDialValue SetDialValueFunction = reinterpret_cast<FunctionCalls::tSetDialValue>(SetDialValueFunctionAddress);

        uintptr_t YWLabelFunctionAddress = ReadLEA32(Patterns::pattern_for_YWLabel, L"YWlabel", 0, 1, 5);

        FunctionCalls::tYWLabel YWLabelFunction = reinterpret_cast<FunctionCalls::tYWLabel>(YWLabelFunctionAddress);

        uintptr_t OpenSingletonWindowFunctionAddress = ReadLEA32(Patterns::pattern_for_OpenSingletonWindow, L"OpenSingletonWindow", 0, 1, 5);

        FunctionCalls::tOpenSingletonWindow OpenSingletonWindowFunction = reinterpret_cast<FunctionCalls::tOpenSingletonWindow>(OpenSingletonWindowFunctionAddress);

        //uintptr_t YWLabelFunctionAddress = ReadLEA32(Patterns::pattern_for_YWLabel, L"YWlabel", 0, 1, 5);

        //FunctionCalls::tYWLabel YWLabelFunction = reinterpret_cast<FunctionCalls::tYWLabel>(YWLabelFunctionAddress);

        uintptr_t ShowDialWidgetFunctionAddress = ReadLEA32(Patterns::pattern_for_DialWidget, L"Dial Widget", 0, 1, 5);
        FunctionCalls::tDialWidget ShowDialWidgetFunction = reinterpret_cast<FunctionCalls::tDialWidget>(ShowDialWidgetFunctionAddress);


        uintptr_t GetOBJEntrynameFunctionAddress = ReadLEA32(Patterns::pattern_for_GetObjEntryName, L"Get obj Entryname", 0, 1, 5);
        FunctionCalls::tGetObjEntryName GetOBJEntrynameFunction = reinterpret_cast<FunctionCalls::tGetObjEntryName>(GetOBJEntrynameFunctionAddress);



        uintptr_t ShowDebugWindowFunctionAddress = ReadLEA32(Patterns::pattern_for_ShowDebugWindow, L"ShowDebugWindow", 0, 1, 5);
        FunctionCalls::tShowDebugWindow ShowDebugWindowFunction = reinterpret_cast<FunctionCalls::tShowDebugWindow>(ShowDebugWindowFunctionAddress);


        uintptr_t AllocYWMemoryFunctionAddress = ReadLEA32(Patterns::pattern_for_Alloc_YW_Memory, L"AllocYWMemory", 0, 1, 5);

        FunctionCalls::tAlloc_YW_Memory AllocYWMemoryFunction = reinterpret_cast<FunctionCalls::tAlloc_YW_Memory>(AllocYWMemoryFunctionAddress);

        FunctionCalls::tAlloc_YW_MemoryNew AllocYWMemoryFunctionNew = reinterpret_cast<FunctionCalls::tAlloc_YW_MemoryNew>(AllocYWMemoryFunctionAddress);


        uintptr_t ContainerLinkFunctionAddress = FindAndPrintPatternW(Patterns::pattern_for_ContainerLink, L"ContainerLink", 0);

        FunctionCalls::tContainerLink ContainerLinkFunction = reinterpret_cast<FunctionCalls::tContainerLink>(ContainerLinkFunctionAddress);

        uintptr_t YWScrollbarFunctionAddress = FindAndPrintPatternW(Patterns::pattern_for_YWScrollbar, L"YWScrollbar", 0);

        FunctionCalls::tYWScrollbar YWScrollbarFunction = reinterpret_cast<FunctionCalls::tYWScrollbar>(YWScrollbarFunctionAddress);

        uintptr_t YWButtonFunctionAddress = ReadLEA32(Patterns::pattern_for_YWButton, L"YWButton", 0, 1, 5);
        FunctionCalls::tYWButton YWButtonFunction = reinterpret_cast<FunctionCalls::tYWButton>(YWButtonFunctionAddress);

        

        
        //auto* Menu = OpenSingletonWindowFunction(windowTitle.c_str(), 280, 156, 0);
        auto allocYWMemLabelPan = AllocYWMemoryFunction((YW::MEMORY*)0x200);
        auto allocYWMemLabelPitch = AllocYWMemoryFunction((YW::MEMORY*)0x200);
        auto allocYWMemLabel3DPos = AllocYWMemoryFunction((YW::MEMORY*)0x200);
        //auto allocYWMemLabelScaleW = AllocYWMemoryFunction((YW::MEMORY*)0x200);
       // auto allocYWMemDialScale = AllocYWMemoryFunction((YW::MEMORY*)0x200);
        auto Menu = AllocYWMemoryFunction((YW::MEMORY*)0x200);

        YWWindowFunction((YW::WINDOW*)Menu, "Audio Playback Editor", 0, 0, 360, 220, 2);

        g_ScaleSliderPitch = (YW::SCROLLBAR*)AllocYWMemoryFunctionNew(0xA0);
        g_ScaleSliderPan = (YW::SCROLLBAR*)AllocYWMemoryFunctionNew(0xA0);
        g_ScaleSlider3DPos = (YW::SCROLLBAR*)AllocYWMemoryFunctionNew(0xA0);
        //g_ScaleSliderW = (YW::SCROLLBAR*)AllocYWMemoryFunctionNew(0xA0);

        int marginX = 10;

        int labelX = marginX;
        int labelWidth = 80;

        int scrollbarX = labelX + labelWidth + 10;
        int scrollbarWidth = 150;
        int scrollbarHeight = 7;

        int rowHeight = 22;


        float panDefault = g_currentPan;         // e.g. 0.0f
        float pitchDefault = g_currentPitch;     // e.g. 1.0f (normal pitch)
        float posDefault = g_current3DPos;       // e.g. 0.0f

        
        YWLabelFunction((YW::LABEL_3*)allocYWMemLabelPan, "Pan:", labelX, 5, labelWidth, 12);
        YWScrollbarFunction(g_ScaleSliderPan, scrollbarX, 5, scrollbarWidth, scrollbarHeight, 0.1, 1.0, 1.0, 1.0);
        g_ScaleSliderPan->Value = panDefault;
        *reinterpret_cast<void***>(g_ScaleSliderPan) = reinterpret_cast<void**>(0x140600F48);

        // Row 1 - Pitch
        YWLabelFunction((YW::LABEL_3*)allocYWMemLabelPitch, "Pitch:", labelX, 5 + rowHeight, labelWidth, 12);
        YWScrollbarFunction(g_ScaleSliderPitch, scrollbarX, 5 + rowHeight, scrollbarWidth, scrollbarHeight, 0.1, 9.9, 1.0, 1.0);
        g_ScaleSliderPitch->Value = pitchDefault;
        *reinterpret_cast<void***>(g_ScaleSliderPitch) = reinterpret_cast<void**>(0x140600F48);

        // Row 2 - 3DPos
        YWLabelFunction((YW::LABEL_3*)allocYWMemLabel3DPos, "3DPos:", labelX, 5 + 2 * rowHeight, labelWidth, 12);
        YWScrollbarFunction(g_ScaleSlider3DPos, scrollbarX, 5 + 2 * rowHeight, scrollbarWidth, scrollbarHeight, 0, 10, 1.0, 1.0);
        g_ScaleSlider3DPos->Value = posDefault;
        *reinterpret_cast<void***>(g_ScaleSlider3DPos) = reinterpret_cast<void**>(0x140600F48);

        auto allocYWMemResetButton = AllocYWMemoryFunction((YW::MEMORY*)0x200);

        // Create Reset button
        YWButtonFunction(
            (YW::BUTTON_3*)allocYWMemResetButton,
            "Reset to Defaults",
            marginX,
            5 + 3 * rowHeight + 12,
            180,
            22,
            ResetSlidersCallback,
            nullptr
        );

        //ShowDialWidgetFunction((YW::DIAL_3*)allocYWMemDialScale, 140, 5, 56, 16, 0, 8, ObjectScaleCallBack);

       // SetDialValueFunction((YW::DIAL_3*)allocYWMemDialScale, gObjectScale);

        ContainerLinkFunction((YW::CONTAINER*)Menu, (YW::WIDGET*)allocYWMemLabelPan);
        ContainerLinkFunction((YW::CONTAINER*)Menu, (YW::WIDGET*)allocYWMemLabelPitch);
        ContainerLinkFunction((YW::CONTAINER*)Menu, (YW::WIDGET*)allocYWMemLabel3DPos);

        //ContainerLinkFunction((YW::CONTAINER*)Menu, (YW::WIDGET*)allocYWMemDialScale);

        ContainerLinkFunction((YW::CONTAINER*)Menu, (YW::WIDGET*)g_ScaleSliderPitch);
        ContainerLinkFunction((YW::CONTAINER*)Menu, (YW::WIDGET*)g_ScaleSliderPan);
        ContainerLinkFunction((YW::CONTAINER*)Menu, (YW::WIDGET*)g_ScaleSlider3DPos);
        ContainerLinkFunction((YW::CONTAINER*)Menu, (YW::WIDGET*)allocYWMemResetButton);
       // ContainerLinkFunction((YW::CONTAINER*)Menu, (YW::WIDGET*)g_ScaleSliderW);

        ShowDebugWindowFunction((YW::WIDGET*)Menu);

    }




    void PlaySoundWithTrackingLoop(const wchar_t* soundFileName) {
        if (soundFileName != nullptr) {
            currentSoundFile = soundFileName;

            

            if (g_soundInitialized) {
                ma_sound_uninit(&g_sound);
                g_soundInitialized = false;
            }

            ma_result result = ma_sound_init_from_file_w(
                &g_engine,
                soundFileName,
                MA_SOUND_FLAG_STREAM, // only stream, not loop flag
                NULL,
                NULL,
                &g_sound
            );

            if (result == MA_SUCCESS) {
                ma_sound_set_looping(&g_sound, MA_TRUE);
                ma_sound_start(&g_sound);
                g_soundInitialized = true;
                wprintf(L"Playing (looped): %s\n", soundFileName);
                AudioPlayBackWidget();
                InitDecoder(soundFileName);
            }
            else {
                wprintf(L"Failed to load: %s\n", soundFileName);
            }
        }
    }

    void PlaySoundWithTracking(const wchar_t* soundFileName) {
        if (soundFileName != nullptr) {
            currentSoundFile = soundFileName;

            if (g_soundInitialized) {
                ma_sound_uninit(&g_sound);
                g_soundInitialized = false;
            }

            ma_result result = ma_sound_init_from_file_w(&g_engine, soundFileName, MA_SOUND_FLAG_STREAM, NULL, NULL, &g_sound);
            if (result == MA_SUCCESS) {
                ma_sound_start(&g_sound);
                g_soundInitialized = true;
                wprintf(L"Playing: %s\n", soundFileName);
                AudioPlayBackWidget();
                InitDecoder(soundFileName);
            }
            else {
                wprintf(L"Failed to load: %s\n", soundFileName);
            }
        }
    }


    void PlayAudio(int songIndex) {
        if (songIndex < playlist.size() && !isDirectoryList[songIndex]) {
            const std::wstring& currentSong = playlist[songIndex];
            std::wstring fullFilePath = currentPath + L"\\" + currentSong;
            if (isLooping) {
                PlaySoundWithTrackingLoop(fullFilePath.c_str());
            }
            else {
                PlaySoundWithTracking(fullFilePath.c_str());
            }
            isPlaying = true;
            currentSongIndex = songIndex;
        }
    }


    /*void StopAudio() {
        PlaySound(NULL, NULL, SND_PURGE);
        currentSoundFile.clear();
        isPlaying = false;
    }*/

    void StopAudio() {
        if (g_soundInitialized) {
            ma_sound_stop(&g_sound);
            ma_sound_uninit(&g_sound);
            g_soundInitialized = false;
        }

        if (g_decoderInitialized) {
            ma_decoder_uninit(&g_decoder);
            g_decoderInitialized = false;
        }

        currentSoundFile.clear();
        isPlaying = false;
    }

    void CustomMusicPlayer(unsigned int* arg);  // Forward declaration

    void CustomMusicPlayCallback(YW::EVENT event, int index, unsigned int* arg) {
        if ((event.Flag & 0x20) != 0) {
            if (index == static_cast<int>(loopingOptionIndex)) {
                isLooping = !isLooping;
                const char* newLoopingOption = isLooping ? "Loop Music: On" : "Loop Music: Off";
                playlistList[loopingOptionIndex] = newLoopingOption;
                return;
            }

            if (index < isDirectoryList.size()) {
                if (isDirectoryList[index]) {
                    if (playlist[index] == L"..\\") {
                        size_t pos = currentPath.find_last_of(L"\\/");
                        if (pos != std::wstring::npos) {
                            currentPath = currentPath.substr(0, pos);
                        }
                    }
                    else {
                        std::wstring folderName = playlist[index];
                        folderName = folderName.substr(0, folderName.size() - 1); // remove '/'
                        currentPath += L"\\" + folderName;
                    }
                    playlistLoaded = false;
                    CustomMusicPlayer(nullptr); // Refresh
                }
               /* else {
                    if (isPlaying && currentSongIndex == index) {
                        StopAudio();
                        isCustomMusicPlaying = false;
                    }
                    else {
                        isCustomMusicPlaying = true;
                        PlayAudio(index);
                    }
                }*/
                else {
                    if (isPlaying && currentSongIndex == index) {
                        StopAudio();
                        isCustomMusicPlaying = false;

                        // Clear the " - playing" label since we're stopping the current song
                        std::string oldStr = WStringToString(playlist[currentSongIndex]);
                        delete[] playlistList[currentSongIndex];
                        playlistList[currentSongIndex] = _strdup(oldStr.c_str());
                    }
                    else {
                        // Save the old index before changing it
                        int previousIndex = currentSongIndex;

                        isCustomMusicPlaying = true;
                        PlayAudio(index);

                        // Remove "- playing" from the previously playing song
                        if (isPlaying && previousIndex < playlist.size()) {
                            std::string oldStr = WStringToString(playlist[previousIndex]);
                            delete[] playlistList[previousIndex];
                            playlistList[previousIndex] = _strdup(oldStr.c_str());
                        }

                        // Add "- playing" to the newly selected song
                        std::string nowPlayingStr = WStringToString(playlist[index]) + " - playing";
                        delete[] playlistList[index];
                        playlistList[index] = _strdup(nowPlayingStr.c_str());
                    }
                }
            }

        }
    }



    void ClearPlaylistList() {
        if (playlistList) {
            for (size_t i = 0; playlistList[i] != nullptr; ++i) {
                delete[] playlistList[i];
            }
            delete[] playlistList;
            playlistList = nullptr;
        }
    }

    void CustomMusicPlayerFinalizer(YW::WIDGET* widget) {
        // Clear the playlist list when the widget is destroyed
        ClearPlaylistList();
    }



    void CustomMusicPlayer(unsigned int* arg) {


        uintptr_t OpenSingletonWindowFunctionAddress = ReadLEA32(Patterns::pattern_for_OpenSingletonWindow, L"OpenSingletonWindow", 0, 1, 5);

        uintptr_t AllocYWMemoryFunctionAddress = ReadLEA32(Patterns::pattern_for_Alloc_YW_Memory, L"AllocYWMemory", 0, 1, 5);

        uintptr_t WListFunctionAddress = ReadLEA32(Patterns::pattern_for_YWList, L"YWList", 0, 1, 5);


        uintptr_t YWFrameFunctionAddress = ReadLEA32(Patterns::pattern_for_YWFrame, L"YWFrame", 0, 1, 5);


        uintptr_t ContainerLinkFunctionAddress = FindAndPrintPatternW(Patterns::pattern_for_ContainerLink, L"ContainerLink", 0);//muuta tohon FindAndPrintPatternW 0


        uintptr_t YWwindowMinSizeFunctionAddress = FindAndPrintPatternW(Patterns::pattern_for_YWwindow_minSize, L"YWwindow Min Size", 0);// muuta tohon FindAndPrintPatternW 0


        uintptr_t YWwindowMaxSizeFunctionAddress = FindAndPrintPatternW(Patterns::pattern_for_YWwindow_maxSize, L"YWwindow Max Size", 0);


        uintptr_t ShowDebugWindowFunctionAddress = ReadLEA32(Patterns::pattern_for_ShowDebugWindow, L"ShowDebugWindow", 0, 1, 5);


        //uintptr_t DebugWindowAddSubMenuFunctionAddress = ReadLEA32(Patterns::pattern_for_DebugWindowAddSubMenu, L"Add Debug Submenu", 0, 1, 5);





        FunctionCalls::tAlloc_YW_Memory AllocYWMemoryFunction = reinterpret_cast<FunctionCalls::tAlloc_YW_Memory>(AllocYWMemoryFunctionAddress);
        FunctionCalls::tYWList YWListFunction = reinterpret_cast<FunctionCalls::tYWList>(WListFunctionAddress);
        FunctionCalls::tYWFrame YWFrameFunction = reinterpret_cast<FunctionCalls::tYWFrame>(YWFrameFunctionAddress);
        FunctionCalls::tContainerLink ContainerLinkFunction = reinterpret_cast<FunctionCalls::tContainerLink>(ContainerLinkFunctionAddress);
        FunctionCalls::tYW_WINDOW_set_min_size YWwindowMinSizeFunction = reinterpret_cast<FunctionCalls::tYW_WINDOW_set_min_size>(YWwindowMinSizeFunctionAddress);
        FunctionCalls::tYW_WINDOW_set_max_size YWwindowMaxSizeFunction = reinterpret_cast<FunctionCalls::tYW_WINDOW_set_max_size>(YWwindowMaxSizeFunctionAddress);
        FunctionCalls::tShowDebugWindow ShowDebugWindowFunction = reinterpret_cast<FunctionCalls::tShowDebugWindow>(ShowDebugWindowFunctionAddress);

        FunctionCalls::tOpenSingletonWindow OpenSingletonWindowFunction = reinterpret_cast<FunctionCalls::tOpenSingletonWindow>(OpenSingletonWindowFunctionAddress);



        // If the window is not open, create it and register the close callback
        //YW::WINDOW* Menu = OpenSingletonWindow("Custom Music Player", 200, 300, 2);

        //YW::YW_WINDOW_39* Menu = OpenSingletonWindowFunction("Custom Music Player", 200, 300, 2);

        std::wstring folderName = GetLastFolderName(currentPath);
        std::wstring fullTitle = L"Custom Music Player - " + folderName;
        std::string titleStr(fullTitle.begin(), fullTitle.end()); // Convert wstring to string
        YW::YW_WINDOW_39* Menu = OpenSingletonWindowFunction(titleStr.c_str(), 200, 300, 2);

        // Clear the playlist list when the window is opened
            //ClearPlaylistList();

            // Initialize the playlist list again


    //auto allocYWMemBtnPlay = Alloc_YW_Memory((YW_MEMORY*)0x200);
    //auto allocYWMemBtnPause = Alloc_YW_Memory((YW_MEMORY*)0x200);
    //auto allocYWMemBtnRewind = Alloc_YW_Memory((YW_MEMORY*)0x200);
    //auto allocYWMemBtnNextTrack = Alloc_YW_Memory((YW_MEMORY*)0x200);
        //auto allocYWMemList = Alloc_YW_Memory((YW_MEMORY*)0x200);

        auto allocYWMemList = AllocYWMemoryFunction((YW::MEMORY*)0x200);

        //auto allocYWMemToggleBtn = Alloc_YW_Memory((YW_MEMORY*)0x200);
        //auto allocYWMemLabelLoop = Alloc_YW_Memory((YW_MEMORY*)0x200);
        
        unsigned int* Frame = AllocYWMemoryFunction((YW::MEMORY*)0x200);

       /* if (!playlistLoaded) {
            SearchAndAddWavFiles(playlist);
            //SearchAndAddWavFiles(playlist);
            //RefreshList();




            if (playlist.empty()) {
                // No .wav files found, show message
                const char* noFilesMessage = "Add .wav files to the DebugMenu/music folder to play music";
                size_t messageLength = strlen(noFilesMessage);
                playlistList = new const char* [2]; // 1 for the message and 1 for the looping option
                playlistList[0] = new char[messageLength + 1]; // +1 for the null terminator
                strcpy_s(const_cast<char*>(playlistList[0]), messageLength + 1, noFilesMessage);
                playlistList[1] = nullptr;
                loopingOptionIndex = 1; // Set looping option index
            }
            else {
                // Allocate memory for const char** playlistList
                playlistList = new const char* [playlist.size() + 2]; // +2 for the songs and the looping option

                // Convert the filenames to string and copy into playlistList
                for (size_t i = 0; i < playlist.size(); ++i) {
                    const std::wstring& wstr = playlist[i];
                    std::string str = WStringToString(wstr);
                    playlistList[i] = new char[str.size() + 1]; // +1 for the null terminator
                    strcpy_s(const_cast<char*>(playlistList[i]), str.size() + 1, str.c_str());
                }
                // Add looping option to the end of the list
                playlistList[playlist.size()] = "Loop Music: Off"; // Default looping state
                playlistList[playlist.size() + 1] = nullptr; // Null-terminate the array
                loopingOptionIndex = playlist.size(); // Set looping option index
            }
            playlistLoaded = true;
        }*/
        if (!playlistLoaded) {
            SearchAndAddAudioFilesAndFolders(currentPath, playlist, isDirectoryList);

            if (playlist.empty()) {
                const char* noFilesMessage = "No files or folders found.";
                size_t messageLength = strlen(noFilesMessage);
                playlistList = new const char* [2];
                playlistList[0] = new char[messageLength + 1];
                strcpy_s(const_cast<char*>(playlistList[0]), messageLength + 1, noFilesMessage);
                playlistList[1] = nullptr;
                loopingOptionIndex = 1;
            }
            else {
                playlistList = new const char* [playlist.size() + 2]; // +loop option

                for (size_t i = 0; i < playlist.size(); ++i) {
                    //wprintf(L"Found entry: %s\n", playlist[i].c_str());
                    std::string str = WStringToString(playlist[i]);
                    playlistList[i] = new char[str.size() + 1];
                    strcpy_s(const_cast<char*>(playlistList[i]), str.size() + 1, str.c_str());
                }

                playlistList[playlist.size()] = "Loop Music: Off";
                playlistList[playlist.size() + 1] = nullptr;
                loopingOptionIndex = playlist.size();
            }
            playlistLoaded = true;
        }


        YW::WIDGET* widget;


        widget = 0;

        // YWLabel((YW_LABEL_3*)allocYWMemLabelCurrentFrame, labelText.c_str(), 0, 0, 80, 12);
        //YWList((YW::YW_LIST_0*)allocYWMemList, 0, 0, playlistList, CustomMusicPlayCallback, allocYWMemList);

        YWListFunction((YW::YW_LIST_0*)allocYWMemList, 0, 0, playlistList, CustomMusicPlayCallback, allocYWMemList);
        widget = (YW::WIDGET*)allocYWMemList;

        //widget->Finalizer = CustomMusicPlayerFinalizer;


        YW::FRAME* v5;
        v5 = 0;

        //YWFrame((YW::FRAME*)Frame, 0, 11, 0, -11, 0, widget->Rect.H);

        YWFrameFunction((YW::FRAME*)Frame, 0, 11, 0, -11, 0, widget->Rect.H);

        v5 = (YW::FRAME*)Frame;

        //ContainerLink((YW::CONTAINER*)v5, (YW::WIDGET*)widget);
        //ContainerLink((YW::CONTAINER*)Menu, (YW::WIDGET*)v5);

        // Estimate title width
        

        

        ContainerLinkFunction((YW::CONTAINER*)v5, (YW::WIDGET*)widget);
        ContainerLinkFunction((YW::CONTAINER*)Menu, (YW::WIDGET*)v5);
        //ContainerLink((YW_CONTAINER_5*)Menu, (YW_WIDGET*)allocYWMemLabelCurrentFrame);

        int contentHeight = widget->Rect.H + 5; // Assuming additional padding of 22
        int contentWidth = widget->Rect.W + 7; // Assuming widget->Rect.W is the width of the list

        int titleTextWidth = EstimateTextWidth(titleStr);
        if (titleTextWidth > contentWidth) {
            contentWidth = titleTextWidth + 20; // Add a bit of padding
        }

        //YW_WINDOW_set_min_size(Menu, contentWidth, contentHeight);
        YWwindowMinSizeFunction(Menu, contentWidth, contentHeight);
        //YW_WINDOW_set_max_size(Menu, contentWidth, contentHeight);
        YWwindowMaxSizeFunction(Menu, contentWidth, contentHeight);

        //ShowDebugWindow((YW_WIDGET_50*)Menu);
        ShowDebugWindowFunction((YW::WIDGET*)Menu);

    }

    void paxEffectDialCallBack(int id) {
        uintptr_t StartPaxAddress = ReadLEA32(Patterns::pattern_for_StartPax, L"start pax effect", 0, 1, 5);
        uintptr_t GetPlayerAddress = ReadLEA32(Patterns::pattern_for_GetPlayer, L"Get Player", 0, 1, 5);
        FunctionCalls::tPaxStart StartPaxFunction = reinterpret_cast<FunctionCalls::tPaxStart>(StartPaxAddress);
        FunctionCalls::tGetPlayer GetPlayerFunction = reinterpret_cast<FunctionCalls::tGetPlayer>(GetPlayerAddress);

        InitializePointers();
        uintptr_t PaxAddress = 0;//updated to steam


        if (DetectGameVersion() == "Steam") {
            PaxAddress = (SteamPointer + 0x1AF2A68);

        }
        else if (DetectGameVersion() == "Epic") {
            PaxAddress = (EpicPointer + 0x1AF2A68);
        }

        //YS::RYJ_PAX* paxObject = new YS::RYJ_PAX();
        //PaxAddress = reinterpret_cast<uintptr_t>(paxObject); // Store the pointer in PaxAddress

        YS::OBJ* playerObj = GetPlayerFunction();


        StartPaxFunction(reinterpret_cast<YS::RYJ_PAX*>(PaxAddress), id, 1, 1, 0, reinterpret_cast<unsigned int*>(playerObj));

    }

    float GetCurrentAmplitude() {
        if (!g_decoderInitialized || !g_soundInitialized) return 0.0f;

        const int frameCount = 1024;
        float samples[frameCount * 2] = { 0 };
        ma_uint64 framesRead = 0;

        ma_uint64 cursor = 0;
        ma_sound_get_cursor_in_pcm_frames(&g_sound, &cursor);
        ma_decoder_seek_to_pcm_frame(&g_decoder, cursor);

        ma_result result = ma_decoder_read_pcm_frames(&g_decoder, samples, frameCount, &framesRead);

        //wprintf(L"Read result: %d, framesRead: %llu\n", result, framesRead);

        if (result != MA_SUCCESS || framesRead == 0) {
            return 0.0f;
        }

        float amplitude = 0.0f;
        for (ma_uint64 i = 0; i < framesRead * 2; i++) {
            amplitude += fabsf(samples[i]);
        }

        amplitude /= static_cast<float>(framesRead * 2);

        if (isnan(amplitude)) {
           // wprintf(L"Amplitude is NaN!\n");
            return 0.0f;
        }

        return amplitude;
    }





    void PaxEffectSelector(unsigned int* arg) {
        uintptr_t YWListButtonFunctionAddress = ReadLEA32(Patterns::pattern_for_YWListButton, L"YW LIST BUTTON", 0, 1, 5);
        uintptr_t OpenSingletonWindowFunctionAddress = ReadLEA32(Patterns::pattern_for_OpenSingletonWindow, L"OpenSingletonWindow", 0, 1, 5);
        uintptr_t AllocYWMemoryFunctionAddress = ReadLEA32(Patterns::pattern_for_Alloc_YW_Memory, L"AllocYWMemory", 0, 1, 5);
        uintptr_t WListFunctionAddress = ReadLEA32(Patterns::pattern_for_YWList, L"YWList", 0, 1, 5);
        uintptr_t YWFrameFunctionAddress = ReadLEA32(Patterns::pattern_for_YWFrame, L"YWFrame", 0, 1, 5);
        uintptr_t ContainerLinkFunctionAddress = FindAndPrintPatternW(Patterns::pattern_for_ContainerLink, L"ContainerLink", 0);
        uintptr_t YWwindowMinSizeFunctionAddress = FindAndPrintPatternW(Patterns::pattern_for_YWwindow_minSize, L"YWwindow Min Size", 0);
        uintptr_t YWwindowMaxSizeFunctionAddress = FindAndPrintPatternW(Patterns::pattern_for_YWwindow_maxSize, L"YWwindow Max Size", 0);
        uintptr_t ShowDebugWindowFunctionAddress = ReadLEA32(Patterns::pattern_for_ShowDebugWindow, L"ShowDebugWindow", 0, 1, 5);
        uintptr_t ShowDialWidgetFunctionAddress = ReadLEA32(Patterns::pattern_for_DialWidget, L"Dial Widget", 0, 1, 5);


        FunctionCalls::tYWListButton YWListButtonFunction = reinterpret_cast<FunctionCalls::tYWListButton>(YWListButtonFunctionAddress);
        FunctionCalls::tAlloc_YW_Memory AllocYWMemoryFunction = reinterpret_cast<FunctionCalls::tAlloc_YW_Memory>(AllocYWMemoryFunctionAddress);
        FunctionCalls::tYWList YWListFunction = reinterpret_cast<FunctionCalls::tYWList>(WListFunctionAddress);
        FunctionCalls::tYWFrame YWFrameFunction = reinterpret_cast<FunctionCalls::tYWFrame>(YWFrameFunctionAddress);
        FunctionCalls::tContainerLink ContainerLinkFunction = reinterpret_cast<FunctionCalls::tContainerLink>(ContainerLinkFunctionAddress);
        FunctionCalls::tYW_WINDOW_set_min_size YWwindowMinSizeFunction = reinterpret_cast<FunctionCalls::tYW_WINDOW_set_min_size>(YWwindowMinSizeFunctionAddress);
        FunctionCalls::tYW_WINDOW_set_max_size YWwindowMaxSizeFunction = reinterpret_cast<FunctionCalls::tYW_WINDOW_set_max_size>(YWwindowMaxSizeFunctionAddress);
        FunctionCalls::tShowDebugWindow ShowDebugWindowFunction = reinterpret_cast<FunctionCalls::tShowDebugWindow>(ShowDebugWindowFunctionAddress);

        FunctionCalls::tOpenSingletonWindow OpenSingletonWindowFunction = reinterpret_cast<FunctionCalls::tOpenSingletonWindow>(OpenSingletonWindowFunctionAddress);
        FunctionCalls::tDialWidget ShowDialWidgetFunction = reinterpret_cast<FunctionCalls::tDialWidget>(ShowDialWidgetFunctionAddress);

        YW::YW_WINDOW_39* Menu = OpenSingletonWindowFunction("Pax Effect Selector", 180, 300, 2);

        auto allocdialwidget = AllocYWMemoryFunction((YW::MEMORY*)0x200);

        ShowDialWidgetFunction((YW::DIAL_3*)allocdialwidget, 0, 0, 96, 16, 0, 999999, paxEffectDialCallBack);

        ContainerLinkFunction((YW::CONTAINER*)Menu, (YW::WIDGET*)allocdialwidget);

        ShowDebugWindowFunction((YW::WIDGET*)Menu);
    }

    void SetObjEntryId(OBJENTRY_ID id) {
        myObjEntryId = id;  // Set the global OBJENTRY_ID
    }

    void OBJButtonCallback(unsigned int* arg) {
        //unsigned int id = GetDialWidgetValue((ryj_DIAL_3*)arg);
        //SetDialWidgetValue((ryj_DIAL_3*)arg,id);
       // callbackID = GetDialValue((ryj_DIAL_3*)arg);
        //OpenMapJump(0);
        //NewObj(callbackID, callBackSpawnAmount);
       // std::thread mapjumpingThread(WaitForMapjumping);

        uintptr_t FreeYWwidgetFunctionAddress = FindAndPrintPatternW(Patterns::pattern_for_FreeYWwindowWidget, L"FREE YW WINDOW WIDGET", 0);

        FunctionCalls::tFreeYWwindowwidget FreeYWwidgetFunction = reinterpret_cast<FunctionCalls::tFreeYWwindowwidget>(FreeYWwidgetFunctionAddress);



        if (IsEntry1Enabled && !IsEntry2Enabled && !IsEntry3Enabled && !IsEntry4Enabled && !IsEntry5Enabled) {
            NewObj(Entry1CallbackID, Entry1CallbackSpawnAmount);

        }
        else if (!IsEntry1Enabled && IsEntry2Enabled && !IsEntry3Enabled && !IsEntry4Enabled && !IsEntry5Enabled) {
            NewObj2(Entry2CallbackID, Entry2CallbackSpawnAmount);

        }
        else if (!IsEntry1Enabled && !IsEntry2Enabled && IsEntry3Enabled && !IsEntry4Enabled && !IsEntry5Enabled) {
            NewObj3(Entry3CallbackID, Entry3CallbackSpawnAmount);

        }
        else if (!IsEntry1Enabled && IsEntry2Enabled && !IsEntry3Enabled && IsEntry4Enabled && !IsEntry5Enabled) {
            NewObj4(Entry4CallbackID, Entry4CallbackSpawnAmount);

        }
        else if (!IsEntry1Enabled && IsEntry2Enabled && !IsEntry3Enabled && !IsEntry4Enabled && IsEntry5Enabled) {
            NewObj5(Entry5CallbackID, Entry5CallbackSpawnAmount);

        }
        else if (IsEntry1Enabled && IsEntry2Enabled && !IsEntry3Enabled && !IsEntry4Enabled && !IsEntry5Enabled) {
            NewObj(Entry1CallbackID, Entry1CallbackSpawnAmount);
            NewObj2(Entry2CallbackID, Entry2CallbackSpawnAmount);

        }
        else if (IsEntry1Enabled && IsEntry2Enabled && IsEntry3Enabled && !IsEntry4Enabled && !IsEntry5Enabled) {
            NewObj(Entry1CallbackID, Entry1CallbackSpawnAmount);
            NewObj2(Entry2CallbackID, Entry2CallbackSpawnAmount);
            NewObj3(Entry3CallbackID, Entry3CallbackSpawnAmount);

        }
        else if (IsEntry1Enabled && IsEntry2Enabled && IsEntry3Enabled && IsEntry4Enabled && !IsEntry5Enabled) {
            NewObj(Entry1CallbackID, Entry1CallbackSpawnAmount);
            NewObj2(Entry2CallbackID, Entry2CallbackSpawnAmount);
            NewObj3(Entry3CallbackID, Entry3CallbackSpawnAmount);
            NewObj4(Entry4CallbackID, Entry4CallbackSpawnAmount);

        }
        else if (!IsEntry1Enabled && IsEntry2Enabled && IsEntry3Enabled && !IsEntry4Enabled && !IsEntry5Enabled) {
            NewObj2(Entry2CallbackID, Entry2CallbackSpawnAmount);
            NewObj3(Entry3CallbackID, Entry3CallbackSpawnAmount);

        }
        else if (!IsEntry1Enabled && !IsEntry2Enabled && IsEntry3Enabled && IsEntry4Enabled && !IsEntry5Enabled) {
            NewObj4(Entry4CallbackID, Entry4CallbackSpawnAmount);
            NewObj3(Entry3CallbackID, Entry3CallbackSpawnAmount);

        }
        else if (!IsEntry1Enabled && !IsEntry2Enabled && !IsEntry3Enabled && IsEntry4Enabled && IsEntry5Enabled) {
            NewObj4(Entry4CallbackID, Entry4CallbackSpawnAmount);
            NewObj5(Entry5CallbackID, Entry5CallbackSpawnAmount);

        }

        else if (IsEntry1Enabled && IsEntry2Enabled && IsEntry3Enabled && IsEntry4Enabled && IsEntry5Enabled) {
            // Execute code when all entries are enabled
            NewObj(Entry1CallbackID, Entry1CallbackSpawnAmount);
            NewObj2(Entry2CallbackID, Entry2CallbackSpawnAmount);
            NewObj3(Entry3CallbackID, Entry3CallbackSpawnAmount);
            NewObj4(Entry4CallbackID, Entry4CallbackSpawnAmount);
            NewObj5(Entry5CallbackID, Entry5CallbackSpawnAmount);

        }

        if (IsPlayerEntryEnabled) {

            //*soraUCMslotEntryID = PlayerEntryCallBackID;
            //NewCaptureObj(PlayerEntryCallBackID);
            NewSora(PlayerEntryCallBackID);
            //NewPlayerObj(PlayerEntryCallBackID);

           // MapJump(area(), 1, 0, 0, 0);

        }

        //OBJENTRYMenu = 0;
        FreeYWwidgetFunction((YW::WIDGET_5*)OBJENTRYMenu, 1);
        OBJENTRYMenu = nullptr;

    }

    void ToggleButton1CallBack(bool flag, unsigned int* arg) {
        if (flag) {
            IsEntry1Enabled = true;
        }
        else {
            IsEntry1Enabled = false;
        }
    }

    void ToggleButton2CallBack(bool flag, unsigned int* arg) {
        if (flag) {
            IsEntry2Enabled = true;
        }
        else {
            IsEntry2Enabled = false;
        }
    }

    void ToggleButton3CallBack(bool flag, unsigned int* arg) {
        if (flag) {
            IsEntry3Enabled = true;
        }
        else {
            IsEntry3Enabled = false;
        }
    }

    void ToggleButton4CallBack(bool flag, unsigned int* arg) {
        if (flag) {
            IsEntry4Enabled = true;
        }
        else {
            IsEntry4Enabled = false;
        }
    }

    void ToggleButton5CallBack(bool flag, unsigned int* arg) {
        if (flag) {
            IsEntry5Enabled = true;
        }
        else {
            IsEntry5Enabled = false;
        }
    }

    void ToggleButtonPCallBack(bool flag, unsigned int* arg) {

        if (flag) {
            if (!IsPlayerEntryEnabled) { // Check if it's not already true
                IsPlayerEntryEnabled = true;
                // FormCaptureEnd(GetSora());
            }

        }
        else {
            // FormCaptureEnd((YS_SORA_5*)GetPlayer());
            IsPlayerEntryEnabled = false;

        }

    }

    void WidgetValuePlayer(int id) {

        if (PlayerEntryCallBackID != id) {
            PlayerEntryCallBackID = id;
            // printf("Player Entry Name: %s\n", GetOBJENTRY(PlayerEntryCallBackID)->EntryName);
        }

       // if (!initializing) {
       //     PlaySystemSEsound(2, 0);
       // }

    }

    void WidgetValue5(int id) {

        if (Entry5CallbackID != id) {
            Entry5CallbackID = id;

            //  printf("Entry 5 Name: %s\n", GetOBJENTRY(Entry5CallbackID)->EntryName);

          //  if (!initializing) {
          //      PlaySystemSEsound(2, 0);
           // }
        }

    }

    void WidgetValue4(int id) {

        if (Entry4CallbackID != id) {
            Entry4CallbackID = id;

            // printf("Entry 4 Name: %s\n", OBJENTRY(Entry4CallbackID)->EntryName);

          //  if (!initializing) {
          //      PlaySystemSEsound(2, 0);
           // }
        }

    }

    void WidgetValue3(int id) {

        if (Entry3CallbackID != id) {
            Entry3CallbackID = id;

            // printf("Entry 3 Name: %s\n", OBJENTRY(Entry3CallbackID)->EntryName);

           // if (!initializing) {
           //     PlaySystemSEsound(2, 0);
           // }
        }

    }

    void WidgetValue2(int id) {

        if (Entry2CallbackID != id) {
            Entry2CallbackID = id;

            // printf("Entry 2 Name: %s\n", OBJENTRY(Entry2CallbackID)->EntryName);



            //if (!initializing) {
            //    PlaySystemSEsound(2, 0);
           // }
        }

    }




    void WidgetValue(int id) {

        if (Entry1CallbackID != id) {
            Entry1CallbackID = id;

            //printf("Entry 1 Name: %s\n", OBJENTRY(Entry1CallbackID)->EntryName);



            //if (!initializing) {
           //     PlaySystemSEsound(2, 0);
            //}
        }

    }

    void WidgetValueSpawnAmount5(int spawn_amount) {

        Entry5CallbackSpawnAmount = spawn_amount;
        //if (!initializing) {
        //    PlaySystemSEsound(2, 0);
       // }

    }

    void WidgetValueSpawnAmount4(int spawn_amount) {

        Entry4CallbackSpawnAmount = spawn_amount;
        //if (!initializing) {
        //    PlaySystemSEsound(2, 0);
        //}

    }

    void WidgetValueSpawnAmount3(int spawn_amount) {

        Entry3CallbackSpawnAmount = spawn_amount;
        //if (!initializing) {
         //   PlaySystemSEsound(2, 0);
        //}

    }

    void WidgetValueSpawnAmount2(int spawn_amount) {

        Entry2CallbackSpawnAmount = spawn_amount;
        // if (!initializing) {
       //     PlaySystemSEsound(2, 0);
       // }

    }

    void WidgetValueSpawnAmount(int spawn_amount) {

        Entry1CallbackSpawnAmount = spawn_amount;
        // if (!initializing) {
       //     PlaySystemSEsound(2, 0);
       // }

    }

    void OnConfirmActorSpawn(unsigned int* arg) {
        uintptr_t FreeYWwidgetFunctionAddress = FindAndPrintPatternW(Patterns::pattern_for_FreeYWwindowWidget, L"FREE YW WINDOW WIDGET", 0);
        // uintptr_t FreeYWbuttonFunctionAddress = FindAndPrintPatternW(Patterns::pattern_for_FreeYWbuttonWidget, L"FREE YW BUTTON WIDGET", 0);
       //  uintptr_t FreeYWlistFunctionAddress = FindAndPrintPatternW(Patterns::pattern_for_FreeYWlistWidget, L"FREE YW LIST WIDGET", 0);
        // uintptr_t FreeYWframeFunctionAddress = FindAndPrintPatternW(Patterns::pattern_for_FreeYWframeWidget, L"FREE YW FRAME WIDGET", 0);

        FunctionCalls::tFreeYWwindowwidget FreeYWwidgetFunction = reinterpret_cast<FunctionCalls::tFreeYWwindowwidget>(FreeYWwidgetFunctionAddress);
        // FunctionCalls::tFreeYWbuttonwidget FreeYWbuttonFunction = reinterpret_cast<FunctionCalls::tFreeYWbuttonwidget>(FreeYWbuttonFunctionAddress);
         //FunctionCalls::tFreeYWlistwidget FreeYWlistFunction = reinterpret_cast<FunctionCalls::tFreeYWlistwidget>(FreeYWlistFunctionAddress);
         //FunctionCalls::tFreeYWframewidget FreeYWframeFunction = reinterpret_cast<FunctionCalls::tFreeYWframewidget>(FreeYWframeFunctionAddress);
         // Unpack the parameters from the passed unsigned int* (cast it to SpawnParams)
        SpawnParams* params = reinterpret_cast<SpawnParams*>(arg);
        //SpawnAmountDialMem
        int spawnAmount = GetDialValueFunction((YW::DIAL_3*)SpawnAmountDialMem);
        // Use the unpacked parameters
        OBJENTRY_ID objId = params->objId;
        // int spawnAmount = params->spawnAmount;

         // Process the spawn action
        if (objId != (OBJENTRY_ID)-1) {
            NewActor(objId, spawnAmount);
            LOG(L"Spawned %d Actor objects of ID: %d\n", spawnAmount, objId);
        }
        else {
            //LOG(L"Invalid OBJENTRY_ID\n");
        }

        // Clean up the allocated memory (optional, depending on the memory management strategy)
        //delete params;
        FreeYWwidgetFunction((YW::WIDGET_5*)ActorInputMenu, 1);
        ActorInputMenu = nullptr;
    }

    void ActorSpawnAmountDialWidgetUpdateCallback(int newValue) {
        WidgetSpawnAmountValue = newValue;  // Update the spawn amount
        LOG(L"Updated spawn amount: %d\n", WidgetSpawnAmountValue);
    }

    void ShowActorSpawnAmountDialog(const char* title, int min, int max, OBJENTRY_ID objId) {
        uintptr_t OpenSingletonWindowFunctionAddress = ReadLEA32(Patterns::pattern_for_OpenSingletonWindow, L"OpenSingletonWindow", 0, 1, 5);
        uintptr_t AllocYWMemoryFunctionAddress = ReadLEA32(Patterns::pattern_for_Alloc_YW_Memory, L"AllocYWMemory", 0, 1, 5);
        uintptr_t ContainerLinkFunctionAddress = FindAndPrintPatternW(Patterns::pattern_for_ContainerLink, L"ContainerLink", 0);
        uintptr_t ShowDebugWindowFunctionAddress = ReadLEA32(Patterns::pattern_for_ShowDebugWindow, L"ShowDebugWindow", 0, 1, 5);
        uintptr_t ShowDialWidgetFunctionAddress = ReadLEA32(Patterns::pattern_for_DialWidget, L"Dial Widget", 0, 1, 5);
        uintptr_t YWLabelFunctionAddress = ReadLEA32(Patterns::pattern_for_YWLabel, L"YWLabel", 0, 1, 5);
        uintptr_t YWButtonFunctionAddress = ReadLEA32(Patterns::pattern_for_YWButton, L"YWButton", 0, 1, 5);
        uintptr_t YWWindowFunctionAddress = ReadLEA32(Patterns::pattern_for_YWwindow, L"YW Window", 0, 1, 5);

        FunctionCalls::tYWWindow YWWindowFunction = reinterpret_cast<FunctionCalls::tYWWindow>(YWWindowFunctionAddress);
        FunctionCalls::tAlloc_YW_Memory AllocYWMemoryFunction = reinterpret_cast<FunctionCalls::tAlloc_YW_Memory>(AllocYWMemoryFunctionAddress);
        FunctionCalls::tContainerLink ContainerLinkFunction = reinterpret_cast<FunctionCalls::tContainerLink>(ContainerLinkFunctionAddress);
        FunctionCalls::tShowDebugWindow ShowDebugWindowFunction = reinterpret_cast<FunctionCalls::tShowDebugWindow>(ShowDebugWindowFunctionAddress);
        FunctionCalls::tDialWidget ShowDialWidgetFunction = reinterpret_cast<FunctionCalls::tDialWidget>(ShowDialWidgetFunctionAddress);
        FunctionCalls::tYWLabel YWLabelFunction = reinterpret_cast<FunctionCalls::tYWLabel>(YWLabelFunctionAddress);
        FunctionCalls::tYWButton YWButtonFunction = reinterpret_cast<FunctionCalls::tYWButton>(YWButtonFunctionAddress);
        FunctionCalls::tOpenSingletonWindow OpenSingletonWindowFunction = reinterpret_cast<FunctionCalls::tOpenSingletonWindow>(OpenSingletonWindowFunctionAddress);



        // Create the window
        if (ActorInputMenu == nullptr) {
            //YWWindowFunction((YW::WINDOW*)WindowMem, title, x, y, w, h, option);
            unsigned int* labelMem = AllocYWMemoryFunction((YW::MEMORY*)0x200);

            unsigned int* buttonMem = AllocYWMemoryFunction((YW::MEMORY*)0x200);
            unsigned int* WindowMem = AllocYWMemoryFunction((YW::MEMORY*)0x200);
            SpawnAmountDialMem = AllocYWMemoryFunction((YW::MEMORY*)0x200);


            // Set up window parameters
            int x = 50;   // X position of the window
            int y = 50;   // Y position of the window
            int w = 200;  // Width of the window
            int h = 150;  // Height of the window
            u_int option = 2;  // Set the options, such as a flag (0 if no specific option is needed)


            ActorInputMenu = OpenSingletonWindowFunction(title, w, h, 1);



            //InputMenu = reinterpret_cast<YW::WINDOW*>(WindowMem);  // Set InputMenu to the correct pointer

            // Create and position label widget
            int labelX = 10;  // Position of label inside window
            int labelY = 10;
            int labelWidth = w - 20;  // Adjust width to fit inside the window
            int labelHeight = 20;
            YWLabelFunction((YW::LABEL_3*)labelMem, title, labelX, labelY, labelWidth, labelHeight);

            // Create and position dial widget
            int dialX = 10;  // Position dial widget inside the window
            int dialY = 40;
            int dialWidth = w - 20;  // Adjust width to fit inside the window
            int dialHeight = 20;
            ShowDialWidgetFunction((YW::DIAL_3*)SpawnAmountDialMem, dialX, dialY, dialWidth, dialHeight, min, max, ActorSpawnAmountDialWidgetUpdateCallback);

            // Create and position button widget
            int buttonX = 10;  // Position button inside the window
            int buttonY = h - 40;
            int buttonWidth = w - 20;  // Adjust width to fit inside the window
            int buttonHeight = 30;
            SpawnParams* params = new SpawnParams{ objId, WidgetSpawnAmountValue };
            YWButtonFunction((YW::BUTTON_3*)buttonMem, "OK", buttonX, buttonY, buttonWidth, buttonHeight, OnConfirmActorSpawn, reinterpret_cast<unsigned int*>(params));

            /*CreateMyCustomButton(
                (YW::BUTTON*)buttonMem,
                "OK",
                buttonX, buttonY, buttonWidth, buttonHeight,
                OnConfirmSpawn,
                reinterpret_cast<unsigned int*>(params)
            );*/



            // Link widgets to the window
            ContainerLinkFunction((YW::CONTAINER*)ActorInputMenu, (YW::WIDGET*)labelMem);
            ContainerLinkFunction((YW::CONTAINER*)ActorInputMenu, (YW::WIDGET*)SpawnAmountDialMem);
            ContainerLinkFunction((YW::CONTAINER*)ActorInputMenu, (YW::WIDGET*)buttonMem);

            // Show the input menu
            ShowDebugWindowFunction((YW::WIDGET*)ActorInputMenu);
        }
    }


    void OnConfirmYesActor(unsigned int* arg) {
        uintptr_t FreeYWwidgetFunctionAddress = FindAndPrintPatternW(Patterns::pattern_for_FreeYWwindowWidget, L"FREE YW WINDOW WIDGET", 0);
        // uintptr_t FreeYWbuttonFunctionAddress = FindAndPrintPatternW(Patterns::pattern_for_FreeYWbuttonWidget, L"FREE YW BUTTON WIDGET", 0);
       //  uintptr_t FreeYWlistFunctionAddress = FindAndPrintPatternW(Patterns::pattern_for_FreeYWlistWidget, L"FREE YW LIST WIDGET", 0);
        // uintptr_t FreeYWframeFunctionAddress = FindAndPrintPatternW(Patterns::pattern_for_FreeYWframeWidget, L"FREE YW FRAME WIDGET", 0);

        FunctionCalls::tFreeYWwindowwidget FreeYWwidgetFunction = reinterpret_cast<FunctionCalls::tFreeYWwindowwidget>(FreeYWwidgetFunctionAddress);
        // FunctionCalls::tFreeYWbuttonwidget FreeYWbuttonFunction = reinterpret_cast<FunctionCalls::tFreeYWbuttonwidget>(FreeYWbuttonFunctionAddress);
         //FunctionCalls::tFreeYWlistwidget FreeYWlistFunction = reinterpret_cast<FunctionCalls::tFreeYWlistwidget>(FreeYWlistFunctionAddress);
         //FunctionCalls::tFreeYWframewidget FreeYWframeFunction = reinterpret_cast<FunctionCalls::tFreeYWframewidget>(FreeYWframeFunctionAddress);
         // Unpack the parameters from the passed unsigned int* (cast it to SpawnParams)
        //SpawnParams* params = reinterpret_cast<SpawnParams*>(arg);
        //SpawnAmountDialMem
        //int spawnAmount = GetDialValueFunction((YW::DIAL_3*)SpawnAmountDialMem);
        // Use the unpacked parameters
       // OBJENTRY_ID objId = params->objId;
        // int spawnAmount = params->spawnAmount;

         // Process the spawn action
       // if (objId != (OBJENTRY_ID)-1) {
        //    NewObj(objId, spawnAmount);
        //    LOG(L"Spawned %d objects of ID: %d\n", spawnAmount, objId);
       // }
      //  else {
            //LOG(L"Invalid OBJENTRY_ID\n");
      //  }

        // Clean up the allocated memory (optional, depending on the memory management strategy)
        //delete params;
        FreeYWwidgetFunction((YW::WIDGET_5*)YesNoMenu, 1);
        YesNoMenu = nullptr;
    }

    void ShowYesNoDialog(const char* name, const char* message, void (*yesCallback)(const char*, int, int, OBJENTRY_ID), void (*noCallback)(unsigned int*)) {
        uintptr_t OpenSingletonWindowFunctionAddress = ReadLEA32(Patterns::pattern_for_OpenSingletonWindow, L"OpenSingletonWindow", 0, 1, 5);
        uintptr_t AllocYWMemoryFunctionAddress = ReadLEA32(Patterns::pattern_for_Alloc_YW_Memory, L"AllocYWMemory", 0, 1, 5);
        uintptr_t ContainerLinkFunctionAddress = FindAndPrintPatternW(Patterns::pattern_for_ContainerLink, L"ContainerLink", 0);
        uintptr_t ShowDebugWindowFunctionAddress = ReadLEA32(Patterns::pattern_for_ShowDebugWindow, L"ShowDebugWindow", 0, 1, 5);
        uintptr_t ShowDialWidgetFunctionAddress = ReadLEA32(Patterns::pattern_for_DialWidget, L"Dial Widget", 0, 1, 5);
        uintptr_t YWLabelFunctionAddress = ReadLEA32(Patterns::pattern_for_YWLabel, L"YWLabel", 0, 1, 5);
        uintptr_t YWButtonFunctionAddress = ReadLEA32(Patterns::pattern_for_YWButton, L"YWButton", 0, 1, 5);
        uintptr_t YWWindowFunctionAddress = ReadLEA32(Patterns::pattern_for_YWwindow, L"YW Window", 0, 1, 5);

        FunctionCalls::tYWWindow YWWindowFunction = reinterpret_cast<FunctionCalls::tYWWindow>(YWWindowFunctionAddress);
        FunctionCalls::tAlloc_YW_Memory AllocYWMemoryFunction = reinterpret_cast<FunctionCalls::tAlloc_YW_Memory>(AllocYWMemoryFunctionAddress);
        FunctionCalls::tContainerLink ContainerLinkFunction = reinterpret_cast<FunctionCalls::tContainerLink>(ContainerLinkFunctionAddress);
        FunctionCalls::tShowDebugWindow ShowDebugWindowFunction = reinterpret_cast<FunctionCalls::tShowDebugWindow>(ShowDebugWindowFunctionAddress);
        FunctionCalls::tDialWidget ShowDialWidgetFunction = reinterpret_cast<FunctionCalls::tDialWidget>(ShowDialWidgetFunctionAddress);
        FunctionCalls::tYWLabel YWLabelFunction = reinterpret_cast<FunctionCalls::tYWLabel>(YWLabelFunctionAddress);
        FunctionCalls::tYWButton YWButtonFunction = reinterpret_cast<FunctionCalls::tYWButton>(YWButtonFunctionAddress);
        FunctionCalls::tOpenSingletonWindow OpenSingletonWindowFunction = reinterpret_cast<FunctionCalls::tOpenSingletonWindow>(OpenSingletonWindowFunctionAddress);




        // Create the window
        if (YesNoMenu == nullptr) {
            //YWWindowFunction((YW::WINDOW*)WindowMem, title, x, y, w, h, option);
            unsigned int* labelMem = AllocYWMemoryFunction((YW::MEMORY*)0x200);

            unsigned int* YESbuttonMem = AllocYWMemoryFunction((YW::MEMORY*)0x200);
            unsigned int* NObuttonMem = AllocYWMemoryFunction((YW::MEMORY*)0x200);
            unsigned int* WindowMem = AllocYWMemoryFunction((YW::MEMORY*)0x200);
            //SpawnAmountDialMem = AllocYWMemoryFunction((YW::MEMORY*)0x200);


            // Set up window parameters
            int x = 50;   // X position of the window
            int y = 50;   // Y position of the window
            int w = 200;  // Width of the window
            int h = 80;  // Height of the window
            u_int option = 2;  // Set the options, such as a flag (0 if no specific option is needed)


            YesNoMenu = OpenSingletonWindowFunction(name, w, h, 2);



            //InputMenu = reinterpret_cast<YW::WINDOW*>(WindowMem);  // Set InputMenu to the correct pointer

            // Create and position label widget
            int labelX = 20;  // Position of label inside window
            int labelY = 20;
            int labelWidth = FontGetTextWidthFunction(message);
            int labelHeight = 12;
            YWLabelFunction((YW::LABEL_3*)labelMem, message, labelX, labelY, labelWidth, labelHeight);

            // Create and position button widget
            int buttonX = 16;  // Position button inside the window
            int buttonY = -32;
            int buttonWidth = 80;  // Adjust width to fit inside the window
            int buttonHeight = 12;
            //SpawnParams* params = new SpawnParams{ objId, WidgetSpawnAmountValue };
           // YWButtonFunction((YW::BUTTON_3*)YESbuttonMem, "yes", buttonX, buttonY, buttonWidth, buttonHeight, yesCallback, YESbuttonMem);

           // YWButtonFunction((YW::BUTTON_3*)NObuttonMem, "no", -96, -32, 80, 12, noCallback, NObuttonMem);

            /*CreateMyCustomButton(
                (YW::BUTTON*)buttonMem,
                "OK",
                buttonX, buttonY, buttonWidth, buttonHeight,
                OnConfirmSpawn,
                reinterpret_cast<unsigned int*>(params)
            );*/



            // Link widgets to the window
            ContainerLinkFunction((YW::CONTAINER*)YesNoMenu, (YW::WIDGET*)labelMem);
           // ContainerLinkFunction((YW::CONTAINER*)InputMenu, (YW::WIDGET*)SpawnAmountDialMem);
            ContainerLinkFunction((YW::CONTAINER*)YesNoMenu, (YW::WIDGET*)YESbuttonMem);
            ContainerLinkFunction((YW::CONTAINER*)YesNoMenu, (YW::WIDGET*)NObuttonMem);

            // Show the input menu
            ShowDebugWindowFunction((YW::WIDGET*)YesNoMenu);
        }
    }


    void OnConfirmSpawn(unsigned int* arg) {
        uintptr_t FreeYWwidgetFunctionAddress = FindAndPrintPatternW(Patterns::pattern_for_FreeYWwindowWidget, L"FREE YW WINDOW WIDGET", 0);
       // uintptr_t FreeYWbuttonFunctionAddress = FindAndPrintPatternW(Patterns::pattern_for_FreeYWbuttonWidget, L"FREE YW BUTTON WIDGET", 0);
      //  uintptr_t FreeYWlistFunctionAddress = FindAndPrintPatternW(Patterns::pattern_for_FreeYWlistWidget, L"FREE YW LIST WIDGET", 0);
       // uintptr_t FreeYWframeFunctionAddress = FindAndPrintPatternW(Patterns::pattern_for_FreeYWframeWidget, L"FREE YW FRAME WIDGET", 0);

        FunctionCalls::tFreeYWwindowwidget FreeYWwidgetFunction = reinterpret_cast<FunctionCalls::tFreeYWwindowwidget>(FreeYWwidgetFunctionAddress);
       // FunctionCalls::tFreeYWbuttonwidget FreeYWbuttonFunction = reinterpret_cast<FunctionCalls::tFreeYWbuttonwidget>(FreeYWbuttonFunctionAddress);
        //FunctionCalls::tFreeYWlistwidget FreeYWlistFunction = reinterpret_cast<FunctionCalls::tFreeYWlistwidget>(FreeYWlistFunctionAddress);
        //FunctionCalls::tFreeYWframewidget FreeYWframeFunction = reinterpret_cast<FunctionCalls::tFreeYWframewidget>(FreeYWframeFunctionAddress);
        // Unpack the parameters from the passed unsigned int* (cast it to SpawnParams)
        SpawnParams* params = reinterpret_cast<SpawnParams*>(arg);
        //SpawnAmountDialMem
        int spawnAmount = GetDialValueFunction((YW::DIAL_3*)SpawnAmountDialMem);
        // Use the unpacked parameters
        OBJENTRY_ID objId = params->objId;
       // int spawnAmount = params->spawnAmount;

        // Process the spawn action
        if (objId != (OBJENTRY_ID)-1) {
            NewObj(objId, spawnAmount);
            LOG(L"Spawned %d objects of ID: %d\n", spawnAmount, objId);
        }
        else {
            //LOG(L"Invalid OBJENTRY_ID\n");
        }

        // Clean up the allocated memory (optional, depending on the memory management strategy)
        //delete params;
        FreeYWwidgetFunction((YW::WIDGET_5*)InputMenu, 1);
        InputMenu = nullptr;
    }

    void DialWidgetUpdateCallback(int newValue) {
        WidgetSpawnAmountValue = newValue;  // Update the spawn amount
        LOG(L"Updated spawn amount: %d\n", WidgetSpawnAmountValue);
    }

    void ShowSpawnAmountDialog(const char* title, int min, int max, OBJENTRY_ID objId) {
        uintptr_t OpenSingletonWindowFunctionAddress = ReadLEA32(Patterns::pattern_for_OpenSingletonWindow, L"OpenSingletonWindow", 0, 1, 5);
        uintptr_t AllocYWMemoryFunctionAddress = ReadLEA32(Patterns::pattern_for_Alloc_YW_Memory, L"AllocYWMemory", 0, 1, 5);
        uintptr_t ContainerLinkFunctionAddress = FindAndPrintPatternW(Patterns::pattern_for_ContainerLink, L"ContainerLink", 0);
        uintptr_t ShowDebugWindowFunctionAddress = ReadLEA32(Patterns::pattern_for_ShowDebugWindow, L"ShowDebugWindow", 0, 1, 5);
        uintptr_t ShowDialWidgetFunctionAddress = ReadLEA32(Patterns::pattern_for_DialWidget, L"Dial Widget", 0, 1, 5);
        uintptr_t YWLabelFunctionAddress = ReadLEA32(Patterns::pattern_for_YWLabel, L"YWLabel", 0, 1, 5);
        uintptr_t YWButtonFunctionAddress = ReadLEA32(Patterns::pattern_for_YWButton, L"YWButton", 0, 1, 5);
        uintptr_t YWWindowFunctionAddress = ReadLEA32(Patterns::pattern_for_YWwindow, L"YW Window", 0, 1, 5);

        FunctionCalls::tYWWindow YWWindowFunction = reinterpret_cast<FunctionCalls::tYWWindow>(YWWindowFunctionAddress);
        FunctionCalls::tAlloc_YW_Memory AllocYWMemoryFunction = reinterpret_cast<FunctionCalls::tAlloc_YW_Memory>(AllocYWMemoryFunctionAddress);
        FunctionCalls::tContainerLink ContainerLinkFunction = reinterpret_cast<FunctionCalls::tContainerLink>(ContainerLinkFunctionAddress);
        FunctionCalls::tShowDebugWindow ShowDebugWindowFunction = reinterpret_cast<FunctionCalls::tShowDebugWindow>(ShowDebugWindowFunctionAddress);
        FunctionCalls::tDialWidget ShowDialWidgetFunction = reinterpret_cast<FunctionCalls::tDialWidget>(ShowDialWidgetFunctionAddress);
        FunctionCalls::tYWLabel YWLabelFunction = reinterpret_cast<FunctionCalls::tYWLabel>(YWLabelFunctionAddress);
        FunctionCalls::tYWButton YWButtonFunction = reinterpret_cast<FunctionCalls::tYWButton>(YWButtonFunctionAddress);
        FunctionCalls::tOpenSingletonWindow OpenSingletonWindowFunction = reinterpret_cast<FunctionCalls::tOpenSingletonWindow>(OpenSingletonWindowFunctionAddress);

        

        // Create the window
        if (InputMenu == nullptr) {
            //YWWindowFunction((YW::WINDOW*)WindowMem, title, x, y, w, h, option);
            unsigned int* labelMem = AllocYWMemoryFunction((YW::MEMORY*)0x200);

            unsigned int* buttonMem = AllocYWMemoryFunction((YW::MEMORY*)0x200);
            unsigned int* WindowMem = AllocYWMemoryFunction((YW::MEMORY*)0x200);
            SpawnAmountDialMem = AllocYWMemoryFunction((YW::MEMORY*)0x200);


            // Set up window parameters
            int x = 50;   // X position of the window
            int y = 50;   // Y position of the window
            int w = 200;  // Width of the window
            int h = 150;  // Height of the window
            u_int option = 2;  // Set the options, such as a flag (0 if no specific option is needed)


            InputMenu = OpenSingletonWindowFunction(title, w, h, 1);



            //InputMenu = reinterpret_cast<YW::WINDOW*>(WindowMem);  // Set InputMenu to the correct pointer

            // Create and position label widget
            int labelX = 10;  // Position of label inside window
            int labelY = 10;
            int labelWidth = w - 20;  // Adjust width to fit inside the window
            int labelHeight = 20;
            YWLabelFunction((YW::LABEL_3*)labelMem, title, labelX, labelY, labelWidth, labelHeight);

            // Create and position dial widget
            int dialX = 10;  // Position dial widget inside the window
            int dialY = 40;
            int dialWidth = w - 20;  // Adjust width to fit inside the window
            int dialHeight = 20;
            ShowDialWidgetFunction((YW::DIAL_3*)SpawnAmountDialMem, dialX, dialY, dialWidth, dialHeight, min, max, DialWidgetUpdateCallback);

            // Create and position button widget
            int buttonX = 10;  // Position button inside the window
            int buttonY = h - 40;
            int buttonWidth = w - 20;  // Adjust width to fit inside the window
            int buttonHeight = 30;
            SpawnParams* params = new SpawnParams{ objId, WidgetSpawnAmountValue };
            YWButtonFunction((YW::BUTTON_3*)buttonMem, "OK", buttonX, buttonY, buttonWidth, buttonHeight, OnConfirmSpawn, reinterpret_cast<unsigned int*>(params));

            /*CreateMyCustomButton(
                (YW::BUTTON*)buttonMem,
                "OK",
                buttonX, buttonY, buttonWidth, buttonHeight,
                OnConfirmSpawn,
                reinterpret_cast<unsigned int*>(params)
            );*/



            // Link widgets to the window
            ContainerLinkFunction((YW::CONTAINER*)InputMenu, (YW::WIDGET*)labelMem);
            ContainerLinkFunction((YW::CONTAINER*)InputMenu, (YW::WIDGET*)SpawnAmountDialMem);
            ContainerLinkFunction((YW::CONTAINER*)InputMenu, (YW::WIDGET*)buttonMem);

            // Show the input menu
            ShowDebugWindowFunction((YW::WIDGET*)InputMenu);
        }
    }


    void ShowOBJEntryWindow(unsigned int* arg) {

        
        uintptr_t OpenSingletonWindowFunctionAddress = ReadLEA32(Patterns::pattern_for_OpenSingletonWindow, L"OpenSingletonWindow", 0, 1, 5);
        uintptr_t AllocYWMemoryFunctionAddress = ReadLEA32(Patterns::pattern_for_Alloc_YW_Memory, L"AllocYWMemory", 0, 1, 5);
        uintptr_t ContainerLinkFunctionAddress = FindAndPrintPatternW(Patterns::pattern_for_ContainerLink, L"ContainerLink", 0);
        uintptr_t ShowDebugWindowFunctionAddress = ReadLEA32(Patterns::pattern_for_ShowDebugWindow, L"ShowDebugWindow", 0, 1, 5);
        uintptr_t ShowDialWidgetFunctionAddress = ReadLEA32(Patterns::pattern_for_DialWidget, L"Dial Widget", 0, 1, 5);
        uintptr_t YWToggleButtonFunctionAddress = ReadLEA32(Patterns::pattern_for_YWToggleButton, L"YWToggleButton", 0, 1, 5);
        uintptr_t SetDialValueFunctionAddress = FindAndPrintPatternW(Patterns::pattern_for_SetDialValue, L"Set DialValue", 0);
        uintptr_t YWLabelFunctionAddress = ReadLEA32(Patterns::pattern_for_YWLabel, L"YWlabel", 0, 1, 5);
        uintptr_t YWButtonFunctionAddress = ReadLEA32(Patterns::pattern_for_YWButton, L"YWButton", 0, 1, 5);


        FunctionCalls::tAlloc_YW_Memory AllocYWMemoryFunction = reinterpret_cast<FunctionCalls::tAlloc_YW_Memory>(AllocYWMemoryFunctionAddress);
        FunctionCalls::tContainerLink ContainerLinkFunction = reinterpret_cast<FunctionCalls::tContainerLink>(ContainerLinkFunctionAddress);
        FunctionCalls::tShowDebugWindow ShowDebugWindowFunction = reinterpret_cast<FunctionCalls::tShowDebugWindow>(ShowDebugWindowFunctionAddress);

        FunctionCalls::tOpenSingletonWindow OpenSingletonWindowFunction = reinterpret_cast<FunctionCalls::tOpenSingletonWindow>(OpenSingletonWindowFunctionAddress);
        FunctionCalls::tDialWidget ShowDialWidgetFunction = reinterpret_cast<FunctionCalls::tDialWidget>(ShowDialWidgetFunctionAddress);
        FunctionCalls::tYWToggleButton YWToggleButtonFunction = reinterpret_cast<FunctionCalls::tYWToggleButton>(YWToggleButtonFunctionAddress);
        FunctionCalls::tSetDialValue SetDialValueFunction = reinterpret_cast<FunctionCalls::tSetDialValue>(SetDialValueFunctionAddress);
        FunctionCalls::tYWLabel YWLabelFunction = reinterpret_cast<FunctionCalls::tYWLabel>(YWLabelFunctionAddress);
        FunctionCalls::tYWButton YWButtonFunction = reinterpret_cast<FunctionCalls::tYWButton>(YWButtonFunctionAddress);

        if (OBJENTRYMenu == nullptr) {
            // Allocate memory and create a new window
            OBJENTRYMenu = OpenSingletonWindowFunction("OBJENTRY", 520, 400, 2);

            const char* entryIDstring("First Entry ID:");
            const char* spawnAmountstring("First Entry Spawn Amount:");
            auto allocYWMemDial = AllocYWMemoryFunction((YW::MEMORY*)0x200);
            auto allocYWMemDial2 = AllocYWMemoryFunction((YW::MEMORY*)0x200);
            auto allocYWMemDial3 = AllocYWMemoryFunction((YW::MEMORY*)0x200);
            auto allocYWMemDial4 = AllocYWMemoryFunction((YW::MEMORY*)0x200);
            auto allocYWMemDial5 = AllocYWMemoryFunction((YW::MEMORY*)0x200);
            auto allocYWMemDial6 = AllocYWMemoryFunction((YW::MEMORY*)0x200);
            auto allocYWMemDial7 = AllocYWMemoryFunction((YW::MEMORY*)0x200);
            auto allocYWMemDial8 = AllocYWMemoryFunction((YW::MEMORY*)0x200);
            auto allocYWMemDial9 = AllocYWMemoryFunction((YW::MEMORY*)0x200);
            auto allocYWMemDial10 = AllocYWMemoryFunction((YW::MEMORY*)0x200);
            auto allocYWMemDial11 = AllocYWMemoryFunction((YW::MEMORY*)0x200);
            auto allocYWMemBtn = AllocYWMemoryFunction((YW::MEMORY*)0x200);
            auto allocYWMemLabel = AllocYWMemoryFunction((YW::MEMORY*)0x200);
            auto allocYWMemLabel2 = AllocYWMemoryFunction((YW::MEMORY*)0x200);
            auto allocYWMemLabel3 = AllocYWMemoryFunction((YW::MEMORY*)0x200);
            auto allocYWMemLabel4 = AllocYWMemoryFunction((YW::MEMORY*)0x200);
            auto allocYWMemLabel5 = AllocYWMemoryFunction((YW::MEMORY*)0x200);
            auto allocYWMemLabel6 = AllocYWMemoryFunction((YW::MEMORY*)0x200);
            auto allocYWMemLabel7 = AllocYWMemoryFunction((YW::MEMORY*)0x200);
            auto allocYWMemLabel8 = AllocYWMemoryFunction((YW::MEMORY*)0x200);
            auto allocYWMemLabel9 = AllocYWMemoryFunction((YW::MEMORY*)0x200);
            auto allocYWMemLabel10 = AllocYWMemoryFunction((YW::MEMORY*)0x200);
            auto allocYWMemLabel11 = AllocYWMemoryFunction((YW::MEMORY*)0x200);
            auto allocYWMemCheckBox1 = AllocYWMemoryFunction((YW::MEMORY*)0x200);
            auto allocYWMemCheckBox2 = AllocYWMemoryFunction((YW::MEMORY*)0x200);
            auto allocYWMemCheckBox3 = AllocYWMemoryFunction((YW::MEMORY*)0x200);
            auto allocYWMemCheckBox4 = AllocYWMemoryFunction((YW::MEMORY*)0x200);
            auto allocYWMemCheckBox5 = AllocYWMemoryFunction((YW::MEMORY*)0x200);
            auto allocYWMemCheckBox6 = AllocYWMemoryFunction((YW::MEMORY*)0x200);

            YWToggleButtonFunction((YW::TOGGLE_BUTTON_0*)allocYWMemCheckBox1, "1", 0, 330, 18, 7, IsEntry1Enabled, ToggleButton1CallBack, allocYWMemCheckBox1);
            YWToggleButtonFunction((YW::TOGGLE_BUTTON_0*)allocYWMemCheckBox2, "2", 21, 330, 18, 7, IsEntry2Enabled, ToggleButton2CallBack, allocYWMemCheckBox2);
            YWToggleButtonFunction((YW::TOGGLE_BUTTON_0*)allocYWMemCheckBox3, "3", 42, 330, 18, 7, IsEntry3Enabled, ToggleButton3CallBack, allocYWMemCheckBox3);
            YWToggleButtonFunction((YW::TOGGLE_BUTTON_0*)allocYWMemCheckBox4, "4", 63, 330, 18, 7, IsEntry4Enabled, ToggleButton4CallBack, allocYWMemCheckBox4);
            YWToggleButtonFunction((YW::TOGGLE_BUTTON_0*)allocYWMemCheckBox5, "5", 84, 330, 18, 7, IsEntry5Enabled, ToggleButton5CallBack, allocYWMemCheckBox5);
            YWToggleButtonFunction((YW::TOGGLE_BUTTON_0*)allocYWMemCheckBox6, "p", 105, 330, 18, 7, IsPlayerEntryEnabled, ToggleButtonPCallBack, allocYWMemCheckBox6);

            YWLabelFunction((YW::LABEL_3*)allocYWMemLabel, entryIDstring, 5, 10, 225, 12);
            YWLabelFunction((YW::LABEL_3*)allocYWMemLabel2, spawnAmountstring, 5, 40, 220, 12);
            YWLabelFunction((YW::LABEL_3*)allocYWMemLabel3, "Second Entry ID:", 5, 70, 225, 12);
            YWLabelFunction((YW::LABEL_3*)allocYWMemLabel4, "Second Entry Spawn Amount:", 5, 100, 225, 12);
            YWLabelFunction((YW::LABEL_3*)allocYWMemLabel5, "Third Entry ID:", 5, 130, 225, 12);
            YWLabelFunction((YW::LABEL_3*)allocYWMemLabel6, "Third Entry Spawn Amount:", 5, 160, 225, 12);
            YWLabelFunction((YW::LABEL_3*)allocYWMemLabel7, "Fourth Entry ID:", 5, 190, 225, 12);
            YWLabelFunction((YW::LABEL_3*)allocYWMemLabel8, "Fourth Entry Spawn Amount:", 5, 220, 225, 12);
            YWLabelFunction((YW::LABEL_3*)allocYWMemLabel9, "Fifth Entry ID:", 5, 250, 225, 12);
            YWLabelFunction((YW::LABEL_3*)allocYWMemLabel10, "Fifth Entry Spawn Amount:", 5, 280, 225, 12);
            YWLabelFunction((YW::LABEL_3*)allocYWMemLabel11, "Player Entry ID:", 5, 310, 225, 12);

            ShowDialWidgetFunction((YW::DIAL_3*)allocYWMemDial, 260, 10, 96, 16, 1, 2561, WidgetValue);
            ShowDialWidgetFunction((YW::DIAL_3*)allocYWMemDial2, 260, 40, 96, 16, 1, 50, WidgetValueSpawnAmount);
            ShowDialWidgetFunction((YW::DIAL_3*)allocYWMemDial3, 260, 70, 96, 16, 1, 2561, WidgetValue2);
            ShowDialWidgetFunction((YW::DIAL_3*)allocYWMemDial4, 260, 100, 96, 16, 1, 50, WidgetValueSpawnAmount2);
            ShowDialWidgetFunction((YW::DIAL_3*)allocYWMemDial5, 260, 130, 96, 16, 1, 2561, WidgetValue3);
            ShowDialWidgetFunction((YW::DIAL_3*)allocYWMemDial6, 260, 160, 96, 16, 1, 50, WidgetValueSpawnAmount3);
            ShowDialWidgetFunction((YW::DIAL_3*)allocYWMemDial7, 260, 190, 96, 16, 1, 2561, WidgetValue4);
            ShowDialWidgetFunction((YW::DIAL_3*)allocYWMemDial8, 260, 220, 96, 16, 1, 50, WidgetValueSpawnAmount4);
            ShowDialWidgetFunction((YW::DIAL_3*)allocYWMemDial9, 260, 250, 96, 16, 1, 2561, WidgetValue5);
            ShowDialWidgetFunction((YW::DIAL_3*)allocYWMemDial10, 260, 280, 96, 16, 1, 50, WidgetValueSpawnAmount5);
            ShowDialWidgetFunction((YW::DIAL_3*)allocYWMemDial11, 260, 310, 96, 16, 1, 2561, WidgetValuePlayer);

            SetDialValueFunction((YW::DIAL_3*)allocYWMemDial, Entry1CallbackID);
            SetDialValueFunction((YW::DIAL_3*)allocYWMemDial2, Entry1CallbackSpawnAmount);

            SetDialValueFunction((YW::DIAL_3*)allocYWMemDial3, Entry2CallbackID);
            SetDialValueFunction((YW::DIAL_3*)allocYWMemDial4, Entry2CallbackSpawnAmount);

            SetDialValueFunction((YW::DIAL_3*)allocYWMemDial5, Entry3CallbackID);
            SetDialValueFunction((YW::DIAL_3*)allocYWMemDial6, Entry3CallbackSpawnAmount);

            SetDialValueFunction((YW::DIAL_3*)allocYWMemDial7, Entry4CallbackID);
            SetDialValueFunction((YW::DIAL_3*)allocYWMemDial8, Entry4CallbackSpawnAmount);

            SetDialValueFunction((YW::DIAL_3*)allocYWMemDial9, Entry5CallbackID);
            SetDialValueFunction((YW::DIAL_3*)allocYWMemDial10, Entry5CallbackSpawnAmount);

            SetDialValueFunction((YW::DIAL_3*)allocYWMemDial11, PlayerEntryCallBackID);

            YWButtonFunction((YW::BUTTON_3*)allocYWMemBtn, "Spawn object(s)", 20, 350, 157, 13, OBJButtonCallback, allocYWMemBtn);

            if (OBJENTRYMenu) {
                ContainerLinkFunction((YW::CONTAINER*)OBJENTRYMenu, (YW::WIDGET*)allocYWMemBtn);
                ContainerLinkFunction((YW::CONTAINER*)OBJENTRYMenu, (YW::WIDGET*)allocYWMemDial);
                ContainerLinkFunction((YW::CONTAINER*)OBJENTRYMenu, (YW::WIDGET*)allocYWMemDial2);
                ContainerLinkFunction((YW::CONTAINER*)OBJENTRYMenu, (YW::WIDGET*)allocYWMemDial3);
                ContainerLinkFunction((YW::CONTAINER*)OBJENTRYMenu, (YW::WIDGET*)allocYWMemDial4);
                ContainerLinkFunction((YW::CONTAINER*)OBJENTRYMenu, (YW::WIDGET*)allocYWMemDial5);
                ContainerLinkFunction((YW::CONTAINER*)OBJENTRYMenu, (YW::WIDGET*)allocYWMemDial6);
                ContainerLinkFunction((YW::CONTAINER*)OBJENTRYMenu, (YW::WIDGET*)allocYWMemDial7);
                ContainerLinkFunction((YW::CONTAINER*)OBJENTRYMenu, (YW::WIDGET*)allocYWMemDial8);
                ContainerLinkFunction((YW::CONTAINER*)OBJENTRYMenu, (YW::WIDGET*)allocYWMemDial9);
                ContainerLinkFunction((YW::CONTAINER*)OBJENTRYMenu, (YW::WIDGET*)allocYWMemDial10);
                ContainerLinkFunction((YW::CONTAINER*)OBJENTRYMenu, (YW::WIDGET*)allocYWMemDial11);
                ContainerLinkFunction((YW::CONTAINER*)OBJENTRYMenu, (YW::WIDGET*)allocYWMemLabel);
                ContainerLinkFunction((YW::CONTAINER*)OBJENTRYMenu, (YW::WIDGET*)allocYWMemLabel2);
                ContainerLinkFunction((YW::CONTAINER*)OBJENTRYMenu, (YW::WIDGET*)allocYWMemLabel3);
                ContainerLinkFunction((YW::CONTAINER*)OBJENTRYMenu, (YW::WIDGET*)allocYWMemLabel4);
                ContainerLinkFunction((YW::CONTAINER*)OBJENTRYMenu, (YW::WIDGET*)allocYWMemLabel5);
                ContainerLinkFunction((YW::CONTAINER*)OBJENTRYMenu, (YW::WIDGET*)allocYWMemLabel6);
                ContainerLinkFunction((YW::CONTAINER*)OBJENTRYMenu, (YW::WIDGET*)allocYWMemLabel7);
                ContainerLinkFunction((YW::CONTAINER*)OBJENTRYMenu, (YW::WIDGET*)allocYWMemLabel8);
                ContainerLinkFunction((YW::CONTAINER*)OBJENTRYMenu, (YW::WIDGET*)allocYWMemLabel9);
                ContainerLinkFunction((YW::CONTAINER*)OBJENTRYMenu, (YW::WIDGET*)allocYWMemLabel10);
                ContainerLinkFunction((YW::CONTAINER*)OBJENTRYMenu, (YW::WIDGET*)allocYWMemLabel11);
                ContainerLinkFunction((YW::CONTAINER*)OBJENTRYMenu, (YW::WIDGET*)allocYWMemCheckBox1);
                ContainerLinkFunction((YW::CONTAINER*)OBJENTRYMenu, (YW::WIDGET*)allocYWMemCheckBox2);
                ContainerLinkFunction((YW::CONTAINER*)OBJENTRYMenu, (YW::WIDGET*)allocYWMemCheckBox3);
                ContainerLinkFunction((YW::CONTAINER*)OBJENTRYMenu, (YW::WIDGET*)allocYWMemCheckBox4);
                ContainerLinkFunction((YW::CONTAINER*)OBJENTRYMenu, (YW::WIDGET*)allocYWMemCheckBox5);
                ContainerLinkFunction((YW::CONTAINER*)OBJENTRYMenu, (YW::WIDGET*)allocYWMemCheckBox6);

                ShowDebugWindowFunction((YW::WIDGET*)OBJENTRYMenu);
                printf("OBJENTRYMENU is not NULL");
            }
            else {
                printf("Failed to create OBJENTRYMenu\n");
            }
        }
        else {
            OBJENTRYMenu = OpenSingletonWindowFunction("OBJENTRY", 520, 400, 2);


            const char* entryIDstring("First Entry ID:");
            const char* spawnAmountstring("First Entry Spawn Amount:");
            auto allocYWMemDial = AllocYWMemoryFunction((YW::MEMORY*)0x200);
            auto allocYWMemDial2 = AllocYWMemoryFunction((YW::MEMORY*)0x200);
            auto allocYWMemDial3 = AllocYWMemoryFunction((YW::MEMORY*)0x200);
            auto allocYWMemDial4 = AllocYWMemoryFunction((YW::MEMORY*)0x200);
            auto allocYWMemDial5 = AllocYWMemoryFunction((YW::MEMORY*)0x200);
            auto allocYWMemDial6 = AllocYWMemoryFunction((YW::MEMORY*)0x200);
            auto allocYWMemDial7 = AllocYWMemoryFunction((YW::MEMORY*)0x200);
            auto allocYWMemDial8 = AllocYWMemoryFunction((YW::MEMORY*)0x200);
            auto allocYWMemDial9 = AllocYWMemoryFunction((YW::MEMORY*)0x200);
            auto allocYWMemDial10 = AllocYWMemoryFunction((YW::MEMORY*)0x200);
            auto allocYWMemDial11 = AllocYWMemoryFunction((YW::MEMORY*)0x200);
            auto allocYWMemBtn = AllocYWMemoryFunction((YW::MEMORY*)0x200);
            auto allocYWMemLabel = AllocYWMemoryFunction((YW::MEMORY*)0x200);
            auto allocYWMemLabel2 = AllocYWMemoryFunction((YW::MEMORY*)0x200);
            auto allocYWMemLabel3 = AllocYWMemoryFunction((YW::MEMORY*)0x200);
            auto allocYWMemLabel4 = AllocYWMemoryFunction((YW::MEMORY*)0x200);
            auto allocYWMemLabel5 = AllocYWMemoryFunction((YW::MEMORY*)0x200);
            auto allocYWMemLabel6 = AllocYWMemoryFunction((YW::MEMORY*)0x200);
            auto allocYWMemLabel7 = AllocYWMemoryFunction((YW::MEMORY*)0x200);
            auto allocYWMemLabel8 = AllocYWMemoryFunction((YW::MEMORY*)0x200);
            auto allocYWMemLabel9 = AllocYWMemoryFunction((YW::MEMORY*)0x200);
            auto allocYWMemLabel10 = AllocYWMemoryFunction((YW::MEMORY*)0x200);
            auto allocYWMemLabel11 = AllocYWMemoryFunction((YW::MEMORY*)0x200);
            auto allocYWMemCheckBox1 = AllocYWMemoryFunction((YW::MEMORY*)0x200);
            auto allocYWMemCheckBox2 = AllocYWMemoryFunction((YW::MEMORY*)0x200);
            auto allocYWMemCheckBox3 = AllocYWMemoryFunction((YW::MEMORY*)0x200);
            auto allocYWMemCheckBox4 = AllocYWMemoryFunction((YW::MEMORY*)0x200);
            auto allocYWMemCheckBox5 = AllocYWMemoryFunction((YW::MEMORY*)0x200);
            auto allocYWMemCheckBox6 = AllocYWMemoryFunction((YW::MEMORY*)0x200);
 
            YWToggleButtonFunction((YW::TOGGLE_BUTTON_0*)allocYWMemCheckBox1, "1", 0, 330, 18, 7, IsEntry1Enabled, ToggleButton1CallBack, allocYWMemCheckBox1);
            YWToggleButtonFunction((YW::TOGGLE_BUTTON_0*)allocYWMemCheckBox2, "2", 21, 330, 18, 7, IsEntry2Enabled, ToggleButton2CallBack, allocYWMemCheckBox2);
            YWToggleButtonFunction((YW::TOGGLE_BUTTON_0*)allocYWMemCheckBox3, "3", 42, 330, 18, 7, IsEntry3Enabled, ToggleButton3CallBack, allocYWMemCheckBox3);
            YWToggleButtonFunction((YW::TOGGLE_BUTTON_0*)allocYWMemCheckBox4, "4", 63, 330, 18, 7, IsEntry4Enabled, ToggleButton4CallBack, allocYWMemCheckBox4);
            YWToggleButtonFunction((YW::TOGGLE_BUTTON_0*)allocYWMemCheckBox5, "5", 84, 330, 18, 7, IsEntry5Enabled, ToggleButton5CallBack, allocYWMemCheckBox5);
            YWToggleButtonFunction((YW::TOGGLE_BUTTON_0*)allocYWMemCheckBox6, "p", 105, 330, 18, 7, IsPlayerEntryEnabled, ToggleButtonPCallBack, allocYWMemCheckBox6);

            YWLabelFunction((YW::LABEL_3*)allocYWMemLabel, entryIDstring, 5, 10, 225, 12);
            YWLabelFunction((YW::LABEL_3*)allocYWMemLabel2, spawnAmountstring, 5, 40, 220, 12);
            YWLabelFunction((YW::LABEL_3*)allocYWMemLabel3, "Second Entry ID:", 5, 70, 225, 12);
            YWLabelFunction((YW::LABEL_3*)allocYWMemLabel4, "Second Entry Spawn Amount:", 5, 100, 225, 12);
            YWLabelFunction((YW::LABEL_3*)allocYWMemLabel5, "Third Entry ID:", 5, 130, 225, 12);
            YWLabelFunction((YW::LABEL_3*)allocYWMemLabel6, "Third Entry Spawn Amount:", 5, 160, 225, 12);
            YWLabelFunction((YW::LABEL_3*)allocYWMemLabel7, "Fourth Entry ID:", 5, 190, 225, 12);
            YWLabelFunction((YW::LABEL_3*)allocYWMemLabel8, "Fourth Entry Spawn Amount:", 5, 220, 225, 12);
            YWLabelFunction((YW::LABEL_3*)allocYWMemLabel9, "Fifth Entry ID:", 5, 250, 225, 12);
            YWLabelFunction((YW::LABEL_3*)allocYWMemLabel10, "Fifth Entry Spawn Amount:", 5, 280, 225, 12);
            YWLabelFunction((YW::LABEL_3*)allocYWMemLabel11, "Player Entry ID:", 5, 310, 225, 12);

            ShowDialWidgetFunction((YW::DIAL_3*)allocYWMemDial, 260, 10, 96, 16, 1, 2561, WidgetValue);
            ShowDialWidgetFunction((YW::DIAL_3*)allocYWMemDial2, 260, 40, 96, 16, 1, 50, WidgetValueSpawnAmount);
            ShowDialWidgetFunction((YW::DIAL_3*)allocYWMemDial3, 260, 70, 96, 16, 1, 2561, WidgetValue2);
            ShowDialWidgetFunction((YW::DIAL_3*)allocYWMemDial4, 260, 100, 96, 16, 1, 50, WidgetValueSpawnAmount2);
            ShowDialWidgetFunction((YW::DIAL_3*)allocYWMemDial5, 260, 130, 96, 16, 1, 2561, WidgetValue3);
            ShowDialWidgetFunction((YW::DIAL_3*)allocYWMemDial6, 260, 160, 96, 16, 1, 50, WidgetValueSpawnAmount3);
            ShowDialWidgetFunction((YW::DIAL_3*)allocYWMemDial7, 260, 190, 96, 16, 1, 2561, WidgetValue4);
            ShowDialWidgetFunction((YW::DIAL_3*)allocYWMemDial8, 260, 220, 96, 16, 1, 50, WidgetValueSpawnAmount4);
            ShowDialWidgetFunction((YW::DIAL_3*)allocYWMemDial9, 260, 250, 96, 16, 1, 2561, WidgetValue5);
            ShowDialWidgetFunction((YW::DIAL_3*)allocYWMemDial10, 260, 280, 96, 16, 1, 50, WidgetValueSpawnAmount5);
            ShowDialWidgetFunction((YW::DIAL_3*)allocYWMemDial11, 260, 310, 96, 16, 1, 2561, WidgetValuePlayer);

            SetDialValueFunction((YW::DIAL_3*)allocYWMemDial, Entry1CallbackID);
            SetDialValueFunction((YW::DIAL_3*)allocYWMemDial2, Entry1CallbackSpawnAmount);

            SetDialValueFunction((YW::DIAL_3*)allocYWMemDial3, Entry2CallbackID);
            SetDialValueFunction((YW::DIAL_3*)allocYWMemDial4, Entry2CallbackSpawnAmount);

            SetDialValueFunction((YW::DIAL_3*)allocYWMemDial5, Entry3CallbackID);
            SetDialValueFunction((YW::DIAL_3*)allocYWMemDial6, Entry3CallbackSpawnAmount);

            SetDialValueFunction((YW::DIAL_3*)allocYWMemDial7, Entry4CallbackID);
            SetDialValueFunction((YW::DIAL_3*)allocYWMemDial8, Entry4CallbackSpawnAmount);

            SetDialValueFunction((YW::DIAL_3*)allocYWMemDial9, Entry5CallbackID);
            SetDialValueFunction((YW::DIAL_3*)allocYWMemDial10, Entry5CallbackSpawnAmount);

            SetDialValueFunction((YW::DIAL_3*)allocYWMemDial11, PlayerEntryCallBackID);

            YWButtonFunction((YW::BUTTON_3*)allocYWMemBtn, "Spawn object(s)", 20, 350, 157, 13, OBJButtonCallback, allocYWMemBtn);

            if (OBJENTRYMenu) {
                ContainerLinkFunction((YW::CONTAINER*)OBJENTRYMenu, (YW::WIDGET*)allocYWMemBtn);
                ContainerLinkFunction((YW::CONTAINER*)OBJENTRYMenu, (YW::WIDGET*)allocYWMemDial);
                ContainerLinkFunction((YW::CONTAINER*)OBJENTRYMenu, (YW::WIDGET*)allocYWMemDial2);
                ContainerLinkFunction((YW::CONTAINER*)OBJENTRYMenu, (YW::WIDGET*)allocYWMemDial3);
                ContainerLinkFunction((YW::CONTAINER*)OBJENTRYMenu, (YW::WIDGET*)allocYWMemDial4);
                ContainerLinkFunction((YW::CONTAINER*)OBJENTRYMenu, (YW::WIDGET*)allocYWMemDial5);
                ContainerLinkFunction((YW::CONTAINER*)OBJENTRYMenu, (YW::WIDGET*)allocYWMemDial6);
                ContainerLinkFunction((YW::CONTAINER*)OBJENTRYMenu, (YW::WIDGET*)allocYWMemDial7);
                ContainerLinkFunction((YW::CONTAINER*)OBJENTRYMenu, (YW::WIDGET*)allocYWMemDial8);
                ContainerLinkFunction((YW::CONTAINER*)OBJENTRYMenu, (YW::WIDGET*)allocYWMemDial9);
                ContainerLinkFunction((YW::CONTAINER*)OBJENTRYMenu, (YW::WIDGET*)allocYWMemDial10);
                ContainerLinkFunction((YW::CONTAINER*)OBJENTRYMenu, (YW::WIDGET*)allocYWMemDial11);
                ContainerLinkFunction((YW::CONTAINER*)OBJENTRYMenu, (YW::WIDGET*)allocYWMemLabel);
                ContainerLinkFunction((YW::CONTAINER*)OBJENTRYMenu, (YW::WIDGET*)allocYWMemLabel2);
                ContainerLinkFunction((YW::CONTAINER*)OBJENTRYMenu, (YW::WIDGET*)allocYWMemLabel3);
                ContainerLinkFunction((YW::CONTAINER*)OBJENTRYMenu, (YW::WIDGET*)allocYWMemLabel4);
                ContainerLinkFunction((YW::CONTAINER*)OBJENTRYMenu, (YW::WIDGET*)allocYWMemLabel5);
                ContainerLinkFunction((YW::CONTAINER*)OBJENTRYMenu, (YW::WIDGET*)allocYWMemLabel6);
                ContainerLinkFunction((YW::CONTAINER*)OBJENTRYMenu, (YW::WIDGET*)allocYWMemLabel7);
                ContainerLinkFunction((YW::CONTAINER*)OBJENTRYMenu, (YW::WIDGET*)allocYWMemLabel8);
                ContainerLinkFunction((YW::CONTAINER*)OBJENTRYMenu, (YW::WIDGET*)allocYWMemLabel9);
                ContainerLinkFunction((YW::CONTAINER*)OBJENTRYMenu, (YW::WIDGET*)allocYWMemLabel10);
                ContainerLinkFunction((YW::CONTAINER*)OBJENTRYMenu, (YW::WIDGET*)allocYWMemLabel11);
                ContainerLinkFunction((YW::CONTAINER*)OBJENTRYMenu, (YW::WIDGET*)allocYWMemCheckBox1);
                ContainerLinkFunction((YW::CONTAINER*)OBJENTRYMenu, (YW::WIDGET*)allocYWMemCheckBox2);
                ContainerLinkFunction((YW::CONTAINER*)OBJENTRYMenu, (YW::WIDGET*)allocYWMemCheckBox3);
                ContainerLinkFunction((YW::CONTAINER*)OBJENTRYMenu, (YW::WIDGET*)allocYWMemCheckBox4);
                ContainerLinkFunction((YW::CONTAINER*)OBJENTRYMenu, (YW::WIDGET*)allocYWMemCheckBox5);
                ContainerLinkFunction((YW::CONTAINER*)OBJENTRYMenu, (YW::WIDGET*)allocYWMemCheckBox6);

                ShowDebugWindowFunction((YW::WIDGET*)OBJENTRYMenu);
            }
            else {
                printf("Failed to create OBJENTRYMenu\n");
            }
        }

    }


    void CustomOBJENTRYMenuSelectionOpenCallback(YW::EVENT event, int index, unsigned int* arg) {
        OBJENTRY_0* v1;

        if ((event.Flag & 0x20) != 0) {
            
                //if (AddPlayerEntries) {
                    switch (index) {
                        case 0:
                            NewSora(0x5E9);
                            break;

                        case 1:
                            NewSora(0x68F);
                            break;

                        case 2:
                            NewSora(0x3C8);
                            break;

                        case 3:
                            NewSora(0x386);
                            break;

                        case 4:
                            NewSora(0x6BE);
                            break;

                        case 5:
                            NewSora(0x4B4);
                            break;

                        case 6:
                            NewSora(0x601);
                            break;

                        case 7:
                            NewSora(0x602);
                            break;

                        case 8:
                            NewSora(0x236);
                            break;

                        case 9:
                            NewSora(0x35B);
                            break;

                        case 10:
                            NewSora(0x054);
                            break;

                        case 11:
                            NewSora(0x602);
                            break;


                    //}


                    
            } 
        }
    }

    void typeSelectCallback(int selectedIndex, unsigned int* arg) {
        // Array to store the strings
        //const char* PlayerList[1024];
        //size_t playerCount = 0;
        uintptr_t FreeYWwindowWidgetFunctionAddress = FindAndPrintPatternW(Patterns::pattern_for_FreeYWwindowWidget, L"Free YW Window Widget", 0);
        FunctionCalls::tFreeYWwindowwidget FreeYWwindowWidgetFunction = reinterpret_cast<FunctionCalls::tFreeYWwindowwidget>(FreeYWwindowWidgetFunctionAddress);


        // Handle different selections
        switch (selectedIndex) {
        case 0:
            // TYPE_PLAYER_9 (Player)
            
            ProcessOBJENTRYData(TYPE_PLAYER_9, PlayerList, playerCount);
            break;

        case 1:
            // TYPE_FRIEND_4 (Friendly characters)
            ProcessOBJENTRYData(TYPE_FRIEND_4, PlayerList, playerCount);
            break;

        case 2:
            // TYPE_NPC (Non-player characters)
            ProcessOBJENTRYData(TYPE_NPC, PlayerList, playerCount);
            break;

        case 3:
            // TYPE_BOSS_0 (Boss enemies)
            ProcessOBJENTRYData(TYPE_BOSS_0, PlayerList, playerCount);
            break;

        case 4:
            // TYPE_ZAKO (Minor enemies)
            ProcessOBJENTRYData(TYPE_ZAKO, PlayerList, playerCount);
            break;

        case 5:
            // TYPE_WEAPON_0 (Weapons)
            ProcessOBJENTRYData(TYPE_WEAPON_0, PlayerList, playerCount);
            break;

        case 6:
            // TYPE_E_WEAPON (Enemy weapons)
            ProcessOBJENTRYData(TYPE_E_WEAPON, PlayerList, playerCount);
            break;

        case 7:
            // TYPE_SAVEPOINT_0 (Save points)
            ProcessOBJENTRYData(TYPE_SAVEPOINT_0, PlayerList, playerCount);
            break;

        case 8:
            // TYPE_FIELDOBJ (Field objects)
            ProcessOBJENTRYData(TYPE_FIELDOBJ, PlayerList, playerCount);
            break;

        case 9:
            // TYPE_BTLNPC_0 (Battle NPCs)
            ProcessOBJENTRYData(TYPE_BTLNPC_0, PlayerList, playerCount);
            break;

        case 10:
            // TYPE_TREASURE_BOX (Treasure chests)
            ProcessOBJENTRYData(TYPE_TREASURE_BOX, PlayerList, playerCount);
            break;

        case 11:
            // TYPE_SUBMENU_0 (Submenu elements)
            ProcessOBJENTRYData(TYPE_SUBMENU_0, PlayerList, playerCount);
            break;

        case 12:
            // TYPE_LARGEBOSS (Large boss enemies)
            ProcessOBJENTRYData(TYPE_LARGEBOSS, PlayerList, playerCount);
            break;

        case 13:
            // TYPE_GIMMICKOBJ (Gimmick objects)
            ProcessOBJENTRYData(TYPE_GIMMICKOBJ, PlayerList, playerCount);
            break;

        case 14:
            // TYPE_MEMO (Memo items)
            ProcessOBJENTRYData(TYPE_MEMO, PlayerList, playerCount);
            break;

        case 15:
            // TYPE_RTN (Return points)
            ProcessOBJENTRYData(TYPE_RTN, PlayerList, playerCount);
            break;

        case 16:
            // TYPE_MINIGAME_3 (Mini-games)
            ProcessOBJENTRYData(TYPE_MINIGAME_3, PlayerList, playerCount);
            break;

        case 17:
            // TYPE_WORLDMAP_1 (World map elements)
            ProcessOBJENTRYData(TYPE_WORLDMAP_1, PlayerList, playerCount);
            break;

        case 18:
            // TYPE_PRIZEBOX (Prize boxes)
            ProcessOBJENTRYData(TYPE_PRIZEBOX, PlayerList, playerCount);
            break;

        case 19:
            // TYPE_SUMMON_2 (Summons)
            ProcessOBJENTRYData(TYPE_SUMMON_2, PlayerList, playerCount);
            break;

        case 20:
            // TYPE_SHOPPOINT (Shop points)
            ProcessOBJENTRYData(TYPE_SHOPPOINT, PlayerList, playerCount);
            break;

        case 21:
            // TYPE_L_ZAKO (Large minor enemies)
            ProcessOBJENTRYData(TYPE_L_ZAKO, PlayerList, playerCount);
            break;

        case 22:
            // TYPE_MASSEFFECT (Mass effects)
            ProcessOBJENTRYData(TYPE_MASSEFFECT, PlayerList, playerCount);
            break;

        case 23:
            // TYPE_ENEMYOBJ (Enemy objects)
            ProcessOBJENTRYData(TYPE_ENEMYOBJ, PlayerList, playerCount);
            break;

        case 24:
            // TYPE_JIGSAW (Jigsaw pieces)
            ProcessOBJENTRYData(TYPE_JIGSAW, PlayerList, playerCount);
            break;

        default:
            // Default case if no valid type is selected
            std::cout << "Invalid selection!" << std::endl;
            break;
        }

        //FreeYWwindowWidgetFunction((YW::WIDGET_5*)OBJENTRYMenu, 1);
        //OBJENTRYMenu = NULL;
        // Debugging: Print PlayerList contents
        //for (size_t i = 0; i < playerCount; ++i) {
        //    std::cout << PlayerList[i] << std::endl;
        //}
    }


    void CustomOBJENTRYMenuSelectionOpen(const char* title, u_int type, void (*callback)(OBJENTRY_ID)) {

        //AddPlayerEntries();

        uintptr_t YWListButtonFunctionAddress = ReadLEA32(Patterns::pattern_for_YWListButton, L"YW LIST BUTTON", 0, 1, 5);
        uintptr_t OpenSingletonWindowFunctionAddress = ReadLEA32(Patterns::pattern_for_OpenSingletonWindow, L"OpenSingletonWindow", 0, 1, 5);
        uintptr_t AllocYWMemoryFunctionAddress = ReadLEA32(Patterns::pattern_for_Alloc_YW_Memory, L"AllocYWMemory", 0, 1, 5);
        uintptr_t WListFunctionAddress = ReadLEA32(Patterns::pattern_for_YWList, L"YWList", 0, 1, 5);
        uintptr_t YWFrameFunctionAddress = ReadLEA32(Patterns::pattern_for_YWFrame, L"YWFrame", 0, 1, 5);
        uintptr_t ContainerLinkFunctionAddress = FindAndPrintPatternW(Patterns::pattern_for_ContainerLink, L"ContainerLink", 0);
        uintptr_t YWwindowMinSizeFunctionAddress = FindAndPrintPatternW(Patterns::pattern_for_YWwindow_minSize, L"YWwindow Min Size", 0);
        uintptr_t YWwindowMaxSizeFunctionAddress = FindAndPrintPatternW(Patterns::pattern_for_YWwindow_maxSize, L"YWwindow Max Size", 0);
        uintptr_t ShowDebugWindowFunctionAddress = ReadLEA32(Patterns::pattern_for_ShowDebugWindow, L"ShowDebugWindow", 0, 1, 5);


        //uintptr_t DebugWindowAddSubMenuFunctionAddress = ReadLEA32(Patterns::pattern_for_DebugWindowAddSubMenu, L"Add Debug Submenu", 0, 1, 5);


        globalCallback = callback;


        FunctionCalls::tYWListButton YWListButtonFunction = reinterpret_cast<FunctionCalls::tYWListButton>(YWListButtonFunctionAddress);
        FunctionCalls::tAlloc_YW_Memory AllocYWMemoryFunction = reinterpret_cast<FunctionCalls::tAlloc_YW_Memory>(AllocYWMemoryFunctionAddress);
        FunctionCalls::tYWList YWListFunction = reinterpret_cast<FunctionCalls::tYWList>(WListFunctionAddress);
        FunctionCalls::tYWFrame YWFrameFunction = reinterpret_cast<FunctionCalls::tYWFrame>(YWFrameFunctionAddress);
        FunctionCalls::tContainerLink ContainerLinkFunction = reinterpret_cast<FunctionCalls::tContainerLink>(ContainerLinkFunctionAddress);
        FunctionCalls::tYW_WINDOW_set_min_size YWwindowMinSizeFunction = reinterpret_cast<FunctionCalls::tYW_WINDOW_set_min_size>(YWwindowMinSizeFunctionAddress);
        FunctionCalls::tYW_WINDOW_set_max_size YWwindowMaxSizeFunction = reinterpret_cast<FunctionCalls::tYW_WINDOW_set_max_size>(YWwindowMaxSizeFunctionAddress);
        FunctionCalls::tShowDebugWindow ShowDebugWindowFunction = reinterpret_cast<FunctionCalls::tShowDebugWindow>(ShowDebugWindowFunctionAddress);

        FunctionCalls::tOpenSingletonWindow OpenSingletonWindowFunction = reinterpret_cast<FunctionCalls::tOpenSingletonWindow>(OpenSingletonWindowFunctionAddress);

        YW::YW_WINDOW_39* CustomOBJENTRYMenu = OpenSingletonWindowFunction(title, 180, 300, 2);

        auto allocYWMemList = AllocYWMemoryFunction((YW::MEMORY*)0x200);
        unsigned int* Frame = AllocYWMemoryFunction((YW::MEMORY*)0x200);
        unsigned int* categoryListButton = AllocYWMemoryFunction((YW::MEMORY*)0x200);
       // int v23 = 0;
        //int v24 = 0;
       // int v25 = 0;

        // for (int i = 0; i < 25; ++i) {
        //     TypeElem[i] = TypeStrings[i]; // Assign each pointer to point to corresponding string in TypeStrings
        // }

        // TypeElem[25] = nullptr; // Ensure the last pointer is set to nullptr to mark the end of the array
       //  YW::WIDGET* widget1;

         //widget1 = 0;

        YWListButtonFunction((YW::LIST_BUTTON*)categoryListButton, 0, 0, 80, 11, type, TypeStrings, typeSelectCallback, categoryListButton);
        //widget1 = (YW::WIDGET*)(categoryListButton);

        YW::WIDGET* widget;


        widget = 0;

        YWListFunction((YW::YW_LIST_0*)allocYWMemList, 0, 10, PlayerList, CustomOBJENTRYMenuSelectionOpenCallback, allocYWMemList);
        widget = (YW::WIDGET*)allocYWMemList;

        //widget->Finalizer = CustomMusicPlayerFinalizer;


        YW::FRAME* v5;
        v5 = 0;

        //YWFrame((YW::FRAME*)Frame, 0, 11, 0, -11, 0, widget->Rect.H);

        YWFrameFunction((YW::FRAME*)Frame, 0, 11, 0, -11, 0, widget->Rect.H);

        v5 = (YW::FRAME*)Frame;

        //ContainerLink((YW::CONTAINER*)v5, (YW::WIDGET*)widget);
        //ContainerLink((YW::CONTAINER*)Menu, (YW::WIDGET*)v5);
            ContainerLinkFunction((YW::CONTAINER*)CustomOBJENTRYMenu, (YW::WIDGET*)categoryListButton);
            ContainerLinkFunction((YW::CONTAINER*)v5, (YW::WIDGET*)widget);
            ContainerLinkFunction((YW::CONTAINER*)CustomOBJENTRYMenu, (YW::WIDGET*)v5);
            //ContainerLink((YW_CONTAINER_5*)Menu, (YW_WIDGET*)allocYWMemLabelCurrentFrame);

            //int contentHeight = widget->Rect.H + 5; // Assuming additional padding of 22
            //int contentWidth = widget->Rect.W + 7; // Assuming widget->Rect.W is the width of the list
            /*v23 = 180;
            if (widget->Rect.W + 10 > 180)
            {
                v23 = widget->Rect.W + 10;
                v24 = v23 - 10;
                OBJENTRYMenu->Rect.W = v23;
                v25 = 300;
                widget->Rect.W = v24;
                if (widget->Rect.H < 300)
                {
                    v25 = widget->Rect.H;

                }

            }*/


            //YW_WINDOW_set_min_size(Menu, contentWidth, contentHeight);
            //YWwindowMinSizeFunction(OBJENTRYMenu, OBJENTRYMenu->Rect.W, v25 + 11);
            //YW_WINDOW_set_max_size(Menu, contentWidth, contentHeight);
            //YWwindowMaxSizeFunction(OBJENTRYMenu, OBJENTRYMenu->Rect.W, widget->Rect.H + 11);

            //ShowDebugWindow((YW_WIDGET_50*)Menu);
            ShowDebugWindowFunction((YW::WIDGET*)OBJENTRYMenu);
        
    }

    /*void CustomOBJENTRYSelectMenuCallback(int n) {
        YS::SORA* v1;
        bool v2;
        int v3;
        

        uintptr_t CreateFieldThreadFunctionAddress = ReadLEA32(Patterns::pattern_for_CreateFieldThread, L"CreateField Thread", 0, 1, 5);
        FunctionCalls::tCreateFieldThread CreateFieldThreadFunction = reinterpret_cast<FunctionCalls::tCreateFieldThread>(CreateFieldThreadFunctionAddress);

        if (n) {
            switch (n) {
            case 1:
                CustomOBJENTRYMenuSelectionOpen("friend", 1,NewFriend);
                break;
            case 2:
                CustomOBJENTRYMenuSelectionOpen("capture", 0,NewCapture);
                break;
            case 3:
                CustomOBJENTRYMenuSelectionOpen("1 obj", 3,NewObj1);
                break;
            case 4:
                CustomOBJENTRYMenuSelectionOpen("3 obj", entry_type,NewObj3i);
                break;
            case 5:
                CustomOBJENTRYMenuSelectionOpen("5 obj", entry_type,NewObj5i);
                break;
            case 6:
                CustomOBJENTRYMenuSelectionOpen("10 obj", entry_type,NewObj10);
                break;
            case 7:
                CustomOBJENTRYMenuSelectionOpen("20 obj", entry_type,NewObj20);
                break;
            case 8:
                CustomOBJENTRYMenuSelectionOpen("30 obj", entry_type,NewObj30);
                break;
            case 9:
                CustomOBJENTRYMenuSelectionOpen("50 obj", entry_type,NewObj50);
                break;

            default:
                v2 = n == 11;
                if (n == 10) {
                    CustomOBJENTRYMenuSelectionOpen("actor", entry_type,NewActor);
                    
                }
                else {
                    v3 = n - 12;
                    if (v2) {
                        if (myObjEntryId) {
                            CreateFieldThreadFunction(1, 1000, (TASK_FUNC)&ReloadThread, 0x2000);
                        }
                    }
                    else if (!v3) {
                        CreateFieldThreadFunction(1, 1000, (TASK_FUNC)&LeaveAllThread, 0x2000);
                    }
                }
                break;
            }
        }
        else {
            CustomOBJENTRYMenuSelectionOpen("player", 0,NewSora);
        }
    }*/



    void CustomMusicWindowCallback(int selectedItemIndex) {


        CustomMusicPlayer(0);
    }

    

    void CustomPaxEffectSelectorWindowCallback(int selectedItemIndex) {
        PaxEffectSelector(0);
    }

    /*void CustomOBJENTRYSelectMenu(int selectedItemIndex) {
        CustomOBJENTRYSelectMenuCallback(0);
    }

    void CustomOBJENTRYSelectMenu1(int selectedItemIndex) {
        CustomOBJENTRYSelectMenuCallback(1);
    }

    void CustomOBJENTRYSelectMenu2(int selectedItemIndex) {
        CustomOBJENTRYSelectMenuCallback(2);
    }

    void CustomOBJENTRYSelectMenu3(int selectedItemIndex) {
        CustomOBJENTRYSelectMenuCallback(3);
    }

    void CustomOBJENTRYSelectMenu4(int selectedItemIndex) {
        CustomOBJENTRYSelectMenuCallback(4);
    }

    void CustomOBJENTRYSelectMenu5(int selectedItemIndex) {
        CustomOBJENTRYSelectMenuCallback(5);
    }

    void CustomOBJENTRYSelectMenu6(int selectedItemIndex) {
        CustomOBJENTRYSelectMenuCallback(6);
    }

    void CustomOBJENTRYSelectMenu7(int selectedItemIndex) {
        CustomOBJENTRYSelectMenuCallback(7);
    }

    void CustomOBJENTRYSelectMenu8(int selectedItemIndex) {
        CustomOBJENTRYSelectMenuCallback(8);
    }

    void CustomOBJENTRYSelectMenu9(int selectedItemIndex) {
        CustomOBJENTRYSelectMenuCallback(9);
    }

    void CustomOBJENTRYSelectMenu10(int selectedItemIndex) {
        CustomOBJENTRYSelectMenuCallback(10);
    }

    void CustomOBJENTRYSelectMenu11(int selectedItemIndex) {
        CustomOBJENTRYSelectMenuCallback(11);
    }
    void CustomOBJENTRYSelectMenu12(int selectedItemIndex) {
        CustomOBJENTRYSelectMenuCallback(12);
    }*/

    void GiveOBJLongStarEffect(unsigned int* arg) {
        // Fetch the global object pointer
        //uintptr_t GetMenuOBJFunctionAddress = FindAndPrintPatternW(Patterns::pattern_for_GetOBJMenuObj, L"Get OBJ Menu OBJ", 0);
       // FunctionCalls::tGetOBJMenuObj GetMenuOBJFunction = reinterpret_cast<FunctionCalls::tGetOBJMenuObj>(GetMenuOBJFunctionAddress);
        

        
            // Calculate the obj_sheet pointer based on gGlobalObjPtr
            uintptr_t* obj_sheet = reinterpret_cast<uintptr_t*>(reinterpret_cast<char*>(gGlobalObjPtr1) + 0x5C0);

            // Check if obj_sheet is a valid pointer using IsBadReadPtr function
            if (IsBadReadPtr(obj_sheet, sizeof(uintptr_t))) {
                //OutputDebugStringA("Invalid memory at obj_sheet\n");
                return;
            }

            // Dereference obj_sheet to get the value it points to
            uintptr_t target_address = *obj_sheet;

            // Add the offset 0x08 (which is 8 bytes) to the target_address
            target_address += 0x08;

            // Print the address inside obj_sheet where boolean should be set
            //char insideObjSheetMessage[256];
            //snprintf(insideObjSheetMessage, sizeof(insideObjSheetMessage), "Address inside obj_sheet where boolean should be set: %p\n", reinterpret_cast<void*>(target_address));
            //OutputDebugStringA(insideObjSheetMessage);

            // Set the boolean flag at the target_address
            bool* no_dead_flag = reinterpret_cast<bool*>(target_address);
            *no_dead_flag = true; // Set to true to indicate no_dead

            // Construct the final debug message confirming the boolean flag setting
            //char finalMessage[256];
            //snprintf(finalMessage, sizeof(finalMessage), "No dead flag set to: %s at address %p\n",
               // *no_dead_flag ? "true" : "false", no_dead_flag);

            // Output the final debug message to Visual Studio Output window
           // OutputDebugStringA(finalMessage);
        
    }

    void SearchAndAddHosts(std::vector<std::wstring>& usernames, std::vector<std::wstring>& ipAddresses) {
        inipp::Ini<wchar_t> ini;
        std::wstring config_path = L"" PROJECT_NAME ".ini";
        std::wifstream iniFile(config_path);

        if (!iniFile) {
            LOG(L"Failed to load config file.\n");
            return;
        }

        ini.parse(iniFile);

        // Look for hosts in the ini file's "Hosts" section
        auto hostsSection = ini.sections.find(L"Hosts");
        if (hostsSection != ini.sections.end()) {
            for (const auto& hostEntry : hostsSection->second) {
                std::wstring username = hostEntry.first;  // The key in the section is the username
                std::wstring ipAddress = hostEntry.second; // The value is the corresponding IP address

                usernames.push_back(username);    // Store the username
                ipAddresses.push_back(ipAddress);  // Store the corresponding IP address
            }
        }

        LOG(L"Added %d hosts to the hostList.\n", usernames.size());
    }

    void CustomHostSelectCallback(YW::EVENT event, int index, unsigned int* arg) {

        if ((event.Flag & 0x20) != 0) {
        
            // Get the selected username
            std::wstring selectedUsername = usernames[index]; // Get the username
            // Get the corresponding IP address
            std::wstring selectedHostIP = ipAddresses[index]; // Get the corresponding IP

            // Start the client thread
            std::thread clientThread([selectedHostIP] {
                //P2P::StartUDPClient(selectedHostIP.c_str(), 50000);
                });

            clientThread.detach(); // Detach the thread

        }
        
    }

    void CustomHostSelector() {

        uintptr_t YWListButtonFunctionAddress = ReadLEA32(Patterns::pattern_for_YWListButton, L"YW LIST BUTTON", 0, 1, 5);
        uintptr_t OpenSingletonWindowFunctionAddress = ReadLEA32(Patterns::pattern_for_OpenSingletonWindow, L"OpenSingletonWindow", 0, 1, 5);
        uintptr_t AllocYWMemoryFunctionAddress = ReadLEA32(Patterns::pattern_for_Alloc_YW_Memory, L"AllocYWMemory", 0, 1, 5);
        uintptr_t WListFunctionAddress = ReadLEA32(Patterns::pattern_for_YWList, L"YWList", 0, 1, 5);
        uintptr_t YWFrameFunctionAddress = ReadLEA32(Patterns::pattern_for_YWFrame, L"YWFrame", 0, 1, 5);
        uintptr_t ContainerLinkFunctionAddress = FindAndPrintPatternW(Patterns::pattern_for_ContainerLink, L"ContainerLink", 0);
        uintptr_t YWwindowMinSizeFunctionAddress = FindAndPrintPatternW(Patterns::pattern_for_YWwindow_minSize, L"YWwindow Min Size", 0);
        uintptr_t YWwindowMaxSizeFunctionAddress = FindAndPrintPatternW(Patterns::pattern_for_YWwindow_maxSize, L"YWwindow Max Size", 0);
        uintptr_t ShowDebugWindowFunctionAddress = ReadLEA32(Patterns::pattern_for_ShowDebugWindow, L"ShowDebugWindow", 0, 1, 5);

        

        //uintptr_t DebugWindowAddSubMenuFunctionAddress = ReadLEA32(Patterns::pattern_for_DebugWindowAddSubMenu, L"Add Debug Submenu", 0, 1, 5);

        
        


        FunctionCalls::tYWListButton YWListButtonFunction = reinterpret_cast<FunctionCalls::tYWListButton>(YWListButtonFunctionAddress);
        FunctionCalls::tAlloc_YW_Memory AllocYWMemoryFunction = reinterpret_cast<FunctionCalls::tAlloc_YW_Memory>(AllocYWMemoryFunctionAddress);
        FunctionCalls::tYWList YWListFunction = reinterpret_cast<FunctionCalls::tYWList>(WListFunctionAddress);
        FunctionCalls::tYWFrame YWFrameFunction = reinterpret_cast<FunctionCalls::tYWFrame>(YWFrameFunctionAddress);
        FunctionCalls::tContainerLink ContainerLinkFunction = reinterpret_cast<FunctionCalls::tContainerLink>(ContainerLinkFunctionAddress);
        FunctionCalls::tYW_WINDOW_set_min_size YWwindowMinSizeFunction = reinterpret_cast<FunctionCalls::tYW_WINDOW_set_min_size>(YWwindowMinSizeFunctionAddress);
        FunctionCalls::tYW_WINDOW_set_max_size YWwindowMaxSizeFunction = reinterpret_cast<FunctionCalls::tYW_WINDOW_set_max_size>(YWwindowMaxSizeFunctionAddress);
        FunctionCalls::tShowDebugWindow ShowDebugWindowFunction = reinterpret_cast<FunctionCalls::tShowDebugWindow>(ShowDebugWindowFunctionAddress);

        FunctionCalls::tOpenSingletonWindow OpenSingletonWindowFunction = reinterpret_cast<FunctionCalls::tOpenSingletonWindow>(OpenSingletonWindowFunctionAddress);

        YW::YW_WINDOW_39* Menu = OpenSingletonWindowFunction("Custom Host Selector", 180, 300, 2);

        auto allocYWMemList = AllocYWMemoryFunction((YW::MEMORY*)0x200);
        unsigned int* Frame = AllocYWMemoryFunction((YW::MEMORY*)0x200);
        //unsigned int* categoryListButton = AllocYWMemoryFunction((YW::MEMORY*)0x200);

        if (!hostlistLoaded) {
            // Clear previous data in the vectors
            usernames.clear();
            ipAddresses.clear();

            // Load usernames and IP addresses
            SearchAndAddHosts(usernames, ipAddresses);

            // Prepare hostlistList to only contain usernames
            if (usernames.empty()) {
                // No hosts found, show message
                const char* noFilesMessage = "No one is hosting or I can't find the public IP/username from the .ini file.";
                size_t messageLength = strlen(noFilesMessage);
                hostlistList = new const char* [2]; // 1 for the message and 1 for the looping option
                hostlistList[0] = new char[messageLength + 1]; // +1 for the null terminator
                strcpy_s(const_cast<char*>(hostlistList[0]), messageLength + 1, noFilesMessage);
                hostlistList[1] = nullptr;
                loopingOptionIndex = 1; // Set looping option index
            }
            else {
                // Allocate memory for const char** hostlistList
                hostlistList = new const char* [usernames.size() + 2]; // +2 for the message and the looping option

                // Copy usernames into hostlistList
                for (size_t i = 0; i < usernames.size(); ++i) {
                    const std::wstring& wstr = usernames[i];
                    std::string str = WStringToString(wstr);
                    hostlistList[i] = new char[str.size() + 1]; // +1 for the null terminator
                    strcpy_s(const_cast<char*>(hostlistList[i]), str.size() + 1, str.c_str());
                }
                hostlistList[usernames.size()] = nullptr; // Set the last element to nullptr for looping option
            }
            hostlistLoaded = true;
        }


        //YWListButtonFunction((YW::LIST_BUTTON*)categoryListButton, 0, 0, 80, 11, type, TypeElem, typeSelectCallback, categoryListButton);

        YW::WIDGET* widget;


        widget = 0;

        YWListFunction((YW::YW_LIST_0*)allocYWMemList, 0, 10, hostlistList, CustomOBJENTRYMenuSelectionOpenCallback, allocYWMemList);
        widget = (YW::WIDGET*)allocYWMemList;

        //widget->Finalizer = CustomMusicPlayerFinalizer;


        YW::FRAME* v5;
        v5 = 0;

        //YWFrame((YW::FRAME*)Frame, 0, 11, 0, -11, 0, widget->Rect.H);

        YWFrameFunction((YW::FRAME*)Frame, 0, 11, 0, -11, 0, widget->Rect.H);

        v5 = (YW::FRAME*)Frame;

        //ContainerLink((YW::CONTAINER*)v5, (YW::WIDGET*)widget);
        //ContainerLink((YW::CONTAINER*)Menu, (YW::WIDGET*)v5);
        
        ContainerLinkFunction((YW::CONTAINER*)v5, (YW::WIDGET*)widget);
        ContainerLinkFunction((YW::CONTAINER*)Menu, (YW::WIDGET*)v5);
        //ContainerLink((YW_CONTAINER_5*)Menu, (YW_WIDGET*)allocYWMemLabelCurrentFrame);

        int contentHeight = widget->Rect.H + 5; // Assuming additional padding of 22
        int contentWidth = widget->Rect.W + 7; // Assuming widget->Rect.W is the width of the list

        //YW_WINDOW_set_min_size(Menu, contentWidth, contentHeight);
        YWwindowMinSizeFunction(Menu, contentWidth, contentHeight);
        //YW_WINDOW_set_max_size(Menu, contentWidth, contentHeight);
        YWwindowMaxSizeFunction(Menu, contentWidth, contentHeight);

        //ShowDebugWindow((YW_WIDGET_50*)Menu);
        ShowDebugWindowFunction((YW::WIDGET*)Menu);

    }

    // Helper functions to print enum names
    const char* GetZoneOptionName(UNIT_DATA_ZONE_OPTION option) {
        switch (option) {
        case OPTION_JUMP: return "OPTION_JUMP";
        case OPTION_GUIDE: return "OPTION_GUIDE";
        case OPTION_GUIDE_TRAIN: return "OPTION_GUIDE_TRAIN";
        case OPTION_TRIGGER: return "OPTION_TRIGGER";
        case OPTION_FORMATION: return "OPTION_FORMATION";
        case OPTION_PRIZE: return "OPTION_PRIZE";
        default: return "UNKNOWN_OPTION";
        }
    }

    const char* GetZoneShapeName(UNIT_DATA_ZONE_SHAPE shape) {
        switch (shape) {
        case SHAPE_BOX: return "SHAPE_BOX";
        case SHAPE_CYLINDER: return "SHAPE_CYLINDER";
        case SHAPE_INFINITY: return "SHAPE_INFINITY";
        default: return "UNKNOWN_SHAPE";
        }
    }

    void PrintUnitDataZone(const UNIT_DATA_ZONE* zoneData) {
        if (!zoneData) {
            printf("UNIT_DATA_ZONE pointer is null.\n");
            return;
        }

        printf("UNIT_DATA_ZONE Details:\n");
        printf("  Shape: %s\n", GetZoneShapeName(zoneData->Shape));
        printf("  Option: %s\n", GetZoneOptionName(zoneData->Option));
        printf("  Xpos: %.2f\n", zoneData->Xpos);
        printf("  Ypos: %.2f\n", zoneData->Ypos);
        printf("  Zpos: %.2f\n", zoneData->Zpos);
        printf("  Xsize: %.2f\n", zoneData->Xsize);
        printf("  Ysize: %.2f\n", zoneData->Ysize);
        printf("  Zsize: %.2f\n", zoneData->Zsize);
        printf("  Xrot: %.2f\n", zoneData->Xrot);
        printf("  Yrot: %.2f\n", zoneData->Yrot);
        printf("  Zrot: %.2f\n", zoneData->Zrot);
        printf("  Flag: 0x%08X\n", zoneData->Flag);
        printf("  Kind: %hu\n", zoneData->Kind);
        printf("  OnBgGroup: %hhu\n", zoneData->OnBgGroup);
        printf("  OffBgGroup: %hhu\n", zoneData->OffBgGroup);

        // Print padding if necessary
        printf("  Padding: ");
        for (int i = 0; i < 16; ++i) {
            printf("%02X ", (unsigned char)zoneData->Padding[i]);
        }
        printf("\n");
    }

    
    

    // Function to read the unit pointers
    void ReadUnitPointers() {
        InitializePointers();
        size_t UnitPTREntriesAmount = 0;
        uintptr_t StartOfUnitEntries = 0;
        uintptr_t baseAddress = 0;

        // Determine the base address for unit entries based on the game version
        if (DetectGameVersion() == "Steam") {
            StartOfUnitEntries = SteamPointer + 0x1F02A78;
        }
        else if (DetectGameVersion() == "Epic") {
            StartOfUnitEntries = EpicPointer + 0x1F02A78;
        }

        if (DetectGameVersion() == "Steam") {
            UnitPTREntriesAmount = *(size_t*)(SteamPointer + 0x1F02E80);
        }
        else if (DetectGameVersion() == "Epic") {
            UnitPTREntriesAmount = *(size_t*)(SteamPointer + 0x1F02E80);
        }

        // After determining the StartOfUnitEntries
       // printf("StartOfUnitEntries = 0x%016llX\n", StartOfUnitEntries);
       // printf("UnitPTREntriesAmount = %zu\n", UnitPTREntriesAmount);

        for (size_t i = 0; i < UnitPTREntriesAmount; ++i) {
            // Calculate the address of the current entry (each entry is 0x10 bytes apart)
            uintptr_t currentEntryAddress = StartOfUnitEntries + (i * 0x10);

            // Read the pointer from the entry at offset 0x08
            uintptr_t unitPointerAddr = *(uintptr_t*)(currentEntryAddress + 0x08); // Pointer to UNIT_GROUP

            if (unitPointerAddr) {
                // Cast unitPointerAddr to UNIT_GROUP*
                g_unitGroup = reinterpret_cast<UNIT_GROUP*>(unitPointerAddr);

                // Now you can access UNIT_GROUP members through unitPointer
              //  printf("Entry %zu: UNIT_GROUP UnitName = 0x%08X\n", i, g_unitGroup->UnitName);
               // printf("Entry %zu: UNIT_GROUP EnemyNum = %d\n", i, g_unitGroup->EnemyNum);
               // printf("Entry %zu: UNIT_GROUP Level = %u\n", i, g_unitGroup->Level);
                // Add additional prints for other members as needed

                const UNIT_DATA_ZONE* zoneData = g_unitGroup->ZoneData;
                if (g_unitGroup->ZoneData) {
                    //PrintUnitDataZone(g_unitGroup->ZoneData);
                }
                else {
                 //   printf("ZoneData is null.\n");
                }
            }
            else {
                // Handle invalid unitPointerAddr
               // printf("Entry %zu: Invalid UNIT_GROUP pointer!\n", i);
            }
        }
    }

    

   

    void HostWindowCallback(int selectedItemIndex) {
        //std::thread serverThread(P2P::StartServer);
        //std::thread serverThread(P2P::StartUDPDiscoveryServer);
        
        //P2P::StartClient("192.168.0.200"); // Replace with actual server IP if needed
       // P2P::StartUDPClient("127.0.0.1", 50000);
        
        //serverThread.detach();
        std::string publicIp = GetPublicIP();
        //LOG(L"Public IP Address of the host: %s\n", StringToWstring(publicIp).c_str());

        
            // Start UDP Discovery Server in a separate thread
        std::thread serverThread([publicIp] {
            P2P::StartUDPDiscoveryServer(publicIp.c_str(),50000);
            });

        

        // Optionally wait for both threads to finish
        serverThread.detach();
        
        
        
    }

    void ClientWindowCallback(int selectedItemIndex) {
        // Start UDP Client in a separate thread
        std::thread clientThread([] {
            P2P::StartUDPClient("127.0.0.1", 50000);
            });

        clientThread.detach();
    }


    void UpdateFreeCamera() {

        static int s_prevTriggers = 0;   // remember trigger state from last frame



        obj_camera* objcamera = GetOBJCameraInstanceFunction();
        objcamera = reinterpret_cast<obj_camera*>(reinterpret_cast<uintptr_t>(objcamera) + 0x08);

        g_cinematicCamera.attachTarget(objcamera);

        YS::PAD* leftStickPad = GetHardPadFunction(0);
       // LOG("LeftStickPad Buttons: 0x%X\n", leftStickPad->Button);
        YS::PAD* rightStickPad = GetSoftPadFunction(0);
        //LOG("RightStickPad Buttons: 0x%X\n", rightStickPad->Button);
        kn_camera* camera = GetKNCameraFunction(1);

        float frameTime = GetFuncFrametimeFunction();
        float deadZone = 0.05f;
        float movementSpeedModifier = 0.052359879f;

        if (LevelEditorToggle)
        {

            const int SHIFT_MASK = static_cast<int>(YS::HARDPAD::SHIFT_OFF) | static_cast<int>(YS::HARDPAD::SHIFT_ON);

            int buttons = leftStickPad->Button & ~SHIFT_MASK;

            if ((buttons & static_cast<int>(YS::HARDPAD::L1)) != 0) {
                // L1 pressed logic here
            }

            // Adjust FreecamSpeed with L2/R2 as before
            if ((buttons & static_cast<int>(YS::HARDPAD::L2)) != 0) { // L2 decreases speed
                FreecamSpeed = std::max(1.0f, FreecamSpeed - 32.0f);
            }
            if ((buttons & static_cast<int>(YS::HARDPAD::R2)) != 0) { // R2 increases speed
                FreecamSpeed = std::min(1000.0f, FreecamSpeed + 32.0f);
            }

            static float orbitRadius = 10.0f;

            if ((buttons & static_cast<int>(YS::HARDPAD::UP)) != 0) {
                 // Up arrow - zoom in (reduce radius)
                orbitRadius -= 20.0f * frameTime;
                if (orbitRadius < 50.0f) orbitRadius = 50.0f;  // Min zoom distance to fully see cursor
            }

            if ((buttons & static_cast<int>(YS::HARDPAD::DOWN)) != 0) { // Down arrow - zoom out (increase radius)
                orbitRadius += 20.0f * frameTime;
                // No max limit here — infinite zoom out
            }

            // --- INPUT HANDLING ---
            float leftStickX = rightStickPad->Stick[1].x; // Left stick
            float leftStickY = rightStickPad->Stick[1].y;
            float rightStickX = rightStickPad->Stick[0].x; // Right stick
            float rightStickY = rightStickPad->Stick[0].y;

            // Dead zone
            if (fabs(leftStickX) < deadZone) leftStickX = 0.0f;
            if (fabs(leftStickY) < deadZone) leftStickY = 0.0f;
            if (fabs(rightStickX) < deadZone) rightStickX = 0.0f;
            if (fabs(rightStickY) < deadZone) rightStickY = 0.0f;

            float moveSpeed = FreecamSpeed * frameTime * movementSpeedModifier;

            // --- MOVE AXIS CURSOR (on ground plane) ---
            XMVECTOR up = XMVectorSet(0, 1, 0, 0);
            XMVECTOR eye = XMVectorSet(objcamera->m_eye.x, objcamera->m_eye.y, objcamera->m_eye.z, 1);
            XMVECTOR at = XMVectorSet(objcamera->m_at.x, objcamera->m_at.y, objcamera->m_at.z, 1);
            XMVECTOR forward = XMVector3Normalize(XMVectorSubtract(at, eye));
            XMVECTOR right = XMVector3Normalize(XMVector3Cross(up, forward));

            // Flatten to XZ plane
            forward = XMVectorSetY(forward, 0.0f);
            forward = XMVector3Normalize(forward);
            right = XMVectorSetY(right, 0.0f);
            right = XMVector3Normalize(right);

            XMVECTOR move = XMVectorScale(forward, -leftStickY * moveSpeed);
            move = XMVectorAdd(move, XMVectorScale(right, leftStickX * moveSpeed));

            XMVECTOR cursorPos = XMVectorSet(AxisCursorPos.x, AxisCursorPos.y, AxisCursorPos.z, 1.0f);
            cursorPos = XMVectorAdd(cursorPos, move);

            // Up/Down movement using L1/R1 (0x400 / 0x800)
            if ((buttons & static_cast<int>(YS::HARDPAD::L1)) != 0) // L1
                cursorPos = XMVectorAdd(cursorPos, XMVectorSet(0, moveSpeed, 0, 0));
            if ((buttons & static_cast<int>(YS::HARDPAD::R1)) != 0) // R1
                cursorPos = XMVectorSubtract(cursorPos, XMVectorSet(0, moveSpeed, 0, 0));

            
            XMFLOAT4 tempCursorPos;
            XMStoreFloat4(&tempCursorPos, cursorPos);
            AxisCursorPos.x = std::clamp(tempCursorPos.x, -100000.0f, 100000.0f);
            AxisCursorPos.y = std::clamp(tempCursorPos.y, -100000.0f, 100000.0f);
            AxisCursorPos.z = std::clamp(tempCursorPos.z, -100000.0f, 100000.0f);
            AxisCursorPos.w = tempCursorPos.w;

            // --- ORBIT CAMERA AROUND AxisCursorPos ---
            static float orbitYaw = 0.0f;
            static float orbitPitch = 0.0f;
            //static float orbitRadius = 10.0f;

            orbitYaw += -rightStickX * frameTime * movementSpeedModifier * 1.5f;
            orbitPitch += rightStickY * frameTime * movementSpeedModifier * 1.5f;
            orbitPitch = std::clamp(orbitPitch, -1.5f, 1.5f);

            // Orbit position (spherical to Cartesian)
            float x = orbitRadius * cosf(orbitPitch) * cosf(orbitYaw);
            float y = orbitRadius * sinf(orbitPitch);
            float z = orbitRadius * cosf(orbitPitch) * sinf(orbitYaw);

            XMVECTOR camEye = XMVectorSet(AxisCursorPos.x + x, AxisCursorPos.y + y, AxisCursorPos.z + z, 1.0f);
            XMVECTOR camTarget = XMVectorSet(AxisCursorPos.x, AxisCursorPos.y, AxisCursorPos.z, 1.0f);

            
            XMFLOAT4 tempEye, tempTarget;
            XMStoreFloat4(&tempEye, camEye);
            XMStoreFloat4(&tempTarget, camTarget);

            objcamera->m_eye = { tempEye.x, tempEye.y, tempEye.z, tempEye.w };
            objcamera->m_eyeTarget = objcamera->m_eye;
            objcamera->m_at = { tempTarget.x, tempTarget.y, tempTarget.z, tempTarget.w };
            objcamera->m_atTarget = objcamera->m_at;
        }


        else {
            // --- FREE CAMERA MODE ---


           

            
            const int SHIFT_MASK = static_cast<int>(YS::HARDPAD::SHIFT_OFF) | static_cast<int>(YS::HARDPAD::SHIFT_ON);

            int buttons = leftStickPad->Button & ~SHIFT_MASK;
            int triggers = leftStickPad->Trigger & ~SHIFT_MASK;

            int triggersPressed = triggers & ~s_prevTriggers;

            // ---------- CINEMATIC CAMERA CONTROLS (only when toggle is ON AND gameplay-cine is OFF) ----------
            if (CinematicCameraToggle && !g_cineGameplayEnabled)
            {
                // Timeline cursor with DPAD LEFT/RIGHT (held is ok)
                if (buttons & static_cast<int>(YS::HARDPAD::LEFT_0)) {
                    g_cineTimelineTime -= frameTime;
                    if (g_cineTimelineTime < 0.0f) g_cineTimelineTime = 0.0f;
                }
                if (buttons & static_cast<int>(YS::HARDPAD::RIGHT_0)) {
                    g_cineTimelineTime += frameTime;
                }

                // Add keyframe at current time (R3) – ONCE per press
                if (triggersPressed & static_cast<int>(YS::HARDPAD::R3)) {
                    g_cinematicCamera.addKeyframe(g_cineTimelineTime);
                }

                // Play once (CROSS) – toggle
                if (triggersPressed & static_cast<int>(YS::HARDPAD::CROSS))
                {
                    if (g_cinematicCamera.isPlaying())
                        g_cinematicCamera.stopPlayback();
                    else
                        g_cinematicCamera.startPlayback(false);
                }

                // Play loop (TRIANGLE) – toggle looped playback
                if (triggersPressed & static_cast<int>(YS::HARDPAD::TRIANGLE))
                {
                    if (g_cinematicCamera.isPlaying())
                        g_cinematicCamera.stopPlayback();
                    else
                        g_cinematicCamera.startPlayback(true);
                }

                // Stop playback (CIRCLE)
                if (triggersPressed & static_cast<int>(YS::HARDPAD::CIRCLE)) {
                    g_cinematicCamera.stopPlayback();
                }

                // Remove last keyframe (L3)
                if (triggersPressed & static_cast<int>(YS::HARDPAD::L3)) {
                    g_cinematicCamera.removeLastKeyframe();
                }

                // Toggle spline visibility (SQUARE)
                if (triggersPressed & static_cast<int>(YS::HARDPAD::SQUARE)) {
                    g_cineShowSpline = !g_cineShowSpline;
                }

                // Open settings window (SELECT)
                if (triggersPressed & static_cast<int>(YS::HARDPAD::SELECT)) {
                    CinematicCameraSettingsMenuCallback(0);
                }
            }
            // ---------- END CINEMATIC CAMERA CONTROLS ----------

            // Set mode flag only for editor usage
            if (CinematicCameraToggle && !g_cinematicCamera.isPlaying())
            {
                g_cinematicCamera.setMode(CinematicCamera::Mode::Editing);
            }
            else if (!g_cinematicCamera.isPlaying())
            {
                g_cinematicCamera.setMode(CinematicCamera::Mode::Disabled);
            }

            // ---------- ALWAYS advance playback when it's running ----------
            if (g_cinematicCamera.isPlaying())
            {
                g_cinematicCamera.update(frameTime);

                // In EDITOR/freecam mode, let cinema control camera and skip freecam movement
                if (CinematicCameraToggle && !g_cineGameplayEnabled)
                    return;
            }


            FreecamPos = objcamera->m_eyeTarget;
            FreecamTargetPos = objcamera->m_atTarget;

            float leftStickX = leftStickPad->Stick[1].x;
            float leftStickY = -leftStickPad->Stick[1].y;
            float rightStickX = rightStickPad->Stick[0].x;
            float rightStickY = rightStickPad->Stick[0].y;
            float rightStickW = rightStickPad->Stick[0].w;

            if (fabs(leftStickX) < deadZone) leftStickX = 0.0f;
            if (fabs(leftStickY) < deadZone) leftStickY = 0.0f;
            if (fabs(rightStickX) < deadZone) rightStickX = 0.0f;
            if (fabs(rightStickY) < deadZone) rightStickY = 0.0f;
            if (fabs(rightStickW) < deadZone) rightStickW = 0.0f;

            // Update yaw and pitch based on right stick
            if (fabs(rightStickX) > deadZone && rightStickW > 0.0f) {
                objcamera->m_Yaw -= rightStickX * frameTime * movementSpeedModifier;
            }
            objcamera->m_Pitch += rightStickY * frameTime * movementSpeedModifier;

            float MaxCameraPitch = 1.2217305f;
            float MinCameraPitch = -1.2217305f;
            if (objcamera->m_Pitch > MaxCameraPitch) objcamera->m_Pitch = MaxCameraPitch;
            if (objcamera->m_Pitch < MinCameraPitch) objcamera->m_Pitch = MinCameraPitch;

            Vector4 forwardDir;
            forwardDir.x = cosf(objcamera->m_Yaw) * cosf(objcamera->m_Pitch);
            forwardDir.y = sinf(objcamera->m_Pitch);
            forwardDir.z = sinf(objcamera->m_Yaw) * cosf(objcamera->m_Pitch);
            NormalizeVector4Function(&forwardDir);

            Vector4 rightDir;
            rightDir.x = forwardDir.z;
            rightDir.z = -forwardDir.x;
            rightDir.y = 0;
            NormalizeVector4Function(&rightDir);

            FreecamPos.x += (leftStickY * forwardDir.x + leftStickX * rightDir.x) * FreecamSpeed * frameTime * movementSpeedModifier;
            FreecamPos.y += (leftStickY * forwardDir.y) * FreecamSpeed * frameTime * movementSpeedModifier;
            FreecamPos.z += (leftStickY * forwardDir.z + leftStickX * rightDir.z) * FreecamSpeed * frameTime * movementSpeedModifier;

            if ((triggers & static_cast<int>(YS::HARDPAD::CIRCLE)) != 0) { //square button to reset position and pitch and yaw and fov
                objcamera->m_Pitch = 0.0f;
                objcamera->m_Yaw = 0.0f;
                FreecamPos = (Vector4)(0.0f, 0.0f, 0.0f, 0.0f);
                FreecamTargetPos = (Vector4)(0.0f, 0.0f, 0.0f, 0.0f);
                objcamera->m_fov = 1.50f;
            }

            if ((buttons & static_cast<int>(YS::HARDPAD::L1)) != 0) {
                FreecamPos.y += FreecamSpeed * frameTime * movementSpeedModifier;
            }
            if ((buttons & static_cast<int>(YS::HARDPAD::R1)) != 0) {
                FreecamPos.y -= FreecamSpeed * frameTime * movementSpeedModifier;
            }

            if ((buttons & static_cast<int>(YS::HARDPAD::L2)) != 0) { // L2 decreases speed
                FreecamSpeed = std::max(1.0f, FreecamSpeed - 32.0f);
            }
            if ((buttons & static_cast<int>(YS::HARDPAD::R2)) != 0) { // R2 increases speed
                FreecamSpeed = std::min(1000.0f, FreecamSpeed + 32.0f);
            }

            if ((buttons & static_cast<int>(YS::HARDPAD::UP)) != 0) {
                objcamera->m_fov = objcamera->m_fov + (float)0.0099999998f;
            }

            if ((buttons & static_cast<int>(YS::HARDPAD::DOWN)) != 0) {
                objcamera->m_fov = objcamera->m_fov - (float)0.0099999998f;
            }

            if ((triggers & static_cast<int>(YS::HARDPAD::TRIANGLE)) != 0) { // Triangle to pause game
                // Toggle the paused state
                isPaused = !isPaused;

                // Call the appropriate function based on the new state
                if (isPaused) {
                    DBGWINPauseFunction();  // Toggle on
                }
                else {
                    DBGWINPauseOffFunction();  // Toggle off
                }
            }

            // Other button handling (speed adjust, FOV, reset, pause) here...

            FreecamTargetPos.x = FreecamPos.x + forwardDir.x;
            FreecamTargetPos.y = FreecamPos.y + forwardDir.y;
            FreecamTargetPos.z = FreecamPos.z + forwardDir.z;

            // Clamp minimum distance if needed
            Vector4 toCamera = FreecamPos - FreecamTargetPos;
            float distance = sqrtf(toCamera.x * toCamera.x + toCamera.y * toCamera.y + toCamera.z * toCamera.z);
            float minDistance = 1.0f;
            if (distance < minDistance && distance > 0.0001f) {
                toCamera.x /= distance;
                toCamera.y /= distance;
                toCamera.z /= distance;
                FreecamPos.x = FreecamTargetPos.x + toCamera.x * minDistance;
                FreecamPos.y = FreecamTargetPos.y + toCamera.y * minDistance;
                FreecamPos.z = FreecamTargetPos.z + toCamera.z * minDistance;
            }

            objcamera->m_eye = FreecamPos;
            objcamera->m_at = FreecamTargetPos;
            objcamera->m_eyeTarget = FreecamPos;
            objcamera->m_atTarget = FreecamTargetPos;

            /*uintptr_t EventCameraEye = 0;
            uintptr_t EventCameraAt = 0;

            if (DetectGameVersion() == "Steam") {
                EventCameraEye = SteamPointer - 0x3CCB10;
                EventCameraAt = SteamPointer - 0x3CCB00;

            }
            else if (DetectGameVersion() == "Epic") {
                EventCameraEye = EpicPointer - 0x3CCB10;
                EventCameraAt = EpicPointer - 0x3CCB00;
            }

            *(Vector4*)EventCameraEye = FreecamPos;
            *(Vector4*)EventCameraAt = FreecamTargetPos;*/


            //camera->Pos = FreecamPos;
            //camera->Ref = FreecamTargetPos;
            myXMVECTOR = DirectX::XMLoadFloat4(reinterpret_cast<const DirectX::XMFLOAT4*>(&FreecamTargetPos));


            // LOG("Forward Dir X: %.2f, Y: %.2f, Z: %.2f\n", forwardDir.x, forwardDir.y, forwardDir.z);
             //LOG("Right Dir X: %.2f, Y: %.2f, Z: %.2f\n", rightDir.x, rightDir.y, rightDir.z);
            // LOG("Left Stick X: %.2f, Left Stick Y: %.2f\n", leftStickX, leftStickY);
            StatusPrintfFunction("FreecamPos X: %.2f, Y: %.2f, Z: %.2f\n", FreecamPos.x, FreecamPos.y, FreecamPos.z);
            StatusPrintfFunction("FreecamAt X: %.2f, Y: %.2f, Z: %.2f\n", FreecamTargetPos.x, FreecamTargetPos.y, FreecamTargetPos.z);
            //LOG("Yaw: %.2f, Pitch: %.2f\n", objcamera->m_Yaw, objcamera->m_Pitch);
            //LOG("Frame Time: %.2f\n", frameTime);
            StatusPrintfFunction("FreeCamSpeed: %.2f\n", FreecamSpeed);
            StatusPrintfFunction("FOV: %.2f\n", objcamera->m_fov);

            StatusPrintfFunction("@066Freecam Controls: \n");
            StatusPrintfFunction("@660Circle button to reset position and pitch and yaw and fov\n");
            StatusPrintfFunction("L2 to decrease FreeCamSpeed and R2 to increase FreeCamSpeed by 32.0 float\n");
            StatusPrintfFunction("DPAD-UP to increase FOV and DPAD-DOWN to decrease FOV\n");
            StatusPrintfFunction("L1 to decrease Freecam height and R1 to increase Freecam height\n");
            StatusPrintfFunction("Triangle to Pause the Game\n");

            s_prevTriggers = triggers;
        }

    }

    void MatMul(const float* A, const float* B, float* R)
    {
        for (int r = 0; r < 4; r++)
        {
            for (int c = 0; c < 4; c++)
            {
                R[r * 4 + c] =
                    A[r * 4 + 0] * B[0 * 4 + c] +
                    A[r * 4 + 1] * B[1 * 4 + c] +
                    A[r * 4 + 2] * B[2 * 4 + c] +
                    A[r * 4 + 3] * B[3 * 4 + c];
            }
        }
    }

    void WriteViewMatrix(uintptr_t viewMatrixAddr)
    {
        float dx = vm_lookat.x - vm_eye.x;
        float dy = vm_lookat.y - vm_eye.y;
        float dz = vm_lookat.z - vm_eye.z;

        float len = sqrtf(dx * dx + dy * dy + dz * dz);
        if (len < 0.0001f) return;

        dx /= len;
        dy /= len;
        dz /= len;

        // derive yaw/pitch EXACTLY like your movement
        yaw = atan2f(dz, dx);
        pitch = asinf(dy);

        float pCos = cosf(pitch);
        float pSin = sinf(pitch);
        float yCos = cosf(yaw);
        float ySin = sinf(yaw);

        float view[16] =
        {
            yCos,          0.0f, -ySin,         0.0f,
            pSin * ySin,   pCos,  pSin * yCos,  0.0f,
            ySin * pCos,  -pSin,  yCos * pCos,  0.0f,
            -vm_eye.x,    -vm_eye.y, -vm_eye.z, 1.0f
        };

        memcpy((void*)viewMatrixAddr, view, sizeof(view));

        // CRITICAL — keep engine in sync
        Vector4 lookVec = { vm_lookat.x, vm_lookat.y, vm_lookat.z, 1.0f };
        Vector4 eyeVec = { vm_eye.x,    vm_eye.y,    vm_eye.z,    1.0f };

        memcpy((void*)(moduleBase1 + 0x718C68), &lookVec, sizeof(Vector4));
        memcpy((void*)(moduleBase1 + 0x718C78), &eyeVec, sizeof(Vector4));
    }

    void UpdateFreecamInputOnly()
    {
        float frameTime = GetFuncFrametimeFunction();
        float deadZone = 0.05f;

        YS::PAD* leftPad = GetHardPadFunction(0);
        YS::PAD* rightPad = GetSoftPadFunction(0);

        float move_x = leftPad->Stick[1].x;
        float move_y = -leftPad->Stick[1].y;
        float look_x = rightPad->Stick[0].x;
        float look_y = rightPad->Stick[0].y;

        if (fabs(move_x) < deadZone) move_x = 0;
        if (fabs(move_y) < deadZone) move_y = 0;
        if (fabs(look_x) < deadZone) look_x = 0;
        if (fabs(look_y) < deadZone) look_y = 0;

        float move_speed = FreecamSpeed * frameTime * 0.052359879f;
        float look_speed = frameTime * 0.052359879f;

        look_x *= -look_speed;
        look_y *= look_speed;

        yaw += look_x;
        pitch += look_y;

        pitch = std::clamp(pitch, -1.57f, 1.57f);

        float pCos = cosf(pitch);
        float pSin = sinf(pitch);
        float yCos = cosf(yaw);
        float ySin = sinf(yaw);

        float mov_x = (move_y * -ySin * pCos + move_x * yCos) * move_speed;
        float mov_y = (move_y * -pSin) * move_speed;
        float mov_z = (move_y * yCos * pCos + move_x * ySin) * move_speed;

        vm_eye.x += mov_x;
        vm_eye.y += mov_y;
        vm_eye.z += mov_z;
    }

    void UpdateFreeCameraNormal_VM(uintptr_t viewMatrixAddr)
    {
        if (!freecamInitialized)
        {
            float* vm = (float*)viewMatrixAddr;

            // Extract position from view matrix
            vm_eye.x = -vm[12];
            vm_eye.y = -vm[13];
            vm_eye.z = -vm[14];
            vm_eye.w = 1.0f;

            // Extract rotation
            yaw = atan2f(vm[8], vm[0]);
            pitch = -atan2f(vm[6], vm[5]);

            freecamInitialized = true;

            printf("[Freecam INIT] Eye: %.2f %.2f %.2f\n", vm_eye.x, vm_eye.y, vm_eye.z);
        }


        float frameTime = GetFuncFrametimeFunction();
        float deadZone = 0.05f;

        YS::PAD* leftPad = GetHardPadFunction(0);
        YS::PAD* rightPad = GetSoftPadFunction(0);

        //-----------------------------------------
        // Input
        //-----------------------------------------

        float move_x_scale = leftPad->Stick[1].x;
        float move_y_scale = -leftPad->Stick[1].y;
        float look_x_scale = rightPad->Stick[0].x;
        float look_y_scale = rightPad->Stick[0].y;

        if (fabs(move_x_scale) < deadZone) move_x_scale = 0.0f;
        if (fabs(move_y_scale) < deadZone) move_y_scale = 0.0f;
        if (fabs(look_x_scale) < deadZone) look_x_scale = 0.0f;
        if (fabs(look_y_scale) < deadZone) look_y_scale = 0.0f;

        const int SHIFT_MASK =
            static_cast<int>(YS::HARDPAD::SHIFT_OFF) |
            static_cast<int>(YS::HARDPAD::SHIFT_ON);

        int buttons = leftPad->Button & ~SHIFT_MASK;

        //-----------------------------------------
        // Speed scaling
        //-----------------------------------------

        float move_speed = FreecamSpeed * frameTime * 0.052359879f;
        float move_speed_orig = move_speed;

        move_x_scale *= move_speed;
        move_y_scale *= move_speed;

        float look_speed = frameTime * 0.052359879f;

        look_x_scale *= -look_speed;
        look_y_scale *= look_speed;

        //-----------------------------------------
        // Rotation update
        //-----------------------------------------

        yaw += look_x_scale;
        pitch += look_y_scale;

        if (pitch > 1.57f) pitch = 1.57f;
        if (pitch < -1.57f) pitch = -1.57f;

        float pCos = cosf(pitch);
        float pSin = sinf(pitch);
        float yCos = cosf(yaw);
        float ySin = sinf(yaw);

        //-----------------------------------------
        // Movement (same math as Lua)
        //-----------------------------------------

        float mov_x = (move_y_scale * -ySin * pCos + move_x_scale * yCos);
        float mov_y = (move_y_scale * -pSin);
        float mov_z = (move_y_scale * yCos * pCos + move_x_scale * ySin);

        vm_eye.x += mov_x;
        vm_eye.y += mov_y;
        vm_eye.z += mov_z;

        //-----------------------------------------
        // Vertical movement
        //-----------------------------------------

        if (buttons & static_cast<int>(YS::HARDPAD::L1))
            vm_eye.y += move_speed;

        if (buttons & static_cast<int>(YS::HARDPAD::R1))
            vm_eye.y -= move_speed;

        //-----------------------------------------
        // Look direction
        //-----------------------------------------

        vm_lookat.x = vm_eye.x + move_speed_orig * -ySin * pCos;
        vm_lookat.y = vm_eye.y + move_speed_orig * -pSin;
        vm_lookat.z = vm_eye.z + move_speed_orig * yCos * pCos;

        //-----------------------------------------
        // Build matrices
        //-----------------------------------------

        float yaw_mat[16] =
        {
            yCos, 0.f, -ySin, 0.f,
            0.f,  1.f,  0.f,  0.f,
            ySin, 0.f,  yCos, 0.f,
            0.f,  0.f,  0.f,  1.f
        };

        float pitch_mat[16] =
        {
            1.f, 0.f, 0.f, 0.f,
            0.f, pCos, -pSin, 0.f,
            0.f, pSin, pCos, 0.f,
            0.f, 0.f, 0.f, 1.f
        };

        float trans_mat[16] =
        {
            1.f,0.f,0.f,0.f,
            0.f,1.f,0.f,0.f,
            0.f,0.f,1.f,0.f,
            -vm_eye.x,-vm_eye.y,-vm_eye.z,1.f
        };

        //-----------------------------------------
        // rot = yaw * pitch
        //-----------------------------------------

        float rot_mat[16];

        MatMul(yaw_mat, pitch_mat, rot_mat);

        //-----------------------------------------
        // view = trans * rot
        //-----------------------------------------

        float view_matrix[16];

        MatMul(trans_mat, rot_mat, view_matrix);

        //-----------------------------------------
        // Write matrix
        //-----------------------------------------

        memcpy((void*)viewMatrixAddr, view_matrix, sizeof(view_matrix));

        Vector4 lookVec = { vm_lookat.x, vm_lookat.y, vm_lookat.z, 1.0f };
        Vector4 eyeVec = { vm_eye.x, vm_eye.y, vm_eye.z, 1.0f };

        memcpy((void*)(moduleBase1 + 0x718C68), &lookVec, sizeof(Vector4));
        memcpy((void*)(moduleBase1 + 0x718C78), &eyeVec, sizeof(Vector4));

        //-----------------------------------------
        // Debug
        //-----------------------------------------

        StatusPrintfFunction("Eye: %.2f %.2f %.2f\n", vm_eye.x, vm_eye.y, vm_eye.z);
        StatusPrintfFunction("Yaw: %.2f Pitch: %.2f\n", yaw, pitch);
    }

    void UpdateLevelEditorCamera_VM(uintptr_t viewMatrixAddr)
    {
        float frameTime = GetFuncFrametimeFunction();
        float deadZone = 0.05f;

        YS::PAD* leftPad = GetHardPadFunction(0);
        YS::PAD* rightPad = GetSoftPadFunction(0);

        const int SHIFT_MASK =
            static_cast<int>(YS::HARDPAD::SHIFT_OFF) |
            static_cast<int>(YS::HARDPAD::SHIFT_ON);

        int buttons = leftPad->Button & ~SHIFT_MASK;

        //-----------------------------------------
        // INPUT
        //-----------------------------------------

        float lx = leftPad->Stick[1].x;
        float ly = leftPad->Stick[1].y;
        float rx = rightPad->Stick[0].x;
        float ry = rightPad->Stick[0].y;

        if (fabs(lx) < deadZone) lx = 0.0f;
        if (fabs(ly) < deadZone) ly = 0.0f;
        if (fabs(rx) < deadZone) rx = 0.0f;
        if (fabs(ry) < deadZone) ry = 0.0f;

        float moveSpeed = FreecamSpeed * frameTime * 0.052359879f;

        //-----------------------------------------
        // 1. MOVE CURSOR (LEFT STICK)
        //-----------------------------------------

        static float orbitYaw = 0.0f;
        static float orbitPitch = 0.0f;
        static float orbitRadius = 200.0f;

        Vector3 forward =
        {
            cosf(orbitPitch) * cosf(orbitYaw),
            sinf(orbitPitch),
            cosf(orbitPitch) * sinf(orbitYaw)
        };

        Vector3 right =
        {
            -forward.z,
            0.0f,
            forward.x
        };

        AxisCursorPos.x += (forward.x * ly + right.x * lx) * moveSpeed;
        AxisCursorPos.y += (forward.y * ly) * moveSpeed;
        AxisCursorPos.z += (forward.z * ly + right.z * lx) * moveSpeed;

        if (buttons & static_cast<int>(YS::HARDPAD::L1))
            AxisCursorPos.y += moveSpeed;

        if (buttons & static_cast<int>(YS::HARDPAD::R1))
            AxisCursorPos.y -= moveSpeed;

        //-----------------------------------------
        // 2. ORBIT CAMERA (RIGHT STICK) — FIXED
        //-----------------------------------------

        // sensitivity
        float rotSpeed = frameTime * 0.8f;

        // response curve (smooth near center)
        auto ApplyCurve = [](float v)
            {
                float sign = (v >= 0.0f) ? 1.0f : -1.0f;
                return sign * powf(fabs(v), 1.5f);
            };

        float rxCurved = ApplyCurve(rx);
        float ryCurved = ApplyCurve(ry);

        // clamp per-frame rotation
        float maxStep = 0.05f;

        float yawStep = std::clamp(-rxCurved * rotSpeed, -maxStep, maxStep);
        float pitchStep = std::clamp(ryCurved * rotSpeed, -maxStep, maxStep);

        orbitYaw += yawStep;
        orbitPitch += pitchStep;

        orbitPitch = std::clamp(orbitPitch, -1.5f, 1.5f);

        //-----------------------------------------
        // 3. BUILD CAMERA FROM ORBIT
        //-----------------------------------------

        float x = orbitRadius * cosf(orbitPitch) * cosf(orbitYaw);
        float y = orbitRadius * sinf(orbitPitch);
        float z = orbitRadius * cosf(orbitPitch) * sinf(orbitYaw);

        vm_eye.x = AxisCursorPos.x + x;
        vm_eye.y = AxisCursorPos.y + y;
        vm_eye.z = AxisCursorPos.z + z;
        vm_eye.w = 1.0f;

        vm_lookat.x = AxisCursorPos.x;
        vm_lookat.y = AxisCursorPos.y;
        vm_lookat.z = AxisCursorPos.z;
        vm_lookat.w = 1.0f;

        //-----------------------------------------
        // 4. SYNC yaw/pitch (for other systems)
        //-----------------------------------------

        float dx = vm_lookat.x - vm_eye.x;
        float dy = vm_lookat.y - vm_eye.y;
        float dz = vm_lookat.z - vm_eye.z;

        yaw = atan2f(dz, dx);
        pitch = asinf(dy / sqrtf(dx * dx + dy * dy + dz * dz));

        //-----------------------------------------
        // 5. WRITE VIEW MATRIX
        //-----------------------------------------

        WriteViewMatrix(viewMatrixAddr);
    }

    void UpdateCinematicCamera_VM(uintptr_t viewMatrixAddr)
    {
        float frameTime = GetFuncFrametimeFunction();

        YS::PAD* leftPad = GetHardPadFunction(0);

        const int SHIFT_MASK =
            static_cast<int>(YS::HARDPAD::SHIFT_OFF) |
            static_cast<int>(YS::HARDPAD::SHIFT_ON);

        int buttons = leftPad->Button & ~SHIFT_MASK;
        int triggers = leftPad->Trigger & ~SHIFT_MASK;

        static int s_prevTriggers = 0;
        int triggersPressed = triggers & ~s_prevTriggers;

        //-----------------------------------------
        // TIMELINE CONTROLS (EDITOR MODE)
        //-----------------------------------------

        if (!g_cinematicCamera.isPlaying())
        {
            if (buttons & static_cast<int>(YS::HARDPAD::LEFT_0))
            {
                g_cineTimelineTime -= frameTime;
                if (g_cineTimelineTime < 0.0f)
                    g_cineTimelineTime = 0.0f;
            }

            if (buttons & static_cast<int>(YS::HARDPAD::RIGHT_0))
            {
                g_cineTimelineTime += frameTime;
            }

            if (triggersPressed & static_cast<int>(YS::HARDPAD::R3))
            {
                // RECORD CURRENT FREECAM STATE
                g_cinematicCamera.addKeyframe(
                    g_cineTimelineTime
                );
            }
        }

        //-----------------------------------------
        // PLAYBACK CONTROLS
        //-----------------------------------------

        if (triggersPressed & static_cast<int>(YS::HARDPAD::CROSS))
        {
            if (g_cinematicCamera.isPlaying())
                g_cinematicCamera.stopPlayback();
            else
                g_cinematicCamera.startPlayback(false);
        }

        if (triggersPressed & static_cast<int>(YS::HARDPAD::TRIANGLE))
        {
            if (g_cinematicCamera.isPlaying())
                g_cinematicCamera.stopPlayback();
            else
                g_cinematicCamera.startPlayback(true);
        }

        if (triggersPressed & static_cast<int>(YS::HARDPAD::CIRCLE))
        {
            g_cinematicCamera.stopPlayback();
        }

        //-----------------------------------------
        // UPDATE PLAYBACK
        //-----------------------------------------

        if (g_cinematicCamera.isPlaying())
        {
            g_cinematicCamera.update(frameTime);

            obj_camera* objcamera = GetOBJCameraInstanceFunction();
            objcamera = reinterpret_cast<obj_camera*>(
                reinterpret_cast<uintptr_t>(objcamera) + 0x08);

            vm_eye = objcamera->m_eye;
            vm_lookat = objcamera->m_at;

            // derive yaw/pitch
            float dx = vm_lookat.x - vm_eye.x;
            float dy = vm_lookat.y - vm_eye.y;
            float dz = vm_lookat.z - vm_eye.z;

            float len = sqrtf(dx * dx + dy * dy + dz * dz);

            if (len > 0.0001f)
            {
                dx /= len;
                dy /= len;
                dz /= len;

                yaw = atan2f(dz, dx);
                pitch = asinf(dy);
            }

            WriteViewMatrix(viewMatrixAddr);

            s_prevTriggers = triggers;
            return;
        }

        //-----------------------------------------
        // EDIT MODE -> fall back to FREECAM
        //-----------------------------------------

        UpdateFreeCameraNormal_VM(viewMatrixAddr);

        s_prevTriggers = triggers;
    }


    void UpdateFreeCamera_ViewMatrix(uintptr_t viewMatrixAddr)
    {
        if (LevelEditorToggle)
        {
            UpdateLevelEditorCamera_VM(viewMatrixAddr);
            return;
        }

        if (CinematicCameraToggle && !g_cineGameplayEnabled)
        {
            UpdateCinematicCamera_VM(viewMatrixAddr);
            return;
        }

        UpdateFreeCameraNormal_VM(viewMatrixAddr);
    }


    






























    void ApplyCameraRotationToEntity(Entity* entity, Matrix4x4& cameraRotation) {
        // Set the entity's rotation matrix to the camera's rotation matrix
        // This will align the entity with the camera's rotation
        entity->Matrix = cameraRotation;
    }

    Matrix4x4 ExtractCameraRotationMatrix(kn_camera* camera) {
        Matrix4x4 rotationMatrix;

        // Extract the rotation part of the camera matrix (assuming it's in the upper-left 3x3 part of the 4x4 matrix)
        rotationMatrix.v0.x = camera->Matrix.v0.x; // Right vector
        rotationMatrix.v0.y = camera->Matrix.v0.y;
        rotationMatrix.v0.z = camera->Matrix.v0.z;

        rotationMatrix.v1.x = camera->Matrix.v1.x; // Up vector
        rotationMatrix.v1.y = camera->Matrix.v1.y;
        rotationMatrix.v1.z = camera->Matrix.v1.z;

        rotationMatrix.v2.x = camera->Matrix.v2.x; // Forward vector (negative Z)
        rotationMatrix.v2.y = camera->Matrix.v2.y;
        rotationMatrix.v2.z = camera->Matrix.v2.z;

        rotationMatrix.v3 = { 0.0f, 0.0f, 0.0f, 1.0f }; // Homogeneous coordinate

        return rotationMatrix;
    }


    void UpdateEntityMovement()
    {
        const float offsetDistance = 225.0f;
        const float cameraHeightOffset = -200.0f;

        Entity* entity = reinterpret_cast<Entity*>(gGlobalObjPtr1);
        if (!entity) return;

        //-----------------------------------------
        // Stable forward from yaw
        //-----------------------------------------

        Vector4 forwardDir;

        forwardDir.x = cosf(pitch) * cosf(yaw);
        forwardDir.y = 0.0f;
        forwardDir.z = cosf(pitch) * sinf(yaw);
        forwardDir.w = 0.0f;

        NormalizeVector4Function(&forwardDir);

        //-----------------------------------------
        // Position
        //-----------------------------------------

        entity->ent_pos.x = vm_eye.x + forwardDir.x * offsetDistance;
        entity->ent_pos.z = vm_eye.z + forwardDir.z * offsetDistance;

        entity->ent_pos.y = vm_eye.y - cameraHeightOffset;
        entity->ent_pos.w = 1.0f;

        //-----------------------------------------
        // Rotation
        //-----------------------------------------

        OBJWishDirFunction(gGlobalObjPtr1, forwardDir);

        StatusPrintfFunction(
            "Entity synced: %.2f %.2f %.2f\n",
            entity->ent_pos.x,
            entity->ent_pos.y,
            entity->ent_pos.z
        );
    }







    void printAddresses(obj_camera* camera) {
        printf("Address of m_at: %p\n", (void*)&camera->m_at);
        printf("Address of m_eye: %p\n", (void*)&camera->m_eye);
        printf("Address of m_fovV: %p\n", (void*)&camera->m_fovV);
        printf("Address of m_fov: %p\n", (void*)&camera->m_fov);
        printf("Address of m_roll: %p\n", (void*)&camera->m_roll);
        printf("Address of m_fixed: %p\n", (void*)&camera->m_fixed);
        printf("Address of m_homeQuick: %p\n", (void*)&camera->m_homeQuick);
        printf("Address of m_kind: %p\n", (void*)&camera->m_kind);
        printf("Address of m_lockon: %p\n", (void*)&camera->m_lockon);
        printf("Address of m_type: %p\n", (void*)&camera->m_type);
        printf("Address of m_mode: %p\n", (void*)&camera->m_mode);
        printf("Address of m_obj: %p\n", (void*)&camera->m_obj);
        printf("Address of m_radius: %p\n", (void*)&camera->m_radius);
        printf("Address of m_yRot: %p\n", (void*)&camera->m_yRot);
        printf("Address of m_backYRot: %p\n", (void*)&camera->m_backYRot);
        printf("Address of m_eyeTarget: %p\n", (void*)&camera->m_eyeTarget);
        printf("Address of m_eyeTargetPrev: %p\n", (void*)&camera->m_eyeTargetPrev);
        printf("Address of m_atTarget: %p\n", (void*)&camera->m_atTarget);
        printf("Address of m_atTargetPrev: %p\n", (void*)&camera->m_atTargetPrev);
        printf("Address of m_fovVTarget: %p\n", (void*)&camera->m_fovVTarget);
        printf("Address of m_fovVTargetPrev: %p\n", (void*)&camera->m_fovVTargetPrev);
        printf("Address of m_Yaw: %p\n", (void*)&camera->m_Yaw);
        printf("Address of m_Pitch: %p\n", (void*)&camera->m_Pitch);
        printf("Address of m_objectiveInitRadius: %p\n", (void*)&camera->m_objectiveInitRadius);
        printf("Address of m_objectiveLockRadius: %p\n", (void*)&camera->m_objectiveLockRadius);
        printf("Address of m_objectiveRadiusMin: %p\n", (void*)&camera->m_objectiveRadiusMin);
        printf("Address of m_objectiveRadiusMax: %p\n", (void*)&camera->m_objectiveRadiusMax);
        printf("Address of m_objectiveUpCurve: %p\n", (void*)&camera->m_objectiveUpCurve);
        printf("Address of m_defaultFov: %p\n", (void*)&camera->m_defaultFov);
        printf("Address of m_defaultRoll: %p\n", (void*)&camera->m_defaultRoll);
    }


    void CreateEntityMovement() {
        uintptr_t CreateOBJCameraFunctionAddress = ReadLEA32(Patterns::pattern_for_CreateObjCamera, L"Create OBJ camera", 0, 1, 5);
        FunctionCalls::tCreateObjCamera CreateOBJCameraFunction = reinterpret_cast<FunctionCalls::tCreateObjCamera>(CreateOBJCameraFunctionAddress);

        uintptr_t IsOBJValidFunctionAddress = ReadLEA32(Patterns::pattern_for_IsObjValid, L"Is OBJ Valid?", 0, 1, 5);
        FunctionCalls::tIsOBJValid IsOBJValidFunction = reinterpret_cast<FunctionCalls::tIsOBJValid>(IsOBJValidFunctionAddress);

        YS::OBJ* obj = GetMenuOBJFunction();
        if (!obj) {
            return;
        }

        gGlobalObjPtr1 = obj;

        GetOBJControlOFFunction(0);
        CommonLayerOFFFunction();

        

        
        
            CreateOBJCameraFunction(gGlobalObjPtr1, 0);
        

    }

    void DestroyEntityMovement() {

        uintptr_t CreateOBJCameraFunctionAddress = ReadLEA32(Patterns::pattern_for_CreateObjCamera, L"Create OBJ camera", 0, 1, 5);
        FunctionCalls::tCreateObjCamera CreateOBJCameraFunction = reinterpret_cast<FunctionCalls::tCreateObjCamera>(CreateOBJCameraFunctionAddress);

        //if (gGlobalObjPtr1 == nullptr) {
        //    gGlobalObjPtr1 = GetMenuOBJFunction();
        //}

        GetOBJControlONFunction(0);
        CommonLayerONFunction();

       YS::OBJ* player = GetPlayerFunction();

       if (player) {
           CreateOBJCameraFunction(player, 0);
       }

    }

    void CreateFreeCamera() {
        //LOG("Attempting to create Free Camera...\n");

        // Get the camera object instance
       // obj_camera* objcamera = GetOBJCameraInstanceFunction();
        //obj_camera* objcamera = reinterpret_cast<obj_camera*>(0x140718C60);


      //  if (objcamera == nullptr) {
  //         LOG("Failed to get camera instance.\n");
      //      return;  // Return early if camera instance is null
      //  }
        
      //  objcamera = reinterpret_cast<obj_camera*>(reinterpret_cast<uintptr_t>(objcamera) + 0x08);



        // Cancel any existing subjective camera settings
        //CancelSubjectiveFunction(30.0);
        
        //YS::OBJ* cameraEntity = *reinterpret_cast<YS::OBJ**>(reinterpret_cast<uintptr_t>(objcamera) + 0x48);

       // Entity* entity = reinterpret_cast<Entity*>(cameraEntity);

        /*if (cameraEntity != nullptr) {
            // The camera entity is valid, perform actions on the entity
            //printf("Camera entity is valid. Performing actions...\n");

            //DirectX::XMVECTOR entPosVec = DirectX::XMLoadFloat4(reinterpret_cast<const DirectX::XMFLOAT4*>(&entity->ent_pos));

            Vector4 entPosVec = entity->ent_pos;
            

            FreecamPos = entPosVec;
            FreecamTargetPos = entPosVec;
        }
        else {
            // The camera entity is invalid (nullptr), handle this case
            FreecamPos = objcamera->m_eye;
            FreecamTargetPos = objcamera->m_eye;
        }*/

       /* if (cameraEntity != nullptr) {
            Vector4 entPosVec = entity->ent_pos;

            FreecamPos = entPosVec;

            // Give a default forward direction to look "ahead"
            FreecamTargetPos.x = entPosVec.x + 1.0f;  // Adjust as needed
            FreecamTargetPos.y = entPosVec.y;
            FreecamTargetPos.z = entPosVec.z;
            FreecamTargetPos.w = 1.0f;
        }
        else {
            FreecamPos = objcamera->m_eye;

            // Same here: give it forward direction
            FreecamTargetPos.x = FreecamPos.x + 1.0f;
            FreecamTargetPos.y = FreecamPos.y;
            FreecamTargetPos.z = FreecamPos.z;
            FreecamTargetPos.w = 1.0f;
        }*/


        //printf("Address of the camera entity: %p\n", (void*)cameraEntity);

        // Set the camera mode to Freecam (11)
       // objcamera->m_mode = 11;
      //  if (objcamera->m_mode != 11) {
           // LOG("Failed to set camera mode to Freecam.\n");
       //     return;  // Return if the camera mode isn't set to 11
      //  }
        //LOG("Camera mode set to Freecam (11).\n");

        //for (YS::OBJ* i = EachOBJFunction(nullptr); i; i = EachOBJFunction(i)) {

       // }

        // Save previous camera positions
       // objcamera->m_atTargetPrev = objcamera->m_atTarget;
        //objcamera->m_eyeTargetPrev = objcamera->m_eyeTarget;
       // objcamera->m_fovVTargetPrev = objcamera->m_fovVTarget;

        // Log previous camera position values (for debugging)
        //LOG("Previous Target: %f, %f, %f", objcamera->m_atTargetPrev.x, objcamera->m_atTargetPrev.y, objcamera->m_atTargetPrev.z);
        //LOG("Previous Eye Target: %f, %f, %f", objcamera->m_eyeTargetPrev.x, objcamera->m_eyeTargetPrev.y, objcamera->m_eyeTargetPrev.z);
        //LOG("Previous FOV Target: %f", objcamera->m_fovVTargetPrev);

        // Set camera mode (using some function, potentially to do other setups)
      //  SetKNCameraModeFunction(0);
        //LOG("KNCamera mode set.\n");

        // Log the successful creation of the free camera
       // LOG("Free Camera created successfully!\n");

        // Disable object control and layers
        GetOBJControlOFFunction(0);
        CommonLayerOFFFunction();
    }

    void CreateCinematicCamera() {
        //LOG("Attempting to create Free Camera...\n");

        // Get the camera object instance
        obj_camera* objcamera = GetOBJCameraInstanceFunction();
        //obj_camera* objcamera = reinterpret_cast<obj_camera*>(0x140718C60);


        if (objcamera == nullptr) {
            LOG("Failed to get camera instance.\n");
            return;  // Return early if camera instance is null
        }

        objcamera = reinterpret_cast<obj_camera*>(reinterpret_cast<uintptr_t>(objcamera) + 0x08);



        // Cancel any existing subjective camera settings
        CancelSubjectiveFunction(30.0);

        YS::OBJ* cameraEntity = *reinterpret_cast<YS::OBJ**>(reinterpret_cast<uintptr_t>(objcamera) + 0x48);

        Entity* entity = reinterpret_cast<Entity*>(cameraEntity);

        /*if (cameraEntity != nullptr) {
            // The camera entity is valid, perform actions on the entity
            //printf("Camera entity is valid. Performing actions...\n");

            //DirectX::XMVECTOR entPosVec = DirectX::XMLoadFloat4(reinterpret_cast<const DirectX::XMFLOAT4*>(&entity->ent_pos));

            Vector4 entPosVec = entity->ent_pos;


            FreecamPos = entPosVec;
            FreecamTargetPos = entPosVec;
        }
        else {
            // The camera entity is invalid (nullptr), handle this case
            FreecamPos = objcamera->m_eye;
            FreecamTargetPos = objcamera->m_eye;
        }*/

        if (cameraEntity != nullptr) {
            Vector4 entPosVec = entity->ent_pos;

            FreecamPos = entPosVec;

            // Give a default forward direction to look "ahead"
            FreecamTargetPos.x = entPosVec.x + 1.0f;  // Adjust as needed
            FreecamTargetPos.y = entPosVec.y;
            FreecamTargetPos.z = entPosVec.z;
            FreecamTargetPos.w = 1.0f;
        }
        else {
            FreecamPos = objcamera->m_eye;

            // Same here: give it forward direction
            FreecamTargetPos.x = FreecamPos.x + 1.0f;
            FreecamTargetPos.y = FreecamPos.y;
            FreecamTargetPos.z = FreecamPos.z;
            FreecamTargetPos.w = 1.0f;
        }


        //printf("Address of the camera entity: %p\n", (void*)cameraEntity);

        // Set the camera mode to Freecam (11)
        objcamera->m_mode = 11;
        if (objcamera->m_mode != 11) {
            // LOG("Failed to set camera mode to Freecam.\n");
            return;  // Return if the camera mode isn't set to 11
        }
        //LOG("Camera mode set to Freecam (11).\n");

        //for (YS::OBJ* i = EachOBJFunction(nullptr); i; i = EachOBJFunction(i)) {

       // }

        // Save previous camera positions
        objcamera->m_atTargetPrev = objcamera->m_atTarget;
        objcamera->m_eyeTargetPrev = objcamera->m_eyeTarget;
        objcamera->m_fovVTargetPrev = objcamera->m_fovVTarget;

        // Log previous camera position values (for debugging)
        //LOG("Previous Target: %f, %f, %f", objcamera->m_atTargetPrev.x, objcamera->m_atTargetPrev.y, objcamera->m_atTargetPrev.z);
        //LOG("Previous Eye Target: %f, %f, %f", objcamera->m_eyeTargetPrev.x, objcamera->m_eyeTargetPrev.y, objcamera->m_eyeTargetPrev.z);
        //LOG("Previous FOV Target: %f", objcamera->m_fovVTargetPrev);

        // Set camera mode (using some function, potentially to do other setups)
        SetKNCameraModeFunction(0);
        //LOG("KNCamera mode set.\n");

        // Log the successful creation of the free camera
       // LOG("Free Camera created successfully!\n");

        // Disable object control and layers
        GetOBJControlOFFunction(0);
        CommonLayerOFFFunction();
    }

    void DestroyFreeCamera() {

       // uintptr_t OBJCameraReturnModeFunctionAddress = FindAndPrintPatternW(Patterns::pattern_for_ObjCameraReturnMode, L"Return OBJ Camera Mode", 0);
       // FunctionCalls::tOBjCameraReturnMode OBJCameraReturnModeFunction = reinterpret_cast<FunctionCalls::tOBjCameraReturnMode>(OBJCameraReturnModeFunctionAddress);

        //RestorePatch(NopCutsceneCamera, originalCutsceneCameraBytes);

      //  obj_camera* cameraInstance = GetOBJCameraInstanceFunction();

        //cameraInstance = reinterpret_cast<obj_camera*>(reinterpret_cast<uintptr_t>(cameraInstance) + 0x08);

        //cameraInstance->m_mode = 0;
      //  OBJCameraReturnModeFunction(cameraInstance);

        GetOBJControlONFunction(0);
        CommonLayerONFunction();
        DBGWINPauseOffFunction();

    }

    void DestroyCinematicCamera() {

        uintptr_t OBJCameraReturnModeFunctionAddress = FindAndPrintPatternW(Patterns::pattern_for_ObjCameraReturnMode, L"Return OBJ Camera Mode", 0);
        FunctionCalls::tOBjCameraReturnMode OBJCameraReturnModeFunction = reinterpret_cast<FunctionCalls::tOBjCameraReturnMode>(OBJCameraReturnModeFunctionAddress);

        //RestorePatch(NopCutsceneCamera, originalCutsceneCameraBytes);

        obj_camera* cameraInstance = GetOBJCameraInstanceFunction();

        //cameraInstance = reinterpret_cast<obj_camera*>(reinterpret_cast<uintptr_t>(cameraInstance) + 0x08);

        //cameraInstance->m_mode = 0;
        OBJCameraReturnModeFunction(cameraInstance);

        GetOBJControlONFunction(0);
        CommonLayerONFunction();
        DBGWINPauseOffFunction();

    }

    

    void CinematicCameraOffCallback(int selectedItemIndex) {
        CinematicCameraToggle = false;

        DestroyFreeCamera();
    }



    void FreeCameraOnCallback(int selectedItemIndex) {
        FreeCameraToggle = true;

        

    }

    void FreeCameraOffCallback(int selectedItemIndex) {
        FreeCameraToggle = false;

        DestroyFreeCamera();

    }

    YS::OBJ* GetLastSpawnedEntityPointer() {
        if (!g_spawnedEntities.empty()) {
            return g_spawnedEntities.back().obj;
        }
        return nullptr;
    }

    void RestoreSpawnedEntities() {
        uintptr_t IsOBJValidFunctionAddress = ReadLEA32(Patterns::pattern_for_IsObjValid, L"Is OBJ Valid?", 0, 1, 5);
        FunctionCalls::tIsOBJValid IsOBJValidFunction = reinterpret_cast<FunctionCalls::tIsOBJValid>(IsOBJValidFunctionAddress);

        for (auto& data : g_spawnedEntities) {
            if (!data.isDeleted && !IsOBJValidFunction(data.obj)) {
                AxisCursorPos = Vector4{ data.originalPosition.x, data.originalPosition.y, data.originalPosition.z, 1.0f };
                g_SpawnRotation = data.originalRotation;
                myObjEntryId = data.entryId;

                NewLevelEditorObj(myObjEntryId, 1);

                // Update the pointer to the new object
                data.obj = GetLastSpawnedEntityPointer();
                data.isDeleted = false;
            }
            else if (IsOBJValidFunction(data.obj)) {
                Entity* entity = ToEntity(data.obj);
                entity->ent_pos = data.originalPosition;
                *(float*)((char*)data.obj + 0x7A8) = data.originalRotation;
            }
        }
    }

    void LevelEditorOnCallback(int selectedItemIndex) {
        LevelEditorToggle = true;
        DBGWINPauseFunction();

        if (previewEntityId != -1) {
            
            NewLevelEditorPreviewActor(previewEntityId, 1);
        }

    }

    void LevelEditorOffCallback(int selectedItemIndex) {
        LevelEditorToggle = false;
        DBGWINPauseOffFunction();

        // Delete preview entity if it exists
        if (isPreviewSpawned && previewObject) {
            DestroyobjFunction(previewObject);
            RealDestroyobjFunction(previewObject);
            previewObject = nullptr;
            previewEntity = nullptr;
            isPreviewSpawned = false;
        }

    }

    void MultiCameraONCallback(int selectedItemIndex) {
        createmulticamera();
    }

    void PartyModeOnCallback(int selectedItemIndex)
    {
        printf("[PartyMode ON] entered (index=%d)\n", selectedItemIndex);

        uintptr_t addr = ReadLEA32(
            Patterns::pattern_for_GetMapFogDefaultColors,
            L"Get Map Fog Default Colors",
            0, 1, 5
        );

        printf("[PartyMode ON] fog func addr = 0x%p\n", (void*)addr);

        if (!addr)
        {
            printf("[PartyMode ON] ERROR: fog function address is null\n");
            return;
        }

        FunctionCalls::tGetMapFogDefaultColors GetMapFogDefaultColorsFunction =
            reinterpret_cast<FunctionCalls::tGetMapFogDefaultColors>(addr);

        printf("[PartyMode ON] calling fog function...\n");

        gCurrentFog = GetMapFogDefaultColorsFunction();

        printf("[PartyMode ON] fog ptr = %p\n", gCurrentFog);

        if (gCurrentFog)
        {
            printf(
                "[PartyMode ON] fog values: "
                "RGBA=(%u,%u,%u,%u) near=%f far=%f min=%f max=%f\n",
                gCurrentFog->m_color.R,
                gCurrentFog->m_color.G,
                gCurrentFog->m_color.B,
                gCurrentFog->m_color.A,
                gCurrentFog->m_near,
                gCurrentFog->m_far,
                gCurrentFog->m_min,
                gCurrentFog->m_max
            );
        }
        else
        {
            printf("[PartyMode ON] fog is NULL (map has no fog?)\n");
        }

        if (!originalFogSaved && gCurrentFog)
        {
            originalFog = *gCurrentFog;
            originalFogSaved = true;
            printf("[PartyMode ON] original fog saved\n");
        }

        PartyModeToggle = true;
        printf("[PartyMode ON] PartyModeToggle = true\n");
    }


    void PartyModeOffCallback(int selectedItemIndex)
    {
        printf("[PartyMode OFF] entered (index=%d)\n", selectedItemIndex);

        uintptr_t addr = ReadLEA32(
            Patterns::pattern_for_GetMapFogDefaultColors,
            L"Get Map Fog Default Colors",
            0, 1, 5
        );

        printf("[PartyMode OFF] fog func addr = 0x%p\n", (void*)addr);

        if (!addr)
        {
            printf("[PartyMode OFF] ERROR: fog function address is null\n");
            return;
        }

        FunctionCalls::tGetMapFogDefaultColors GetMapFogDefaultColorsFunction =
            reinterpret_cast<FunctionCalls::tGetMapFogDefaultColors>(addr);

        printf("[PartyMode OFF] calling fog function...\n");

        gCurrentFog = GetMapFogDefaultColorsFunction();

        printf("[PartyMode OFF] fog ptr = %p\n", gCurrentFog);

        if (gCurrentFog && originalFogSaved)
        {
            printf("[PartyMode OFF] restoring original fog...\n");

            *gCurrentFog = originalFog;

            printf("[PartyMode OFF] fog restored\n");
        }
        else
        {
            printf(
                "[PartyMode OFF] restore skipped (fog=%p saved=%d)\n",
                gCurrentFog,
                originalFogSaved
            );
        }

        PartyModeToggle = false;
        printf("[PartyMode OFF] PartyModeToggle = false\n");
    }


    void EntityMovementOnCallback(int selectedItemIndex) {
        uintptr_t IsOBJValidFunctionAddress = ReadLEA32(Patterns::pattern_for_IsObjValid, L"Is OBJ Valid?", 0, 1, 5);
        FunctionCalls::tIsOBJValid IsOBJValidFunction = reinterpret_cast<FunctionCalls::tIsOBJValid>(IsOBJValidFunctionAddress);

        YS::OBJ* obj = GetMenuOBJFunction();
        if (!obj) {
            return;
        }

        gGlobalObjPtr1 = obj;
        
            
        

        EntityMovementToggle = true;

        
    }

    void ChaosModOnCallback(int selectedItemIndex) {

        //uintptr_t OpenCharacterLevelUpInformationWindowFunctionAddress = ReadLEA32(Patterns::pattern_for_OpenCharacterLevelUpInformationWindow, L" Open Character Level Up Information Window", 0,1,5);
        //FunctionCalls::tOpenCharacterLevelUpInformationWindow OpenCharacterLevelUpInformationWindowFunction = reinterpret_cast<FunctionCalls::tOpenCharacterLevelUpInformationWindow>(OpenCharacterLevelUpInformationWindowFunctionAddress);
        

        if (!chaosManager.IsChaosEnabled()) {
            if (!chaosManager.IsInitialized()) {
                effectManager.Init();   // Register all effects
                chaosManager.Init();    // Setup chaos manager (enable chaos, reset timers)
                chaosManager.SetInitialized(true);  // Mark initialized
            }
            chaosManager.EnableChaos(true);
            LOG(L"Chaos Mod turned ON");

            // Start Twitch voting
            StartTwitchChatThread(
                WStringToString(twitchConfig.oauth_token),
                WStringToString(twitchConfig.username),
                WStringToString(twitchConfig.channel)
            );

            /*std::vector<uint8_t> name = KHSCII::ToKHSCII("");
            std::vector<uint8_t> title = KHSCII::ToKHSCII("ChaosMod is now enabled");
            std::vector<uint8_t> desc = KHSCII::ToKHSCII("");

            OpenCharacterLevelUpInformationWindowFunction(
                reinterpret_cast<const char*>(name.data()),
                reinterpret_cast<const char*>(title.data()),
                reinterpret_cast<const char*>(desc.data()),
                99,                      // Level
                2                        // WinColor
            );*/

        }
        else {
            LOG(L"Chaos Mod was already ON");
        }
    }

    void ChaosModOffCallback(int selectedItemIndex) {

        uintptr_t OpenCharacterLevelUpInformationWindowFunctionAddress = ReadLEA32(Patterns::pattern_for_OpenCharacterLevelUpInformationWindow, L" Open Character Level Up Information Window", 0, 1, 5);
        FunctionCalls::tOpenCharacterLevelUpInformationWindow OpenCharacterLevelUpInformationWindowFunction = reinterpret_cast<FunctionCalls::tOpenCharacterLevelUpInformationWindow>(OpenCharacterLevelUpInformationWindowFunctionAddress);

        if (chaosManager.IsChaosEnabled()) {
            chaosManager.EnableChaos(false);
            LOG(L"Chaos Mod turned OFF");

            StopTwitchChatThread();
            voteManager.ForceStopVoting();

            /*std::vector<uint8_t> name = KHSCII::ToKHSCII("");
            std::vector<uint8_t> title = KHSCII::ToKHSCII("ChaosMod is now disabled");
            std::vector<uint8_t> desc = KHSCII::ToKHSCII("");

            OpenCharacterLevelUpInformationWindowFunction(
                reinterpret_cast<const char*>(name.data()),
                reinterpret_cast<const char*>(title.data()),
                reinterpret_cast<const char*>(desc.data()),
                0,                      // Level
                0                        // WinColor
            );*/

        }
        else {
            LOG(L"Chaos Mod was already OFF");
        }
    }



    void EntityMovementOffCallback(int selectedItemIndex) {
        EntityMovementToggle = false;

        DestroyEntityMovement();

    }

    void CheckRoomLoop() {
        


        InitializePointers();
        uintptr_t GetPlayerFunctionAddress = ReadLEA32(Patterns::pattern_for_GetPlayer, L"Get Player", 0, 1, 5);
        uintptr_t GetSoftPADFunctionAddress = ReadLEA32(Patterns::pattern_for_GetSoftPad, L"Get Soft PAD", 0, 1, 5);
        uintptr_t GetOBJENTRYFunctionAddress = ReadLEA32(Patterns::pattern_for_GetOBJENTRY, L"Get OBJENTRY", 0, 1, 5);
        uintptr_t EachOBJFunctionAddress = ReadLEA32(Patterns::pattern_for_EachOBJ, L"Each OBJ", 0, 1, 5);
        uintptr_t GetOBJEntryNameFunctionAddress = ReadLEA32(Patterns::pattern_for_GetObjEntryName, L"Get OBJ EntryName", 0, 1, 5);
        uintptr_t InitializePlayerFunctionAddress = FindAndPrintPatternW(Patterns::pattern_for_InitializePlayerCommand, L"Initialize Player Command", 0);
        FunctionCalls::tGetPlayer GetPlayerFunction = reinterpret_cast<FunctionCalls::tGetPlayer>(GetPlayerFunctionAddress);
        FunctionCalls::tGetOBJENTRY GetOBJENTRYFunction = reinterpret_cast<FunctionCalls::tGetOBJENTRY>(GetOBJENTRYFunctionAddress);
        FunctionCalls::tEachObj EachOBJFunction = reinterpret_cast<FunctionCalls::tEachObj>(EachOBJFunctionAddress);
        FunctionCalls::tGetObjEntryName GetOBJEntryNameFunction = reinterpret_cast<FunctionCalls::tGetObjEntryName>(GetOBJEntryNameFunctionAddress);
        FunctionCalls::tGetSoftPad GetSoftPADFunction = reinterpret_cast<FunctionCalls::tGetSoftPad>(GetSoftPADFunctionAddress);
        FunctionCalls::tInitializePlayerCommand InitializePlayerCommandFunction = reinterpret_cast<FunctionCalls::tInitializePlayerCommand>(InitializePlayerFunctionAddress);

        bool hasEnteredArea = false;
        //bool isInArea = (*(bool*)(SteamPointer - 0x152CC8));
        bool isInArea = false;



        while (1) {
            //uintptr_t SteamPointer = reinterpret_cast<uintptr_t>(baseModule) + 0xB0D598;
            //uintptr_t EpicPointer = reinterpret_cast<uintptr_t>(baseModule) + 0xB0D018;
            //LOG("Detected Game Version: %s\n", DetectGameVersion().c_str());  // Ensure gameVersion is correct

            if (gameVersion == "Steam") {
                isInArea = (*(bool*)(SteamPointer - 0x152CC8));

            }
            else if (gameVersion == "Epic") {
                isInArea = (*(bool*)(EpicPointer - 0x152CC8));
            }



            //LOG("isInArea: %d\n", isInArea);

            if (isInArea && !hasEnteredArea) {
                hasEnteredArea = true;
                //AREA area;
                //ReadSteamAreaPointer(area);
                ReadUnitPointers();




                    // Get the base module address (current executable)
                    //HMODULE baseModule = GetModuleHandle(NULL); // Get the base address of the current module
                

                   // uintptr_t baseAddress = reinterpret_cast<uintptr_t>(baseModule); // Convert HMODULE to uintptr_t

                   // YS::OBJ* player = GetPlayerFunction(); // Get the player structure directly

                    // Define the offset to the player structure
                   // const uintptr_t offset = 0xDB8; // Offset from the base address

                    // Calculate the player structure address
                   // uintptr_t playerStructureAddress = reinterpret_cast<uintptr_t>(player) + offset; // Add the offset to the player pointer

                   // uintptr_t value = baseAddress + 0xBF3208;
                    // Cast to the appropriate structure type
                   // YS::OBJ* playerStruct = reinterpret_cast<YS::OBJ*>(playerStructureAddress); // Cast to YS::OBJ*

                    // Write the value to the appropriate member of the player structure
                   // *reinterpret_cast<uintptr_t*>(reinterpret_cast<uintptr_t>(playerStruct) + offset) = value; // Write value to player structure
                    //uintptr_t valueAtPlayerIndication = *playerIndication;

                    //InitializePlayerCommandFunction((YS::PLAYER_COMMAND*)playerIndication, (YS::PLAYER*)GetPlayerFunction, GetSoftPADFunction(1));

                    //NewObj(0x5A, 1); // Spawn the enemy
                    //NewSora(0x54);
                    //printf("Spawned Terra Ally!!\n");

                    // Use the value or log it
                
            }
                
                if (!isInArea && hasEnteredArea) {
                    hasEnteredArea = false; // Reset the "entered area" flag
                    LOG("AREA::Exit\n");
                }

                // Your object searching and team assignment code remains unchanged


                /*YS::OBJ* attacker = nullptr;
                YS::OBJ* objPtr = EachOBJFunction(nullptr);

                while (objPtr) {
                    const char* entryName = GetOBJEntryNameFunction((YS::OBJ*)objPtr);

                    if (strcmp(entryName, "P_EX110") == 0) {
                        attacker = objPtr;

                        //uintptr_t* targetAddress = reinterpret_cast<uintptr_t*>(reinterpret_cast<uintptr_t>(attacker) + 0xDB8);
                        //uintptr_t valueToWrite = SteamPointer + 0xE5C70;

                        //LOG(L"Writing to address: %p, Value: %p\n", targetAddress, valueToWrite);
                        //LOG("Address of Entity: %p", attacker);

                        // Perform the write operation
                        //YS::PAD* pad = GetSoftPADFunction(1); // Call the function to get the pad pointer

                        //uintptr_t* targetAddress = reinterpret_cast<uintptr_t*>(reinterpret_cast<uintptr_t>(attacker) + 0xDB8);
                        //*targetAddress = reinterpret_cast<uintptr_t>(pad); // Cast the YS::PAD* to uintptr_t
                    }

                    objPtr = EachOBJFunction(objPtr);
                }

                // Sleep for 100ms before the next iteration
                std::this_thread::sleep_for(std::chrono::milliseconds(100));*/
            }
        }

        void ReadFileToMemory(const char* filename, void* destination, size_t size)
        {
            std::ifstream file(filename, std::ios::binary);
            if (!file)
            {
                // Handle file open error
                return;
            }

            file.read(static_cast<char*>(destination), size);
            file.close();
        }

        void ObjectTexanmCallBack(int n) {

            uintptr_t ObjTexAnmFunctionAddress = ReadLEA32(Patterns::pattern_for_ObjTexanmStart, L"obj texanm start", 0, 1, 5);
            FunctionCalls::tOBJtexanmStart ObjTexAnmFunction = reinterpret_cast<FunctionCalls::tOBJtexanmStart>(ObjTexAnmFunctionAddress);

            //YS::OBJ* obj = GetCachedMenuOBJ();

            //if (!obj) {
           //     return;
           // }
            
            gObjectTexanmID = n;
            
                ObjTexAnmFunction(gGlobalObjPtr1, n);
            
        }


        void ObjectScaleCallBack(int n) {

            //gGlobalObjPtr = GetMenuOBJFunction();

            //std::cout << "gGlobalObjPtr at callback: " << gGlobalObjPtr << std::endl;

           // YS::OBJ* obj = GetCachedMenuOBJ();

           // if (!obj) {
           //     return;
          //  }


            switch (n) {
            case 0:
                if (gGlobalObjPtr1 != nullptr) {
                    // Calculate the addresses of x, y, z, w components relative to the object pointer
                    float* scaleX = reinterpret_cast<float*>(reinterpret_cast<char*>(gGlobalObjPtr1) + 0x30);
                    float* scaleY = reinterpret_cast<float*>(reinterpret_cast<char*>(gGlobalObjPtr1) + 0x34);
                    float* scaleZ = reinterpret_cast<float*>(reinterpret_cast<char*>(gGlobalObjPtr1) + 0x38);
                    float* scaleW = reinterpret_cast<float*>(reinterpret_cast<char*>(gGlobalObjPtr1) + 0x3C);

                    // Assign values to the x, y, z, w components
                    *scaleX = 1.0f;
                    *scaleY = 1.0f;
                    *scaleZ = 1.0f;
                    *scaleW = 1.0f;
                }
                gObjectScale = 0;
                break;
            case 1:
                if (gGlobalObjPtr1 != nullptr) {
                    // Calculate the addresses of x, y, z, w components relative to the object pointer
                    float* scaleX = reinterpret_cast<float*>(reinterpret_cast<char*>(gGlobalObjPtr1) + 0x30);
                    float* scaleY = reinterpret_cast<float*>(reinterpret_cast<char*>(gGlobalObjPtr1) + 0x34);
                    float* scaleZ = reinterpret_cast<float*>(reinterpret_cast<char*>(gGlobalObjPtr1) + 0x38);
                    float* scaleW = reinterpret_cast<float*>(reinterpret_cast<char*>(gGlobalObjPtr1) + 0x3C);

                    // Assign values to the x, y, z, w components
                    *scaleX = 2.0f;
                    *scaleY = 2.0f;
                    *scaleZ = 2.0f;
                }
                gObjectScale = 1;
                break;
            case 2:
                if (gGlobalObjPtr1 != nullptr) {
                    // Calculate the addresses of x, y, z, w components relative to the object pointer
                    float* scaleX = reinterpret_cast<float*>(reinterpret_cast<char*>(gGlobalObjPtr1) + 0x30);
                    float* scaleY = reinterpret_cast<float*>(reinterpret_cast<char*>(gGlobalObjPtr1) + 0x34);
                    float* scaleZ = reinterpret_cast<float*>(reinterpret_cast<char*>(gGlobalObjPtr1) + 0x38);
                    float* scaleW = reinterpret_cast<float*>(reinterpret_cast<char*>(gGlobalObjPtr1) + 0x3C);

                    // Assign values to the x, y, z, w components
                    *scaleX = 4.0f;
                    *scaleY = 4.0f;
                    *scaleZ = 4.0f;
                }
                gObjectScale = 2;
                break;
            case 3:
                if (gGlobalObjPtr1 != nullptr) {
                    // Calculate the addresses of x, y, z, w components relative to the object pointer
                    float* scaleX = reinterpret_cast<float*>(reinterpret_cast<char*>(gGlobalObjPtr1) + 0x30);
                    float* scaleY = reinterpret_cast<float*>(reinterpret_cast<char*>(gGlobalObjPtr1) + 0x34);
                    float* scaleZ = reinterpret_cast<float*>(reinterpret_cast<char*>(gGlobalObjPtr1) + 0x38);
                    float* scaleW = reinterpret_cast<float*>(reinterpret_cast<char*>(gGlobalObjPtr1) + 0x3C);

                    // Assign values to the x, y, z, w components
                    *scaleX = 5.0f;
                    *scaleY = 5.0f;
                    *scaleZ = 5.0f;
                }
                gObjectScale = 3;
                break;
            case 4:
                if (gGlobalObjPtr1 != nullptr) {
                    // Calculate the addresses of x, y, z, w components relative to the object pointer
                    float* scaleX = reinterpret_cast<float*>(reinterpret_cast<char*>(gGlobalObjPtr1) + 0x30);
                    float* scaleY = reinterpret_cast<float*>(reinterpret_cast<char*>(gGlobalObjPtr1) + 0x34);
                    float* scaleZ = reinterpret_cast<float*>(reinterpret_cast<char*>(gGlobalObjPtr1) + 0x38);
                    float* scaleW = reinterpret_cast<float*>(reinterpret_cast<char*>(gGlobalObjPtr1) + 0x3C);

                    // Assign values to the x, y, z, w components
                    *scaleX = 5.5f;
                    *scaleY = 5.5f;
                    *scaleZ = 5.5f;
                }
                gObjectScale = 4;
                break;
            case 5:
                if (gGlobalObjPtr1 != nullptr) {
                    // Calculate the addresses of x, y, z, w components relative to the object pointer
                    float* scaleX = reinterpret_cast<float*>(reinterpret_cast<char*>(gGlobalObjPtr1) + 0x30);
                    float* scaleY = reinterpret_cast<float*>(reinterpret_cast<char*>(gGlobalObjPtr1) + 0x34);
                    float* scaleZ = reinterpret_cast<float*>(reinterpret_cast<char*>(gGlobalObjPtr1) + 0x38);
                    float* scaleW = reinterpret_cast<float*>(reinterpret_cast<char*>(gGlobalObjPtr1) + 0x3C);

                    // Assign values to the x, y, z, w components
                    *scaleX = 6.0f;
                    *scaleY = 6.0f;
                    *scaleZ = 6.0f;
                }
                gObjectScale = 5;
                break;
            case 6:
                if (gGlobalObjPtr1 != nullptr) {
                    // Calculate the addresses of x, y, z, w components relative to the object pointer
                    float* scaleX = reinterpret_cast<float*>(reinterpret_cast<char*>(gGlobalObjPtr1) + 0x30);
                    float* scaleY = reinterpret_cast<float*>(reinterpret_cast<char*>(gGlobalObjPtr1) + 0x34);
                    float* scaleZ = reinterpret_cast<float*>(reinterpret_cast<char*>(gGlobalObjPtr1) + 0x38);
                    float* scaleW = reinterpret_cast<float*>(reinterpret_cast<char*>(gGlobalObjPtr1) + 0x3C);

                    // Assign values to the x, y, z, w components
                    *scaleX = 6.5f;
                    *scaleY = 6.5f;
                    *scaleZ = 6.5f;
                }
                gObjectScale = 6;
                break;
            case 7:
                if (gGlobalObjPtr1 != nullptr) {
                    // Calculate the addresses of x, y, z, w components relative to the object pointer
                    float* scaleX = reinterpret_cast<float*>(reinterpret_cast<char*>(gGlobalObjPtr1) + 0x30);
                    float* scaleY = reinterpret_cast<float*>(reinterpret_cast<char*>(gGlobalObjPtr1) + 0x34);
                    float* scaleZ = reinterpret_cast<float*>(reinterpret_cast<char*>(gGlobalObjPtr1) + 0x38);
                    float* scaleW = reinterpret_cast<float*>(reinterpret_cast<char*>(gGlobalObjPtr1) + 0x3C);

                    // Assign values to the x, y, z, w components
                    *scaleX = 7.0f;
                    *scaleY = 7.0f;
                    *scaleZ = 7.0f;
                }
                gObjectScale = 7;
                break;
            case 8:
                if (gGlobalObjPtr1 != nullptr) {
                    // Calculate the addresses of x, y, z, w components relative to the object pointer
                    float* scaleX = reinterpret_cast<float*>(reinterpret_cast<char*>(gGlobalObjPtr1) + 0x30);
                    float* scaleY = reinterpret_cast<float*>(reinterpret_cast<char*>(gGlobalObjPtr1) + 0x34);
                    float* scaleZ = reinterpret_cast<float*>(reinterpret_cast<char*>(gGlobalObjPtr1) + 0x38);
                    float* scaleW = reinterpret_cast<float*>(reinterpret_cast<char*>(gGlobalObjPtr1) + 0x3C);

                    // Assign values to the x, y, z, w components
                    *scaleX = 9.8999996f;
                    *scaleY = 9.8999996f;
                    *scaleZ = 9.8999996f;
                }
                gObjectScale = 8;
                break;
            default:
                std::cout << "The number is not 0, 1, 2, 3, 4, 5, 6, 7, or 8." << std::endl;
                break;
            }

        }
    

    void GetServerIPWindowCallback(int selectedItemIndex) {

        uintptr_t OpenSingletonWindowFunctionAddress = ReadLEA32(Patterns::pattern_for_OpenSingletonWindow, L"OpenSingletonWindow", 0, 1, 5);

        uintptr_t AllocYWMemoryFunctionAddress = ReadLEA32(Patterns::pattern_for_Alloc_YW_Memory, L"AllocYWMemory", 0, 1, 5);

        uintptr_t WListFunctionAddress = ReadLEA32(Patterns::pattern_for_YWList, L"YWList", 0, 1, 5);


        uintptr_t YWFrameFunctionAddress = ReadLEA32(Patterns::pattern_for_YWFrame, L"YWFrame", 0, 1, 5);


        uintptr_t ContainerLinkFunctionAddress = FindAndPrintPatternW(Patterns::pattern_for_ContainerLink, L"ContainerLink", 0);//muuta tohon FindAndPrintPatternW 0


        uintptr_t YWwindowMinSizeFunctionAddress = FindAndPrintPatternW(Patterns::pattern_for_YWwindow_minSize, L"YWwindow Min Size", 0);// muuta tohon FindAndPrintPatternW 0


        uintptr_t YWwindowMaxSizeFunctionAddress = FindAndPrintPatternW(Patterns::pattern_for_YWwindow_maxSize, L"YWwindow Max Size", 0);


        uintptr_t ShowDebugWindowFunctionAddress = ReadLEA32(Patterns::pattern_for_ShowDebugWindow, L"ShowDebugWindow", 0, 1, 5);


        //uintptr_t DebugWindowAddSubMenuFunctionAddress = ReadLEA32(Patterns::pattern_for_DebugWindowAddSubMenu, L"Add Debug Submenu", 0, 1, 5);





        FunctionCalls::tAlloc_YW_Memory AllocYWMemoryFunction = reinterpret_cast<FunctionCalls::tAlloc_YW_Memory>(AllocYWMemoryFunctionAddress);
        FunctionCalls::tYWList YWListFunction = reinterpret_cast<FunctionCalls::tYWList>(WListFunctionAddress);
        FunctionCalls::tYWFrame YWFrameFunction = reinterpret_cast<FunctionCalls::tYWFrame>(YWFrameFunctionAddress);
        FunctionCalls::tContainerLink ContainerLinkFunction = reinterpret_cast<FunctionCalls::tContainerLink>(ContainerLinkFunctionAddress);
        FunctionCalls::tYW_WINDOW_set_min_size YWwindowMinSizeFunction = reinterpret_cast<FunctionCalls::tYW_WINDOW_set_min_size>(YWwindowMinSizeFunctionAddress);
        FunctionCalls::tYW_WINDOW_set_max_size YWwindowMaxSizeFunction = reinterpret_cast<FunctionCalls::tYW_WINDOW_set_max_size>(YWwindowMaxSizeFunctionAddress);
        FunctionCalls::tShowDebugWindow ShowDebugWindowFunction = reinterpret_cast<FunctionCalls::tShowDebugWindow>(ShowDebugWindowFunctionAddress);

        FunctionCalls::tOpenSingletonWindow OpenSingletonWindowFunction = reinterpret_cast<FunctionCalls::tOpenSingletonWindow>(OpenSingletonWindowFunctionAddress);


        YW::YW_WINDOW_39* Menu = OpenSingletonWindowFunction("Custom Music Player", 200, 300, 2);

        auto allocYWMemSlideBar = AllocYWMemoryFunction((YW::MEMORY*)0x200);

        if (!editor) {
            editor = new YW::FLOAT_EDITOR(0.0f, 100.0f, 1.0f, 50.0f, 100, 100, 200, 20);
        }

        editor->Render();

        ContainerLinkFunction((YW::CONTAINER*)Menu, (YW::WIDGET*)editor);

        ShowDebugWindowFunction((YW::WIDGET*)Menu);

        //P2P::GetLocalIPAddress();
        //AREA* areaPointer = ReadSteamPointer(); // Assuming this returns AREA*
        //ReadSteamAreaPointer();

        /*InitializePointers();
        uintptr_t GetPlayerFunctionAddress = ReadLEA32(Patterns::pattern_for_GetPlayer, L"Get Player", 0, 1, 5);
        uintptr_t GetSoftPADFunctionAddress = ReadLEA32(Patterns::pattern_for_GetSoftPad, L"Get Soft PAD", 0, 1, 5);
        uintptr_t OBJENTRY_APPEARFunctionAddress = ReadLEA32(Patterns::pattern_for_OBJENTRY_APPEAR, L"OBJENTRY APPEAR", 0, 1, 5);
        uintptr_t InitPlayerCommandFunctionAddress = FindAndPrintPatternW(Patterns::pattern_for_InitializePlayerCommand, L"Init Player Command", 0);
        uintptr_t InitFieldCommandFunctionAddress = FindAndPrintPatternW(Patterns::pattern_for_InitializeFieldCommand, L"Init Field Command", 0);
        uintptr_t PlayerOVRMakeMovementFunctionAddress = FindAndPrintPatternW(Patterns::pattern_for_PlayerOVRMakeMovement, L"Player OVR Make Movement", 0);

        FunctionCalls::tOBJENTRY_APPEAR OBJENTRY_APPEARFunction = reinterpret_cast<FunctionCalls::tOBJENTRY_APPEAR>(OBJENTRY_APPEARFunctionAddress);
        FunctionCalls::tInitializePlayerCommand InitPlayerCommandFunction = reinterpret_cast<FunctionCalls::tInitializePlayerCommand>(InitPlayerCommandFunctionAddress);
        FunctionCalls::tInitializeFieldCommand InitializeFieldCommandFunction = reinterpret_cast<FunctionCalls::tInitializeFieldCommand>(InitFieldCommandFunctionAddress);
        FunctionCalls::tGetPlayer GetPlayerFunction = reinterpret_cast<FunctionCalls::tGetPlayer>(GetPlayerFunctionAddress);
        FunctionCalls::tGetSoftPad GetSoftPADFunction = reinterpret_cast<FunctionCalls::tGetSoftPad>(GetSoftPADFunctionAddress);
        FunctionCalls::tPlayerOVRMakeMovement PlayerOVRMakeMovementFunction = reinterpret_cast<FunctionCalls::tPlayerOVRMakeMovement>(PlayerOVRMakeMovementFunctionAddress);
        //uintptr_t* PlayerIndicator = NULL;

        //if (gameVersion == "Steam") {
       //     PlayerIndicator = ((uintptr_t*)SteamPointer + 0x1F03088);

      //  }
       // else if (gameVersion == "Epic") {
        //    PlayerIndicator = ((uintptr_t*)EpicPointer + 0x1F03088);
       // }
        //if (GetPlayerFunction()) {

         //   NewSora(0x54);
            //PlayerOVRMakeMovementFunction((YS::PLAYER*)GetPlayerFunction);
        //}
        //uintptr_t PlayerIndicator = NULL;
        //YS::PAD* pad = GetSoftPADFunction(1);
        //InitializeFieldCommandFunction((YS::FIELD_COMMAND*)&PlayerIndicator, (YS::PLAYER*)GetPlayerFunction, pad);
        //InitPlayerCommandFunction(())
        std::thread roomLoopThread(CheckRoomLoop);

        roomLoopThread.detach();
        //HookSoftPad();*/
       // InitializePointers();
       // uintptr_t PaxAddress = 0;//updated to steam


       // if (DetectGameVersion() == "Steam") {
      //      PaxAddress = (SteamPointer + 0x1AF2A68);

       // }
       // else if (DetectGameVersion() == "Epic") {
       //     PaxAddress = (EpicPointer + 0x1AF2A68);
       // }

        //ReadFileToMemory("C:\\eh_event_301.pax", reinterpret_cast<void*>(PaxAddress), 0x451C0);



    }

    void ShowRealConfigWindow(unsigned int* arg) {

        uintptr_t OpenSingletonWindowFunctionAddress = ReadLEA32(Patterns::pattern_for_OpenSingletonWindow, L"OpenSingletonWindow", 0, 1, 5);
        FunctionCalls::tOpenSingletonWindow OpenSingletonWindowFunction = reinterpret_cast<FunctionCalls::tOpenSingletonWindow>(OpenSingletonWindowFunctionAddress);

        uintptr_t AllocYWMemoryFunctionAddress = ReadLEA32(Patterns::pattern_for_Alloc_YW_Memory, L"AllocYWMemory", 0, 1, 5);
        FunctionCalls::tAlloc_YW_Memory AllocYWMemoryFunction = reinterpret_cast<FunctionCalls::tAlloc_YW_Memory>(AllocYWMemoryFunctionAddress);

        uintptr_t ConfigWidgetFunctionAddress = FindAndPrintPatternW(Patterns::pattern_for_ConfigWidget, L"Config Widget", 0);

        FunctionCalls::tConfigWidget ConfigWidgetFunction = reinterpret_cast<FunctionCalls::tConfigWidget>(ConfigWidgetFunctionAddress);

        uintptr_t ContainerLinkFunctionAddress = FindAndPrintPatternW(Patterns::pattern_for_ContainerLink, L"ContainerLink", 0);

        FunctionCalls::tContainerLink ContainerLinkFunction = reinterpret_cast<FunctionCalls::tContainerLink>(ContainerLinkFunctionAddress);

        uintptr_t ShowDebugWindowFunctionAddress = ReadLEA32(Patterns::pattern_for_ShowDebugWindow, L"ShowDebugWindow", 0, 1, 5);

        FunctionCalls::tShowDebugWindow ShowDebugWindowFunction = reinterpret_cast<FunctionCalls::tShowDebugWindow>(ShowDebugWindowFunctionAddress);

        YW::YW_WINDOW_39* Menu = OpenSingletonWindowFunction("Config", 280, 400, 0);
        auto config = AllocYWMemoryFunction((YW::MEMORY*)0x200);

        ConfigWidgetFunction((YW::CONFIG_WIDGET*)config);

        ContainerLinkFunction((YW::CONTAINER*)Menu, (YW::WIDGET*)config);

        ShowDebugWindowFunction((YW::WIDGET*)Menu);

    }

    void copyFileToAddress(const char* filename, void* destination, size_t size) {
        // Open the file
        std::ifstream file(filename, std::ios::binary);
        if (!file.is_open()) {
            std::cerr << "Error opening file: " << filename << std::endl;
            return;
        }

        // Read the content of the file into a vector
        std::vector<char> fileContent((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());

        // Close the file
        file.close();

        // Check if the file content is larger than the specified size
        if (fileContent.size() > size) {
            std::cerr << "File size exceeds the specified size." << std::endl;
            return;
        }

        // Copy the file content to the specified address
        std::memcpy(destination, fileContent.data(), fileContent.size());

        std::cout << "File content copied successfully." << std::endl;
        std::cout << "File size: " << fileContent.size() << " bytes." << std::endl;
    }

    std::streampos getFileSize(const char* filename) {
        // Open the file
        std::ifstream file(filename, std::ios::binary | std::ios::ate);

        if (!file.is_open()) {
            std::cerr << "Error opening file: " << filename << std::endl;
            return -1; // Return -1 to indicate an error
        }

        // Get the file size
        std::streampos size = file.tellg();

        if (size == -1) {
            std::cerr << "Error getting file size for: " << filename << std::endl;
        }

        // Close the file
        file.close();

        return size;
    }

  



    void SetBDXscript(YW::EVENT event, int n, unsigned int* arg) {

        uintptr_t StartUnitScriptFunctionAddress = ReadLEA32(Patterns::pattern_for_StartUnitScript, L"Start Unit Script", 0, 1, 5);
        FunctionCalls::tStartUnitScript StartUnitScriptFunction = reinterpret_cast<FunctionCalls::tStartUnitScript>(StartUnitScriptFunctionAddress);
        //VM_332* v0;  // Declare v0 as an array

        if ((event.Flag & 0x20) != 0) {
            //if (DebugInfo1 >= 0) {
           //     StopUnitScript(DebugInfo1);
       // }
           // std::string filepath = "bdx/";
           // filepath += bdxfileNamesArray[n];

            //const char* filepath_cstr = filepath.c_str();  // Get the C-style string
            //File_Read(bdxfileNamesArray[n], DebugInfo2);

            

            printf("Address of DebugInfo2: %p\n", static_cast<void*>(&DebugInfo2));

            //v0 = StartVM(reinterpret_cast<unsigned int*>(DebugInfo2), nullptr);
            //CallVM(v0, 0, 0,0);
            //ExecuteVM(v0, 0);

            StartUnitScriptFunction(reinterpret_cast<unsigned int*>(DebugInfo2));
            //FileReadBack(bdxfileNamesArray[n], DebugInfo2, (YS_FILE_READ_CALLBACK)YS_FileReadCallback, nullptr);
        }
    }

    void ListBdx(unsigned int* arg) {
        uintptr_t OpenSingletonWindowFunctionAddress = ReadLEA32(Patterns::pattern_for_OpenSingletonWindow, L"OpenSingletonWindow", 0, 1, 5);
        uintptr_t AllocYWMemoryFunctionAddress = ReadLEA32(Patterns::pattern_for_Alloc_YW_Memory, L"AllocYWMemory", 0, 1, 5);
        uintptr_t WListFunctionAddress = ReadLEA32(Patterns::pattern_for_YWList, L"YWList", 0, 1, 5);
        uintptr_t YWFrameFunctionAddress = ReadLEA32(Patterns::pattern_for_YWFrame, L"YWFrame", 0, 1, 5);
        uintptr_t ContainerLinkFunctionAddress = FindAndPrintPatternW(Patterns::pattern_for_ContainerLink, L"ContainerLink", 0);
        uintptr_t YWwindowMinSizeFunctionAddress = FindAndPrintPatternW(Patterns::pattern_for_YWwindow_minSize, L"YWwindow Min Size", 0);
        uintptr_t YWwindowMaxSizeFunctionAddress = FindAndPrintPatternW(Patterns::pattern_for_YWwindow_maxSize, L"YWwindow Max Size", 0);
        uintptr_t ShowDebugWindowFunctionAddress = ReadLEA32(Patterns::pattern_for_ShowDebugWindow, L"ShowDebugWindow", 0, 1, 5);
        uintptr_t YSDrawMessageFunctionAddress = ReadLEA32(Patterns::pattern_for_YSMessageDraw, L"YS MESSAGE DRAW", 0, 1, 5);
        
        // Function pointer casts
        auto AllocYWMemoryFunction = reinterpret_cast<FunctionCalls::tAlloc_YW_Memory>(AllocYWMemoryFunctionAddress);
        auto YWListFunction = reinterpret_cast<FunctionCalls::tYWList>(WListFunctionAddress);
        auto YWFrameFunction = reinterpret_cast<FunctionCalls::tYWFrame>(YWFrameFunctionAddress);
        auto ContainerLinkFunction = reinterpret_cast<FunctionCalls::tContainerLink>(ContainerLinkFunctionAddress);
        auto YWwindowMinSizeFunction = reinterpret_cast<FunctionCalls::tYW_WINDOW_set_min_size>(YWwindowMinSizeFunctionAddress);
        auto YWwindowMaxSizeFunction = reinterpret_cast<FunctionCalls::tYW_WINDOW_set_max_size>(YWwindowMaxSizeFunctionAddress);
        auto ShowDebugWindowFunction = reinterpret_cast<FunctionCalls::tShowDebugWindow>(ShowDebugWindowFunctionAddress);
        auto OpenSingletonWindowFunction = reinterpret_cast<FunctionCalls::tOpenSingletonWindow>(OpenSingletonWindowFunctionAddress);
        auto YSDrawMessageFunction = reinterpret_cast<FunctionCalls::tYSMessageDraw>(YSDrawMessageFunctionAddress);

        const char* bdxFolderPath = "DebugMenu/bdx\\*";

        WIN32_FIND_DATAA findFileData;
        HANDLE hFind = FindFirstFileA(bdxFolderPath, &findFileData);

        if (hFind != INVALID_HANDLE_VALUE) {
            do {
                if (!(findFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
                    std::string fullPath = "DebugMenu/bdx/" + std::string(findFileData.cFileName);
                    bdxfileNames.push_back(fullPath);
                }
            } while (FindNextFileA(hFind, &findFileData) != 0);
            FindClose(hFind);

            for (size_t i = 0; i < bdxfileNames.size(); ++i) {
                bdxfileNamesArray[i] = _strdup(bdxfileNames[i].c_str());
            }
            bdxfileNamesArray[bdxfileNames.size()] = nullptr;
            bdxfileNames.clear();
        }
        else {
            std::cerr << "Error opening directory." << std::endl;
            return;
        }

        // Open the debug window
        auto* Menu = OpenSingletonWindowFunction("Load BDX", 160, 200, 0);

        LOG("[DEBUG] OpenSingletonWindowFunction returned Menu pointer: %p\n", Menu);

        if (Menu != nullptr) {
            LOG("[DEBUG] Menu is open\n");
        }
        else {
            LOG("[DEBUG] Menu failed to open or is closed\n");
        }

        g_MenuUI.bdxMenu = Menu->Rect;

        g_MenuUI.bdxMenuPtr = Menu;

        //g_MenuUI.isBdxMenuOpen = (Menu != nullptr);

        //g_MenuUI.isBdxMenuOpen = (g_MenuUI.bdxMenuPtr != nullptr);
        

        auto allocYWMemList = AllocYWMemoryFunction((YW::MEMORY*)0x200);
        auto wrapperFrame = AllocYWMemoryFunction((YW::MEMORY*)0x200);  // Container frame to hold list + scrollbars

        // Create the list
        YWListFunction((YW::YW_LIST_0*)allocYWMemList, 0, 0, bdxfileNamesArray, SetBDXscript, allocYWMemList);

        //YWFrameFunction((YW::FRAME*)wrapperFrame, 0, 0, g_MenuRects.bdxMenu.W, g_MenuRects.bdxMenu.H, g_MenuRects.bdxMenu.W, g_MenuRects.bdxMenu.H);

        

        // Link list + scrollbars into wrapper frame
        //ContainerLinkFunction((YW::CONTAINER*)wrapperFrame, (YW::WIDGET*)allocYWMemList);

        // Add wrapper frame to window
        ContainerLinkFunction((YW::CONTAINER*)Menu, (YW::WIDGET*)allocYWMemList);

        // Finalize window size
        //YWwindowMinSizeFunction(Menu, g_MenuRects.bdxMenu.W, g_MenuRects.bdxMenu.H);
       // YWwindowMaxSizeFunction(Menu, g_MenuRects.bdxMenu.W, g_MenuRects.bdxMenu.H);

        ShowDebugWindowFunction((YW::WIDGET*)Menu);
    }

    


    void SaveCNF(unsigned int* arg) {

        //uintptr_t Savekh2CNFFunctionAddress = FindAndPrintPatternW(Patterns::pattern_for_Savekh2CNF, L"Save kh2cnf", 0);

      //  FunctionCalls::tSavekh2CNF Savekh2CNFFunction = reinterpret_cast<FunctionCalls::tSavekh2CNF>(Savekh2CNFFunctionAddress);

       // Savekh2CNFFunction();

       // LOG("Saved kh2.cnf");
        
        ListBdx(0);


    }

    void ShowRealSecondConfigWindow(unsigned int* arg) {

        uintptr_t OpenSingletonWindowFunctionAddress = ReadLEA32(Patterns::pattern_for_OpenSingletonWindow, L"OpenSingletonWindow", 0, 1, 5);
        FunctionCalls::tOpenSingletonWindow OpenSingletonWindowFunction = reinterpret_cast<FunctionCalls::tOpenSingletonWindow>(OpenSingletonWindowFunctionAddress);

        uintptr_t AllocYWMemoryFunctionAddress = ReadLEA32(Patterns::pattern_for_Alloc_YW_Memory, L"AllocYWMemory", 0, 1, 5);
        FunctionCalls::tAlloc_YW_Memory AllocYWMemoryFunction = reinterpret_cast<FunctionCalls::tAlloc_YW_Memory>(AllocYWMemoryFunctionAddress);

        uintptr_t Config2WidgetFunctionAddress = FindAndPrintPatternW(Patterns::pattern_for_ConfigWidget2, L"Config Widget", 0);

        FunctionCalls::tConfigWidget2 Config2WidgetFunction = reinterpret_cast<FunctionCalls::tConfigWidget2>(Config2WidgetFunctionAddress);

        uintptr_t ContainerLinkFunctionAddress = FindAndPrintPatternW(Patterns::pattern_for_ContainerLink, L"ContainerLink", 0);

        FunctionCalls::tContainerLink ContainerLinkFunction = reinterpret_cast<FunctionCalls::tContainerLink>(ContainerLinkFunctionAddress);

        uintptr_t ShowDebugWindowFunctionAddress = ReadLEA32(Patterns::pattern_for_ShowDebugWindow, L"ShowDebugWindow", 0, 1, 5);

        FunctionCalls::tShowDebugWindow ShowDebugWindowFunction = reinterpret_cast<FunctionCalls::tShowDebugWindow>(ShowDebugWindowFunctionAddress);

        YW::YW_WINDOW_39* Menu = OpenSingletonWindowFunction("Second Config", 202, 70, 0);
        auto config = AllocYWMemoryFunction((YW::MEMORY*)0x200);

        Config2WidgetFunction((YW::CONFIG_WIDGET_2*)config);

        ContainerLinkFunction((YW::CONTAINER*)Menu, (YW::WIDGET*)config);

        ShowDebugWindowFunction((YW::WIDGET*)Menu);

    }

    void GiveObjectItem(unsigned int* arg) {

        uintptr_t GetPartyCharaFunctionAddress = ReadLEA32(Patterns::pattern_for_GetPartyChara, L"Get Party Chara", 0, 1, 5);
        FunctionCalls::tGetPartyChara GetPartyCharaFunction = reinterpret_cast<FunctionCalls::tGetPartyChara>(GetPartyCharaFunctionAddress);

        uintptr_t SetCommandItemFunctionAddress = FindAndPrintPatternW(Patterns::pattern_for_SetCommandItem, L"Set Command Item", 0);

        FunctionCalls::tSetCommandItem SetCommandItemFunction = reinterpret_cast<FunctionCalls::tSetCommandItem>(SetCommandItemFunctionAddress);

        

        //gGlobalObjPtr = GetMenuOBJFunction();

        YS::OBJ* obj = GetMenuOBJFunction();
        if (!obj) {
            return;
        }


        
        gGlobalObjPtr1 = obj;
       

            SetCommandItemFunction((YS::FRIEND*)GetPartyCharaFunction(1), 7, gGlobalObjPtr1, 0);
        
    }

    void ScatterPrizeOBJ(unsigned int* arg) {

        uintptr_t ScatterPrizeFunctionAddress = ReadLEA32(Patterns::pattern_for_OBJscatterPrize, L"OBJ Scatter Prize", 0, 1, 5);
        FunctionCalls::tOBJscatterPrize ScatterPrizeFunction = reinterpret_cast<FunctionCalls::tOBJscatterPrize>(ScatterPrizeFunctionAddress);

        YS::OBJ* obj = GetMenuOBJFunction();
        if (!obj) {
            return;
        }



        gGlobalObjPtr1 = obj;

        
            // Create a random number generator engine
            std::random_device rd;
            std::mt19937 generator(rd());

            // Define the range for random values
            std::uniform_int_distribution<int> distribution(0, 10);

            // Generate a random number within the specified range
            int randomId = distribution(generator);
            ScatterPrizeFunction(gGlobalObjPtr1, randomId);

        

    }

    void AttachOBJToCameraCallBack(unsigned int* arg) {

        uintptr_t AttachOBJToCameraFunctionAddress = ReadLEA32(Patterns::pattern_for_OBJAttachCamera, L"Attach OBJ to Camera", 0, 1, 5);
        FunctionCalls::tOBJAttachCamera AttachOBJToCameraFunction = reinterpret_cast<FunctionCalls::tOBJAttachCamera>(AttachOBJToCameraFunctionAddress);

        YS::OBJ* obj = GetMenuOBJFunction();
        if (!obj) {
            return;
        }

        gGlobalObjPtr1 = obj;

        
            AttachOBJToCameraFunction(gGlobalObjPtr1, 0);
        

    }

    void AttachPlayerToOBJCallBack(unsigned int* arg) {
        uintptr_t AttachPlayerToOBJFunctionAddress = ReadLEA32(Patterns::pattern_for_AttachOBj, L"Attach Player to OBJ", 0, 1, 5);
        FunctionCalls::tAttachOBj AttachOBJFunction = reinterpret_cast<FunctionCalls::tAttachOBj>(AttachPlayerToOBJFunctionAddress);
        
        YS::OBJ* obj = GetMenuOBJFunction();
        if (!obj) {
            return;
        }

        gGlobalObjPtr1 = obj;

        
            AttachOBJFunction(GetPlayerFunction(), gGlobalObjPtr1, 0x4000, 0);

        

    }

    void AttachOBJToPlayerCallBack(unsigned int* arg) {

        uintptr_t AttachPlayerToOBJFunctionAddress = ReadLEA32(Patterns::pattern_for_AttachOBj, L"Attach Player to OBJ", 0, 1, 5);
        FunctionCalls::tAttachOBj AttachOBJFunction = reinterpret_cast<FunctionCalls::tAttachOBj>(AttachPlayerToOBJFunctionAddress);
        
        YS::OBJ* obj = GetMenuOBJFunction();
        if (!obj) {
            return;
        }

        gGlobalObjPtr1 = obj;

        
            
            AttachOBJFunction(gGlobalObjPtr1, GetPlayerFunction(), 0x56, 0);
            

        

    }

    void DetachOBJCallback(unsigned int* arg) {

        uintptr_t DetatchOBJFunctionAddress = ReadLEA32(Patterns::pattern_for_DetatchObject, L"Detatch Object", 0, 1, 5);
        FunctionCalls::tDetatchObject DetatchOBJFunction = reinterpret_cast<FunctionCalls::tDetatchObject>(DetatchOBJFunctionAddress);
        
        YS::OBJ* obj = GetMenuOBJFunction();
        if (!obj) {
            return;
        }

        gGlobalObjPtr1 = obj;


        
            DetatchOBJFunction(gGlobalObjPtr1);
        

    }

    void AttachCameraToOBJCallBack(unsigned int* arg) {

        uintptr_t CreateOBJCameraFunctionAddress = ReadLEA32(Patterns::pattern_for_CreateObjCamera, L"Create OBJ camera", 0, 1, 5);
        FunctionCalls::tCreateObjCamera CreateOBJCameraFunction = reinterpret_cast<FunctionCalls::tCreateObjCamera>(CreateOBJCameraFunctionAddress);
        
        YS::OBJ* obj = GetMenuOBJFunction();
        if (!obj) {
            return;
        }

        gGlobalObjPtr1 = obj;

       
            CreateOBJCameraFunction(gGlobalObjPtr1, 0);
        

    }

    void obj_menu_callback(unsigned int* arg) {

        uintptr_t AllocYWMemoryFunctionAddress = ReadLEA32(Patterns::pattern_for_Alloc_YW_Memory, L"AllocYWMemory", 0, 1, 5);
        FunctionCalls::tAlloc_YW_Memory AllocYWMemoryFunction = reinterpret_cast<FunctionCalls::tAlloc_YW_Memory>(AllocYWMemoryFunctionAddress);

        uintptr_t TestObjMenuFunctionAddress = ReadLEA32(Patterns::pattern_for_TestObjMenu, L"Test obj menu", 0, 1, 5);
        FunctionCalls::tTEST_OBJMENU_TEST_OBJMENU TestObjMenuFunction = reinterpret_cast<FunctionCalls::tTEST_OBJMENU_TEST_OBJMENU>(TestObjMenuFunctionAddress);

        

        YS::OBJ* v1;
        unsigned int* v3;

        v1 = EachOBJFunction(0);
        if (v1) {
            v3 = AllocYWMemoryFunction((YW::MEMORY*)0x200);
            if (v3) {
                TestObjMenuFunction((YS::TEST_OBJMENU*)v3, v1, 200, 100);
            }
        }

    }

    void ChangeOBJteamToPlayer(unsigned int* arg) {

        YS::OBJ* obj = GetMenuOBJFunction();
        if (!obj) {
            return;
        }

        gGlobalObjPtr1 = obj;

        int team = *(reinterpret_cast<int*>(reinterpret_cast<char*>(gGlobalObjPtr1) + 0x4DC)) = 0x01;

    }

    void DonateButtonCallBack(unsigned int* arg) {
        system("start https://ko-fi.com/gg3502");
    }

    void SetOBJMaxHP(int maxHP) {

        

        
            uintptr_t* obj_sheet = reinterpret_cast<uintptr_t*>(reinterpret_cast<char*>(gGlobalObjPtr1) + 0x5C0);
            if (IsBadReadPtr(obj_sheet, sizeof(uintptr_t))) {
                printf("Invalid memory at obj_sheet\n");
                return;
            }

            // Navigate to the next pointer
            uintptr_t* maxHpPtr = reinterpret_cast<uintptr_t*>(*obj_sheet);

            OBJMaxHP = maxHP;

            if (!IsBadReadPtr(maxHpPtr, sizeof(uintptr_t))) {
                // Check if the memory is writable
                if (IsBadWritePtr(maxHpPtr + 1, sizeof(int))) {
                    printf("Memory is not writable\n");
                    return;
                }

                // Update the max HP value at the memory location
                *reinterpret_cast<int*>(reinterpret_cast<char*>(maxHpPtr) + sizeof(int)) = OBJMaxHP;

                //printf("Updated Max HP: %d\n", maxHP);
            }
            else {
                printf("Invalid memory at maxHpPtr\n");
            }
        
    }

    void SetOBJDefense(int defense) {

        

        
            uintptr_t* obj_sheet = reinterpret_cast<uintptr_t*>(reinterpret_cast<char*>(gGlobalObjPtr1) + 0x5C0);
            if (IsBadReadPtr(obj_sheet, sizeof(uintptr_t))) {
                printf("Invalid memory at obj_sheet\n");
                return;
            }

            // Navigate to the next pointer
            uintptr_t* DefPtr = reinterpret_cast<uintptr_t*>(*obj_sheet + 0x18C);

            if (!IsBadReadPtr(DefPtr, sizeof(uintptr_t))) {
                // Check if the memory is writable
                if (IsBadWritePtr(DefPtr, sizeof(int))) {
                    printf("Memory is not writable\n");
                    return;
                }

                // Update the Defense value at the memory location
                *reinterpret_cast<int*>(DefPtr) = defense;

                printf("Updated Defense: %d\n", defense);
            }
            else {
                printf("Invalid memory at DefPtr\n");
            }
        
        
    }

    void SetOBJAttack(int attack) {

        

        
            uintptr_t* obj_sheet = reinterpret_cast<uintptr_t*>(reinterpret_cast<char*>(gGlobalObjPtr1) + 0x5C0);
            if (IsBadReadPtr(obj_sheet, sizeof(uintptr_t))) {
                printf("Invalid memory at obj_sheet\n");
                return;
            }

            // Navigate to the next pointer
            uintptr_t* AtkPtr = reinterpret_cast<uintptr_t*>(*obj_sheet + 0x188);

            if (!IsBadReadPtr(AtkPtr, sizeof(uintptr_t))) {
                // Check if the memory is writable
                if (IsBadWritePtr(AtkPtr, sizeof(int))) {
                    printf("Memory is not writable\n");
                    return;
                }

                // Update the Attack value at the memory location
                *reinterpret_cast<int*>(AtkPtr) = attack;

                printf("Updated Attack: %d\n", attack);
            }
            else {
                printf("Invalid memory at AtkPtr\n");
            }
        
    }

    void SetOBJCurrentHP(int currentHP) {
        
        

       
            uintptr_t* obj_sheet = reinterpret_cast<uintptr_t*>(reinterpret_cast<char*>(gGlobalObjPtr1) + 0x5C0);
            if (IsBadReadPtr(obj_sheet, sizeof(uintptr_t))) {
                printf("Invalid memory at obj_sheet\n");
                return;
            }

            // Navigate to the next pointer
            uintptr_t* maxHpPtr = reinterpret_cast<uintptr_t*>(*obj_sheet);

            OBJCurrentHP = currentHP;

            if (!IsBadReadPtr(maxHpPtr, sizeof(uintptr_t))) {
                // Check if the memory is writable
                if (IsBadWritePtr(maxHpPtr, sizeof(int))) {
                    printf("Memory is not writable\n");
                    return;
                }

                // Update the current HP value at the memory location
                *reinterpret_cast<int*>(maxHpPtr) = OBJCurrentHP;

                printf("Updated Current HP: %d\n", currentHP);
            }
            else {
                printf("Invalid memory at maxHpPtr\n");
            }
        
    }

    void ChangeOBJStats(unsigned int* arg) {

        uintptr_t SetDialValueFunctionAddress = FindAndPrintPatternW(Patterns::pattern_for_SetDialValue, L"Set DialValue", 0);

        FunctionCalls::tSetDialValue SetDialValueFunction = reinterpret_cast<FunctionCalls::tSetDialValue>(SetDialValueFunctionAddress);

        uintptr_t YWLabelFunctionAddress = ReadLEA32(Patterns::pattern_for_YWLabel, L"YWlabel", 0, 1, 5);

        FunctionCalls::tYWLabel YWLabelFunction = reinterpret_cast<FunctionCalls::tYWLabel>(YWLabelFunctionAddress);

        uintptr_t OpenSingletonWindowFunctionAddress = ReadLEA32(Patterns::pattern_for_OpenSingletonWindow, L"OpenSingletonWindow", 0, 1, 5);

        FunctionCalls::tOpenSingletonWindow OpenSingletonWindowFunction = reinterpret_cast<FunctionCalls::tOpenSingletonWindow>(OpenSingletonWindowFunctionAddress);

        //uintptr_t YWLabelFunctionAddress = ReadLEA32(Patterns::pattern_for_YWLabel, L"YWlabel", 0, 1, 5);

        //FunctionCalls::tYWLabel YWLabelFunction = reinterpret_cast<FunctionCalls::tYWLabel>(YWLabelFunctionAddress);

        uintptr_t ShowDialWidgetFunctionAddress = ReadLEA32(Patterns::pattern_for_DialWidget, L"Dial Widget", 0, 1, 5);
        FunctionCalls::tDialWidget ShowDialWidgetFunction = reinterpret_cast<FunctionCalls::tDialWidget>(ShowDialWidgetFunctionAddress);


        uintptr_t GetOBJEntrynameFunctionAddress = ReadLEA32(Patterns::pattern_for_GetObjEntryName, L"Get obj Entryname", 0, 1, 5);
        FunctionCalls::tGetObjEntryName GetOBJEntrynameFunction = reinterpret_cast<FunctionCalls::tGetObjEntryName>(GetOBJEntrynameFunctionAddress);



        uintptr_t ShowDebugWindowFunctionAddress = ReadLEA32(Patterns::pattern_for_ShowDebugWindow, L"ShowDebugWindow", 0, 1, 5);
        FunctionCalls::tShowDebugWindow ShowDebugWindowFunction = reinterpret_cast<FunctionCalls::tShowDebugWindow>(ShowDebugWindowFunctionAddress);


        uintptr_t AllocYWMemoryFunctionAddress = ReadLEA32(Patterns::pattern_for_Alloc_YW_Memory, L"AllocYWMemory", 0, 1, 5);

        FunctionCalls::tAlloc_YW_Memory AllocYWMemoryFunction = reinterpret_cast<FunctionCalls::tAlloc_YW_Memory>(AllocYWMemoryFunctionAddress);


        uintptr_t ContainerLinkFunctionAddress = FindAndPrintPatternW(Patterns::pattern_for_ContainerLink, L"ContainerLink", 0);

        FunctionCalls::tContainerLink ContainerLinkFunction = reinterpret_cast<FunctionCalls::tContainerLink>(ContainerLinkFunctionAddress);


        
        
        
            uintptr_t* obj_sheet = reinterpret_cast<uintptr_t*>(reinterpret_cast<char*>(gGlobalObjPtr1) + 0x5C0);
            if (IsBadReadPtr(obj_sheet, sizeof(uintptr_t))) {
                printf("Invalid memory at obj_sheet\n");
                return;
            }

            // Navigate to the next pointer
            uintptr_t* maxHpPtr = reinterpret_cast<uintptr_t*>(*obj_sheet);

            // Check if maxHpPtr points to valid memory before dereferencing
            if (!IsBadReadPtr(maxHpPtr, sizeof(uintptr_t))) {
                //assert(false);
                //ErrorRaise();
            }


            // Access the current and max HP values
            if (!IsBadReadPtr(maxHpPtr, 2 * sizeof(int))) {

                for (int i = 0; i < 8; ++i) {
                    //printf("Byte %d: 0x%02X\n", i, reinterpret_cast<unsigned char*>(maxHpPtr)[i]);
                }

                int currentHp = *reinterpret_cast<int*>(maxHpPtr);
                int maxHp = *(reinterpret_cast<int*>(maxHpPtr) + 1);

                unsigned char defense = *(reinterpret_cast<unsigned char*>(maxHpPtr) + 0x18C);

                unsigned char attack = *(reinterpret_cast<unsigned char*>(maxHpPtr) + 0x188);


                OBJMaxHP = maxHp;
                OBJCurrentHP = currentHp;
                OBJDefense = defense;
                OBJAttack = attack;


                // Optionally, you can print the values
               // printf("Current HP: %d, Max HP: %d\n", currentHp, maxHp);
                //printf("Defense: %d\n", defense);
               // printf("Attack: %d\n", attack);

                //printf("Address of currentHp: %p\n", maxHpPtr);
               // printf("Address of MaxHp: %p\n", maxHpPtr + 1);//reinterpret_cast<int*>(maxHpPtr) + sizeof(int));
               // printf("Address of defense: %p\n", maxHpPtr + 0x18C / sizeof(uintptr_t));
               // printf("Address of attack: %p\n", maxHpPtr + 0x188 / sizeof(uintptr_t));
            }

        

        const char* title = GetOBJEntrynameFunction(gGlobalObjPtr1);

        auto* Menu = OpenSingletonWindowFunction(title, 330, 100, 0);

        auto allocYWMemDialCurrentHP = AllocYWMemoryFunction((YW::MEMORY*)0x200);
        auto allocYWMemDialMaxHP = AllocYWMemoryFunction((YW::MEMORY*)0x200);
        auto allocYWMemDialDefense = AllocYWMemoryFunction((YW::MEMORY*)0x200);
        auto allocYWMemDialAttack = AllocYWMemoryFunction((YW::MEMORY*)0x200);

        auto allocYWMemLabelCurrentHP = AllocYWMemoryFunction((YW::MEMORY*)0x200);
        auto allocYWMemLabelMaxHP = AllocYWMemoryFunction((YW::MEMORY*)0x200);
        auto allocYWMemLabelDefense = AllocYWMemoryFunction((YW::MEMORY*)0x200);
        auto allocYWMemLabelAttack = AllocYWMemoryFunction((YW::MEMORY*)0x200);

        YWLabelFunction((YW::LABEL_3*)allocYWMemLabelCurrentHP, "Current HP:", 5, 5, 120, 12);
        YWLabelFunction((YW::LABEL_3*)allocYWMemLabelMaxHP, "Max HP:", 5, 25, 80, 12);
        YWLabelFunction((YW::LABEL_3*)allocYWMemLabelDefense, "Defense:", 5, 45, 80, 12);
        YWLabelFunction((YW::LABEL_3*)allocYWMemLabelAttack, "Attack:", 5, 65, 80, 12);

        ShowDialWidgetFunction((YW::DIAL_3*)allocYWMemDialCurrentHP, 260, 5, 96, 16, 1, 3400, SetOBJCurrentHP);
        ShowDialWidgetFunction((YW::DIAL_3*)allocYWMemDialMaxHP, 260, 25, 96, 16, 1, 3400, SetOBJMaxHP);
        ShowDialWidgetFunction((YW::DIAL_3*)allocYWMemDialDefense, 260, 45, 96, 16, 0, 255, SetOBJDefense);
        ShowDialWidgetFunction((YW::DIAL_3*)allocYWMemDialAttack, 260, 65, 96, 16, 0, 255, SetOBJAttack);

        SetDialValueFunction((YW::DIAL_3*)allocYWMemDialCurrentHP, OBJCurrentHP);
        SetDialValueFunction((YW::DIAL_3*)allocYWMemDialMaxHP, OBJMaxHP);
        SetDialValueFunction((YW::DIAL_3*)allocYWMemDialDefense, OBJDefense);
        SetDialValueFunction((YW::DIAL_3*)allocYWMemDialAttack, OBJAttack);

        ContainerLinkFunction((YW::CONTAINER*)Menu, (YW::WIDGET*)allocYWMemDialCurrentHP);
        ContainerLinkFunction((YW::CONTAINER*)Menu, (YW::WIDGET*)allocYWMemDialMaxHP);
        ContainerLinkFunction((YW::CONTAINER*)Menu, (YW::WIDGET*)allocYWMemDialDefense);
        ContainerLinkFunction((YW::CONTAINER*)Menu, (YW::WIDGET*)allocYWMemDialAttack);

        ContainerLinkFunction((YW::CONTAINER*)Menu, (YW::WIDGET*)allocYWMemLabelCurrentHP);
        ContainerLinkFunction((YW::CONTAINER*)Menu, (YW::WIDGET*)allocYWMemLabelMaxHP);
        ContainerLinkFunction((YW::CONTAINER*)Menu, (YW::WIDGET*)allocYWMemLabelDefense);
        ContainerLinkFunction((YW::CONTAINER*)Menu, (YW::WIDGET*)allocYWMemLabelAttack);

        ShowDebugWindowFunction((YW::WIDGET*)Menu);

    }

    void ApplyOBJScale()
    {
        if (!gGlobalObjPtr1) return;

        float* vec4 = reinterpret_cast<float*>(
            reinterpret_cast<char*>(gGlobalObjPtr1) + 0x30
            );

        vec4[0] = g_ScaleSliderX->Value; // X
        vec4[1] = g_ScaleSliderY->Value; // Y
        vec4[2] = g_ScaleSliderZ->Value; // Z
        vec4[3] = g_ScaleSliderW->Value; // W (uniform)

        
       
    }



    void OBJscaleMenuCallback(unsigned int* arg) {

        uintptr_t SetDialValueFunctionAddress = FindAndPrintPatternW(Patterns::pattern_for_SetDialValue, L"Set DialValue", 0);

        FunctionCalls::tSetDialValue SetDialValueFunction = reinterpret_cast<FunctionCalls::tSetDialValue>(SetDialValueFunctionAddress);

        uintptr_t YWLabelFunctionAddress = ReadLEA32(Patterns::pattern_for_YWLabel, L"YWlabel", 0, 1, 5);

        FunctionCalls::tYWLabel YWLabelFunction = reinterpret_cast<FunctionCalls::tYWLabel>(YWLabelFunctionAddress);

        uintptr_t OpenSingletonWindowFunctionAddress = ReadLEA32(Patterns::pattern_for_OpenSingletonWindow, L"OpenSingletonWindow", 0, 1, 5);

        FunctionCalls::tOpenSingletonWindow OpenSingletonWindowFunction = reinterpret_cast<FunctionCalls::tOpenSingletonWindow>(OpenSingletonWindowFunctionAddress);

        //uintptr_t YWLabelFunctionAddress = ReadLEA32(Patterns::pattern_for_YWLabel, L"YWlabel", 0, 1, 5);

        //FunctionCalls::tYWLabel YWLabelFunction = reinterpret_cast<FunctionCalls::tYWLabel>(YWLabelFunctionAddress);

        uintptr_t ShowDialWidgetFunctionAddress = ReadLEA32(Patterns::pattern_for_DialWidget, L"Dial Widget", 0, 1, 5);
        FunctionCalls::tDialWidget ShowDialWidgetFunction = reinterpret_cast<FunctionCalls::tDialWidget>(ShowDialWidgetFunctionAddress);


        uintptr_t GetOBJEntrynameFunctionAddress = ReadLEA32(Patterns::pattern_for_GetObjEntryName, L"Get obj Entryname", 0, 1, 5);
        FunctionCalls::tGetObjEntryName GetOBJEntrynameFunction = reinterpret_cast<FunctionCalls::tGetObjEntryName>(GetOBJEntrynameFunctionAddress);



        uintptr_t ShowDebugWindowFunctionAddress = ReadLEA32(Patterns::pattern_for_ShowDebugWindow, L"ShowDebugWindow", 0, 1, 5);
        FunctionCalls::tShowDebugWindow ShowDebugWindowFunction = reinterpret_cast<FunctionCalls::tShowDebugWindow>(ShowDebugWindowFunctionAddress);


        uintptr_t AllocYWMemoryFunctionAddress = ReadLEA32(Patterns::pattern_for_Alloc_YW_Memory, L"AllocYWMemory", 0, 1, 5);

        FunctionCalls::tAlloc_YW_Memory AllocYWMemoryFunction = reinterpret_cast<FunctionCalls::tAlloc_YW_Memory>(AllocYWMemoryFunctionAddress);

        FunctionCalls::tAlloc_YW_MemoryNew AllocYWMemoryFunctionNew = reinterpret_cast<FunctionCalls::tAlloc_YW_MemoryNew>(AllocYWMemoryFunctionAddress);


        uintptr_t ContainerLinkFunctionAddress = FindAndPrintPatternW(Patterns::pattern_for_ContainerLink, L"ContainerLink", 0);

        FunctionCalls::tContainerLink ContainerLinkFunction = reinterpret_cast<FunctionCalls::tContainerLink>(ContainerLinkFunctionAddress);

        uintptr_t YWScrollbarFunctionAddress = FindAndPrintPatternW(Patterns::pattern_for_YWScrollbar, L"YWScrollbar", 0);

        FunctionCalls::tYWScrollbar YWScrollbarFunction = reinterpret_cast<FunctionCalls::tYWScrollbar>(YWScrollbarFunctionAddress);

        //int v7;
       // unsigned int* v11;

        
        
        

        //std::cout << "gGlobalObjPtr at menu callback: " << gGlobalObjPtr << std::endl;

        const char* myObj = GetOBJEntrynameFunction(gGlobalObjPtr1);
        std::string windowTitle = std::string("Scale ") + myObj;
        auto* Menu = OpenSingletonWindowFunction(windowTitle.c_str(), 280, 156, 0);
        auto allocYWMemLabelScaleX = AllocYWMemoryFunction((YW::MEMORY*)0x200);
        auto allocYWMemLabelScaleY = AllocYWMemoryFunction((YW::MEMORY*)0x200);
        auto allocYWMemLabelScaleZ = AllocYWMemoryFunction((YW::MEMORY*)0x200);
        auto allocYWMemLabelScaleW = AllocYWMemoryFunction((YW::MEMORY*)0x200);
        auto allocYWMemDialScale = AllocYWMemoryFunction((YW::MEMORY*)0x200);

        g_ScaleSliderX = (YW::SCROLLBAR*)AllocYWMemoryFunctionNew(0xA0);
        g_ScaleSliderY = (YW::SCROLLBAR*)AllocYWMemoryFunctionNew(0xA0);
        g_ScaleSliderZ = (YW::SCROLLBAR*)AllocYWMemoryFunctionNew(0xA0);
        g_ScaleSliderW = (YW::SCROLLBAR*)AllocYWMemoryFunctionNew(0xA0);


        YWScrollbarFunction(g_ScaleSliderX, 25, 10, -36, 7, 0.1, 9.8999996, 1.0, 1.0);

        *reinterpret_cast<void***>(g_ScaleSliderX) = reinterpret_cast<void**>(moduleBase1 + 0x600F48);

        YWScrollbarFunction(g_ScaleSliderY, 25, 20, -36, 7, 0.1, 9.8999996, 1.0, 1.0);

        *reinterpret_cast<void***>(g_ScaleSliderY) = reinterpret_cast<void**>(moduleBase1 + 0x600F48);

        YWScrollbarFunction(g_ScaleSliderZ, 25, 30, -36, 7, 0.1, 9.8999996, 1.0, 1.0);

        *reinterpret_cast<void***>(g_ScaleSliderZ) = reinterpret_cast<void**>(moduleBase1 + 0x600F48);

        YWScrollbarFunction(g_ScaleSliderW, 25, 40, -36, 7, 0.1, 9.8999996, 1.0, 1.0);

        *reinterpret_cast<void***>(g_ScaleSliderW) = reinterpret_cast<void**>(moduleBase1 + 0x600F48);
       
        
        YWLabelFunction((YW::LABEL_3*)allocYWMemLabelScaleX, "X:", 5, 5, 120, 12);

        YWLabelFunction((YW::LABEL_3*)allocYWMemLabelScaleY, "Y:", 5, 15, 120, 12);

        YWLabelFunction((YW::LABEL_3*)allocYWMemLabelScaleZ, "Z:", 5, 25, 120, 12);

        YWLabelFunction((YW::LABEL_3*)allocYWMemLabelScaleW, "W:", 5, 35, 120, 12);

        //ShowDialWidgetFunction((YW::DIAL_3*)allocYWMemDialScale, 140, 5, 56, 16, 0, 8, ObjectScaleCallBack);

       // SetDialValueFunction((YW::DIAL_3*)allocYWMemDialScale, gObjectScale);

        ContainerLinkFunction((YW::CONTAINER*)Menu, (YW::WIDGET*)allocYWMemLabelScaleX);
        ContainerLinkFunction((YW::CONTAINER*)Menu, (YW::WIDGET*)allocYWMemLabelScaleY);
        ContainerLinkFunction((YW::CONTAINER*)Menu, (YW::WIDGET*)allocYWMemLabelScaleZ);
        ContainerLinkFunction((YW::CONTAINER*)Menu, (YW::WIDGET*)allocYWMemLabelScaleW);

        //ContainerLinkFunction((YW::CONTAINER*)Menu, (YW::WIDGET*)allocYWMemDialScale);

        ContainerLinkFunction((YW::CONTAINER*)Menu, (YW::WIDGET*)g_ScaleSliderX);
        ContainerLinkFunction((YW::CONTAINER*)Menu, (YW::WIDGET*)g_ScaleSliderY);
        ContainerLinkFunction((YW::CONTAINER*)Menu, (YW::WIDGET*)g_ScaleSliderZ);
        ContainerLinkFunction((YW::CONTAINER*)Menu, (YW::WIDGET*)g_ScaleSliderW);

        ShowDebugWindowFunction((YW::WIDGET*)Menu);
    }

    static std::wstring GetFilenameOnly(const std::wstring& path)
    {
        size_t pos = path.find_last_of(L"\\/");
        return (pos == std::wstring::npos) ? path : path.substr(pos + 1);
    }

    void BuildPngTextureFileList()
    {
        // Remember current selection's filename (optional)
        std::wstring prevSelectedName;
        if (!g_PngFiles.empty() && g_SelectedPngIndex >= 0 && (size_t)g_SelectedPngIndex < g_PngFiles.size())
            prevSelectedName = GetFilenameOnly(g_PngFiles[g_SelectedPngIndex]);

        // Cleanup old list + paths
        FreeCStringList(PngTexturesList);
        g_PngFiles.clear();
        g_PngListLoaded = false;

        const std::wstring rootPath = L"DebugMenu\\CustomTextures";
        const std::wstring searchPattern = rootPath + L"\\*.png";

        WIN32_FIND_DATAW fd{};
        HANDLE hFind = FindFirstFileW(searchPattern.c_str(), &fd);

        if (hFind == INVALID_HANDLE_VALUE)
        {
            const char* msg = "No PNG textures found.";
            PngTexturesList = new const char* [2];
            PngTexturesList[0] = new char[strlen(msg) + 1];
            strcpy_s(const_cast<char*>(PngTexturesList[0]), strlen(msg) + 1, msg);
            PngTexturesList[1] = nullptr;

            g_SelectedPngIndex = 0;
            LOG("[PNGMenu] No png files found in %ls\n", rootPath.c_str());
            return;
        }

        do
        {
            // Skip directories and "." / ".."
            if (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
                continue;

            // Full path (this is what you'll feed to ImguiTextureManager)
            std::wstring fullPath = rootPath + L"\\" + fd.cFileName;
            g_PngFiles.push_back(std::move(fullPath));

        } while (FindNextFileW(hFind, &fd));

        FindClose(hFind);

        if (g_PngFiles.empty())
        {
            const char* msg = "No PNG textures found.";
            PngTexturesList = new const char* [2];
            PngTexturesList[0] = new char[strlen(msg) + 1];
            strcpy_s(const_cast<char*>(PngTexturesList[0]), strlen(msg) + 1, msg);
            PngTexturesList[1] = nullptr;

            g_SelectedPngIndex = 0;
            LOG("[PNGMenu] No png files after scan (unexpected)\n");
            return;
        }

        // Sort by filename (so list order is stable)
        std::sort(g_PngFiles.begin(), g_PngFiles.end(),
            [](const std::wstring& a, const std::wstring& b)
            {
                return _wcsicmp(GetFilenameOnly(a).c_str(), GetFilenameOnly(b).c_str()) < 0;
            });

        // Build const char** (filenames only) for your YWList
        PngTexturesList = new const char* [g_PngFiles.size() + 1];

        for (size_t i = 0; i < g_PngFiles.size(); ++i)
        {
            std::wstring wname = GetFilenameOnly(g_PngFiles[i]);
            std::string nameStr = WStringToString(wname);

            char* s = new char[nameStr.size() + 1];
            strcpy_s(s, nameStr.size() + 1, nameStr.c_str());
            PngTexturesList[i] = s;
        }

        PngTexturesList[g_PngFiles.size()] = nullptr;
        g_PngListLoaded = true;

        // Restore selection by filename if possible (optional)
        if (!prevSelectedName.empty())
        {
            g_SelectedPngIndex = 0;
            for (size_t i = 0; i < g_PngFiles.size(); ++i)
            {
                if (_wcsicmp(GetFilenameOnly(g_PngFiles[i]).c_str(), prevSelectedName.c_str()) == 0)
                {
                    g_SelectedPngIndex = (int)i;
                    break;
                }
            }
        }
        else
        {
            g_SelectedPngIndex = 0;
        }

        LOG("[PNGMenu] Found %zu png files\n", g_PngFiles.size());
    }

    void PngListCallback(YW::EVENT event, int index, unsigned int* arg)
    {
        // Only react on "confirm/activate" like cinecam does
        if ((event.Flag & 0x20) == 0)
            return;

        if (g_PngFiles.empty())
            return;

        if (index < 0 || index >= (int)g_PngFiles.size())
            return;

        g_SelectedPngIndex = index;
        g_SelectedPngFullPath = g_PngFiles[index];

        // filename only (nice for UI/logs)
        {
            const std::wstring& wpath = g_SelectedPngFullPath;
            size_t pos = wpath.find_last_of(L"\\/");
            std::wstring wname = (pos == std::wstring::npos) ? wpath : wpath.substr(pos + 1);
            g_SelectedPngName = WStringToString(wname);
        }

        // Kick load/caching (safe to call repeatedly)
        //auto view = ImguiTextureManager::GetOrLoadPNG(g_SelectedPngFullPath);

       /* LOG("[PNGMenu] Selected idx=%d name=%s path=%ls (loaded=%d %dx%d)\n",
            index,
            g_SelectedPngName.c_str(),
            g_SelectedPngFullPath.c_str(),
            view.gpuHandlePtr != 0,
            view.width, view.height);*/
    }

    void PngTextureLoadMenu(unsigned int* arg)
    {
        // These are the same pattern finds you already do (adjust pattern names to yours)
        uintptr_t OpenSingletonWindowFunctionAddress = ReadLEA32(Patterns::pattern_for_OpenSingletonWindow, L"OpenSingletonWindow", 0, 1, 5);
        uintptr_t AllocYWMemoryFunctionAddress = ReadLEA32(Patterns::pattern_for_Alloc_YW_Memory, L"AllocYWMemory", 0, 1, 5);
        uintptr_t WListFunctionAddress = ReadLEA32(Patterns::pattern_for_YWList, L"YWList", 0, 1, 5);
        uintptr_t YWFrameFunctionAddress = ReadLEA32(Patterns::pattern_for_YWFrame, L"YWFrame", 0, 1, 5);
        uintptr_t ContainerLinkFunctionAddress = FindAndPrintPatternW(Patterns::pattern_for_ContainerLink, L"ContainerLink", 0);
        uintptr_t YWwindowMinSizeFunctionAddress = FindAndPrintPatternW(Patterns::pattern_for_YWwindow_minSize, L"YWwindow Min Size", 0);
        uintptr_t YWwindowMaxSizeFunctionAddress = FindAndPrintPatternW(Patterns::pattern_for_YWwindow_maxSize, L"YWwindow Max Size", 0);
        uintptr_t ShowDebugWindowFunctionAddress = ReadLEA32(Patterns::pattern_for_ShowDebugWindow, L"ShowDebugWindow", 0, 1, 5);

        auto AllocYWMemoryFunction = reinterpret_cast<FunctionCalls::tAlloc_YW_Memory>(AllocYWMemoryFunctionAddress);
        auto YWListFunction = reinterpret_cast<FunctionCalls::tYWList>(WListFunctionAddress);
        auto YWFrameFunction = reinterpret_cast<FunctionCalls::tYWFrame>(YWFrameFunctionAddress);
        auto ContainerLinkFunction = reinterpret_cast<FunctionCalls::tContainerLink>(ContainerLinkFunctionAddress);
        auto YWwindowMinSizeFunction = reinterpret_cast<FunctionCalls::tYW_WINDOW_set_min_size>(YWwindowMinSizeFunctionAddress);
        auto YWwindowMaxSizeFunction = reinterpret_cast<FunctionCalls::tYW_WINDOW_set_max_size>(YWwindowMaxSizeFunctionAddress);
        auto ShowDebugWindowFunction = reinterpret_cast<FunctionCalls::tShowDebugWindow>(ShowDebugWindowFunctionAddress);
        auto OpenSingletonWindowFunc = reinterpret_cast<FunctionCalls::tOpenSingletonWindow>(OpenSingletonWindowFunctionAddress);

        // Build / rebuild file list
        BuildPngTextureFileList();

        // Window title
        YW::YW_WINDOW_39* Menu = OpenSingletonWindowFunc("PNG Texture Picker", 260, 320, 2);

        // Allocate widget memory blocks (same style as your cine menu)
        auto listMem = AllocYWMemoryFunction((YW::MEMORY*)0x200);
        auto frameMem = AllocYWMemoryFunction((YW::MEMORY*)0x200);

        // Create list bound to our PngTexturesList
        // IMPORTANT: callback must match the signature your engine expects
        YWListFunction(
            (YW::YW_LIST_0*)listMem,
            0, 0,
            PngTexturesList,
            PngListCallback,          
            listMem                   
        );

        YW::WIDGET* listWidget = (YW::WIDGET*)listMem;

        // Frame that contains the list widget
        YWFrameFunction((YW::FRAME*)frameMem, 0, 11, 0, -11, 0, listWidget->Rect.H);
        YW::FRAME* vFrame = (YW::FRAME*)frameMem;

        // Link it up
        ContainerLinkFunction((YW::CONTAINER*)vFrame, (YW::WIDGET*)listWidget);
        ContainerLinkFunction((YW::CONTAINER*)Menu, (YW::WIDGET*)vFrame);

        // Lock window size to content (like your cinecam menu)
        int contentHeight = listWidget->Rect.H + 5;
        int contentWidth = listWidget->Rect.W + 7;

        YWwindowMinSizeFunction(Menu, contentWidth, contentHeight);
        YWwindowMaxSizeFunction(Menu, contentWidth, contentHeight);

        ShowDebugWindowFunction((YW::WIDGET*)Menu);
    }

    







    // PLAY (once)
    void CineBtn_Play(unsigned int* arg)
    {
        g_cinematicCamera.startPlayback(false);
    }

    // STOP
    void CineBtn_Stop(unsigned int* arg)
    {
        g_cinematicCamera.stopPlayback();
    }

    // Record Key (add keyframe at current timeline time)
    void CineBtn_RecordKey(unsigned int* arg)
    {
        g_cinematicCamera.addKeyframe(g_cineTimelineTime);
    }

    // Reset Keys (remove all)
    void CineBtn_ResetKeys(unsigned int* arg)
    {
        g_cinematicCamera.clearKeyframes();
        g_cineSelectedKF = -1;
        g_cineTimelineTime = 0.0f;
    }

    // Next keyframe
    void CineBtn_NextKey(unsigned int* arg)
    {
        const auto& keys = g_cinematicCamera.getKeyframes();
        if (keys.empty())
            return;

        if (g_cineSelectedKF < 0 || g_cineSelectedKF >= (int)keys.size())
            g_cineSelectedKF = 0;
        else
            g_cineSelectedKF = (g_cineSelectedKF + 1) % (int)keys.size();

        g_cinematicCamera.applyKeyframeInstant((size_t)g_cineSelectedKF);
        g_cineTimelineTime = keys[g_cineSelectedKF].time;
    }

    // Previous keyframe
    void CineBtn_PrevKey(unsigned int* arg)
    {
        const auto& keys = g_cinematicCamera.getKeyframes();
        if (keys.empty())
            return;

        if (g_cineSelectedKF < 0 || g_cineSelectedKF >= (int)keys.size())
            g_cineSelectedKF = (int)keys.size() - 1;
        else
            g_cineSelectedKF = (g_cineSelectedKF - 1 + (int)keys.size()) % (int)keys.size();

        g_cinematicCamera.applyKeyframeInstant((size_t)g_cineSelectedKF);
        g_cineTimelineTime = keys[g_cineSelectedKF].time;
    }

    // Delete current selected keyframe
    void CineBtn_DeleteKey(unsigned int* arg)
    {
        const auto& keys = g_cinematicCamera.getKeyframes();
        if (keys.empty() || g_cineSelectedKF < 0 || g_cineSelectedKF >= (int)keys.size())
            return;

        // delete currently selected keyframe
        size_t idx = (size_t)g_cineSelectedKF;
        g_cinematicCamera.removeKeyframe(idx);

        const auto& keys2 = g_cinematicCamera.getKeyframes();
        if (keys2.empty())
        {
            g_cineSelectedKF = -1;
            g_cineTimelineTime = 0.0f;
            return;
        }

        // clamp selected index to new size
        if (g_cineSelectedKF >= (int)keys2.size())
            g_cineSelectedKF = (int)keys2.size() - 1;

        // snap camera + timeline to the new selected keyframe
        g_cinematicCamera.applyKeyframeInstant((size_t)g_cineSelectedKF);
        g_cineTimelineTime = keys2[g_cineSelectedKF].time;
    }

    bool SaveCinematicKeyframesToFile(const char* path)
    {
        FILE* f = nullptr;
        fopen_s(&f, path, "w");
        if (!f) return false;

        const auto& keys = g_cinematicCamera.getKeyframes();
        for (size_t i = 0; i < keys.size(); ++i)
        {
            const auto& k = keys[i];
            const auto& s = k.state;
            fprintf(f,
                "%.6f "      // time
                "%.6f %.6f %.6f %.6f "   // eye
                "%.6f %.6f %.6f %.6f "   // at
                "%.6f %.6f %.6f\n",      // yaw, pitch, fov
                k.time,
                s.eye.x, s.eye.y, s.eye.z, s.eye.w,
                s.at.x, s.at.y, s.at.z, s.at.w,
                s.yaw, s.pitch, s.fov);
        }

        fclose(f);
        return true;
    }

    static std::string SanitizeAreaName(const char* rawName)
    {
        if (!rawName || !*rawName)
            return "Unknown";

        std::string s(rawName);

        for (char& c : s)
        {
            if (c == '\\' || c == '/' || c == ':' ||
                c == '*' || c == '?' || c == '"' ||
                c == '<' || c == '>' || c == '|' ||
                c == ' ')
            {
                c = '_';
            }
        }

        return s;
    }

    static std::wstring MakeUniqueCineCamPath(const std::wstring& rootPath,
        const std::wstring& areaNameW)
    {
        // Try indices 1..999
        for (int i = 1; i <= 999; ++i)
        {
            wchar_t fileName[256];
            swprintf(fileName, 256, L"CineCam_%s_%03d.txt", areaNameW.c_str(), i);

            std::wstring fullPath = rootPath + L"\\" + fileName;

            DWORD attrib = GetFileAttributesW(fullPath.c_str());
            if (attrib == INVALID_FILE_ATTRIBUTES)
            {
                // File does not exist -> this name is free to use
                return fullPath;
            }
        }

        // Fallback: if somehow all 1..999 exist, just overwrite the last one
        wchar_t fileName[256];
        swprintf(fileName, 256, L"CineCam_%s_999.txt", areaNameW.c_str());
        return rootPath + L"\\" + fileName;
    }


    void __fastcall CineBtn_SaveKeys(unsigned int* arg)
    {
        AREA* CurrentArea = GetCurrentArea();
        const char* AreaNameRaw = GetAreaNameFunction(CurrentArea, 0);

        if (CurrentArea)
        {
            printf(
                "AREA: World=%d  Area=%d  Entrance=%d  Map=%d  Battle=%d  Event=%d\n",
                CurrentArea->World,
                CurrentArea->Area,
                CurrentArea->Entrance,
                CurrentArea->Map,
                CurrentArea->Battle,
                CurrentArea->Event
            );
        }
        else
        {
            printf("AREA pointer is NULL\n");
        }

        std::string sanitized = SanitizeAreaName(AreaNameRaw);
        std::wstring areaNameW(sanitized.begin(), sanitized.end());

        // Root folder (like your music folder style)
        const std::wstring rootPath = L"DebugMenu\\CineCam";

        // Ensure folder exists
        CreateDirectoryW(L"DebugMenu", nullptr);
        CreateDirectoryW(rootPath.c_str(), nullptr);

        // Get unique path: DebugMenu\CineCam\CineCam_<Area>_XXX.txt
        std::wstring fullPathW = MakeUniqueCineCamPath(rootPath, areaNameW);

        // Convert to narrow for the existing SaveCinematicKeyframesToFile
        std::string fullPathN(fullPathW.begin(), fullPathW.end());

        SaveCinematicKeyframesToFile(fullPathN.c_str());
    }


    void BuildCinematicCameraFileList()
    {
        // cleanup old list if any
        if (CinematicCamerasList)
        {
            // free inner strings
            for (size_t i = 0; CinematicCamerasList[i] != nullptr; ++i)
            {
                delete[] CinematicCamerasList[i];
            }
            delete[] CinematicCamerasList;
            CinematicCamerasList = nullptr;
        }

        g_CineCamFiles.clear();

        const std::wstring rootPath = L"DebugMenu\\CineCam";

        WIN32_FIND_DATAW fd{};
        std::wstring searchPattern = rootPath + L"\\CineCam_*.txt";

        HANDLE hFind = FindFirstFileW(searchPattern.c_str(), &fd);
        if (hFind == INVALID_HANDLE_VALUE)
        {
            // no files found -> show a single “no files” entry
            const char* msg = "No CineCam files found.";
            size_t len = strlen(msg);

            CinematicCamerasList = new const char* [2];
            CinematicCamerasList[0] = new char[len + 1];
            strcpy_s(const_cast<char*>(CinematicCamerasList[0]), len + 1, msg);
            CinematicCamerasList[1] = nullptr;

            return;
        }

        do
        {
            if (!(fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
            {
                std::wstring fullPath = rootPath + L"\\" + fd.cFileName;
                g_CineCamFiles.push_back(fullPath);
            }
        } while (FindNextFileW(hFind, &fd));

        FindClose(hFind);

        if (g_CineCamFiles.empty())
        {
            const char* msg = "No CineCam files found.";
            size_t len = strlen(msg);

            CinematicCamerasList = new const char* [2];
            CinematicCamerasList[0] = new char[len + 1];
            strcpy_s(const_cast<char*>(CinematicCamerasList[0]), len + 1, msg);
            CinematicCamerasList[1] = nullptr;
            return;
        }

        // build const char** list from filenames (no full path, just file names)
        CinematicCamerasList = new const char* [g_CineCamFiles.size() + 1];

        for (size_t i = 0; i < g_CineCamFiles.size(); ++i)
        {
            // show just filename, not the full path
            const std::wstring& wpath = g_CineCamFiles[i];

            size_t pos = wpath.find_last_of(L"\\/");
            std::wstring wname = (pos == std::wstring::npos) ? wpath : wpath.substr(pos + 1);

            std::string nameStr = WStringToString(wname);
            CinematicCamerasList[i] = new char[nameStr.size() + 1];
            strcpy_s(const_cast<char*>(CinematicCamerasList[i]), nameStr.size() + 1, nameStr.c_str());
        }

        CinematicCamerasList[g_CineCamFiles.size()] = nullptr; // null-terminate

        g_CineListLoaded = true;
    }

    bool LoadCinematicKeyframesFromFile(const char* path)
    {
        std::ifstream in(path);
        if (!in.is_open())
            return false;

        std::vector<CinematicKeyframe> loaded;
        std::string line;

        while (std::getline(in, line))
        {
            if (line.empty())
                continue;
            if (line[0] == '#' || (line.size() > 1 && line[0] == '/' && line[1] == '/'))
                continue;

            std::istringstream iss(line);
            float t;
            float ex, ey, ez, ew;
            float ax, ay, az, aw;
            float yaw, pitch, fov;

            // Must match SaveCinematicKeyframesToFile fprintf order
            if (!(iss >> t
                >> ex >> ey >> ez >> ew
                >> ax >> ay >> az >> aw
                >> yaw >> pitch >> fov))
            {
                // malformed line, skip
                continue;
            }

            CinematicKeyframe kf{};
            kf.time = t;

            kf.state.eye.x = ex;
            kf.state.eye.y = ey;
            kf.state.eye.z = ez;
            kf.state.eye.w = ew;

            kf.state.at.x = ax;
            kf.state.at.y = ay;
            kf.state.at.z = az;
            kf.state.at.w = aw;

            kf.state.yaw = yaw;
            kf.state.pitch = pitch;
            kf.state.fov = fov;

            loaded.push_back(kf);
        }

        in.close();

        if (loaded.empty())
            return false;

        // Sort by time just in case
        std::sort(loaded.begin(), loaded.end(),
            [](const CinematicKeyframe& a, const CinematicKeyframe& b)
            {
                return a.time < b.time;
            });

        // Push into the cinematic camera
        g_cinematicCamera.replaceKeyframes(loaded);

        // Optionally: snap to first keyframe
        g_cineTimelineTime = loaded.front().time;
        g_cinematicCamera.applyKeyframeInstant(0);

        return true;
    }

    void CineCamListCallback(YW::EVENT event, int index, unsigned int* arg)
    {
        
        if ((event.Flag & 0x20) != 0) {

            
            if (g_CineCamFiles.empty())
                return;

            if (index < 0 || index >= (int)g_CineCamFiles.size())
                return;

            const std::wstring& wpath = g_CineCamFiles[index];
            std::string pathN = WStringToString(wpath);

            LoadCinematicKeyframesFromFile(pathN.c_str());
        }
    }

    void CineBtn_AutoPlay(unsigned int* arg)
    {
        const auto& keys = g_cinematicCamera.getKeyframes();
        if (keys.size() < 2)
            return; // nothing to play

        // reset timeline and start non-looped playback
        g_cineTimelineTime = 0.0f;
        g_cinematicCamera.startPlayback(false);
        g_cineAutoPlayOnce = true;
    }

    void CineToggle_FreecamDuringPlayback(bool on, unsigned int* arg)
    {
        g_cineGameplayEnabled = on;

        if (on)
        {
            // Re-enable player controls & HUD
            GetOBJControlONFunction(0);
            CommonLayerONFunction();

            // Force cinematic playback in infinite loop
            g_cinematicCamera.startPlayback(true);

            // Also force camera mode to PLAYING
            g_cinematicCamera.setMode(CinematicCamera::Mode::Playing);

            // IMPORTANT: Disable cinematic hotkeys while this mode is active
            CinematicCameraToggle = false;
        }
        else
        {
            // Restore full cinematic editing mode
            GetOBJControlOFFunction(0);
            CommonLayerOFFFunction();

            g_cinematicCamera.stopPlayback();

            // Re-enable cinematic keys
            CinematicCameraToggle = true;
        }
    }


    void CinematicCameraLoadMenu(unsigned int* arg)
    {
        uintptr_t OpenSingletonWindowFunctionAddress = ReadLEA32(Patterns::pattern_for_OpenSingletonWindow, L"OpenSingletonWindow", 0, 1, 5);
        uintptr_t AllocYWMemoryFunctionAddress = ReadLEA32(Patterns::pattern_for_Alloc_YW_Memory, L"AllocYWMemory", 0, 1, 5);
        uintptr_t WListFunctionAddress = ReadLEA32(Patterns::pattern_for_YWList, L"YWList", 0, 1, 5);
        uintptr_t YWFrameFunctionAddress = ReadLEA32(Patterns::pattern_for_YWFrame, L"YWFrame", 0, 1, 5);
        uintptr_t ContainerLinkFunctionAddress = FindAndPrintPatternW(Patterns::pattern_for_ContainerLink, L"ContainerLink", 0);
        uintptr_t YWwindowMinSizeFunctionAddress = FindAndPrintPatternW(Patterns::pattern_for_YWwindow_minSize, L"YWwindow Min Size", 0);
        uintptr_t YWwindowMaxSizeFunctionAddress = FindAndPrintPatternW(Patterns::pattern_for_YWwindow_maxSize, L"YWwindow Max Size", 0);
        uintptr_t ShowDebugWindowFunctionAddress = ReadLEA32(Patterns::pattern_for_ShowDebugWindow, L"ShowDebugWindow", 0, 1, 5);

        auto AllocYWMemoryFunction = reinterpret_cast<FunctionCalls::tAlloc_YW_Memory>(AllocYWMemoryFunctionAddress);
        auto YWListFunction = reinterpret_cast<FunctionCalls::tYWList>(WListFunctionAddress);
        auto YWFrameFunction = reinterpret_cast<FunctionCalls::tYWFrame>(YWFrameFunctionAddress);
        auto ContainerLinkFunction = reinterpret_cast<FunctionCalls::tContainerLink>(ContainerLinkFunctionAddress);
        auto YWwindowMinSizeFunction = reinterpret_cast<FunctionCalls::tYW_WINDOW_set_min_size>(YWwindowMinSizeFunctionAddress);
        auto YWwindowMaxSizeFunction = reinterpret_cast<FunctionCalls::tYW_WINDOW_set_max_size>(YWwindowMaxSizeFunctionAddress);
        auto ShowDebugWindowFunction = reinterpret_cast<FunctionCalls::tShowDebugWindow>(ShowDebugWindowFunctionAddress);
        auto OpenSingletonWindowFunc = reinterpret_cast<FunctionCalls::tOpenSingletonWindow>(OpenSingletonWindowFunctionAddress);

        // Build / rebuild file list
        g_CineListLoaded = false;
        BuildCinematicCameraFileList();

        // Window title
        YW::YW_WINDOW_39* Menu = OpenSingletonWindowFunc("Cinematic Camera – Load", 250, 300, 2);

        auto allocYWMemList = AllocYWMemoryFunction((YW::MEMORY*)0x200);
        unsigned int* Frame = AllocYWMemoryFunction((YW::MEMORY*)0x200);

        // Create YWList bound to our CinematicCamerasList
        YWListFunction((YW::YW_LIST_0*)allocYWMemList,
            0, 0,
            CinematicCamerasList,
            CineCamListCallback,
            allocYWMemList);

        YW::WIDGET* widget = (YW::WIDGET*)allocYWMemList;

        // Frame
        YWFrameFunction((YW::FRAME*)Frame, 0, 11, 0, -11, 0, widget->Rect.H);
        YW::FRAME* vFrame = (YW::FRAME*)Frame;

        ContainerLinkFunction((YW::CONTAINER*)vFrame, (YW::WIDGET*)widget);
        ContainerLinkFunction((YW::CONTAINER*)Menu, (YW::WIDGET*)vFrame);

        int contentHeight = widget->Rect.H + 5;
        int contentWidth = widget->Rect.W + 7;

        YWwindowMinSizeFunction(Menu, contentWidth, contentHeight);
        YWwindowMaxSizeFunction(Menu, contentWidth, contentHeight);

        ShowDebugWindowFunction((YW::WIDGET*)Menu);
    }

    void __fastcall CineSettingsWindowFinalizer(YW::WIDGET* self)
    {
        g_CineSettingsOpen = false;
        g_CineSettingsWindow = nullptr;
    }

    void UpdateCinematicSlidersFromUI()
    {
        // Cine playback speed (only affects camera)
        if (g_CineSlider)
        {
            float newSpeed = g_CineSlider->Value;
            g_cinematicCamera.setPlaybackSpeed(newSpeed);
        }

        // Game timescale (global game speed)
        if (g_TimeScaleSlider)
        {
            g_cineTimeScale = g_TimeScaleSlider->Value;
            WriteGameSpeed(g_cineTimeScale);
        }

        // Timeline cursor (scrubbing)
        if (g_TimelineScaleSlider)
        {
            float oldTime = g_cineTimelineTime;
            g_cineTimelineTime = g_TimelineScaleSlider->Value;
            // later you can make scrubbing jump camera to that time
        }
    }




    void CinematicCameraSettingsMenuCallback(unsigned int* arg) {

        static YW::YW_WINDOW_39* s_lastMenu = nullptr;



        uintptr_t SetDialValueFunctionAddress = FindAndPrintPatternW(Patterns::pattern_for_SetDialValue, L"Set DialValue", 0);

        FunctionCalls::tSetDialValue SetDialValueFunction = reinterpret_cast<FunctionCalls::tSetDialValue>(SetDialValueFunctionAddress);

        uintptr_t YWLabelFunctionAddress = ReadLEA32(Patterns::pattern_for_YWLabel, L"YWlabel", 0, 1, 5);

        FunctionCalls::tYWLabel YWLabelFunction = reinterpret_cast<FunctionCalls::tYWLabel>(YWLabelFunctionAddress);

        uintptr_t OpenSingletonWindowFunctionAddress = ReadLEA32(Patterns::pattern_for_OpenSingletonWindow, L"OpenSingletonWindow", 0, 1, 5);

        FunctionCalls::tOpenSingletonWindow OpenSingletonWindowFunction = reinterpret_cast<FunctionCalls::tOpenSingletonWindow>(OpenSingletonWindowFunctionAddress);

        //uintptr_t YWLabelFunctionAddress = ReadLEA32(Patterns::pattern_for_YWLabel, L"YWlabel", 0, 1, 5);

        //FunctionCalls::tYWLabel YWLabelFunction = reinterpret_cast<FunctionCalls::tYWLabel>(YWLabelFunctionAddress);

        uintptr_t ShowDialWidgetFunctionAddress = ReadLEA32(Patterns::pattern_for_DialWidget, L"Dial Widget", 0, 1, 5);
        FunctionCalls::tDialWidget ShowDialWidgetFunction = reinterpret_cast<FunctionCalls::tDialWidget>(ShowDialWidgetFunctionAddress);


        uintptr_t GetOBJEntrynameFunctionAddress = ReadLEA32(Patterns::pattern_for_GetObjEntryName, L"Get obj Entryname", 0, 1, 5);
        FunctionCalls::tGetObjEntryName GetOBJEntrynameFunction = reinterpret_cast<FunctionCalls::tGetObjEntryName>(GetOBJEntrynameFunctionAddress);



        uintptr_t ShowDebugWindowFunctionAddress = ReadLEA32(Patterns::pattern_for_ShowDebugWindow, L"ShowDebugWindow", 0, 1, 5);
        FunctionCalls::tShowDebugWindow ShowDebugWindowFunction = reinterpret_cast<FunctionCalls::tShowDebugWindow>(ShowDebugWindowFunctionAddress);


        uintptr_t AllocYWMemoryFunctionAddress = ReadLEA32(Patterns::pattern_for_Alloc_YW_Memory, L"AllocYWMemory", 0, 1, 5);

        FunctionCalls::tAlloc_YW_Memory AllocYWMemoryFunction = reinterpret_cast<FunctionCalls::tAlloc_YW_Memory>(AllocYWMemoryFunctionAddress);

        FunctionCalls::tAlloc_YW_MemoryNew AllocYWMemoryFunctionNew = reinterpret_cast<FunctionCalls::tAlloc_YW_MemoryNew>(AllocYWMemoryFunctionAddress);


        uintptr_t ContainerLinkFunctionAddress = FindAndPrintPatternW(Patterns::pattern_for_ContainerLink, L"ContainerLink", 0);

        FunctionCalls::tContainerLink ContainerLinkFunction = reinterpret_cast<FunctionCalls::tContainerLink>(ContainerLinkFunctionAddress);

        uintptr_t YWScrollbarFunctionAddress = FindAndPrintPatternW(Patterns::pattern_for_YWScrollbar, L"YWScrollbar", 0);

        FunctionCalls::tYWScrollbar YWScrollbarFunction = reinterpret_cast<FunctionCalls::tYWScrollbar>(YWScrollbarFunctionAddress);

        uintptr_t WListFunctionAddress = ReadLEA32(Patterns::pattern_for_YWList, L"YWList", 0, 1, 5);
        uintptr_t YWFrameFunctionAddress = ReadLEA32(Patterns::pattern_for_YWFrame, L"YWFrame", 0, 1, 5);

        auto YWListFunction = reinterpret_cast<FunctionCalls::tYWList>(WListFunctionAddress);
        auto YWFrameFunction = reinterpret_cast<FunctionCalls::tYWFrame>(YWFrameFunctionAddress);

        uintptr_t YWButtonFunctionAddress = ReadLEA32(Patterns::pattern_for_YWButton, L"YWButton", 0, 1, 5);
        FunctionCalls::tYWButton YWButtonFunction = reinterpret_cast<FunctionCalls::tYWButton>(YWButtonFunctionAddress);

        uintptr_t YWToggleButtonFunctionAddress = ReadLEA32(Patterns::pattern_for_YWToggleButton, L"YWToggleButton", 0, 1, 5);
        FunctionCalls::tYWToggleButton YWToggleButtonFunction = reinterpret_cast<FunctionCalls::tYWToggleButton>(YWToggleButtonFunctionAddress);

        auto* Menu = OpenSingletonWindowFunction("Cinematic Camera Settings", 280, 240, 0);
        if (!Menu)
            return;

        if (Menu != s_lastMenu)
        {
            s_lastMenu = Menu;

            // --- PLAY / STOP / AUTO buttons ---
            auto* btnPlay = (YW::BUTTON_3*)AllocYWMemoryFunction((YW::MEMORY*)0x200);
            auto* btnStop = (YW::BUTTON_3*)AllocYWMemoryFunction((YW::MEMORY*)0x200);
            auto* btnAuto = (YW::BUTTON_3*)AllocYWMemoryFunction((YW::MEMORY*)0x200);

            // 3 buttons, 80 px wide each, small gaps, all fit in 280px
            YWButtonFunction(btnPlay, "PLAY", 10, 10, 80, 16, CineBtn_Play, nullptr);
            YWButtonFunction(btnStop, "STOP", 100, 10, 80, 16, CineBtn_Stop, nullptr);
            YWButtonFunction(btnAuto, "AUTO", 190, 10, 80, 16, CineBtn_AutoPlay, nullptr);

            ContainerLinkFunction((YW::CONTAINER*)Menu, (YW::WIDGET*)btnPlay);
            ContainerLinkFunction((YW::CONTAINER*)Menu, (YW::WIDGET*)btnStop);
            ContainerLinkFunction((YW::CONTAINER*)Menu, (YW::WIDGET*)btnAuto);

            // --- Toggle: Game + Cine (under buttons) ---
            auto* btnToggleFreecam = (YW::TOGGLE_BUTTON_0*)AllocYWMemoryFunction((YW::MEMORY*)0x200);
            YWToggleButtonFunction(
                btnToggleFreecam,
                "Game + Cine",
                10, 30,                // below top row
                260, 16,
                g_cineGameplayEnabled,
                CineToggle_FreecamDuringPlayback,
                nullptr
            );
            ContainerLinkFunction((YW::CONTAINER*)Menu, (YW::WIDGET*)btnToggleFreecam);

            // --- Cine Speed label + slider (shifted down to avoid toggle) ---
            auto* lblCineSpeed = AllocYWMemoryFunction((YW::MEMORY*)0x200);
            YWLabelFunction((YW::LABEL_3*)lblCineSpeed, "Cine Speed:", 10, 55, 100, 12);
            ContainerLinkFunction((YW::CONTAINER*)Menu, (YW::WIDGET*)lblCineSpeed);

            g_CineSlider = (YW::SCROLLBAR*)AllocYWMemoryFunctionNew(0xA0);
            {
                float cineSpeed = g_cinematicCamera.getPlaybackSpeed();
                cineSpeed = std::clamp(cineSpeed, 0.1f, 5.0f);

                YWScrollbarFunction(
                    g_CineSlider,
                    110, 55,        // y matches label
                    150, 7,
                    0.1f,
                    5.0f,
                    cineSpeed,
                    0.1f
                );
                *reinterpret_cast<void***>(g_CineSlider) = reinterpret_cast<void**>(moduleBase1 + 0x600F48);
                ContainerLinkFunction((YW::CONTAINER*)Menu, (YW::WIDGET*)g_CineSlider);
            }

            // --- Time Scale label + slider ---
            auto* lblTimeScale = AllocYWMemoryFunction((YW::MEMORY*)0x200);
            YWLabelFunction((YW::LABEL_3*)lblTimeScale, "Time Scale:", 10, 75, 100, 12);
            ContainerLinkFunction((YW::CONTAINER*)Menu, (YW::WIDGET*)lblTimeScale);

            g_TimeScaleSlider = (YW::SCROLLBAR*)AllocYWMemoryFunctionNew(0xA0);
            {
                float gameSpeed = ReadGameSpeed();
                YWScrollbarFunction(
                    g_TimeScaleSlider,
                    110, 75,
                    150, 7,
                    0.1f, 3.0f,
                    gameSpeed,
                    0.1f
                );
                *reinterpret_cast<void***>(g_TimeScaleSlider) = reinterpret_cast<void**>(moduleBase1 + 0x600F48);
                g_TimeScaleSlider->Value = gameSpeed;
                ContainerLinkFunction((YW::CONTAINER*)Menu, (YW::WIDGET*)g_TimeScaleSlider);
            }

            // --- Timeline cursor slider ---
            auto* lblCursor = AllocYWMemoryFunction((YW::MEMORY*)0x200);
            YWLabelFunction((YW::LABEL_3*)lblCursor, "Timeline:", 10, 95, 100, 12);
            ContainerLinkFunction((YW::CONTAINER*)Menu, (YW::WIDGET*)lblCursor);

            g_TimelineScaleSlider = (YW::SCROLLBAR*)AllocYWMemoryFunctionNew(0xA0);
            YWScrollbarFunction(g_TimelineScaleSlider, 110, 95, 150, 7, 0.0f, 60.0f, g_cineTimelineTime, 0.1f);
            *reinterpret_cast<void***>(g_TimelineScaleSlider) = reinterpret_cast<void**>(moduleBase1 + 0x600F48);
            ContainerLinkFunction((YW::CONTAINER*)Menu, (YW::WIDGET*)g_TimelineScaleSlider);

            // --- Record Key / Reset Keys ---
            auto* btnRecord = (YW::BUTTON_3*)AllocYWMemoryFunction((YW::MEMORY*)0x200);
            auto* btnResetKeys = (YW::BUTTON_3*)AllocYWMemoryFunction((YW::MEMORY*)0x200);

            YWButtonFunction(btnRecord, "Record Key", 10, 120, 100, 16, CineBtn_RecordKey, nullptr);
            YWButtonFunction(btnResetKeys, "Reset Keys", 140, 120, 100, 16, CineBtn_ResetKeys, nullptr);

            ContainerLinkFunction((YW::CONTAINER*)Menu, (YW::WIDGET*)btnRecord);
            ContainerLinkFunction((YW::CONTAINER*)Menu, (YW::WIDGET*)btnResetKeys);

            // --- Prev / Next / Delete / Save / Load ---
            auto* btnPrev = (YW::BUTTON_3*)AllocYWMemoryFunction((YW::MEMORY*)0x200);
            auto* btnNext = (YW::BUTTON_3*)AllocYWMemoryFunction((YW::MEMORY*)0x200);
            auto* btnDelete = (YW::BUTTON_3*)AllocYWMemoryFunction((YW::MEMORY*)0x200);
            auto* btnSave = (YW::BUTTON_3*)AllocYWMemoryFunction((YW::MEMORY*)0x200);
            auto* btnLoad = (YW::BUTTON_3*)AllocYWMemoryFunction((YW::MEMORY*)0x200);

            YWButtonFunction(btnPrev, "< Prev", 10, 145, 60, 16, CineBtn_PrevKey, nullptr);
            YWButtonFunction(btnNext, "Next >", 80, 145, 60, 16, CineBtn_NextKey, nullptr);
            YWButtonFunction(btnDelete, "Delete", 150, 145, 60, 16, CineBtn_DeleteKey, nullptr);
            YWButtonFunction(btnSave, "Save", 215, 145, 50, 16, CineBtn_SaveKeys, nullptr);
            YWButtonFunction(btnLoad, "Load...", 10, 170, 80, 16, CinematicCameraLoadMenu, nullptr);

            ContainerLinkFunction((YW::CONTAINER*)Menu, (YW::WIDGET*)btnPrev);
            ContainerLinkFunction((YW::CONTAINER*)Menu, (YW::WIDGET*)btnNext);
            ContainerLinkFunction((YW::CONTAINER*)Menu, (YW::WIDGET*)btnDelete);
            ContainerLinkFunction((YW::CONTAINER*)Menu, (YW::WIDGET*)btnSave);
            ContainerLinkFunction((YW::CONTAINER*)Menu, (YW::WIDGET*)btnLoad);
        }

        ShowDebugWindowFunction((YW::WIDGET*)Menu);
    }

    void ChangeOBJTarget(unsigned int* arg) {
        uintptr_t ReplaceTargetFunctionAddress = ReadLEA32(Patterns::pattern_for_ReplaceTarget, L"Replace target", 0, 1, 5);
        FunctionCalls::tTargetReplace ReplaceTargetFunction = reinterpret_cast<FunctionCalls::tTargetReplace>(ReplaceTargetFunctionAddress);

        uintptr_t IsOBJValidFunctionAddress = ReadLEA32(Patterns::pattern_for_IsObjValid, L"Is OBJ Valid?", 0, 1, 5);
        FunctionCalls::tIsOBJValid IsOBJValidFunction = reinterpret_cast<FunctionCalls::tIsOBJValid>(IsOBJValidFunctionAddress);

        YS::OBJ* obj = GetMenuOBJFunction();
        if (!obj) {
            return;
        }

        gGlobalObjPtr1 = obj;

        
            ReplaceTargetFunction(GetPlayerFunction(), gGlobalObjPtr1);
            
        

    }

    void ChangeObjTexanm(unsigned int* arg) {
        uintptr_t SetDialValueFunctionAddress = FindAndPrintPatternW(Patterns::pattern_for_SetDialValue, L"Set DialValue", 0);

        FunctionCalls::tSetDialValue SetDialValueFunction = reinterpret_cast<FunctionCalls::tSetDialValue>(SetDialValueFunctionAddress);

        uintptr_t YWLabelFunctionAddress = ReadLEA32(Patterns::pattern_for_YWLabel, L"YWlabel", 0, 1, 5);

        FunctionCalls::tYWLabel YWLabelFunction = reinterpret_cast<FunctionCalls::tYWLabel>(YWLabelFunctionAddress);

        uintptr_t OpenSingletonWindowFunctionAddress = ReadLEA32(Patterns::pattern_for_OpenSingletonWindow, L"OpenSingletonWindow", 0, 1, 5);

        FunctionCalls::tOpenSingletonWindow OpenSingletonWindowFunction = reinterpret_cast<FunctionCalls::tOpenSingletonWindow>(OpenSingletonWindowFunctionAddress);

        //uintptr_t YWLabelFunctionAddress = ReadLEA32(Patterns::pattern_for_YWLabel, L"YWlabel", 0, 1, 5);

        //FunctionCalls::tYWLabel YWLabelFunction = reinterpret_cast<FunctionCalls::tYWLabel>(YWLabelFunctionAddress);

        uintptr_t ShowDialWidgetFunctionAddress = ReadLEA32(Patterns::pattern_for_DialWidget, L"Dial Widget", 0, 1, 5);
        FunctionCalls::tDialWidget ShowDialWidgetFunction = reinterpret_cast<FunctionCalls::tDialWidget>(ShowDialWidgetFunctionAddress);


        uintptr_t GetOBJEntrynameFunctionAddress = ReadLEA32(Patterns::pattern_for_GetObjEntryName, L"Get obj Entryname", 0, 1, 5);
        FunctionCalls::tGetObjEntryName GetOBJEntrynameFunction = reinterpret_cast<FunctionCalls::tGetObjEntryName>(GetOBJEntrynameFunctionAddress);



        uintptr_t ShowDebugWindowFunctionAddress = ReadLEA32(Patterns::pattern_for_ShowDebugWindow, L"ShowDebugWindow", 0, 1, 5);
        FunctionCalls::tShowDebugWindow ShowDebugWindowFunction = reinterpret_cast<FunctionCalls::tShowDebugWindow>(ShowDebugWindowFunctionAddress);


        uintptr_t AllocYWMemoryFunctionAddress = ReadLEA32(Patterns::pattern_for_Alloc_YW_Memory, L"AllocYWMemory", 0, 1, 5);

        FunctionCalls::tAlloc_YW_Memory AllocYWMemoryFunction = reinterpret_cast<FunctionCalls::tAlloc_YW_Memory>(AllocYWMemoryFunctionAddress);


        uintptr_t ContainerLinkFunctionAddress = FindAndPrintPatternW(Patterns::pattern_for_ContainerLink, L"ContainerLink", 0);

        FunctionCalls::tContainerLink ContainerLinkFunction = reinterpret_cast<FunctionCalls::tContainerLink>(ContainerLinkFunctionAddress);

       // YS::OBJ* obj = GetCachedMenuOBJ();

        //if (!obj) {
       //     return;
      //  }

        const char* myObj = GetOBJEntrynameFunction(gGlobalObjPtr1);
        std::string windowTitle = std::string("Texanm ") + myObj;
        auto* Menu = OpenSingletonWindowFunction(windowTitle.c_str(), 280, 156, 0);
        auto allocYWMemLabelScale = AllocYWMemoryFunction((YW::MEMORY*)0x200);
        auto allocYWMemDialScale = AllocYWMemoryFunction((YW::MEMORY*)0x200);




        YWLabelFunction((YW::LABEL_3*)allocYWMemLabelScale, "Texanm ID:", 5, 5, 120, 12);

        ShowDialWidgetFunction((YW::DIAL_3*)allocYWMemDialScale, 140, 5, 56, 16, 0, 200, ObjectTexanmCallBack);

        SetDialValueFunction((YW::DIAL_3*)allocYWMemDialScale, gObjectTexanmID);

        ContainerLinkFunction((YW::CONTAINER*)Menu, (YW::WIDGET*)allocYWMemLabelScale);

        ContainerLinkFunction((YW::CONTAINER*)Menu, (YW::WIDGET*)allocYWMemDialScale);

        LOG("gGlobalObjPtr1 = 0x%p\n", gGlobalObjPtr1);

        ShowDebugWindowFunction((YW::WIDGET*)Menu);



    }

    void knPartsSelectorTogglesCallBack(kn::PartsSelector* a, int n) {

        uintptr_t knModelOBJResetStealthFunctionAddress = FindAndPrintPatternW(Patterns::pattern_for_knModelOBJResetStealth, L"kn model obj reset stealth", 0);

        FunctionCalls::tknModelOBJResetStealth knModelOBJResetStealthFunction = reinterpret_cast<FunctionCalls::tknModelOBJResetStealth>(knModelOBJResetStealthFunctionAddress);

        uintptr_t knModelOBJSetStealthFunctionAddress = FindAndPrintPatternW(Patterns::pattern_for_knModelOBJSetStealth, L"kn model obj set stealth", 0);

        FunctionCalls::tknModelOBJSetStealth knModelOBJSetStealthFunction = reinterpret_cast<FunctionCalls::tknModelOBJSetStealth>(knModelOBJSetStealthFunctionAddress);

        uintptr_t OBJShowPartFunctionAddress = FindAndPrintPatternW(Patterns::pattern_for_YSOBJShowPart, L"Show OBJ Part", 0);

        FunctionCalls::tShowOBJPart OBJShowPartFunction = reinterpret_cast<FunctionCalls::tShowOBJPart>(OBJShowPartFunctionAddress);

        uintptr_t OBJHidePartFunctionAddress = FindAndPrintPatternW(Patterns::pattern_for_YSOBJHidePart, L"Hide OBJ Part", 0);

        FunctionCalls::tHideOBJPart OBJHidePartFunction = reinterpret_cast<FunctionCalls::tHideOBJPart>(OBJHidePartFunctionAddress);

        int v3; 
        kn::PartsSelector::MODE v4; 
        kn::PartsSelector::MODE v5;

        v3 = 1 << n;
        if ((a->parts_ & (1 << n)) != 0) {
            a->label_[n] = PartsSelectorLabels[n];
            v4 = a->mode_;
            if (v4 > kn::PartsSelector::MODE::MODE_DISPLAY) {
                if (v4 <= kn::PartsSelector::MODE::MODE_STEALTH) 
                    knModelOBJResetStealthFunction((kn::ModelObj_9*)a->Obj + 0x7B0, n);
                

            }
            else if ((static_cast<__int32>(v4) > -1)) {
                OBJHidePartFunction(a->Obj, n);
            }
        }
        else {
            v5 = a->mode_;
            a->label_[n] = PartsSelectorLabelsSelected[n];
            if (v5 > kn::PartsSelector::MODE::MODE_DISPLAY) {
                if (v5 <= kn::PartsSelector::MODE::MODE_STEALTH)
                    knModelOBJSetStealthFunction((kn::ModelObj_9*)a->Obj + 0x7B0, n);
                
            }
            else if ((static_cast<__int32>(v5)) > -1) {
                OBJShowPartFunction(a->Obj, n);
            }
        }

        a->parts_ ^= v3;
    }

    void PartsSelector(YW::EVENT event, int index, unsigned int* arg) {

        knPartsSelectorTogglesCallBack((kn::PartsSelector*)arg, index);

    }

    void knPartsSelector(kn::PartsSelector* a, YS::OBJ* obj, kn::PartsSelector::MODE mode) {
        uintptr_t YWWindowFunctionAddress = ReadLEA32(Patterns::pattern_for_YWwindow, L"YW Window", 0, 1, 5);

        FunctionCalls::tYWWindow YWWindowFunction = reinterpret_cast<FunctionCalls::tYWWindow>(YWWindowFunctionAddress);

        uintptr_t GetOBJEntrynameFunctionAddress = ReadLEA32(Patterns::pattern_for_GetObjEntryName, L"Get obj Entryname", 0, 1, 5);
        FunctionCalls::tGetObjEntryName GetOBJEntrynameFunction = reinterpret_cast<FunctionCalls::tGetObjEntryName>(GetOBJEntrynameFunctionAddress);

        uintptr_t ShowDebugWindowFunctionAddress = ReadLEA32(Patterns::pattern_for_ShowDebugWindow, L"ShowDebugWindow", 0, 1, 5);
        FunctionCalls::tShowDebugWindow ShowDebugWindowFunction = reinterpret_cast<FunctionCalls::tShowDebugWindow>(ShowDebugWindowFunctionAddress);


        uintptr_t AllocYWMemoryFunctionAddress = ReadLEA32(Patterns::pattern_for_Alloc_YW_Memory, L"AllocYWMemory", 0, 1, 5);

        FunctionCalls::tAlloc_YW_Memory AllocYWMemoryFunction = reinterpret_cast<FunctionCalls::tAlloc_YW_Memory>(AllocYWMemoryFunctionAddress);

        uintptr_t ContainerLinkFunctionAddress = FindAndPrintPatternW(Patterns::pattern_for_ContainerLink, L"ContainerLink", 0);

        FunctionCalls::tContainerLink ContainerLinkFunction = reinterpret_cast<FunctionCalls::tContainerLink>(ContainerLinkFunctionAddress);

        uintptr_t YWFrameFunctionAddress = ReadLEA32(Patterns::pattern_for_YWFrame, L"YWFrame", 0, 1, 5);
        FunctionCalls::tYWFrame YWFrameFunction = reinterpret_cast<FunctionCalls::tYWFrame>(YWFrameFunctionAddress);

        uintptr_t WListFunctionAddress = ReadLEA32(Patterns::pattern_for_YWList, L"YWList", 0, 1, 5);
        FunctionCalls::tYWList YWListFunction = reinterpret_cast<FunctionCalls::tYWList>(WListFunctionAddress);

        uintptr_t knModelOBJResetStealthFunctionAddress = FindAndPrintPatternW(Patterns::pattern_for_knModelOBJResetStealth, L"kn model obj reset stealth", 0);

        FunctionCalls::tknModelOBJResetStealth knModelOBJResetStealthFunction = reinterpret_cast<FunctionCalls::tknModelOBJResetStealth>(knModelOBJResetStealthFunctionAddress);

        uintptr_t OBJShowPartFunctionAddress = FindAndPrintPatternW(Patterns::pattern_for_YSOBJShowPart, L"Show OBJ Part", 0);

        FunctionCalls::tShowOBJPart OBJShowPartFunction = reinterpret_cast<FunctionCalls::tShowOBJPart>(OBJShowPartFunctionAddress);

        


        const char* title = "";
        YS::OBJ* selectedOBJ;
        char* v6;
        int v7;
        kn::PartsSelector* v8;
        int v9;
        char** v10;
        char* v11;
        int v13;
        kn::PartsSelector* v14;
        int v15;
        char** v16;
        char** v17;
        int v18;
        char* v19;
        YW::WIDGET* v20;
        unsigned int* v22;
        unsigned int* v23;
        unsigned int* v25;
        unsigned int* v26;
        YW::CONTAINER* v27;

        if (mode == kn::PartsSelector::MODE::MODE_STEALTH) {
            title = "stealth";
        }
        else if (mode == kn::PartsSelector::MODE::MODE_DISPLAY) {
            title = "display";
        }
        YWWindowFunction(a, title, 10, 10, 150, 352, 0);
        a->mode_ = mode;

        if (mode > kn::PartsSelector::MODE::MODE_DISPLAY) {
            if (mode <= kn::PartsSelector::MODE::MODE_STEALTH) {
                selectedOBJ = obj;
                v13 = 0;
                a->parts_ = 0;
                knModelOBJResetStealthFunction((kn::ModelObj_9*)selectedOBJ + 0x7B0, -1);
                v14 = a;
                v15 = 8;

                do {
                    v14->label_[0] = PartsSelectorLabels[v13];
                    v16 = &v14->label_[2];
                    v14->label_[1] = PartsSelectorLabels[v13 + 1];
                    v17 = &v14->label_[3];
                    v18 = v13 * 4 + 12;
                    v19 = PartsSelectorLabels[v13 + 2];
                    v13 += 4;
                    v14 = (kn::PartsSelector*)((char*)v14 + 16);
                    *v16 = v19;
                    *v17 = *(char**)((char*)PartsSelectorLabels + v18);
                    --v15;
                } while (v15);
            }
        }
        else if (static_cast<__int32>(mode) > -1) {
            a->parts_ = -1;
            OBJShowPartFunction(obj, -1);
            v7 = 0;
            v8 = a;
            v9 = 0;

            do
            {
                v10 = v8->label_;
                ++v7;
                v8 = (kn::PartsSelector*)((char*)v8 + 4);
                v11 = PartsSelectorLabelsSelected[v9++];
                *v10 = v11;
            } while (v7 < 32);

        }

        v20 = 0;
        a->label_[32] = 0;

        auto allocYWList = AllocYWMemoryFunction((YW::MEMORY*)0x200);

        v23 = allocYWList;

        YWListFunction((YW::YW_LIST_0*)allocYWList, 0, 0, (const char**)a->label_, PartsSelector, allocYWList);

        v20 = (YW::WIDGET*)v23;

        //auto allocYWFrame = AllocYWMemoryFunction((YW::MEMORY*)0x200);

        //v26 = allocYWFrame;
        //v27 = 0;

        //YWFrameFunction((YW::FRAME*)allocYWFrame, 0, 0, 0, -10, 0, v20->Rect.H);

        //v27 = (YW::CONTAINER*)v26;


        ContainerLinkFunction(a, (YW::WIDGET*)allocYWList);
        //ContainerLinkFunction(v27, v20);

        ShowDebugWindowFunction(a);

    }

    void OBJDisplayMenuCallBack(int n) {
        YS::OBJ* obj = GetCachedMenuOBJ();

        if (!obj) {
            return;
        }

        uintptr_t AllocYWMemoryFunctionAddress = ReadLEA32(Patterns::pattern_for_Alloc_YW_Memory, L"AllocYWMemory", 0, 1, 5);

        FunctionCalls::tAlloc_YW_Memory AllocYWMemoryFunction = reinterpret_cast<FunctionCalls::tAlloc_YW_Memory>(AllocYWMemoryFunctionAddress);

        auto allocknPartsSelector = AllocYWMemoryFunction((YW::MEMORY*)0x200);

        kn::PartsSelector::MODE mode = kn::PartsSelector::MODE::MODE_DISPLAY;

        knPartsSelector((kn::PartsSelector*)allocknPartsSelector, gGlobalObjPtr, mode);

    }

    void OBJStealthMenuCallBack(int n) {
        YS::OBJ* obj = GetCachedMenuOBJ();

        if (!obj) {
            return;
        }

        uintptr_t AllocYWMemoryFunctionAddress = ReadLEA32(Patterns::pattern_for_Alloc_YW_Memory, L"AllocYWMemory", 0, 1, 5);

        FunctionCalls::tAlloc_YW_Memory AllocYWMemoryFunction = reinterpret_cast<FunctionCalls::tAlloc_YW_Memory>(AllocYWMemoryFunctionAddress);

        auto allocknPartsSelector = AllocYWMemoryFunction((YW::MEMORY*)0x200);

        kn::PartsSelector::MODE mode = kn::PartsSelector::MODE::MODE_STEALTH;

        knPartsSelector((kn::PartsSelector*)allocknPartsSelector, gGlobalObjPtr, mode);

    }

    void BonusLevelMenuOpen(int n) {
        uintptr_t YWWindowFunctionAddress = ReadLEA32(Patterns::pattern_for_YWwindow, L"YW Window", 0, 1, 5);

        FunctionCalls::tYWWindow YWWindowFunction = reinterpret_cast<FunctionCalls::tYWWindow>(YWWindowFunctionAddress);

        uintptr_t GetOBJEntrynameFunctionAddress = ReadLEA32(Patterns::pattern_for_GetObjEntryName, L"Get obj Entryname", 0, 1, 5);
        FunctionCalls::tGetObjEntryName GetOBJEntrynameFunction = reinterpret_cast<FunctionCalls::tGetObjEntryName>(GetOBJEntrynameFunctionAddress);

        uintptr_t ShowDebugWindowFunctionAddress = ReadLEA32(Patterns::pattern_for_ShowDebugWindow, L"ShowDebugWindow", 0, 1, 5);
        FunctionCalls::tShowDebugWindow ShowDebugWindowFunction = reinterpret_cast<FunctionCalls::tShowDebugWindow>(ShowDebugWindowFunctionAddress);


        uintptr_t AllocYWMemoryFunctionAddress = ReadLEA32(Patterns::pattern_for_Alloc_YW_Memory, L"AllocYWMemory", 0, 1, 5);

        FunctionCalls::tAlloc_YW_Memory AllocYWMemoryFunction = reinterpret_cast<FunctionCalls::tAlloc_YW_Memory>(AllocYWMemoryFunctionAddress);

        uintptr_t ContainerLinkFunctionAddress = FindAndPrintPatternW(Patterns::pattern_for_ContainerLink, L"ContainerLink", 0);

        FunctionCalls::tContainerLink ContainerLinkFunction = reinterpret_cast<FunctionCalls::tContainerLink>(ContainerLinkFunctionAddress);

        uintptr_t YWFrameFunctionAddress = ReadLEA32(Patterns::pattern_for_YWFrame, L"YWFrame", 0, 1, 5);
        FunctionCalls::tYWFrame YWFrameFunction = reinterpret_cast<FunctionCalls::tYWFrame>(YWFrameFunctionAddress);

        uintptr_t WListFunctionAddress = ReadLEA32(Patterns::pattern_for_YWList, L"YWList", 0, 1, 5);
        FunctionCalls::tYWList YWListFunction = reinterpret_cast<FunctionCalls::tYWList>(WListFunctionAddress);

        uintptr_t TestFlagWidgetFunctionAddress = ReadLEA32(Patterns::pattern_for_TestFlagWidget, L"Test flag widget", 0, 1, 5);

        FunctionCalls::tTestFlagWidget TestFlagWidgetFunction = reinterpret_cast<FunctionCalls::tTestFlagWidget>(TestFlagWidgetFunctionAddress);

        uintptr_t AllocDebugMemoryFunctionAddress = ReadLEA32(Patterns::pattern_for_AllocDbgMem, L"Alloc debug memory", 0, 1, 5);

        FunctionCalls::tAllocDebugMem AllocDebugMemoryFunction = reinterpret_cast<FunctionCalls::tAllocDebugMem>(AllocDebugMemoryFunctionAddress);

        uintptr_t OpenSingletonWindowFunctionAddress = ReadLEA32(Patterns::pattern_for_OpenSingletonWindow, L"OpenSingletonWindow", 0, 1, 5);

        FunctionCalls::tOpenSingletonWindow OpenSingletonWindowFunction = reinterpret_cast<FunctionCalls::tOpenSingletonWindow>(OpenSingletonWindowFunctionAddress);

        uintptr_t YWwindowMinSizeFunctionAddress = FindAndPrintPatternW(Patterns::pattern_for_YWwindow_minSize, L"YWwindow Min Size", 0);
        uintptr_t YWwindowMaxSizeFunctionAddress = FindAndPrintPatternW(Patterns::pattern_for_YWwindow_maxSize, L"YWwindow Max Size", 0);
        FunctionCalls::tYW_WINDOW_set_min_size YWwindowMinSizeFunction = reinterpret_cast<FunctionCalls::tYW_WINDOW_set_min_size>(YWwindowMinSizeFunctionAddress);
        FunctionCalls::tYW_WINDOW_set_max_size YWwindowMaxSizeFunction = reinterpret_cast<FunctionCalls::tYW_WINDOW_set_max_size>(YWwindowMaxSizeFunctionAddress);


        /*YW::WIDGET* v1;
        unsigned int* v2;
        unsigned int* v3;
        YW::CONTAINER* v4;
        unsigned int* v6;
        unsigned int* v7;
        YW::YW_WINDOW_39* v8;*/

        YW::YW_WINDOW_39* Menu = OpenSingletonWindowFunction("bonuslevel", 154, 144, 2);
        
       // unsigned int* TestFlag = AllocDebugMemoryFunction(0x200);
        unsigned int* TestFlag = AllocYWMemoryFunction((YW::MEMORY*)0x200);
        unsigned int* Frame = AllocYWMemoryFunction((YW::MEMORY*)0x200);

        YW::WIDGET* TestFlagwidget;
        TestFlagwidget = 0;
        
            TestFlagWidgetFunction((YS::TEST_FLAG*)TestFlag, "dbg/worldflag.dbg");
            //v1 = (YW::WIDGET*)v3;
            TestFlagwidget = (YW::WIDGET*)TestFlag;

            YW::FRAME* FrameWidget;
            FrameWidget = 0;

            YWFrameFunction((YW::FRAME*)Frame, 0, 0, 0, 0, 0, TestFlagwidget->Rect.H);
            FrameWidget = (YW::FRAME*)Frame;
        
        
        ContainerLinkFunction(FrameWidget, TestFlagwidget);
        
        ContainerLinkFunction(Menu, FrameWidget);
        YWwindowMaxSizeFunction(Menu, 144, TestFlagwidget->Rect.H);
        YWwindowMinSizeFunction(Menu, 144, 0);
        ShowDebugWindowFunction(Menu);


    }

    void PlayerEntrymenuListCallBack(YW::EVENT event, int index, unsigned int* arg) {
        if ((event.Flag & 0x20) != 0) {
            if (index >= 0 && index < sizeof(PlayerList_values) / sizeof(PlayerList_values[0])) {
                unsigned int soraValue = PlayerList_values[index];
                NewSora(soraValue);
            }
        }
    }

    OBJENTRY_ID GetOBJEntryIDByName(const char* targetName) {
        uintptr_t GetOBJENTRYFunctionAddress = ReadLEA32(Patterns::pattern_for_GetOBJENTRY, L"Get OBJENTRY", 0, 1, 5);
        auto GetOBJENTRYFunction = reinterpret_cast<FunctionCalls::tGetOBJENTRY>(GetOBJENTRYFunctionAddress);

        for (OBJENTRY_ID id = 0; id <= 2561; ++id) {
            OBJENTRY_0* entry = GetOBJENTRYFunction(id);
            if (entry && strcmp(entry->EntryName, targetName) == 0) {
                return entry->Id;  // or just return `id`
            }
        }

        return (OBJENTRY_ID)-1;  // Not found
    }

    void PlayerEntryMenuOpen(int n) {
        uintptr_t YWWindowFunctionAddress = ReadLEA32(Patterns::pattern_for_YWwindow, L"YW Window", 0, 1, 5);

        FunctionCalls::tYWWindow YWWindowFunction = reinterpret_cast<FunctionCalls::tYWWindow>(YWWindowFunctionAddress);

        uintptr_t GetOBJEntrynameFunctionAddress = ReadLEA32(Patterns::pattern_for_GetObjEntryName, L"Get obj Entryname", 0, 1, 5);
        FunctionCalls::tGetObjEntryName GetOBJEntrynameFunction = reinterpret_cast<FunctionCalls::tGetObjEntryName>(GetOBJEntrynameFunctionAddress);

        uintptr_t ShowDebugWindowFunctionAddress = ReadLEA32(Patterns::pattern_for_ShowDebugWindow, L"ShowDebugWindow", 0, 1, 5);
        FunctionCalls::tShowDebugWindow ShowDebugWindowFunction = reinterpret_cast<FunctionCalls::tShowDebugWindow>(ShowDebugWindowFunctionAddress);


        uintptr_t AllocYWMemoryFunctionAddress = ReadLEA32(Patterns::pattern_for_Alloc_YW_Memory, L"AllocYWMemory", 0, 1, 5);

        FunctionCalls::tAlloc_YW_Memory AllocYWMemoryFunction = reinterpret_cast<FunctionCalls::tAlloc_YW_Memory>(AllocYWMemoryFunctionAddress);

        uintptr_t ContainerLinkFunctionAddress = FindAndPrintPatternW(Patterns::pattern_for_ContainerLink, L"ContainerLink", 0);

        FunctionCalls::tContainerLink ContainerLinkFunction = reinterpret_cast<FunctionCalls::tContainerLink>(ContainerLinkFunctionAddress);

        uintptr_t YWFrameFunctionAddress = ReadLEA32(Patterns::pattern_for_YWFrame, L"YWFrame", 0, 1, 5);
        FunctionCalls::tYWFrame YWFrameFunction = reinterpret_cast<FunctionCalls::tYWFrame>(YWFrameFunctionAddress);

        uintptr_t WListFunctionAddress = ReadLEA32(Patterns::pattern_for_YWList, L"YWList", 0, 1, 5);
        FunctionCalls::tYWList YWListFunction = reinterpret_cast<FunctionCalls::tYWList>(WListFunctionAddress);

        uintptr_t TestFlagWidgetFunctionAddress = ReadLEA32(Patterns::pattern_for_TestFlagWidget, L"Test flag widget", 0, 1, 5);

        FunctionCalls::tTestFlagWidget TestFlagWidgetFunction = reinterpret_cast<FunctionCalls::tTestFlagWidget>(TestFlagWidgetFunctionAddress);

        uintptr_t AllocDebugMemoryFunctionAddress = ReadLEA32(Patterns::pattern_for_AllocDbgMem, L"Alloc debug memory", 0, 1, 5);

        FunctionCalls::tAllocDebugMem AllocDebugMemoryFunction = reinterpret_cast<FunctionCalls::tAllocDebugMem>(AllocDebugMemoryFunctionAddress);

        uintptr_t OpenSingletonWindowFunctionAddress = ReadLEA32(Patterns::pattern_for_OpenSingletonWindow, L"OpenSingletonWindow", 0, 1, 5);

        FunctionCalls::tOpenSingletonWindow OpenSingletonWindowFunction = reinterpret_cast<FunctionCalls::tOpenSingletonWindow>(OpenSingletonWindowFunctionAddress);

        uintptr_t YWwindowMinSizeFunctionAddress = FindAndPrintPatternW(Patterns::pattern_for_YWwindow_minSize, L"YWwindow Min Size", 0);
        uintptr_t YWwindowMaxSizeFunctionAddress = FindAndPrintPatternW(Patterns::pattern_for_YWwindow_maxSize, L"YWwindow Max Size", 0);
        FunctionCalls::tYW_WINDOW_set_min_size YWwindowMinSizeFunction = reinterpret_cast<FunctionCalls::tYW_WINDOW_set_min_size>(YWwindowMinSizeFunctionAddress);
        FunctionCalls::tYW_WINDOW_set_max_size YWwindowMaxSizeFunction = reinterpret_cast<FunctionCalls::tYW_WINDOW_set_max_size>(YWwindowMaxSizeFunctionAddress);


        /*YW::WIDGET* v1;
        unsigned int* v2;
        unsigned int* v3;
        YW::CONTAINER* v4;
        unsigned int* v6;
        unsigned int* v7;
        YW::YW_WINDOW_39* v8;*/

        YW::YW_WINDOW_39* Menu = OpenSingletonWindowFunction("Player Menu", 154, 144, 2);

        // unsigned int* TestFlag = AllocDebugMemoryFunction(0x200);
        unsigned int* list = AllocYWMemoryFunction((YW::MEMORY*)0x200);
        unsigned int* Frame = AllocYWMemoryFunction((YW::MEMORY*)0x200);

        YW::WIDGET* ListWidget;
        ListWidget = 0;

        //TestFlagWidgetFunction((YS::TEST_FLAG*)TestFlag, "dbg/worldflag.dbg");
        //v1 = (YW::WIDGET*)v3;

        YWListFunction((YW::YW_LIST_0*)list, 0, 0, PlayerList, PlayerEntrymenuListCallBack,list);

        ListWidget = (YW::WIDGET*)list;

        YW::FRAME* FrameWidget;
        FrameWidget = 0;

        YWFrameFunction((YW::FRAME*)Frame, 0, 0, 0, 0, 0, ListWidget->Rect.H);
        FrameWidget = (YW::FRAME*)Frame;


        ContainerLinkFunction(FrameWidget, ListWidget);

        ContainerLinkFunction(Menu, FrameWidget);
        YWwindowMaxSizeFunction(Menu, ListWidget->Rect.W, ListWidget->Rect.H);
        YWwindowMinSizeFunction(Menu, ListWidget->Rect.W, 0);
        ShowDebugWindowFunction(Menu);


    }


    


    void NpcEntryListCallback(YW::EVENT event, int index, unsigned int* arg) {
        if ((event.Flag & 0x20) && index >= 0 && index < npcMenu.count) {
            const char* name = npcMenu.entries[index];
            OBJENTRY_ID id = GetOBJEntryIDByName(name);
            if (id != (OBJENTRY_ID)-1) {
                char message[128];
                snprintf(message, sizeof(message), "How many %s to spawn?", name);
                ShowSpawnAmountDialog(message, 1, 50, id);
                
                
            }
        }
    }

    void BossEntryListCallback(YW::EVENT event, int index, unsigned int* arg) {
        if ((event.Flag & 0x20) && index >= 0 && index < bossMenu.count) {
            const char* name = bossMenu.entries[index];
            OBJENTRY_ID id = GetOBJEntryIDByName(name);
            if (id != (OBJENTRY_ID)-1) {
                char message[128];
                snprintf(message, sizeof(message), "How many %s to spawn?", name);
                ShowSpawnAmountDialog(message, 1, 50, id);
            }
        }
    }

    void ZakoEntryListCallback(YW::EVENT event, int index, unsigned int* arg) {
        if ((event.Flag & 0x20) && index >= 0 && index < zakoMenu.count) {
            const char* name = zakoMenu.entries[index];
            OBJENTRY_ID id = GetOBJEntryIDByName(name);
            if (id != (OBJENTRY_ID)-1) {
                char message[128];
                snprintf(message, sizeof(message), "How many %s to spawn?", name);
                ShowSpawnAmountDialog(message, 1, 50, id);
            }
        }
    }

    void WeaponEntryListCallback(YW::EVENT event, int index, unsigned int* arg) {
        if ((event.Flag & 0x20) && index >= 0 && index < weaponMenu.count) {
            const char* name = weaponMenu.entries[index];
            OBJENTRY_ID id = GetOBJEntryIDByName(name);
            if (id != (OBJENTRY_ID)-1) {
                char message[128];
                snprintf(message, sizeof(message), "How many %s to spawn?", name);
                ShowSpawnAmountDialog(message, 1, 50, id);
            }
        }
    }

    void EnemyWeaponEntryListCallback(YW::EVENT event, int index, unsigned int* arg) {
        if ((event.Flag & 0x20) && index >= 0 && index < enemyWeaponMenu.count) {
            const char* name = enemyWeaponMenu.entries[index];
            OBJENTRY_ID id = GetOBJEntryIDByName(name);
            if (id != (OBJENTRY_ID)-1) {
                char message[128];
                snprintf(message, sizeof(message), "How many %s to spawn?", name);
                ShowSpawnAmountDialog(message, 1, 50, id);
            }
        }
    }

    void SavepointEntryListCallback(YW::EVENT event, int index, unsigned int* arg) {
        if ((event.Flag & 0x20) && index >= 0 && index < savepointMenu.count) {
            const char* name = savepointMenu.entries[index];
            OBJENTRY_ID id = GetOBJEntryIDByName(name);
            if (id != (OBJENTRY_ID)-1) {
                char message[128];
                snprintf(message, sizeof(message), "How many %s to spawn?", name);
                ShowSpawnAmountDialog(message, 1, 50, id);
            }
        }
    }

    void FieldObjEntryListCallback(YW::EVENT event, int index, unsigned int* arg) {
        if ((event.Flag & 0x20) && index >= 0 && index < fieldObjMenu.count) {
            const char* name = fieldObjMenu.entries[index];
            OBJENTRY_ID id = GetOBJEntryIDByName(name);
            if (id != (OBJENTRY_ID)-1) {
                char message[128];
                snprintf(message, sizeof(message), "How many %s to spawn?", name);
                ShowSpawnAmountDialog(message, 1, 50, id);
            }
        }
    }

    void BattleNpcEntryListCallback(YW::EVENT event, int index, unsigned int* arg) {
        if ((event.Flag & 0x20) && index >= 0 && index < battleNpcMenu.count) {
            const char* name = battleNpcMenu.entries[index];
            OBJENTRY_ID id = GetOBJEntryIDByName(name);
            if (id != (OBJENTRY_ID)-1) {
                char message[128];
                snprintf(message, sizeof(message), "How many %s to spawn?", name);
                ShowSpawnAmountDialog(message, 1, 50, id);
            }
        }
    }

    void TreasureBoxEntryListCallback(YW::EVENT event, int index, unsigned int* arg) {
        if ((event.Flag & 0x20) && index >= 0 && index < treasureBoxMenu.count) {
            const char* name = treasureBoxMenu.entries[index];
            OBJENTRY_ID id = GetOBJEntryIDByName(name);
            if (id != (OBJENTRY_ID)-1) {
                char message[128];
                snprintf(message, sizeof(message), "How many %s to spawn?", name);
                ShowSpawnAmountDialog(message, 1, 50, id);
            }
        }
    }

    void SubmenuEntryListCallback(YW::EVENT event, int index, unsigned int* arg) {
        if ((event.Flag & 0x20) && index >= 0 && index < submenuMenu.count) {
            const char* name = submenuMenu.entries[index];
            OBJENTRY_ID id = GetOBJEntryIDByName(name);
            if (id != (OBJENTRY_ID)-1) {
                char message[128];
                snprintf(message, sizeof(message), "How many %s to spawn?", name);
                ShowSpawnAmountDialog(message, 1, 50, id);
            }
        }
    }

    void LargeBossEntryListCallback(YW::EVENT event, int index, unsigned int* arg) {
        if ((event.Flag & 0x20) && index >= 0 && index < largeBossMenu.count) {
            const char* name = largeBossMenu.entries[index];
            OBJENTRY_ID id = GetOBJEntryIDByName(name);
            if (id != (OBJENTRY_ID)-1) {
                char message[128];
                snprintf(message, sizeof(message), "How many %s to spawn?", name);
                ShowSpawnAmountDialog(message, 1, 50, id);
            }
        }
    }

    void GimmickObjEntryListCallback(YW::EVENT event, int index, unsigned int* arg) {
        if ((event.Flag & 0x20) && index >= 0 && index < gimmickObjMenu.count) {
            const char* name = gimmickObjMenu.entries[index];
            OBJENTRY_ID id = GetOBJEntryIDByName(name);
            if (id != (OBJENTRY_ID)-1) {
                char message[128];
                snprintf(message, sizeof(message), "How many %s to spawn?", name);
                ShowSpawnAmountDialog(message, 1, 50, id);
            }
        }
    }

    void MemoEntryListCallback(YW::EVENT event, int index, unsigned int* arg) {
        if ((event.Flag & 0x20) && index >= 0 && index < memoMenu.count) {
            const char* name = memoMenu.entries[index];
            OBJENTRY_ID id = GetOBJEntryIDByName(name);
            if (id != (OBJENTRY_ID)-1) {
                char message[128];
                snprintf(message, sizeof(message), "How many %s to spawn?", name);
                ShowSpawnAmountDialog(message, 1, 50, id);
            }
        }
    }

    void RtnEntryListCallback(YW::EVENT event, int index, unsigned int* arg) {
        if ((event.Flag & 0x20) && index >= 0 && index < rtnMenu.count) {
            const char* name = rtnMenu.entries[index];
            OBJENTRY_ID id = GetOBJEntryIDByName(name);
            if (id != (OBJENTRY_ID)-1) {
                char message[128];
                snprintf(message, sizeof(message), "How many %s to spawn?", name);
                ShowSpawnAmountDialog(message, 1, 50, id);
            }
        }
    }

    void MinigameEntryListCallback(YW::EVENT event, int index, unsigned int* arg) {
        if ((event.Flag & 0x20) && index >= 0 && index < minigameMenu.count) {
            const char* name = minigameMenu.entries[index];
            OBJENTRY_ID id = GetOBJEntryIDByName(name);
            if (id != (OBJENTRY_ID)-1) {
                char message[128];
                snprintf(message, sizeof(message), "How many %s to spawn?", name);
                ShowSpawnAmountDialog(message, 1, 50, id);
            }
        }
    }

    void WorldMapEntryListCallback(YW::EVENT event, int index, unsigned int* arg) {
        if ((event.Flag & 0x20) && index >= 0 && index < worldMapMenu.count) {
            const char* name = worldMapMenu.entries[index];
            OBJENTRY_ID id = GetOBJEntryIDByName(name);
            if (id != (OBJENTRY_ID)-1) {
                char message[128];
                snprintf(message, sizeof(message), "How many %s to spawn?", name);
                ShowSpawnAmountDialog(message, 1, 50, id);
            }
        }
    }

    void PrizeBoxEntryListCallback(YW::EVENT event, int index, unsigned int* arg) {
        if ((event.Flag & 0x20) && index >= 0 && index < prizeBoxMenu.count) {
            const char* name = prizeBoxMenu.entries[index];
            OBJENTRY_ID id = GetOBJEntryIDByName(name);
            if (id != (OBJENTRY_ID)-1) {
                char message[128];
                snprintf(message, sizeof(message), "How many %s to spawn?", name);
                ShowSpawnAmountDialog(message, 1, 50, id);
            }
        }
    }

    void SummonEntryListCallback(YW::EVENT event, int index, unsigned int* arg) {
        if ((event.Flag & 0x20) && index >= 0 && index < summonMenu.count) {
            const char* name = summonMenu.entries[index];
            OBJENTRY_ID id = GetOBJEntryIDByName(name);
            if (id != (OBJENTRY_ID)-1) {
                char message[128];
                snprintf(message, sizeof(message), "How many %s to spawn?", name);
                ShowSpawnAmountDialog(message, 1, 50, id);
            }
        }
    }

    void ShopPointEntryListCallback(YW::EVENT event, int index, unsigned int* arg) {
        if ((event.Flag & 0x20) && index >= 0 && index < shopPointMenu.count) {
            const char* name = shopPointMenu.entries[index];
            OBJENTRY_ID id = GetOBJEntryIDByName(name);
            if (id != (OBJENTRY_ID)-1) {
                char message[128];
                snprintf(message, sizeof(message), "How many %s to spawn?", name);
                ShowSpawnAmountDialog(message, 1, 50, id);
            }
        }
    }

    void LargeZakoEntryListCallback(YW::EVENT event, int index, unsigned int* arg) {
        if ((event.Flag & 0x20) && index >= 0 && index < largeZakoMenu.count) {
            const char* name = largeZakoMenu.entries[index];
            OBJENTRY_ID id = GetOBJEntryIDByName(name);
            if (id != (OBJENTRY_ID)-1) {
                char message[128];
                snprintf(message, sizeof(message), "How many %s to spawn?", name);
                ShowSpawnAmountDialog(message, 1, 50, id);
            }
        }
    }

    void MassEffectEntryListCallback(YW::EVENT event, int index, unsigned int* arg) {
        if ((event.Flag & 0x20) && index >= 0 && index < massEffectMenu.count) {
            const char* name = massEffectMenu.entries[index];
            OBJENTRY_ID id = GetOBJEntryIDByName(name);
            if (id != (OBJENTRY_ID)-1) {
                char message[128];
                snprintf(message, sizeof(message), "How many %s to spawn?", name);
                ShowSpawnAmountDialog(message, 1, 50, id);
            }
        }
    }

    void EnemyObjEntryListCallback(YW::EVENT event, int index, unsigned int* arg) {
        if ((event.Flag & 0x20) && index >= 0 && index < enemyObjMenu.count) {
            const char* name = enemyObjMenu.entries[index];
            OBJENTRY_ID id = GetOBJEntryIDByName(name);
            if (id != (OBJENTRY_ID)-1) {
                char message[128];
                snprintf(message, sizeof(message), "How many %s to spawn?", name);
                ShowSpawnAmountDialog(message, 1, 50, id);
            }
        }
    }

    void JigsawEntryListCallback(YW::EVENT event, int index, unsigned int* arg) {
        if ((event.Flag & 0x20) && index >= 0 && index < jigsawMenu.count) {
            const char* name = jigsawMenu.entries[index];
            OBJENTRY_ID id = GetOBJEntryIDByName(name);
            if (id != (OBJENTRY_ID)-1) {
                char message[128];
                snprintf(message, sizeof(message), "How many %s to spawn?", name);
                ShowSpawnAmountDialog(message, 1, 50, id);
            }
        }
    }

    void HandleEntrySelection(YW::EVENT event, int index, EntryMenuContext& menu) {
        if ((event.Flag & 0x20) && index >= 0 && index < menu.count) {
            if (index == 0) {
                snapToGroundEnabled = !snapToGroundEnabled;
                menu.entries[0] = snapToGroundEnabled ? "Snap To Ground: On" : "Snap To Ground: Off";
            }
            else {
                const char* name = menu.entries[index];
                OBJENTRY_ID id = GetOBJEntryIDByName(name);
                if (id != (OBJENTRY_ID)-1) {
                    g_SelectedEntityID = id;
                    strncpy(g_SelectedEntityName, name, sizeof(g_SelectedEntityName));
                }
            }
        }
    }

    /*void HandleEntrySelection(YW::EVENT event, int index, EntryMenuContext& menu) {
        if ((event.Flag & 0x20) && index >= 0 && index < menu.count) {
            if (index == 0) {
                snapToGroundEnabled = !snapToGroundEnabled;
                menu.entries[0] = snapToGroundEnabled ? "Snap To Ground: On" : "Snap To Ground: Off";
            }
            else if (CanSpawnPreview()) {
                const char* name = menu.entries[index];
                OBJENTRY_ID id = GetOBJEntryIDByName(name);

                if (id != (OBJENTRY_ID)-1 && id != previewEntityId) {
                    ResetPreviewSpawnCooldown();

                    if (previewObject) {
                        DestroyobjFunction(previewObject);
                        RealDestroyobjFunction(previewObject);
                        previewObject = nullptr;
                        previewEntity = nullptr;
                        isPreviewSpawned = false;
                    }

                    previewEntityId = id;
                    g_SelectedEntityID = id;
                    strncpy(g_SelectedEntityName, name, sizeof(g_SelectedEntityName));

                    NewLevelEditorPreviewActor(previewEntityId, 1);
                }
            }
        }*/
    





    void LevelEditorNpcEntryListCallback(YW::EVENT event, int index, unsigned int* arg) {
        HandleEntrySelection(event, index, npcMenu);
    }

    void LevelEditorBossEntryListCallback(YW::EVENT event, int index, unsigned int* arg) {
        HandleEntrySelection(event, index, bossMenu);
    }

    void LevelEditorZakoEntryListCallback(YW::EVENT event, int index, unsigned int* arg) {
        HandleEntrySelection(event, index, zakoMenu);
    }

    void LevelEditorWeaponEntryListCallback(YW::EVENT event, int index, unsigned int* arg) {
        HandleEntrySelection(event, index, weaponMenu);
    }

    void LevelEditorEnemyWeaponEntryListCallback(YW::EVENT event, int index, unsigned int* arg) {
        HandleEntrySelection(event, index, enemyWeaponMenu);
    }

    void LevelEditorSavepointEntryListCallback(YW::EVENT event, int index, unsigned int* arg) {
        HandleEntrySelection(event, index, savepointMenu);
    }

    void LevelEditorFieldObjEntryListCallback(YW::EVENT event, int index, unsigned int* arg) {
        HandleEntrySelection(event, index, fieldObjMenu);
    }

    void LevelEditorBattleNpcEntryListCallback(YW::EVENT event, int index, unsigned int* arg) {
        HandleEntrySelection(event, index, battleNpcMenu);
    }

    void LevelEditorTreasureBoxEntryListCallback(YW::EVENT event, int index, unsigned int* arg) {
        HandleEntrySelection(event, index, treasureBoxMenu);
    }

    void LevelEditorSubmenuEntryListCallback(YW::EVENT event, int index, unsigned int* arg) {
        HandleEntrySelection(event, index, submenuMenu);
    }

    void LevelEditorLargeBossEntryListCallback(YW::EVENT event, int index, unsigned int* arg) {
        HandleEntrySelection(event, index, largeBossMenu);
    }

    void LevelEditorGimmickObjEntryListCallback(YW::EVENT event, int index, unsigned int* arg) {
        HandleEntrySelection(event, index, gimmickObjMenu);
    }

    void LevelEditorMemoEntryListCallback(YW::EVENT event, int index, unsigned int* arg) {
        HandleEntrySelection(event, index, memoMenu);
    }

    void LevelEditorRtnEntryListCallback(YW::EVENT event, int index, unsigned int* arg) {
        HandleEntrySelection(event, index, rtnMenu);
    }

    void LevelEditorMinigameEntryListCallback(YW::EVENT event, int index, unsigned int* arg) {
        HandleEntrySelection(event, index, minigameMenu);
    }

    void LevelEditorWorldMapEntryListCallback(YW::EVENT event, int index, unsigned int* arg) {
        HandleEntrySelection(event, index, worldMapMenu);
    }

    void LevelEditorPrizeBoxEntryListCallback(YW::EVENT event, int index, unsigned int* arg) {
        HandleEntrySelection(event, index, prizeBoxMenu);
    }

    void LevelEditorSummonEntryListCallback(YW::EVENT event, int index, unsigned int* arg) {
        HandleEntrySelection(event, index, summonMenu);
    }

    void LevelEditorShopPointEntryListCallback(YW::EVENT event, int index, unsigned int* arg) {
        HandleEntrySelection(event, index, shopPointMenu);
    }

    void LevelEditorLargeZakoEntryListCallback(YW::EVENT event, int index, unsigned int* arg) {
        HandleEntrySelection(event, index, largeZakoMenu);
    }

    void LevelEditorMassEffectEntryListCallback(YW::EVENT event, int index, unsigned int* arg) {
        HandleEntrySelection(event, index, massEffectMenu);
    }

    void LevelEditorEnemyObjEntryListCallback(YW::EVENT event, int index, unsigned int* arg) {
        HandleEntrySelection(event, index, enemyObjMenu);
    }

    void LevelEditorJigsawEntryListCallback(YW::EVENT event, int index, unsigned int* arg) {
        HandleEntrySelection(event, index, jigsawMenu);
    }


    void ZakoEntryMenuOpen(int n) {
        uintptr_t OpenSingletonWindowFunctionAddress = ReadLEA32(Patterns::pattern_for_OpenSingletonWindow, L"OpenSingletonWindow", 0, 1, 5);
        uintptr_t AllocYWMemoryFunctionAddress = ReadLEA32(Patterns::pattern_for_Alloc_YW_Memory, L"AllocYWMemory", 0, 1, 5);
        uintptr_t ContainerLinkFunctionAddress = FindAndPrintPatternW(Patterns::pattern_for_ContainerLink, L"ContainerLink", 0);
        uintptr_t YWFrameFunctionAddress = ReadLEA32(Patterns::pattern_for_YWFrame, L"YWFrame", 0, 1, 5);
        uintptr_t WListFunctionAddress = ReadLEA32(Patterns::pattern_for_YWList, L"YWList", 0, 1, 5);
        uintptr_t ShowDebugWindowFunctionAddress = ReadLEA32(Patterns::pattern_for_ShowDebugWindow, L"ShowDebugWindow", 0, 1, 5);
        uintptr_t YWwindowMinSizeFunctionAddress = FindAndPrintPatternW(Patterns::pattern_for_YWwindow_minSize, L"YWwindow Min Size", 0);
        uintptr_t YWwindowMaxSizeFunctionAddress = FindAndPrintPatternW(Patterns::pattern_for_YWwindow_maxSize, L"YWwindow Max Size", 0);
        uintptr_t GetOBJENTRYFunctionAddress = ReadLEA32(Patterns::pattern_for_GetOBJENTRY, L"Get OBJENTRY", 0, 1, 5);

        auto OpenSingletonWindowFunction = reinterpret_cast<FunctionCalls::tOpenSingletonWindow>(OpenSingletonWindowFunctionAddress);
        auto AllocYWMemoryFunction = reinterpret_cast<FunctionCalls::tAlloc_YW_Memory>(AllocYWMemoryFunctionAddress);
        auto ContainerLinkFunction = reinterpret_cast<FunctionCalls::tContainerLink>(ContainerLinkFunctionAddress);
        auto YWFrameFunction = reinterpret_cast<FunctionCalls::tYWFrame>(YWFrameFunctionAddress);
        auto YWListFunction = reinterpret_cast<FunctionCalls::tYWList>(WListFunctionAddress);
        auto ShowDebugWindowFunction = reinterpret_cast<FunctionCalls::tShowDebugWindow>(ShowDebugWindowFunctionAddress);
        auto YWwindowMinSizeFunction = reinterpret_cast<FunctionCalls::tYW_WINDOW_set_min_size>(YWwindowMinSizeFunctionAddress);
        auto YWwindowMaxSizeFunction = reinterpret_cast<FunctionCalls::tYW_WINDOW_set_max_size>(YWwindowMaxSizeFunctionAddress);
        auto GetOBJENTRYFunction = reinterpret_cast<FunctionCalls::tGetOBJENTRY>(GetOBJENTRYFunctionAddress);

        // Reset the zakoCount and clear the ZakoList
        zakoCount = 0;
        memset(ZakoList, 0, sizeof(ZakoList)); // Clear out old entries

        // Re-populate the ZakoList with new entries
        for (OBJENTRY_ID id = 0; id <= 2561; ++id) {
            OBJENTRY_0* entry = GetOBJENTRYFunction(id);
            if (entry && entry->Type == TYPE_ZAKO) {
                if (zakoCount < maxZakoCount) {
                    ZakoList[zakoCount++] = entry->EntryName;
                }
            }
        }

        // If no zako entries were found, add a default message
        if (zakoCount == 0) {
            ZakoList[0] = "No zako entries found.";
            zakoCount = 1;
        }

        // Null-terminate the list
        ZakoList[zakoCount] = nullptr;

        // Open the menu window
        YW::YW_WINDOW_39* Menu = OpenSingletonWindowFunction("Zako Entry Menu", 200, 200, 2);

        unsigned int* listMem = AllocYWMemoryFunction((YW::MEMORY*)0x200);
        unsigned int* frameMem = AllocYWMemoryFunction((YW::MEMORY*)0x200);

        YW::WIDGET* ListWidget = nullptr;
        YWListFunction((YW::YW_LIST_0*)listMem, 0, 0, ZakoList, ZakoEntryListCallback, listMem);
        ListWidget = (YW::WIDGET*)listMem;

        YW::FRAME* FrameWidget = (YW::FRAME*)frameMem;
        YWFrameFunction(FrameWidget, 0, 0, 0, 0, 0, ListWidget->Rect.H);

        // Link widgets to the window
        ContainerLinkFunction(FrameWidget, ListWidget);
        ContainerLinkFunction(Menu, FrameWidget);

        // Set the window size limits based on the list dimensions
        YWwindowMaxSizeFunction(Menu, ListWidget->Rect.W, ListWidget->Rect.H);
        YWwindowMinSizeFunction(Menu, ListWidget->Rect.W, 0);

        // Show the window
        ShowDebugWindowFunction(Menu);
    }

    void OpenEntryMenuForType(
        YS_OBJENTRY_TYPE type,
        const char* windowTitle,
        void (*menuCallback)(YW::EVENT, int, unsigned int*),
        EntryMenuContext& context
    ) {
       
        auto OpenSingletonWindowFunction = reinterpret_cast<FunctionCalls::tOpenSingletonWindow>(
            ReadLEA32(Patterns::pattern_for_OpenSingletonWindow, L"OpenSingletonWindow", 0, 1, 5));
        auto AllocYWMemoryFunction = reinterpret_cast<FunctionCalls::tAlloc_YW_Memory>(
            ReadLEA32(Patterns::pattern_for_Alloc_YW_Memory, L"AllocYWMemory", 0, 1, 5));
        auto ContainerLinkFunction = reinterpret_cast<FunctionCalls::tContainerLink>(
            FindAndPrintPatternW(Patterns::pattern_for_ContainerLink, L"ContainerLink", 0));
        auto YWFrameFunction = reinterpret_cast<FunctionCalls::tYWFrame>(
            ReadLEA32(Patterns::pattern_for_YWFrame, L"YWFrame", 0, 1, 5));
        auto YWListFunction = reinterpret_cast<FunctionCalls::tYWList>(
            ReadLEA32(Patterns::pattern_for_YWList, L"YWList", 0, 1, 5));
        auto ShowDebugWindowFunction = reinterpret_cast<FunctionCalls::tShowDebugWindow>(
            ReadLEA32(Patterns::pattern_for_ShowDebugWindow, L"ShowDebugWindow", 0, 1, 5));
        auto YWwindowMinSizeFunction = reinterpret_cast<FunctionCalls::tYW_WINDOW_set_min_size>(
            FindAndPrintPatternW(Patterns::pattern_for_YWwindow_minSize, L"YWwindow Min Size", 0));
        auto YWwindowMaxSizeFunction = reinterpret_cast<FunctionCalls::tYW_WINDOW_set_max_size>(
            FindAndPrintPatternW(Patterns::pattern_for_YWwindow_maxSize, L"YWwindow Max Size", 0));
        auto GetOBJENTRYFunction = reinterpret_cast<FunctionCalls::tGetOBJENTRY>(
            ReadLEA32(Patterns::pattern_for_GetOBJENTRY, L"Get OBJENTRY", 0, 1, 5));

        // Populate context entries
        context.count = 0;
        memset(context.entries, 0, sizeof(context.entries));


        context.entries[context.count++] = snapToGroundEnabled ? "Snap To Ground: On" : "Snap To Ground: Off";

        for (OBJENTRY_ID id = 0; id <= 2561; ++id) {
            OBJENTRY_0* entry = GetOBJENTRYFunction(id);
            if (entry && entry->Type == type) {
                if (context.count < maxEntryCount) {
                    context.entries[context.count++] = entry->EntryName;
                }
            }
        }

        if (context.count == 0) {
            context.entries[0] = "No entries found.";
            context.count = 1;
        }

        

        context.entries[context.count] = nullptr;

       
        

        // Build the menu window
        //YW::YW_WINDOW_39* Menu = OpenSingletonWindowFunction(windowTitle, 180, 300, 0);
        YW::YW_WINDOW_39* Menu = OpenSingletonWindowFunction(windowTitle, 180, 300, 2);
        unsigned int* listMem = AllocYWMemoryFunction((YW::MEMORY*)0x200);
        unsigned int* frameMem = AllocYWMemoryFunction((YW::MEMORY*)0x200);

        YWListFunction((YW::YW_LIST_0*)listMem, 0, 0, context.entries, menuCallback, listMem);
        YW::WIDGET* ListWidget = (YW::WIDGET*)listMem;

        

        YW::FRAME* FrameWidget = (YW::FRAME*)frameMem;
        //ListWidget->Rect.H = listHeight;
        YWFrameFunction(FrameWidget, 0, 11, 0, -11, 0, ListWidget->Rect.H);
        

        ContainerLinkFunction(FrameWidget, ListWidget);
        ContainerLinkFunction(Menu, FrameWidget);

        int finalWidth = 180;
        if (ListWidget->Rect.W + 10 > 180)
            finalWidth = ListWidget->Rect.W + 10;

        int adjustedListWidth = finalWidth - 10;
        ListWidget->Rect.W = adjustedListWidth;
        Menu->Rect.W = finalWidth;

        int cappedHeight = 300;
        if (ListWidget->Rect.H < 300)
            cappedHeight = ListWidget->Rect.H;

        
        YWwindowMinSizeFunction(Menu, Menu->Rect.W, cappedHeight + 11);
        YWwindowMaxSizeFunction(Menu, Menu->Rect.W, ListWidget->Rect.H + 11);

        ShowDebugWindowFunction(Menu);
    }

    void ObjActListCallback(YW::EVENT event, int index, unsigned int* arg) {
        auto StartActFunction = reinterpret_cast<FunctionCalls::tObjActStart>(
            ReadLEA32(Patterns::pattern_for_ObjActStart, L"StartObjAct", 0, 1, 5));

        

            //YS::OBJ* obj = GetMenuOBJFunction();
            YS::ACT* act = reinterpret_cast<YS::ACT*>((uintptr_t)gGlobalObjPtr1 + 0x390);

        if ((event.Flag & 0x20) != 0) {
            const char* selectedLabel = AllActStrings[index];
            if (selectedLabel && StartActFunction) {
                StartActFunction(act, selectedLabel);
            }
        }
    }

    void OpenObjActMenu(
        const char* windowTitle,
        void (*menuCallback)(YW::EVENT, int, unsigned int*)
    ) {

        auto OpenSingletonWindowFunction = reinterpret_cast<FunctionCalls::tOpenSingletonWindow>(
            ReadLEA32(Patterns::pattern_for_OpenSingletonWindow, L"OpenSingletonWindow", 0, 1, 5));
        auto AllocYWMemoryFunction = reinterpret_cast<FunctionCalls::tAlloc_YW_Memory>(
            ReadLEA32(Patterns::pattern_for_Alloc_YW_Memory, L"AllocYWMemory", 0, 1, 5));
        auto ContainerLinkFunction = reinterpret_cast<FunctionCalls::tContainerLink>(
            FindAndPrintPatternW(Patterns::pattern_for_ContainerLink, L"ContainerLink", 0));
        auto YWFrameFunction = reinterpret_cast<FunctionCalls::tYWFrame>(
            ReadLEA32(Patterns::pattern_for_YWFrame, L"YWFrame", 0, 1, 5));
        auto YWListFunction = reinterpret_cast<FunctionCalls::tYWList>(
            ReadLEA32(Patterns::pattern_for_YWList, L"YWList", 0, 1, 5));
        auto ShowDebugWindowFunction = reinterpret_cast<FunctionCalls::tShowDebugWindow>(
            ReadLEA32(Patterns::pattern_for_ShowDebugWindow, L"ShowDebugWindow", 0, 1, 5));
        auto YWwindowMinSizeFunction = reinterpret_cast<FunctionCalls::tYW_WINDOW_set_min_size>(
            FindAndPrintPatternW(Patterns::pattern_for_YWwindow_minSize, L"YWwindow Min Size", 0));
        auto YWwindowMaxSizeFunction = reinterpret_cast<FunctionCalls::tYW_WINDOW_set_max_size>(
            FindAndPrintPatternW(Patterns::pattern_for_YWwindow_maxSize, L"YWwindow Max Size", 0));
        auto GetOBJENTRYFunction = reinterpret_cast<FunctionCalls::tGetOBJENTRY>(
            ReadLEA32(Patterns::pattern_for_GetOBJENTRY, L"Get OBJENTRY", 0, 1, 5));

        

        // Clear list to avoid duplicates or stale data
        AllActStrings.clear();

        uintptr_t ActObjectCurrentTableptr = (uintptr_t)gGlobalObjPtr1 + 0x4A0;
        uintptr_t ActObjectCurrentTableValue64 = *reinterpret_cast<uintptr_t*>(ActObjectCurrentTableptr);
        

        //gGlobalObjPtr = obj;
        

        if (ActObjectCurrentTableValue64 != 0) {
            const size_t MAX_ELEMS = 128;
            const size_t ELEM_SIZE = 0x20;       // Adjust if different
            const size_t LABEL_OFFSET = 0x0;     // Label is first in Elem

            for (size_t i = 0; i < MAX_ELEMS; ++i) {
                uintptr_t elemAddr = ActObjectCurrentTableValue64 + i * ELEM_SIZE;

                uint32_t encodedLabel = *reinterpret_cast<uint32_t*>(elemAddr + LABEL_OFFSET);
                if (encodedLabel == 0)
                    continue;

                uintptr_t decodedAddr = CustomDecodePointer(encodedLabel, moduleBase1);
                if (decodedAddr == 0)
                    continue;

                const char* labelStr = reinterpret_cast<const char*>(decodedAddr);

                // Safety check: avoid garbage strings
                if (labelStr && *labelStr && strlen(labelStr) < 100 && isprint(labelStr[0])) {
                    AllActStrings.push_back(labelStr);
                }
            }
        }
        else {
            AllActStrings.push_back("No Act Table Found.");
        }

        AllActStrings.push_back(nullptr);

        unsigned int* listMem = AllocYWMemoryFunction((YW::MEMORY*)0x200);
        unsigned int* frameMem = AllocYWMemoryFunction((YW::MEMORY*)0x200);

        YWListFunction((YW::YW_LIST_0*)listMem, 0, 0, AllActStrings.data(), menuCallback, listMem);
        YW::WIDGET* ListWidget = (YW::WIDGET*)listMem;


        // Build the menu window
        //YW::YW_WINDOW_39* Menu = OpenSingletonWindowFunction(windowTitle, 180, 300, 0);
        YW::YW_WINDOW_39* Menu = OpenSingletonWindowFunction(windowTitle, 500, 200, 2);
        



        YW::FRAME* FrameWidget = (YW::FRAME*)frameMem;
        //ListWidget->Rect.H = listHeight;
        YWFrameFunction(FrameWidget, 0, 11, 0, -11, 0, ListWidget->Rect.H);


        ContainerLinkFunction(FrameWidget, ListWidget);
        ContainerLinkFunction(Menu, FrameWidget);

        int finalWidth = 180;
        if (ListWidget->Rect.W + 10 > 180)
            finalWidth = ListWidget->Rect.W + 10;

        int adjustedListWidth = finalWidth - 10;
        ListWidget->Rect.W = adjustedListWidth;
        Menu->Rect.W = finalWidth;

        int cappedHeight = 300;
        if (ListWidget->Rect.H < 300)
            cappedHeight = ListWidget->Rect.H;


        YWwindowMinSizeFunction(Menu, Menu->Rect.W, cappedHeight + 11);
        YWwindowMaxSizeFunction(Menu, Menu->Rect.W, ListWidget->Rect.H + 11);

        ShowDebugWindowFunction(Menu);
    }

    void menu_pa_editer_callback(int n) {
        InitPAEditorUI(200, 50, 0);
    }

    



    void ConfigSubmenuItemCallback(int arg) {
        ShowRealConfigWindow(0);
    }

    void SaveCNFCallback(int arg) {
        SaveCNF(0);
    }

    void Config2SubmenuItemCallback(int arg) {
        ShowRealSecondConfigWindow(0);
    }

    void OBJENTRYwindowCallBack(int selectedItemIndex) {
        ShowOBJEntryWindow(0);
    }

    void PlayerMenuCallBack(int selectedItemIndex) {
        AddPlayerEntries();
        PlayerEntryMenuOpen(0);
    }

    //void ZakoMenuCallBack(int selectedItemIndex) {
   //     ZakoEntryMenuOpen(0);
   // }
        
    void OpenActmenu() {
       // YS::OBJ* obj = GetMenuOBJFunction();
        //const char* entryname = GetOBJEntryNameFunction(obj);
        OpenObjActMenu("obj act", ObjActListCallback);

        


    }



    void OpenNpcMenu() {
        OpenEntryMenuForType(TYPE_NPC, "NPC Entry Menu", NpcEntryListCallback, npcMenu);
    }

    void OpenBossMenu() {
        OpenEntryMenuForType(TYPE_BOSS_0, "Boss Entry Menu", BossEntryListCallback, bossMenu);
    }

    void OpenZakoMenu() {
        OpenEntryMenuForType(TYPE_ZAKO, "Zako Entry Menu", ZakoEntryListCallback, zakoMenu);
    }

    void OpenWeaponMenu() {
        OpenEntryMenuForType(TYPE_WEAPON_0, "Weapon Entry Menu", WeaponEntryListCallback, weaponMenu);
    }

    void OpenEnemyWeaponMenu() {
        OpenEntryMenuForType(TYPE_E_WEAPON, "Enemy Weapon Entry Menu", EnemyWeaponEntryListCallback, enemyWeaponMenu);
    }

    void OpenSavepointMenu() {
        OpenEntryMenuForType(TYPE_SAVEPOINT_0, "Savepoint Entry Menu", SavepointEntryListCallback, savepointMenu);
    }

    void OpenFieldObjMenu() {
        OpenEntryMenuForType(TYPE_FIELDOBJ, "Field Object Entry Menu", FieldObjEntryListCallback, fieldObjMenu);
    }

    void OpenBattleNpcMenu() {
        OpenEntryMenuForType(TYPE_BTLNPC_0, "Battle NPC Entry Menu", BattleNpcEntryListCallback, battleNpcMenu);
    }

    void OpenTreasureBoxMenu() {
        OpenEntryMenuForType(TYPE_TREASURE_BOX, "Treasure Box Entry Menu", TreasureBoxEntryListCallback, treasureBoxMenu);
    }

    void OpenSubmenuMenu() {
        OpenEntryMenuForType(TYPE_SUBMENU_0, "Submenu Entry Menu", SubmenuEntryListCallback, submenuMenu);
    }

    void OpenLargeBossMenu() {
        OpenEntryMenuForType(TYPE_LARGEBOSS, "Large Boss Entry Menu", LargeBossEntryListCallback, largeBossMenu);
    }

    void OpenGimmickObjMenu() {
        OpenEntryMenuForType(TYPE_GIMMICKOBJ, "Gimmick Object Entry Menu", GimmickObjEntryListCallback, gimmickObjMenu);
    }

    void OpenMemoMenu() {
        OpenEntryMenuForType(TYPE_MEMO, "Memo Entry Menu", MemoEntryListCallback, memoMenu);
    }

    void OpenRtnMenu() {
        OpenEntryMenuForType(TYPE_RTN, "Return Entry Menu", RtnEntryListCallback, rtnMenu);
    }

    void OpenMinigameMenu() {
        OpenEntryMenuForType(TYPE_MINIGAME_3, "Minigame Entry Menu", MinigameEntryListCallback, minigameMenu);
    }

    void OpenWorldMapMenu() {
        OpenEntryMenuForType(TYPE_WORLDMAP_1, "World Map Entry Menu", WorldMapEntryListCallback, worldMapMenu);
    }

    void OpenPrizeBoxMenu() {
        OpenEntryMenuForType(TYPE_PRIZEBOX, "Prize Box Entry Menu", PrizeBoxEntryListCallback, prizeBoxMenu);
    }

    void OpenSummonMenu() {
        OpenEntryMenuForType(TYPE_SUMMON_2, "Summon Entry Menu", SummonEntryListCallback, summonMenu);
    }

    void OpenShopPointMenu() {
        OpenEntryMenuForType(TYPE_SHOPPOINT, "Shop Point Entry Menu", ShopPointEntryListCallback, shopPointMenu);
    }

    void OpenLargeZakoMenu() {
        OpenEntryMenuForType(TYPE_L_ZAKO, "Large Zako Entry Menu", LargeZakoEntryListCallback, largeZakoMenu);
    }

    void OpenMassEffectMenu() {
        OpenEntryMenuForType(TYPE_MASSEFFECT, "Mass Effect Entry Menu", MassEffectEntryListCallback, massEffectMenu);
    }

    void OpenEnemyObjMenu() {
        OpenEntryMenuForType(TYPE_ENEMYOBJ, "Enemy Object Entry Menu", EnemyObjEntryListCallback, enemyObjMenu);
    }

    void OpenJigsawMenu() {
        OpenEntryMenuForType(TYPE_JIGSAW, "Jigsaw Entry Menu", JigsawEntryListCallback, jigsawMenu);
    }

    void NpcMenuCallBack(int) { OpenNpcMenu(); }
    void ZakoMenuCallBack(int) { OpenZakoMenu(); }
    void BossMenuCallBack(int) { OpenBossMenu(); }
    void WeaponMenuCallBack(int) { OpenWeaponMenu(); }
    void EnemyWeaponMenuCallBack(int) { OpenEnemyWeaponMenu(); }
    void SavepointMenuCallBack(int) { OpenSavepointMenu(); }
    void FieldObjMenuCallBack(int) { OpenFieldObjMenu(); }
    void BattleNpcMenuCallBack(int) { OpenBattleNpcMenu(); }
    void TreasureBoxMenuCallBack(int) { OpenTreasureBoxMenu(); }
    void SubmenuMenuCallBack(int) { OpenSubmenuMenu(); }
    void LargeBossMenuCallBack(int) { OpenLargeBossMenu(); }
    void GimmickObjMenuCallBack(int) { OpenGimmickObjMenu(); }
    void MemoMenuCallBack(int) { OpenMemoMenu(); }
    void RtnMenuCallBack(int) { OpenRtnMenu(); }
    void MinigameMenuCallBack(int) { OpenMinigameMenu(); }
    void WorldMapMenuCallBack(int) { OpenWorldMapMenu(); }
    void PrizeBoxMenuCallBack(int) { OpenPrizeBoxMenu(); }
    void SummonMenuCallBack(int) { OpenSummonMenu(); }
    void ShopPointMenuCallBack(int) { OpenShopPointMenu(); }
    void LargeZakoMenuCallBack(int) { OpenLargeZakoMenu(); }
    void MassEffectMenuCallBack(int) { OpenMassEffectMenu(); }
    void EnemyObjMenuCallBack(int) { OpenEnemyObjMenu(); }
    void JigsawMenuCallBack(int) { OpenJigsawMenu(); }


    void LevelEditorOpenNpcMenu() {
        OpenEntryMenuForType(TYPE_NPC, "Level Editor - NPC Entry Menu", LevelEditorNpcEntryListCallback, npcMenu);
    }

    void LevelEditorOpenBossMenu() {
        OpenEntryMenuForType(TYPE_BOSS_0, "Level Editor - Boss Entry Menu", LevelEditorBossEntryListCallback, bossMenu);
    }

    void LevelEditorOpenZakoMenu() {
        OpenEntryMenuForType(TYPE_ZAKO, "Level Editor - Zako Entry Menu", LevelEditorZakoEntryListCallback, zakoMenu);
    }

    void LevelEditorOpenWeaponMenu() {
        OpenEntryMenuForType(TYPE_WEAPON_0, "Level Editor - Weapon Entry Menu", LevelEditorWeaponEntryListCallback, weaponMenu);
    }

    void LevelEditorOpenEnemyWeaponMenu() {
        OpenEntryMenuForType(TYPE_E_WEAPON, "Level Editor - Enemy Weapon Entry Menu", LevelEditorEnemyWeaponEntryListCallback, enemyWeaponMenu);
    }

    void LevelEditorOpenSavepointMenu() {
        OpenEntryMenuForType(TYPE_SAVEPOINT_0, "Level Editor - Savepoint Entry Menu", LevelEditorSavepointEntryListCallback, savepointMenu);
    }

    void LevelEditorOpenFieldObjMenu() {
        OpenEntryMenuForType(TYPE_FIELDOBJ, "Level Editor - Field Object Entry Menu", LevelEditorFieldObjEntryListCallback, fieldObjMenu);
    }

    void LevelEditorOpenBattleNpcMenu() {
        OpenEntryMenuForType(TYPE_BTLNPC_0, "Level Editor - Battle NPC Entry Menu", LevelEditorBattleNpcEntryListCallback, battleNpcMenu);
    }

    void LevelEditorOpenTreasureBoxMenu() {
        OpenEntryMenuForType(TYPE_TREASURE_BOX, "Level Editor - Treasure Box Entry Menu", LevelEditorTreasureBoxEntryListCallback, treasureBoxMenu);
    }

    void LevelEditorOpenSubmenuMenu() {
        OpenEntryMenuForType(TYPE_SUBMENU_0, "Level Editor - Submenu Entry Menu", LevelEditorSubmenuEntryListCallback, submenuMenu);
    }

    void LevelEditorOpenLargeBossMenu() {
        OpenEntryMenuForType(TYPE_LARGEBOSS, "Level Editor - Large Boss Entry Menu", LevelEditorLargeBossEntryListCallback, largeBossMenu);
    }

    void LevelEditorOpenGimmickObjMenu() {
        OpenEntryMenuForType(TYPE_GIMMICKOBJ, "Level Editor - Gimmick Object Entry Menu", LevelEditorGimmickObjEntryListCallback, gimmickObjMenu);
    }

    void LevelEditorOpenMemoMenu() {
        OpenEntryMenuForType(TYPE_MEMO, "Level Editor - Memo Entry Menu", LevelEditorMemoEntryListCallback, memoMenu);
    }

    void LevelEditorOpenRtnMenu() {
        OpenEntryMenuForType(TYPE_RTN, "Level Editor - Return Entry Menu", LevelEditorRtnEntryListCallback, rtnMenu);
    }

    void LevelEditorOpenMinigameMenu() {
        OpenEntryMenuForType(TYPE_MINIGAME_3, "Level Editor - Minigame Entry Menu", LevelEditorMinigameEntryListCallback, minigameMenu);
    }

    void LevelEditorOpenWorldMapMenu() {
        OpenEntryMenuForType(TYPE_WORLDMAP_1, "Level Editor - World Map Entry Menu", LevelEditorWorldMapEntryListCallback, worldMapMenu);
    }

    void LevelEditorOpenPrizeBoxMenu() {
        OpenEntryMenuForType(TYPE_PRIZEBOX, "Level Editor - Prize Box Entry Menu", LevelEditorPrizeBoxEntryListCallback, prizeBoxMenu);
    }

    void LevelEditorOpenSummonMenu() {
        OpenEntryMenuForType(TYPE_SUMMON_2, "Level Editor - Summon Entry Menu", LevelEditorSummonEntryListCallback, summonMenu);
    }

    void LevelEditorOpenShopPointMenu() {
        OpenEntryMenuForType(TYPE_SHOPPOINT, "Level Editor - Shop Point Entry Menu", LevelEditorShopPointEntryListCallback, shopPointMenu);
    }

    void LevelEditorOpenLargeZakoMenu() {
        OpenEntryMenuForType(TYPE_L_ZAKO, "Level Editor - Large Zako Entry Menu", LevelEditorLargeZakoEntryListCallback, largeZakoMenu);
    }

    void LevelEditorOpenMassEffectMenu() {
        OpenEntryMenuForType(TYPE_MASSEFFECT, "Level Editor - Mass Effect Entry Menu", LevelEditorMassEffectEntryListCallback, massEffectMenu);
    }

    void LevelEditorOpenEnemyObjMenu() {
        OpenEntryMenuForType(TYPE_ENEMYOBJ, "Level Editor - Enemy Object Entry Menu", LevelEditorEnemyObjEntryListCallback, enemyObjMenu);
    }

    void LevelEditorOpenJigsawMenu() {
        OpenEntryMenuForType(TYPE_JIGSAW, "Level Editor - Jigsaw Entry Menu", LevelEditorJigsawEntryListCallback, jigsawMenu);
    }


    void CinematicCameraOnCallback(int selectedItemIndex) {
        CinematicCameraToggle = true;
        CinematicCameraSettingsMenuCallback(0);


    }





    void LevelEditorNpcMenuCallBack(int) { LevelEditorOpenNpcMenu(); }
    void LevelEditorZakoMenuCallBack(int) { LevelEditorOpenZakoMenu(); }
    void LevelEditorBossMenuCallBack(int) { LevelEditorOpenBossMenu(); }
    void LevelEditorWeaponMenuCallBack(int) { LevelEditorOpenWeaponMenu(); }
    void LevelEditorEnemyWeaponMenuCallBack(int) { LevelEditorOpenEnemyWeaponMenu(); }
    void LevelEditorSavepointMenuCallBack(int) { LevelEditorOpenSavepointMenu(); }
    void LevelEditorFieldObjMenuCallBack(int) { LevelEditorOpenFieldObjMenu(); }
    void LevelEditorBattleNpcMenuCallBack(int) { LevelEditorOpenBattleNpcMenu(); }
    void LevelEditorTreasureBoxMenuCallBack(int) { LevelEditorOpenTreasureBoxMenu(); }
    void LevelEditorSubmenuMenuCallBack(int) { LevelEditorOpenSubmenuMenu(); }
    void LevelEditorLargeBossMenuCallBack(int) { LevelEditorOpenLargeBossMenu(); }
    void LevelEditorGimmickObjMenuCallBack(int) { LevelEditorOpenGimmickObjMenu(); }
    void LevelEditorMemoMenuCallBack(int) { LevelEditorOpenMemoMenu(); }
    void LevelEditorRtnMenuCallBack(int) { LevelEditorOpenRtnMenu(); }
    void LevelEditorMinigameMenuCallBack(int) { LevelEditorOpenMinigameMenu(); }
    void LevelEditorWorldMapMenuCallBack(int) { LevelEditorOpenWorldMapMenu(); }
    void LevelEditorPrizeBoxMenuCallBack(int) { LevelEditorOpenPrizeBoxMenu(); }
    void LevelEditorSummonMenuCallBack(int) { LevelEditorOpenSummonMenu(); }
    void LevelEditorShopPointMenuCallBack(int) { LevelEditorOpenShopPointMenu(); }
    void LevelEditorLargeZakoMenuCallBack(int) { LevelEditorOpenLargeZakoMenu(); }
    void LevelEditorMassEffectMenuCallBack(int) { LevelEditorOpenMassEffectMenu(); }
    void LevelEditorEnemyObjMenuCallBack(int) { LevelEditorOpenEnemyObjMenu(); }
    void LevelEditorJigsawMenuCallBack(int) { LevelEditorOpenJigsawMenu(); }



    void OpenActmenuCallback(int) { OpenActmenu(); }

    void CustomTextureSelectorCallback(int selectedItemIndex) {
        PngTextureLoadMenu(0);
    }


    YS::ELEM DebugMenuMultiplayerSubmenuItems[] = {
    {"Host", nullptr, HostWindowCallback},
    {"Client", nullptr, ClientWindowCallback},
    {"Get Server IP", nullptr, GetServerIPWindowCallback},
    {nullptr, nullptr, nullptr} // Null-terminated array
    };

    /*YS::ELEM DebugMenuOBJENTRYSubmenuItems[] = {
    {"PLAYER", nullptr, CustomOBJENTRYSelectMenu},
    {"FRIEND", nullptr, CustomOBJENTRYSelectMenu1},
    {"CAPTURE", nullptr, CustomOBJENTRYSelectMenu2},
    {"1 OBJ", nullptr, CustomOBJENTRYSelectMenu3},
    {"3 OBJ", nullptr, CustomOBJENTRYSelectMenu4},
    {"5 OBJ", nullptr, CustomOBJENTRYSelectMenu5},
    {"10 OBJ", nullptr, CustomOBJENTRYSelectMenu6},
    {"20 OBJ", nullptr, CustomOBJENTRYSelectMenu7},
    {"30 OBJ", nullptr, CustomOBJENTRYSelectMenu8},
    {"50 OBJ", nullptr, CustomOBJENTRYSelectMenu9},
    {"ACTOR", nullptr, CustomOBJENTRYSelectMenu10},
    {"RELOAD", nullptr, CustomOBJENTRYSelectMenu11},
    {"LEAVE ALL", nullptr, CustomOBJENTRYSelectMenu12},
    {nullptr, nullptr, nullptr} // Null-terminated array
    };*/

    // Submenu for Free Camera On/Off options
    YS::ELEM DebugMenuFreeCameraSubmenuItems[] = {
        {"ON", nullptr, FreeCameraOnCallback},
        {"OFF", nullptr, FreeCameraOffCallback},
        {nullptr, nullptr, nullptr} // Null-terminated array
    };

    YS::ELEM DebugMenuCinematicCameraSubmenuItems[] = {
        {"ON", nullptr, CinematicCameraOnCallback},
        {"OFF", nullptr, CinematicCameraOffCallback},
        {nullptr, nullptr, nullptr} // Null-terminated array
    };

    // Submenu for Camera options
    YS::ELEM DebugMenuCameraSubmenuItems[] = {
    {"Free Camera", DebugMenuFreeCameraSubmenuItems, nullptr},
    {"Cinematic Camera", DebugMenuCinematicCameraSubmenuItems, nullptr},
    {nullptr, nullptr, nullptr} // Null-terminated array
    };

    YS::ELEM OBJPartsSubMenuItems[] = {
        {"display", nullptr, OBJDisplayMenuCallBack},
        {"stealth", nullptr, OBJStealthMenuCallBack},
        {nullptr, nullptr, nullptr} // Null-terminated array
    };

    YS::ELEM DebugMenuEntityMovementSubmenuItems[] = {
        {"ON", nullptr, EntityMovementOnCallback},
        {"OFF", nullptr, EntityMovementOffCallback},
        {nullptr, nullptr, nullptr} // Null-terminated array
    };

    YS::ELEM DebugMenuLevelEditorSubmenuItems[] = {
        {"ON", nullptr, LevelEditorOnCallback},
        {"OFF", nullptr, LevelEditorOffCallback},
        {nullptr, nullptr, nullptr} // Null-terminated array
    };

    /*YS::ELEM DebugMenuLevelEditorSubmenuItems[] = {
        {"ON", nullptr, LevelEditorOnCallback},
        {"OFF", nullptr, LevelEditorOffCallback},
        {nullptr, nullptr, nullptr} // Null-terminated array
    };*/

    YS::ELEM ConfigSubmenuItems[] = {
    {"edit", nullptr, ConfigSubmenuItemCallback},
    {"save", nullptr, SaveCNFCallback},
    {"edit 2", nullptr, Config2SubmenuItemCallback},
    {nullptr, nullptr, nullptr} // Null-terminated array
    };

    YS::ELEM ObjEntrySubmenuItems[] = {
    {"Player Menu", nullptr, PlayerMenuCallBack},
    {"Zako Menu", nullptr, ZakoMenuCallBack},
    {"NPC Menu", nullptr, NpcMenuCallBack},
    {"Boss Menu", nullptr, BossMenuCallBack},
    {"Weapon Menu", nullptr, WeaponMenuCallBack},
    {"Enemy Weapon Menu", nullptr, EnemyWeaponMenuCallBack},
    {"Savepoint Menu", nullptr, SavepointMenuCallBack},
    {"Field Object Menu", nullptr, FieldObjMenuCallBack},
    {"Battle NPC Menu", nullptr, BattleNpcMenuCallBack},
    {"Treasure Box Menu", nullptr, TreasureBoxMenuCallBack},
    {"Submenu Menu", nullptr, SubmenuMenuCallBack},
    {"Large Boss Menu", nullptr, LargeBossMenuCallBack},
    {"Gimmick Object Menu", nullptr, GimmickObjMenuCallBack},
    {"Memo Menu", nullptr, MemoMenuCallBack},
    {"RTN Menu", nullptr, RtnMenuCallBack},
    {"Minigame Menu", nullptr, MinigameMenuCallBack},
    {"World Map Menu", nullptr, WorldMapMenuCallBack},
    {"Prize Box Menu", nullptr, PrizeBoxMenuCallBack},
    {"Summon Menu", nullptr, SummonMenuCallBack},
    {"Shop Point Menu", nullptr, ShopPointMenuCallBack},
    {"Large Zako Menu", nullptr, LargeZakoMenuCallBack},
    {"Mass Effect Menu", nullptr, MassEffectMenuCallBack},
    {"Enemy Object Menu", nullptr, EnemyObjMenuCallBack},
    {"Jigsaw Menu", nullptr, JigsawMenuCallBack},
    {"OBJENTRY (OLD)", nullptr, OBJENTRYwindowCallBack},
    {nullptr, nullptr, nullptr} // Null-terminated array
    };


    YS::ELEM LevelEditorObjEntrySubmenuItems[] = {
    //{"Player Menu", nullptr, PlayerMenuCallBack},
    {"Zako Menu", nullptr, LevelEditorZakoMenuCallBack},
    {"NPC Menu", nullptr, LevelEditorNpcMenuCallBack},
    {"Boss Menu", nullptr, LevelEditorBossMenuCallBack},
    {"Weapon Menu", nullptr, LevelEditorWeaponMenuCallBack},
    {"Enemy Weapon Menu", nullptr, LevelEditorEnemyWeaponMenuCallBack},
    {"Savepoint Menu", nullptr, LevelEditorSavepointMenuCallBack},
    {"Field Object Menu", nullptr, LevelEditorFieldObjMenuCallBack},
    {"Battle NPC Menu", nullptr, LevelEditorBattleNpcMenuCallBack},
    {"Treasure Box Menu", nullptr, LevelEditorTreasureBoxMenuCallBack},
    {"Submenu Menu", nullptr, LevelEditorSubmenuMenuCallBack},
    {"Large Boss Menu", nullptr, LevelEditorLargeBossMenuCallBack},
    {"Gimmick Object Menu", nullptr, LevelEditorGimmickObjMenuCallBack},
    {"Memo Menu", nullptr, LevelEditorMemoMenuCallBack},
    {"RTN Menu", nullptr, LevelEditorRtnMenuCallBack},
    {"Minigame Menu", nullptr, LevelEditorMinigameMenuCallBack},
    {"World Map Menu", nullptr, LevelEditorWorldMapMenuCallBack},
    {"Prize Box Menu", nullptr, LevelEditorPrizeBoxMenuCallBack},
    {"Summon Menu", nullptr, LevelEditorSummonMenuCallBack},
    {"Shop Point Menu", nullptr, LevelEditorShopPointMenuCallBack},
    {"Large Zako Menu", nullptr, LevelEditorLargeZakoMenuCallBack},
    {"Mass Effect Menu", nullptr, LevelEditorMassEffectMenuCallBack},
    {"Enemy Object Menu", nullptr, LevelEditorEnemyObjMenuCallBack},
    {"Jigsaw Menu", nullptr, LevelEditorJigsawMenuCallBack},
    //{"OBJENTRY (OLD)", nullptr, OBJENTRYwindowCallBack},
    {nullptr, nullptr, nullptr} // Null-terminated array
    };

    YS::ELEM EffectsSubMenuItems[] = {
        {"pa editer", nullptr, menu_pa_editer_callback},
        {"ps editer", nullptr, menu_pa_editer_callback},
        {nullptr, nullptr, nullptr} // Null-terminated array
    };

    YS::ELEM DebugMenuChaosModSubmenuItems[] = {
        {"ON", nullptr, ChaosModOnCallback},
        {"OFF", nullptr, ChaosModOffCallback},
        {nullptr, nullptr, nullptr} // Null-terminated array
    };

    YS::ELEM DebugMenuPartyModeSubmenuItems[] = {
        {"ON", nullptr, PartyModeOnCallback},
        //{"ON", nullptr, MultiCameraONCallback},      
        {"OFF", nullptr, PartyModeOffCallback},
        {nullptr, nullptr, nullptr} // Null-terminated array
    };

    void NO_DEAD_OBJMenuWindowCallBack(int selectedItemIndex) {

        GiveOBJLongStarEffect(0);
    }

    void ColorOBJMenuWindowCallBack(int selectedItemIndex) {
        uintptr_t IsOBJValidFunctionAddress = ReadLEA32(Patterns::pattern_for_IsObjValid, L"Is OBJ Valid?", 0, 1, 5);
        FunctionCalls::tIsOBJValid IsOBJValidFunction = reinterpret_cast<FunctionCalls::tIsOBJValid>(IsOBJValidFunctionAddress);

        YS::OBJ* obj = GetMenuOBJFunction();
        if (!obj) {
            return;
        }

        gGlobalObjPtr1 = obj;

        ChangeOBJColors(0);
    }

    

    

    void OBJMenuGiveObjMegalixirCallback(int selectedItemIndex) {
        GiveObjectItem(0);
    }

    void OBJMenuScatterPrizeObj(int selectedItemIndex) {
        ScatterPrizeOBJ(0);
    }

    void OBJMenuAttachOBJToCameraCallBack(int selectedItemIndex) {
        AttachOBJToCameraCallBack(0);
    }

    void OBJMenuAttachPlayerToOBJCallBack(int selectedItemIndex) {
        AttachPlayerToOBJCallBack(0);
    }

    void OBJMenuAttachOBJToPlayerCallBack(int selectedItemIndex) {
        AttachOBJToPlayerCallBack(0);
    }

    void OBJMenuDetatchOBJCallback(int selectedItemIndex) {
        DetachOBJCallback(0);
    }

    void OBJMenuAttachCameraToOBJCallBack(int selectedItemIndex) {
        AttachCameraToOBJCallBack(0);
    }

    void OBJMenuChangeOBJteamToPlayerCallBack(int selectedItemIndex) {
        ChangeOBJteamToPlayer(0);
    }

    void OBJMenuobj_menu_callback(int selectedItemIndex) {
        obj_menu_callback(0);
    }

    void OBJMenuDonation_menu_callback(int selectedItemIndex) {
        DonateButtonCallBack(0);
    }

    void OBJMenuActmenucallback(int selectedItemIndex) {

        YS::OBJ* obj = GetMenuOBJFunction();
        if (!obj) {
            return;
        }

        gGlobalObjPtr1 = obj;

        OpenActmenuCallback(0);
    }

    

    void OBJMenuChangeOBJStatsCallBack(int selectedItemIndex) {

        YS::OBJ* obj = GetCachedMenuOBJ();

        if (!obj) {
            return;
        }


        ChangeOBJStats(0);
    }

    void OBJMenuOBJscaleMenuCallback(int selectedItemIndex) {

        YS::OBJ* obj = GetMenuOBJFunction();
        if (!obj) {
            return;
        }

        gGlobalObjPtr1 = obj;

        


        OBJscaleMenuCallback(0);
    }

    void OBJMenuTargetReplaceMenuCallback(int selectedItemIndex) {
        ChangeOBJTarget(0);
    }

    void OBJMenuChangeOBJTexanm(int selectedItemIndex) {
        uintptr_t IsOBJValidFunctionAddress = ReadLEA32(Patterns::pattern_for_IsObjValid, L"Is OBJ Valid?", 0, 1, 5);
        FunctionCalls::tIsOBJValid IsOBJValidFunction = reinterpret_cast<FunctionCalls::tIsOBJValid>(IsOBJValidFunctionAddress);

        YS::OBJ* obj = GetMenuOBJFunction();
        if (!obj) {
            return;
        }

        // Optional: use game’s own validity check
        if (!IsOBJValidFunction(obj)) {
            return;
        }

        // This is the entity we’ll use in the scale menu and all other functions
        gGlobalObjPtr1 = obj;

        LOG("gGlobalObjPtr1 = 0x%p\n", gGlobalObjPtr1);



        ChangeObjTexanm(0);
    }




    void DebugLoop() {

        
        
        InitAudioEngine();
        //InitAudioDevice();

        //uintptr_t AddDebugMenuElemFunctionAddress = FindAndPrintPatternW(Patterns::pattern_for_AddDebugMenuElem, L"Add DebugMenu ELEM", 0);
        uintptr_t DebugWindowAddSubMenuFunctionAddress = FindAndPrintPatternW(Patterns::pattern_for_DebugWindowAddSubMenu, L"Add Debug Submenu", 0); // direct call
        FunctionCalls::tDebugWindowAddSubMenu DebugWindowAddSubMenuFunction = reinterpret_cast<FunctionCalls::tDebugWindowAddSubMenu>(DebugWindowAddSubMenuFunctionAddress);
       // FunctionCalls::tDBGWINaddMenuElem AddDebugMenuElemFunction = reinterpret_cast<FunctionCalls::tDBGWINaddMenuElem>(AddDebugMenuElemFunctionAddress);

        YS::ELEM WindowCustomMusicWindow;
        WindowCustomMusicWindow.Label = "(custom) Music Player";
        WindowCustomMusicWindow.SubMenu = nullptr;
        WindowCustomMusicWindow.Callback = CustomMusicWindowCallback;

        YS::ELEM WindowCustomPaxEffectSelector;
        WindowCustomPaxEffectSelector.Label = "(custom) Pax Effect Selector";
        WindowCustomPaxEffectSelector.SubMenu = nullptr;
        WindowCustomPaxEffectSelector.Callback = CustomPaxEffectSelectorWindowCallback;
        

        YS::ELEM WindowChaosMod;
        WindowChaosMod.Label = "(custom) Chaos Mod";
        WindowChaosMod.SubMenu = DebugMenuChaosModSubmenuItems;

        YS::ELEM WindowPartyMode;
        WindowPartyMode.Label = "(custom) Party Mode";
        WindowPartyMode.SubMenu = DebugMenuPartyModeSubmenuItems;
        


        YS::ELEM WindowLevelEditor;
        WindowLevelEditor.Label = "(custom) Level Editor";
        WindowLevelEditor.SubMenu = DebugMenuLevelEditorSubmenuItems;

        YS::ELEM WindowLevelEditorOBJENTRY;
        WindowLevelEditorOBJENTRY.Label = "(custom) Level Editor OBJENTRY";
        WindowLevelEditorOBJENTRY.SubMenu = LevelEditorObjEntrySubmenuItems;

        YS::ELEM WindowTextureSelector;
        WindowTextureSelector.Label = "(custom) Texture Selector";
        WindowTextureSelector.Callback = CustomTextureSelectorCallback;
        

        YS::ELEM WindowMultiplayer;
        WindowMultiplayer.Label = "(custom) P2P MultiPlayer";
        WindowMultiplayer.SubMenu = DebugMenuMultiplayerSubmenuItems;
        //WindowMultiplayer.Callback = CustomMusicWindowCallback;

        YS::ELEM WindowEffects;
        WindowEffects.Label = "(custom) Effects";
        WindowEffects.SubMenu = EffectsSubMenuItems;

        YS::ELEM WindowOBJENTRY;
        WindowOBJENTRY.Label = "(custom) OBJENTRY";
        //WindowOBJENTRY.SubMenu = DebugMenuOBJENTRYSubmenuItems;
        WindowOBJENTRY.SubMenu = ObjEntrySubmenuItems;
        //WindowOBJENTRY.Callback = OBJENTRYwindowCallBack;

        YS::ELEM ConfigmenuItem;
        ConfigmenuItem.Label = "(custom) Config";
        ConfigmenuItem.SubMenu = ConfigSubmenuItems;

        YS::ELEM WindowFreeCamera;
        WindowFreeCamera.Label = "(custom) camera";
        WindowFreeCamera.SubMenu = DebugMenuCameraSubmenuItems;

        YS::ELEM BonusLevelMenu;
        BonusLevelMenu.Label = "(custom) BonusLevel";
        BonusLevelMenu.SubMenu = nullptr;
        BonusLevelMenu.Callback = BonusLevelMenuOpen;

        //YS::ELEM WindowLevelEditor;
        //WindowLevelEditor.Label = "(custom) level editor";
       // WindowLevelEditor.SubMenu = DebugMenuLevelEditorSubmenuItems;

        YS::ELEM OBJMenuPartyStarItem;
        OBJMenuPartyStarItem.Label = "(custom) no_dead";
        OBJMenuPartyStarItem.SubMenu = nullptr;
        OBJMenuPartyStarItem.Callback = NO_DEAD_OBJMenuWindowCallBack;

        YS::ELEM OBJMenuColormenuItem;
        OBJMenuColormenuItem.Label = "(custom) OBJ Color";
        OBJMenuColormenuItem.SubMenu = nullptr;
        OBJMenuColormenuItem.Callback = ColorOBJMenuWindowCallBack;

        YS::ELEM OBJMenuGiveObjMegalixirmenuItem;
        OBJMenuGiveObjMegalixirmenuItem.Label = "(custom) Give OBJ Megalixir";
        OBJMenuGiveObjMegalixirmenuItem.SubMenu = nullptr; // No submenu assigned, it's set to nullptr
        OBJMenuGiveObjMegalixirmenuItem.Callback = OBJMenuGiveObjMegalixirCallback;

        YS::ELEM OBJMenuScatterPrizemenuItem;
        OBJMenuScatterPrizemenuItem.Label = "(custom) Scatter Prize";
        OBJMenuScatterPrizemenuItem.SubMenu = nullptr; // No submenu assigned, it's set to nullptr
        OBJMenuScatterPrizemenuItem.Callback = OBJMenuScatterPrizeObj;

        YS::ELEM OBJMenuAttachOBJToCameramenuItem;
        OBJMenuAttachOBJToCameramenuItem.Label = "(custom) Attach OBJ to Camera";
        OBJMenuAttachOBJToCameramenuItem.SubMenu = nullptr; // No submenu assigned, it's set to nullptr
        OBJMenuAttachOBJToCameramenuItem.Callback = OBJMenuAttachOBJToCameraCallBack;

        YS::ELEM OBJMenuAttachPlayerToOBJmenuItem;
        OBJMenuAttachPlayerToOBJmenuItem.Label = "(custom) Attach Player to OBJ";
        OBJMenuAttachPlayerToOBJmenuItem.SubMenu = nullptr; // No submenu assigned, it's set to nullptr
        OBJMenuAttachPlayerToOBJmenuItem.Callback = OBJMenuAttachPlayerToOBJCallBack;

        YS::ELEM OBJMenuAttachOBJToPlayermenuItem;
        OBJMenuAttachOBJToPlayermenuItem.Label = "(custom) Attach OBJ to Player";
        OBJMenuAttachOBJToPlayermenuItem.SubMenu = nullptr; // No submenu assigned, it's set to nullptr
        OBJMenuAttachOBJToPlayermenuItem.Callback = OBJMenuAttachOBJToPlayerCallBack;

        YS::ELEM OBJMenuDetatchOBJmenuItem;
        OBJMenuDetatchOBJmenuItem.Label = "(custom) Detatch Object";
        OBJMenuDetatchOBJmenuItem.SubMenu = nullptr;
        OBJMenuDetatchOBJmenuItem.Callback = OBJMenuDetatchOBJCallback;

        YS::ELEM OBJMenuAttachCameraToOBJmenuItem;
        OBJMenuAttachCameraToOBJmenuItem.Label = "(custom) Attach Camera To OBJ";
        OBJMenuAttachCameraToOBJmenuItem.SubMenu = nullptr; // No submenu assigned, it's set to nullptr
        OBJMenuAttachCameraToOBJmenuItem.Callback = OBJMenuAttachCameraToOBJCallBack;

        YS::ELEM OBJMenuChangeOBJteamToPlayermenuItem;
        OBJMenuChangeOBJteamToPlayermenuItem.Label = "(custom) Turn OBJ to Team Player";
        OBJMenuChangeOBJteamToPlayermenuItem.SubMenu = nullptr;
        OBJMenuChangeOBJteamToPlayermenuItem.Callback = OBJMenuChangeOBJteamToPlayerCallBack;

        YS::ELEM OBJMenuOBJscalemenuItem;
        OBJMenuOBJscalemenuItem.Label = "(custom) Scale";
        OBJMenuOBJscalemenuItem.SubMenu = nullptr; // No submenu assigned, it's set to nullptr
        OBJMenuOBJscalemenuItem.Callback = OBJMenuOBJscaleMenuCallback;

        YS::ELEM OBJMenuobj_menu_callbackmenuItem;
        OBJMenuobj_menu_callbackmenuItem.Label = "(custom) obj_menu";
        OBJMenuobj_menu_callbackmenuItem.SubMenu = nullptr;
        OBJMenuobj_menu_callbackmenuItem.Callback = OBJMenuobj_menu_callback;

        YS::ELEM OBJMenuDonation_menu_callbackmenuItem;
        OBJMenuDonation_menu_callbackmenuItem.Label = "(custom) Donation";
        OBJMenuDonation_menu_callbackmenuItem.SubMenu = nullptr;
        OBJMenuDonation_menu_callbackmenuItem.Callback = OBJMenuDonation_menu_callback;

        YS::ELEM OBJMenuChangeOBJStatsmenuItem;
        OBJMenuChangeOBJStatsmenuItem.Label = "(custom) Change OBJ Stats";
        OBJMenuChangeOBJStatsmenuItem.SubMenu = nullptr;
        OBJMenuChangeOBJStatsmenuItem.Callback = OBJMenuChangeOBJStatsCallBack;

        YS::ELEM OBJMenuReplaceTargetItem;
        OBJMenuReplaceTargetItem.Label = "(custom) Replace Player Target with OBJ";
        OBJMenuReplaceTargetItem.SubMenu = nullptr;
        OBJMenuReplaceTargetItem.Callback = OBJMenuTargetReplaceMenuCallback;

        YS::ELEM OBJMenuOBJTexanmStartItem;
        OBJMenuOBJTexanmStartItem.Label = "(custom) texanm";
        OBJMenuOBJTexanmStartItem.SubMenu = nullptr;
        OBJMenuOBJTexanmStartItem.Callback = OBJMenuChangeOBJTexanm;

        YS::ELEM OBJMenuActMenuCallBackItem;
        OBJMenuActMenuCallBackItem.Label = "(custom) act";
        OBJMenuActMenuCallBackItem.SubMenu = nullptr;
        OBJMenuActMenuCallBackItem.Callback = OBJMenuActmenucallback;

        

        YS::ELEM OBJMenuPartsItem;
        OBJMenuPartsItem.Label = "(custom) parts";
        OBJMenuPartsItem.SubMenu = OBJPartsSubMenuItems;


        YS::ELEM OBJMenuControlEntityMovementsItem;
        OBJMenuControlEntityMovementsItem.Label = "(custom) Movement";
        OBJMenuControlEntityMovementsItem.SubMenu = DebugMenuEntityMovementSubmenuItems;

        
        

        






        //DebugWindowAddSubMenuFunction(0, &WindowMultiplayer);
        DebugWindowAddSubMenuFunction(0, &WindowOBJENTRY);
        DebugWindowAddSubMenuFunction(0, &WindowFreeCamera);
        DebugWindowAddSubMenuFunction(0, &WindowLevelEditor);
        DebugWindowAddSubMenuFunction(0, &WindowLevelEditorOBJENTRY);
        //DebugWindowAddSubMenuFunction(0, &WindowTextureSelector);
        
        
        //DebugWindowAddSubMenuFunction(0, &WindowLevelEditor);
       // DebugWindowAddSubMenuFunction(0, &WindowCustomPaxEffectSelector);
        DebugWindowAddSubMenuFunction(0, &ConfigmenuItem);
        DebugWindowAddSubMenuFunction(12, &OBJMenuobj_menu_callbackmenuItem);
        DebugWindowAddSubMenuFunction(0, &OBJMenuDonation_menu_callbackmenuItem);
        //DebugWindowAddSubMenuFunction(10, &WindowEffects);
        DebugWindowAddSubMenuFunction(0, &WindowChaosMod);
        DebugWindowAddSubMenuFunction(0, &WindowPartyMode);
        
        //DebugWindowAddSubMenuFunction(0, &BonusLevelMenu);
        
        
        
        OBJMenuAddMenu(&OBJMenuPartyStarItem);
        OBJMenuAddMenu(&OBJMenuColormenuItem);
        OBJMenuAddMenu(&OBJMenuGiveObjMegalixirmenuItem);
        OBJMenuAddMenu(&OBJMenuScatterPrizemenuItem);
        OBJMenuAddMenu(&OBJMenuAttachOBJToCameramenuItem);
        OBJMenuAddMenu(&OBJMenuAttachPlayerToOBJmenuItem);
        OBJMenuAddMenu(&OBJMenuAttachOBJToPlayermenuItem);
        OBJMenuAddMenu(&OBJMenuAttachCameraToOBJmenuItem);
        OBJMenuAddMenu(&OBJMenuDetatchOBJmenuItem);
        OBJMenuAddMenu(&OBJMenuOBJscalemenuItem);
        OBJMenuAddMenu(&OBJMenuChangeOBJStatsmenuItem);
        OBJMenuAddMenu(&OBJMenuChangeOBJteamToPlayermenuItem);
        OBJMenuAddMenu(&OBJMenuReplaceTargetItem);
        OBJMenuAddMenu(&OBJMenuOBJTexanmStartItem);
        //OBJMenuAddMenu(&OBJMenuPartsItem);
        OBJMenuAddMenu(&OBJMenuControlEntityMovementsItem);
        OBJMenuAddMenu(&OBJMenuActMenuCallBackItem);


        // Log the setup of WindowCustomMusicWindow
        //printf("Setup WindowCustomMusicWindow: Label = %s, SubMenu = %p, Callback = %p\n",
            //WindowCustomMusicWindow.Label, WindowCustomMusicWindow.SubMenu, WindowCustomMusicWindow.Callback);

        // Check if DebugWindowAddSubMenuFunction is valid
        if (DebugWindowAddSubMenuFunction) {
            //LOG(L"Calling DebugWindowAddSubMenuFunction\n");

            // Call the function
            DebugWindowAddSubMenuFunction(0, &WindowCustomMusicWindow);
        }
        else {
            // Log an error if the function pointer is not valid
            LOG(L"Error: DebugWindowAddSubMenuFunction is not initialized\n");
        }

        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO();

        // Load fonts here
        LoadFonts(io);


    }


    


