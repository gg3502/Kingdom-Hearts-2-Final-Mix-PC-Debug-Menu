#include <Windows.h>
#include <chrono>
#define INITGUID
#include <initguid.h>
#include <d3d12.h>
#include <dxgi1_4.h>
//#include <d3dx12.h>
#include <dxgi1_6.h>            // For DXGI interfaces like ID3D12SwapChain4
#include <wrl/client.h>

#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")

#include <memory>

#include "hook_directx12.hpp"

#include "imgui_impl_dx12.h"
#include "imgui_impl_win32.h"
#include "MinHook.h"
#include <wrl.h> 

#include "utils.hpp"
#include "hooks.hpp"
#include "helper.hpp"
#include <vector>
#include <DirectXMath.h>
#include "Entity.hpp"
#include "kh2-dmenu.hpp"
#include <iostream>
#include "YS.hpp"
#include "function_typedefs.hpp"
#include <map>
#include "assert.hpp"
#include "Fonts.hpp"
#include "Damage.hpp"
#include "Caption.hpp"
#include "kn_camera.hpp"
#include "Unit_Group.hpp"
#include "Version.hpp"
#include "VoteManager.hpp"
#include "EffectManager.hpp"
#include "VM.h"
#include "PipeLogger.hpp"
#include <Xinput.h>
#pragma comment(lib, "Xinput9_1_0.lib")
#include <wincodec.h>
#pragma comment(lib, "windowscodecs.lib")

//#include <CinematicCamera.h>
VoteManager voteManager;
std::atomic<bool> twitchConnected{ false };




#ifdef max
#undef max
#endif
#include <algorithm>

#ifdef min
#undef min
#endif

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#include <unordered_map>
#include <d3dcompiler.h>
#include "imgui_internal.h"
#include <random>
#include "MenuRects.hpp"
#include <filesystem>

#define _CRT_SECURE_NO_WARNINGS
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include <ChaosManager.hpp>
#include <SAVERAM.hpp>
#include <mutex>
#include <deque>
#include <CommandRegistry.hpp>
//#include <ImguiTextureManager.h>
#define V ImVec2
#define F float

using namespace DirectX;

uintptr_t moduleBase = (uintptr_t)GetModuleHandle(L"KINGDOM HEARTS II FINAL MIX.exe");

static FunctionCalls::tIsOBJValid g_IsOBJValid = nullptr;



struct Dx12ImguiTexture
{
    ID3D12Resource* texture = nullptr;         // DEFAULT heap texture
    ID3D12Resource* upload = nullptr;          // UPLOAD heap staging (kept alive until copy finishes)
    D3D12_GPU_DESCRIPTOR_HANDLE gpuHandle{};
    UINT srvIndex = 0;
    int width = 0, height = 0;
};


static obj_camera* g_cam2 = nullptr;
static bool g_cam2TaskCreated = false;
static bool g_cam2Inited = false;
static thread_local bool g_inDrawSceneHook = false;
static std::atomic<int> g_renderPassAtomic{ -1 };      // -1 none, 0 left, 1 right

static UINT g_srvDescriptorSize = 0;
static std::atomic_uint g_nextSrvIndex{ 0 };


static bool s_pngTried = false;

std::atomic<int> g_drawSceneCount{ 0 };
std::atomic<int> g_seenCamMask{ 0 };

static thread_local D3D12_VIEWPORT g_lastFullViewport{};
static thread_local bool g_haveLastFullViewport = false;


static bool g_cmdHooksInstalled = false;
static std::atomic<bool> g_inMainBackbufferPass{ false };
static std::atomic<bool> g_inImGuiPass{ false };
static std::atomic_bool g_splitEnabled{ true };
std::atomic<int> g_camNumAtomic{ -1 };
static std::atomic_uint64_t g_currentRTVPtr{ 0 };

// TEMP: force split always (for main backbuffer pass)
static std::atomic<bool> g_forceSplit{ true };

static std::once_flag g_addrOnce;
static std::atomic_bool g_addrsReady{ false };

static std::atomic_bool g_haveFullVp{ false };
static D3D12_VIEWPORT   g_fullVpCached{};

static std::mutex g_camMapMtx;
static std::unordered_map<ID3D12GraphicsCommandList*, int> g_camForCL;


using PFN_RSSetViewports = void (STDMETHODCALLTYPE*)(ID3D12GraphicsCommandList*, UINT, const D3D12_VIEWPORT*);
using PFN_RSSetScissorRects = void (STDMETHODCALLTYPE*)(ID3D12GraphicsCommandList*, UINT, const D3D12_RECT*);
using PFN_ResourceBarrier = void (STDMETHODCALLTYPE*)(ID3D12GraphicsCommandList*, UINT, const D3D12_RESOURCE_BARRIER*);
using PFN_ClearRTV = void (STDMETHODCALLTYPE*)(ID3D12GraphicsCommandList*, D3D12_CPU_DESCRIPTOR_HANDLE, const FLOAT[4], UINT, const D3D12_RECT*);
using PFN_ClearDSV = void (STDMETHODCALLTYPE*)(ID3D12GraphicsCommandList*, D3D12_CPU_DESCRIPTOR_HANDLE, D3D12_CLEAR_FLAGS, FLOAT, UINT8, UINT, const D3D12_RECT*);

static PFN_RSSetViewports    oRSSetViewports = nullptr;
static PFN_RSSetScissorRects oRSSetScissorRects = nullptr;
static PFN_ResourceBarrier   oResourceBarrier = nullptr;
static PFN_ClearRTV          oClearRTV = nullptr;
static PFN_ClearDSV          oClearDSV = nullptr;

static std::atomic<uint32_t> g_drawIdxCount[2] = { 0,0 };
static std::atomic<uint32_t> g_drawCount[2] = { 0,0 };
static std::atomic<uint32_t> g_setVpCount[2] = { 0,0 };
static std::atomic<uint32_t> g_setScCount[2] = { 0,0 };
static std::atomic<uint32_t> g_clearRTV[2] = { 0,0 };
static std::atomic<uint32_t> g_clearDSV[2] = { 0,0 };

// optional: capture a couple of last viewports/scissors for debugging
static std::atomic<int> g_lastCamSeen{ -1 };
static uint64_t g_frameNo = 0;
static thread_local bool g_inApplySplit = false;
static std::atomic<bool> g_haveCachedFullVp{ false };
static D3D12_VIEWPORT    g_cachedFullVp{};

static std::int64_t gP2Entity = 0;


static std::mutex ActiveVMsMutex;
constexpr int MAX_OPCODE_SIZE = 1024; // max number of opcodes to safely read
constexpr size_t OPCODE_HISTORY_SIZE = 16;

int g_cineSelectedKeyFrame = -1;


uintptr_t PAD1_ADDR = moduleBase + 0xBF31A0; // game.exe + BF31A0
uintptr_t PAD2_ADDR = moduleBase + 0xBF3208; // game.exe + BF3208

static inline float Clamp01(float v) { return v < 0.f ? 0.f : (v > 1.f ? 1.f : v); }

// Offset inside the entity where the 64-bit pad pointer lives:
static constexpr std::ptrdiff_t PAD_PTR_OFF = 0xAE8;
static constexpr std::ptrdiff_t PLAYER_PTR_OFF = 2784;  // *(a1 + 2784) == Player*
static int p2Calls = 0;
static int ticks = 0;
static ULONGLONG gMaskL2 = 0, gMaskR2 = 0;
static ULONGLONG gPad1Idle = 0;

// Target function type
using tSub_1403B2340 = void(__fastcall*)(std::int64_t a1);
static tSub_1403B2340 oSub_1403B2340 = nullptr;

std::deque<OpcodeHistoryEntry> OpcodeHistory;

static thread_local uint32_t CurrentPC = 0;
static thread_local bool WaitingForSyscall = false;

using VMConstructor_t = __int64(__fastcall*)(__int64, __int64, __int64, __int64);
VMConstructor_t OriginalVMConstructor = nullptr;

using VMExec_t = __int64(__fastcall*)(__int64, __int64);
VMExec_t oVMExec = nullptr;

using tPlayerMovement = void(__fastcall*)(__int64 a1);
tPlayerMovement OriginalPlayerMovement = nullptr;

using calc_damage_t = unsigned int(__fastcall*)(__int64 a1, __int64 a2, __int64 a3, int a4);
calc_damage_t orig_calc_damage = nullptr;

__int64(__fastcall* orig_AddExp)(int a1) = nullptr;

using ActStart_t = unsigned int(__fastcall*)(__int64 a1, unsigned __int16* a2);
ActStart_t OriginalActStart = nullptr;

using SysCallCallBack_t = __int64(__fastcall*)(__int64 a1, int a2, __int64 a3, int a4, __int64 a5);
SysCallCallBack_t OriginalSysCallCallBack = nullptr;

using YS_LoaderRead_t = size_t(__fastcall*)(const char* filename, unsigned int* addr);
YS_LoaderRead_t OriginalYS_LoaderRead = nullptr;

typedef __int64(__fastcall* tProjectionMatrix)(float* a1, __int64 a2, int a3, int a4, float a5, int a6, int a7);
tProjectionMatrix OriginalProjectionMatrix = nullptr;

using fn_CreateViewMatrix = uint64_t(__fastcall*)(uint64_t a1, uint64_t a2, uint64_t a3, int a4);

// original pointer filled by MinHook
fn_CreateViewMatrix OriginalCreateViewMatrix = nullptr;

using LevelUpAddEXP_t = __int64(__fastcall*)(__int64 a1);
LevelUpAddEXP_t OriginalLevelUpAddEXP = nullptr;

typedef void(__fastcall* rootmenu_focusfunc_t)(__int64 a1, char a2, unsigned int a3, unsigned int a4);
rootmenu_focusfunc_t Originalrootmenu_focusfunc = nullptr;

typedef void(__cdecl* tCreateObjCameraTask)();
tCreateObjCameraTask OriginalCreateObjCameraTask = nullptr;

using tDrawSceneTask = void(__fastcall*)(TASK* self);
static tDrawSceneTask oDrawSceneTask = nullptr;

using tGetCameraA = __int64* (__fastcall*)(bool current);
static tGetCameraA oGetCameraA = nullptr;

using tGetProjA = char* (__fastcall*)(char current);
static tGetProjA oGetProjA = nullptr;

typedef void(__cdecl* tCreateSingleCameraTask)();
tCreateSingleCameraTask oCreateSingleCameraTask = nullptr;

typedef void(__cdecl* createmulticamera_t)();
createmulticamera_t createmulticamera1 = (createmulticamera_t)(moduleBase + 0x162C50);

using tSetCamNum = void(__fastcall*)(int);
tSetCamNum oSetCamNum = nullptr;

static std::add_pointer_t<void STDMETHODCALLTYPE(
    ID3D12GraphicsCommandList*, UINT, UINT, UINT, UINT)> oDrawInstanced = nullptr;

static std::add_pointer_t<void STDMETHODCALLTYPE(
    ID3D12GraphicsCommandList*, UINT, UINT, UINT, INT, UINT)> oDrawIndexedInstanced = nullptr;

using tOMSetRenderTargets = void (STDMETHODCALLTYPE*)(
    ID3D12GraphicsCommandList*,
    UINT,
    const D3D12_CPU_DESCRIPTOR_HANDLE*,
    BOOL,
    const D3D12_CPU_DESCRIPTOR_HANDLE*);

static tOMSetRenderTargets oOMSetRenderTargets = nullptr;

using tExecPlayerCommand = void(__fastcall*)(__int64 a1);

static tExecPlayerCommand oExecPlayerCommand = nullptr;

using tReadPadTask = void(__fastcall*)();

static tReadPadTask oReadPadtask = nullptr;

using tPlayerCtor = void(__fastcall*)(
    YS::PLAYER* self,
    const OBJENTRY_0* entry,
    int form,
    YS::PAD* pad,
    const Vector4* pos,
    double rot
    );

static tPlayerCtor oPlayerCtor = nullptr;

static YS::OBJ** gPlayerSingleton =
reinterpret_cast<YS::OBJ**>(moduleBase + 0x2A10620);

using tCreateDXGIFactory1 = HRESULT(WINAPI*)(
    REFIID riid,
    void** ppFactory);

using tCreateDXGIFactory2 = HRESULT(WINAPI*)(
    UINT Flags,
    REFIID riid,
    void** ppFactory);

static tCreateDXGIFactory1 oCreateDXGIFactory1 = nullptr;
static tCreateDXGIFactory2 oCreateDXGIFactory2 = nullptr;

typedef __int64(__fastcall* tWriteVM)(
    __int64 a1,
    __m128* a2,
    __m128* a3,
    int a4
    );

tWriteVM oWriteVM = nullptr;


std::atomic<uintptr_t> g_camStructPtr{ 0 };
std::atomic<uintptr_t> g_vmPtr{ 0 };

//static uint64_t gPad1Idle = 0;
static uint64_t gPad2Idle = 0;





static Dx12ImguiTexture g_testTex;
static Dx12ImguiTexture g_pngTex;

static bool g_ImGuiInitialized = false;
static ID3D12Resource* g_mainRenderTarget = nullptr;

static ID3D12CommandAllocator* g_imguiAllocator = nullptr;
static ID3D12GraphicsCommandList* g_imguiCmdList = nullptr;
static ID3D12DescriptorHeap* g_imguiSrvHeap = nullptr;
static ID3D12Fence* g_imguiFence = nullptr;
static HANDLE g_imguiFenceEvent = nullptr;
static UINT64 g_imguiFenceValue = 0;

static bool g_imguiInitialized = false;

static bool g_frameBuilt = false;




OpcodeEntry opcode_table[] = {
    // PUSH
    {0,0,-1, "push",        "push int32",           2},
    {0,1,-1, "push.s",      "push single",          0},
    {0,2,0, "push.sp",      "push sp + int16",      1},
    {0,2,1, "push.wp",      "push wp + int16",      1},
    {0,2,2, "push.sp.d",    "push *sp + int16",     1},
    {0,2,3, "push.bd",      "push bd + int16",      1},
    {0,3,0, "push.d.sp",    "push *(sp + int16)",   1},
    {0,3,1, "push.d.wp",    "push *(wp + int16)",   1},
    {0,3,2, "push.d.sp.d",  "push *(*sp + int16)",  1},
    {0,3,3, "push.d.bd",    "push *(bd + int16)",   1},

    // POP
    {1,-1,0, "pop.sp",      "pop *(sp + int16)",    1},
    {1,-1,1, "pop.wp",      "pop *(wp + int16)",    1},
    {1,-1,2, "pop.sp.d",    "pop *(*sp + int16)",   1},
    {1,-1,3, "pop.bd",      "pop *(bd + int16)",    1},

    // MEMCPY TO
    {2,-1,0, "memcpy.sp",   "memcpy to (sp + int16)", 1},
    {2,-1,1, "memcpy.wp",   "memcpy to (wp + int16)", 1},
    {2,-1,2, "memcpy.sp.d", "memcpy to (*sp + int16)", 1},
    {2,-1,3, "memcpy.bd",   "memcpy to (bd + int16)", 1},

    // FETCH
    {3,-1,-1, "push.d.pop", "push *(pop() + int16)", 1},

    // GENERIC MEMCPY
    {4,-1,-1, "memcpy",     "generic memcpy", 0},

    // CONVERSIONS & MATH
    {5,0,0,  "cvt.w.s",    "int to float", 0},
    {5,0,2,  "neg",        "1 to -1, -1 to 1", 0},
    {5,0,3,  "not",        "bitwise not", 0},
    {5,0,4,  "seqz",       "set 1 if 0", 0},
    {5,0,5,  "abs",        "absolute integer", 0},
    {5,0,6,  "sltz",       "set 1 if < 0", 0},
    {5,0,7,  "slez",       "set 1 if <= 0", 0},
    {5,0,8,  "seqz",       "set 1 if 0", 0},
    {5,0,9,  "snez",       "set 1 if != 0", 0},
    {5,0,10, "sgez",       "set 1 if >= 0", 0},
    {5,0,11, "sgtz",       "set 1 if > 0", 0},

    {5,1,1,  "cvt.s.w",    "float to int", 0},
    {5,1,2,  "neg.s",      "negate float", 0},
    {5,1,5,  "abs.s",      "absolute float", 0},
    {5,1,6,  "sltz.s",     "set float < 0", 0},
    {5,1,7,  "slez.s",     "set float <= 0", 0},
    {5,1,8,  "seqz.s",     "set float = 0", 0},
    {5,1,9,  "snez.s",     "set float != 0", 0},
    {5,1,10, "sgez.s",     "set float >= 0", 0},
    {5,1,11, "sgtz.s",     "set float > 0", 0},

    // ARITHMETIC
    {6,0,0,  "add",        "a + b", 0},
    {6,0,1,  "sub",        "a - b", 0},
    {6,0,2,  "mul",        "a * b", 0},
    {6,0,3,  "div",        "a / b", 0},
    {6,0,4,  "mod",        "a % b", 0},
    {6,0,5,  "and",        "bitwise and", 0},
    {6,0,6,  "or",         "bitwise or", 0},
    {6,0,7,  "xor",        "bitwise xor", 0},
    {6,0,8,  "sll",        "shift left logical", 0},
    {6,0,9,  "sra",        "shift right arithmetic", 0},
    {6,0,10, "land",       "logical and", 0},
    {6,0,11, "lor",        "logical or", 0},

    {6,1,0,  "add.s",      "float a + b", 0},
    {6,1,1,  "sub.s",      "float a - b", 0},
    {6,1,2,  "mul.s",      "float a * b", 0},
    {6,1,3,  "div.s",      "float a / b", 0},
    {6,1,4,  "mod.s",      "float a % b", 0},

    // BRANCH
    {7,-1,0, "b",          "branch unconditionally", 1},
    {7,-1,1, "beqz",       "branch if zero", 1},
    {7,-1,2, "bnez",       "branch if non-zero", 1},

    // FUNCTION CALLS
    {8,-1,-1, "jal",       "local function call 16-bit", 1},
    {11,-1,-1,"jal32",     "local function call 32-bit", 2},

    // HALT & CONTROL
    {9,-1,0,  "halt",      "halt", 0},
    {9,-1,1,  "exit",      "exit", 0},
    {9,-1,2,  "ret",       "return", 0},
    {9,-1,3,  "drop",      "drop one value from tp", 0},
    {9,-1,5,  "dup",       "duplicate one value at tp", 0},

    // MATH FUNCTIONS
    {9,-1,6,  "sin",       "sin", 0},
    {9,-1,7,  "cos",       "cos", 0},
    {9,-1,8,  "degr",      "radian to degree", 0},
    {9,-1,9,  "radd",      "degree to radian", 0},

    // SYSCALL
    {10,-1,-1,"syscall",   "syscall", 0},
};

XMMATRIX g_ProjectionMatrix;

struct PCAllocator
{
    void* vftable;     // 0x00
    char pad[0x40];    // 0x08 - 0x3F
    uintptr_t heap_top; // 0x40
    uintptr_t heap_end; // 0x48
    uintptr_t used;     // 0x50 / 0x60? depends
};

struct MapEntry { WORD xbtn; ULONGLONG gameMask; };
static MapEntry gMap[] = {
  { XINPUT_GAMEPAD_START, 0 },
  { XINPUT_GAMEPAD_BACK, 0 },
  { XINPUT_GAMEPAD_A, 0 },
  { XINPUT_GAMEPAD_B, 0 },
  { XINPUT_GAMEPAD_X, 0 },
  { XINPUT_GAMEPAD_Y, 0 },
  { XINPUT_GAMEPAD_LEFT_SHOULDER, 0 },
  { XINPUT_GAMEPAD_RIGHT_SHOULDER, 0 },
  { XINPUT_GAMEPAD_LEFT_THUMB, 0 },
  { XINPUT_GAMEPAD_RIGHT_THUMB, 0 },
  { XINPUT_GAMEPAD_DPAD_UP, 0 },
  { XINPUT_GAMEPAD_DPAD_RIGHT, 0 },
  { XINPUT_GAMEPAD_DPAD_DOWN, 0 },
  { XINPUT_GAMEPAD_DPAD_LEFT, 0 },
};

struct HardMapEntry { std::uint64_t hardBit; std::uint64_t gameMask; bool learned; };

static HardMapEntry gHardMap[] = {
    { (ULONGLONG)YS::HARDPAD::UP,        0, false },
    { (ULONGLONG)YS::HARDPAD::DOWN,      0, false },
    { (ULONGLONG)YS::HARDPAD::LEFT_0,    0, false },
    { (ULONGLONG)YS::HARDPAD::RIGHT_0,   0, false },

    { (ULONGLONG)YS::HARDPAD::START,     0, false },
    { (ULONGLONG)YS::HARDPAD::SELECT,    0, false },

    { (ULONGLONG)YS::HARDPAD::L1,        0, false },
    { (ULONGLONG)YS::HARDPAD::R1,        0, false },
    { (ULONGLONG)YS::HARDPAD::L2,        0, false },
    { (ULONGLONG)YS::HARDPAD::R2,        0, false },

    { (ULONGLONG)YS::HARDPAD::L3,        0, false },
    { (ULONGLONG)YS::HARDPAD::R3,        0, false },

    { (ULONGLONG)YS::HARDPAD::CROSS,     0, false },
    { (ULONGLONG)YS::HARDPAD::CIRCLE,    0, false },
    { (ULONGLONG)YS::HARDPAD::SQUARE,    0, false },
    { (ULONGLONG)YS::HARDPAD::TRIANGLE,  0, false },
};

static bool OneBit(WORD v) { return v && !(v & (v - 1)); }

static void CalibrateFromPad1(const XINPUT_STATE& st, const YS::PAD* pad1)
{
    WORD w = st.Gamepad.wButtons;
    if (!OneBit(w) || gPad1Idle == 0) return;

    for (auto& e : gMap)
    {
        if (e.xbtn == w && e.gameMask == 0)
        {
            e.gameMask = pad1->Button & ~gPad1Idle; // IMPORTANT
            std::printf("[MAP] learned xbtn=0x%X -> gameMask=0x%llX\n", e.xbtn, e.gameMask);
            break;
        }
    }
}





static void EnsureGameAddrsReady()
{
    std::call_once(g_addrOnce, [] {
        UpdateFunctionAddresses();     // your pattern + resolve routine
        g_addrsReady.store(true, std::memory_order_release);
        });
}

static inline float NormShort(SHORT v)
{
    return (v < 0) ? (v / 32768.0f) : (v / 32767.0f);
}

static inline void FillStick(Vector4& out, float x, float y)
{
    out.x = x;
    out.y = y;
    out.z = 0.0f;

    float mag = std::sqrt(x * x + y * y);
    const float deadzone = 0.20f;
    out.w = (mag <= deadzone) ? 0.0f : Clamp01((mag - deadzone) / (1.0f - deadzone));
}

static inline void UpdateEdges(YS::PAD& p, ULONGLONG newButtons)
{
    ULONGLONG old = p.Button;
    ULONGLONG changed = newButtons ^ old;

    p.Button = newButtons;
    p.Trigger = newButtons & changed;
    p.Release = changed & ~newButtons;

    // Simple repeat: just use Trigger initially
    p.Repeat = p.Trigger;
}

static ULONGLONG MapXInputToGameMask(WORD wButtons)
{
    ULONGLONG out = 0;
    for (auto& e : gMap)
        if (e.gameMask && (wButtons & e.xbtn))
            out |= e.gameMask;
    return out;
}

static ULONGLONG HardpadToGameMask(ULONGLONG hardMask)
{
    ULONGLONG out = 0;
    for (auto& e : gHardMap)
        if (e.learned && (hardMask & e.hardBit))
            out |= e.gameMask;
    return out;
}

static inline bool OneHardBit(std::uint64_t v) { return v && !(v & (v - 1)); }

static inline ULONGLONG MapXInputToHardpad(const XINPUT_STATE& st)
{
    ULONGLONG b = 0;
    const WORD w = st.Gamepad.wButtons;

    // DPad
    if (w & XINPUT_GAMEPAD_DPAD_UP)    b |= (ULONGLONG)YS::HARDPAD::UP;
    if (w & XINPUT_GAMEPAD_DPAD_RIGHT) b |= (ULONGLONG)YS::HARDPAD::RIGHT_0;
    if (w & XINPUT_GAMEPAD_DPAD_DOWN)  b |= (ULONGLONG)YS::HARDPAD::DOWN;
    if (w & XINPUT_GAMEPAD_DPAD_LEFT)  b |= (ULONGLONG)YS::HARDPAD::LEFT_0;

    // Start/Select
    if (w & XINPUT_GAMEPAD_START)      b |= (ULONGLONG)YS::HARDPAD::START;
    if (w & XINPUT_GAMEPAD_BACK)       b |= (ULONGLONG)YS::HARDPAD::SELECT;

    // Shoulder buttons
    if (w & XINPUT_GAMEPAD_LEFT_SHOULDER)  b |= (ULONGLONG)YS::HARDPAD::L1;
    if (w & XINPUT_GAMEPAD_RIGHT_SHOULDER) b |= (ULONGLONG)YS::HARDPAD::R1;

    // Stick clicks
    if (w & XINPUT_GAMEPAD_LEFT_THUMB)  b |= (ULONGLONG)YS::HARDPAD::L3;
    if (w & XINPUT_GAMEPAD_RIGHT_THUMB) b |= (ULONGLONG)YS::HARDPAD::R3;

    // Face buttons (Xbox -> PS style)
    if (w & XINPUT_GAMEPAD_A) b |= (ULONGLONG)YS::HARDPAD::CROSS;
    if (w & XINPUT_GAMEPAD_B) b |= (ULONGLONG)YS::HARDPAD::CIRCLE;
    if (w & XINPUT_GAMEPAD_X) b |= (ULONGLONG)YS::HARDPAD::SQUARE;
    if (w & XINPUT_GAMEPAD_Y) b |= (ULONGLONG)YS::HARDPAD::TRIANGLE;

    // Triggers as digital L2/R2
    constexpr BYTE trigThresh = 30;
    if (st.Gamepad.bLeftTrigger > trigThresh) b |= (ULONGLONG)YS::HARDPAD::L2;
    if (st.Gamepad.bRightTrigger > trigThresh) b |= (ULONGLONG)YS::HARDPAD::R2;

    return b;
}



void __fastcall hk_rootmenu_focusfunc(__int64 a1, char a2, unsigned int a3, unsigned int a4)
{
    __int64 drag = *(__int64*)(a1 + 136);

    // Only trigger on drag end: drag exists && !move flag
    if (drag && !(a2 & 1) && !(a2 & 0x20) && !(a2 & 0x40))
    {
        printf(
            "<wpos %d, %d>\n",
            *(int*)(drag + 8),
            *(int*)(drag + 12)
        );
    }


    
    Originalrootmenu_focusfunc(a1, a2, a3, a4);
}

void __fastcall hkSetCamNum(int n)
{
    g_camNumAtomic.store(n, std::memory_order_relaxed);
    oSetCamNum(n);
}

static bool IsReadablePtr(const void* p, std::size_t bytes = 8)
{
    MEMORY_BASIC_INFORMATION mbi{};
    if (!VirtualQuery(p, &mbi, sizeof(mbi)))
        return false;
    if (mbi.State != MEM_COMMIT)
        return false;
    if (mbi.Protect & (PAGE_NOACCESS | PAGE_GUARD))
        return false;
    auto start = reinterpret_cast<std::uintptr_t>(p);
    auto end = start + bytes;
    auto regionEnd = reinterpret_cast<std::uintptr_t>(mbi.BaseAddress) + mbi.RegionSize;
    return end <= regionEnd;
}

static __forceinline std::uint64_t* PadPtrSlot(std::int64_t a1)
{
    return reinterpret_cast<std::uint64_t*>(a1 + PAD_PTR_OFF);
}

static __forceinline std::int64_t GetPlayerPtr(__int64 a1)
{
    return *reinterpret_cast<std::int64_t*>(a1 + PLAYER_PTR_OFF);
}


void debug_task_pc(void* Allocator)
{
    auto* alloc = (PCAllocator*)Allocator;
    float totalMB = (float)(alloc->heap_end - alloc->heap_top) * 0.00000095367432f;
    float usedMB = (float)(alloc->used) * 0.00000095367432f;  // you may need to compute used = heap_top - head.next
    float percent = (usedMB / totalMB) * 100.0f;

    printf("areawork %4.2fmb/%4.2fmb (%3.1f%%)\n", usedMB, totalMB, percent);
}



static bool IsReadablePointer(void* p)
{
    if (!p) return false;
    MEMORY_BASIC_INFORMATION mbi;
    if (VirtualQuery(p, &mbi, sizeof(mbi)) == 0) return false;
    if (mbi.State != MEM_COMMIT) return false;
    // Check for guard or no access
    if ((mbi.Protect & PAGE_NOACCESS) || (mbi.Protect & PAGE_GUARD)) return false;
    // read/write/execute are ok
    return true;
}




const OpcodeEntry* GetOpcodeInfo(unsigned short opcode)
{
    unsigned int main = opcode >> 8;
    unsigned int sub = (opcode >> 4) & 0xF;
    unsigned int ssub = opcode & 0xF;

    for (size_t i = 0; i < sizeof(opcode_table) / sizeof(opcode_table[0]); ++i)
    {
        const OpcodeEntry& entry = opcode_table[i];
        if (entry.main != main) continue;
        if (entry.sub != -1 && entry.sub != (int)sub) continue;
        if (entry.ssub != -1 && entry.ssub != (int)ssub) continue;

        return &entry;
    }

    return nullptr;
}

void DisassembleVM(unsigned short* codeBase, size_t codeSize)
{
    size_t pc = 0;
    while (pc < codeSize)
    {
        unsigned short rawOpcode = codeBase[pc];
        const OpcodeEntry* entry = GetOpcodeInfo(rawOpcode);

        if (entry)
        {
            printf("Opcode 0x%04X: %s - %s", rawOpcode, entry->name, entry->description);

            // Handle extra arguments
            for (int i = 0; i < entry->argWords; ++i)
            {
                printf(" ARG[%d]=0x%04X", i, codeBase[pc + 1 + i]);
            }

            printf("\n");
            pc += 1 + entry->argWords;
        }
        else
        {
            printf("Opcode 0x%04X: UNKNOWN\n", rawOpcode);
            pc++;
        }
    }
}

static inline int SafeCam()
{
    if (!GetCameraNumberFunction) return -1;
    return GetCameraNumberFunction() & 1;
}

static void StampCam(ID3D12GraphicsCommandList* cmd)
{
    int cam = g_camNumAtomic.load(std::memory_order_relaxed) & 1;
    std::lock_guard<std::mutex> lk(g_camMapMtx);
    g_camForCL[cmd] = cam;
}

static int CamForThisCL(ID3D12GraphicsCommandList* cmd)
{
    std::lock_guard<std::mutex> lk(g_camMapMtx);
    auto it = g_camForCL.find(cmd);
    if (it == g_camForCL.end()) return 0; // default
    return it->second;
}



void InitializeIsObjValidPointer()
{
    uintptr_t addr = ReadLEA32(Patterns::pattern_for_IsObjValid, L"Is OBJ Valid?", 0, 1, 5);
    if (addr)
    {
        g_IsOBJValid = reinterpret_cast<FunctionCalls::tIsOBJValid>(addr);
        //LOG("[INIT] g_IsOBJValid resolved at 0x%llX\n", addr);
    }
    else
    {
        LOG("[INIT] Failed to resolve g_IsOBJValid!\n");
    }
}

size_t __fastcall HookedYS_LoaderRead(const char* filename, unsigned int* addr)
{
    size_t result = OriginalYS_LoaderRead(filename, addr);

    if (result == 0)
    {
        // File not found or failed to load
        //LOG_CHANNEL("ALL", "File not found: %s\n", filename);
        printf("File not found: %s\n", filename);
    }
    else
    {
        size_t bytes = result;
        float sizeMB = static_cast<float>(bytes) / 1024.0f / 1024.0f;

        //LOG_CHANNEL("ALL", "Loaded file: %s, size=%.2f MB (%zu bytes)\n", filename, sizeMB, bytes);
        printf("Loaded file: %s, size=%.2f MB (%zu bytes)\n", filename, sizeMB, bytes);
    }

    return result;
}

__int64 __fastcall Hooked_ActStart(__int64 a1, unsigned __int16* a2)
{
    if (!OriginalActStart)
    {
        LOG("[HOOK] OriginalActStart is null!\n");
        return 0;
    }

    if (!GetOBJEntryNameFunction)
    {
        LOG("[HOOK] GetOBJEntryNameFunction is null!\n");
        return OriginalActStart(a1, a2);
    }



    __int64 result = OriginalActStart(a1, a2);

    uint64_t vmObj = *(uint64_t*)(a1 + 0x108);
    //printf("[HOOK] VM Obj pointer: 0x%llX\n", vmObj);
    if (!vmObj)
    {
        LOG("[HOOK] VM Obj is null!\n");
        return result;
    }

    uint64_t EncodedObjPtr = *(uint64_t*)(vmObj + 0x4);
    uint64_t objPtr = CustomDecodePointer(EncodedObjPtr, moduleBase);
    
    if (!objPtr)
    {
        LOG("[HOOK] Obj pointer is null!\n");
        return result;
    }

    const char* entryName = GetOBJEntryNameFunction((YS::OBJ*)objPtr);
    if (!entryName) entryName = "<null>";

    uint32_t encodedElemName = *(uint32_t*)a2;
    uint64_t elemNamePtr = CustomDecodePointer(encodedElemName, moduleBase);

    const char* elemName = (const char*)elemNamePtr;
    


    wchar_t welemname[128];
    wchar_t wentryname[128];
    mbstowcs_s(nullptr, welemname, elemName ? elemName : "Unknown", _TRUNCATE);
    mbstowcs_s(nullptr, wentryname, entryName ? entryName : "Unknown", _TRUNCATE);

    // Print with wprintf for wide string
    //LOG_CHANNEL("ALL", "%s act start '%s'\n", entryName, elemName);
   LOG("%s act start '%s'\n", wentryname, welemname);
    

    //if (wcsstr(welemname, L"dead"))
        //LOG_CHANNEL("ALL", "dead\n");
      //  LOG_CHANNEL("ACT", "dead\n");
    //if (wcsstr(welemname, L"appear"))
     //   LOG_CHANNEL("ALL", "appear\n");
     //   LOG_CHANNEL("ACT", "appear\n");

    return result;
}



void PrintAttackInfo(void* a1, void* a2)
{
    // Attacker -> a2 + 8
    uint64_t attacker = *(uint64_t*)((char*)a2 + 8);
    printf("[DEBUG] Attacker BTLOBJ: 0x%llX\n", attacker);

    // Owner -> a2 + 16
    uint64_t encodedOwner = *(uint32_t*)((char*)a2 + 16);
    uint64_t owner = CustomDecodePointer(encodedOwner, moduleBase);
    printf("[DEBUG] Owner BTLOBJ: 0x%llX\n", owner);

    // Receiver BTLOBJ -> a1 + 0x268
    uint64_t receiver = *(uint64_t*)((char*)a1 + 0x268);
    printf("[DEBUG] Receiver BTLOBJ: 0x%llX\n", receiver);

    // Target OBJ -> receiver + 0x0 (Target.Obj is first member)
    if (receiver)
    {
        uint64_t targetObj = *(uint64_t*)(receiver + 0x0); // Target.Obj offset
        printf("[DEBUG] Receiver OBJ address: 0x%llX\n", targetObj);
    }
    else
    {
        printf("[DEBUG] Receiver is null\n");
    }
}


unsigned int __fastcall Hooked_calc_damage(__int64 a1, __int64 a2, __int64 a3, int a4)
{
    // Call the original function first
    unsigned int damage = orig_calc_damage(a1, a2, a3, a4);

    // Read encoded owner pointer
    unsigned int encodedOwner = *(unsigned int*)(a2 + 16);
    uint64_t owner = CustomDecodePointer(encodedOwner, moduleBase);
   
    //PrintAttackInfo(a1, a2);

    LOG_CHANNEL("ALL", "Encoded Owner: 0x%X\n", encodedOwner);
    LOG_CHANNEL("DAMAGE", "Encoded Owner: 0x%X\n", encodedOwner);
    LOG_CHANNEL("ALL", "Decoded Owner: 0x%llX\n", owner);
    LOG_CHANNEL("DAMAGE", "Decoded Owner: 0x%llX\n", owner);

    uint8_t DamageMultiplier = 1; // Will be overwritten by memory values below

    if (owner)
    {
        // Read klass flag
        uint32_t klassFlag = *(uint32_t*)((char*)owner + 0x6C8);
        LOG_CHANNEL("ALL", "Attacker klassFlag: 0x%X\n", klassFlag);
        LOG_CHANNEL("DAMAGE", "Attacker klassFlag: 0x%X\n", klassFlag);

        // Read the default multipliers from memory
        uint8_t playerMultiplier = *(uint8_t*)(moduleBase + 0x74980C);
        uint8_t friendMultiplier = *(uint8_t*)(moduleBase + 0x74980D);
        uint8_t enemyMultiplier = *(uint8_t*)(moduleBase + 0x74980E);

        // Apply the proper multiplier
        if (klassFlag & 0x80)        // Player
            DamageMultiplier = playerMultiplier;
        else if (klassFlag & 0x4000) // Friend
            DamageMultiplier = friendMultiplier;
        else                          // Enemy / default
            DamageMultiplier = enemyMultiplier;

        LOG_CHANNEL("ALL", "Using DamageMultiplier: %u\n", DamageMultiplier);
        LOG_CHANNEL("DAMAGE", "Using DamageMultiplier: %u\n", DamageMultiplier);

        // Apply multiplier
        damage *= DamageMultiplier;
    }
    else
    {
        LOG_CHANNEL("ALL", "Owner is null, using default multiplier memory values\n");
        LOG_CHANNEL("DAMAGE", "Owner is null, using default multiplier memory values\n");
    }
    LOG_CHANNEL("ALL", "Final Damage Output: %u\n", damage);
    LOG_CHANNEL("DAMAGE", "Final Damage Output: %u\n", damage);

    return damage;
}





void __fastcall HookedPlayerMovement(__int64 a1)
{
    OriginalPlayerMovement(a1);
    if (!a1) return;

    const bool isP2 = (g_player2Obj != nullptr && (void*)a1 == (void*)g_player2Obj);

    if (isP2)
    {
        YS::PAD* pad2 = (YS::PAD*)PAD2_ADDR;
        const Vector4& ls = pad2->Stick[1]; // LEFT stick

        Vector4 stick{};
        stick.x = ls.x;
        stick.y = ls.y;
        stick.z = 0.0f;

        float magRaw = std::sqrt(stick.x * stick.x + stick.y * stick.y);
        const float deadzone = 0.20f;
        stick.w = (magRaw <= deadzone) ? 0.0f : Clamp01((magRaw - deadzone) / (1.0f - deadzone));

        PlayerPadToMovementFunction((void*)(a1 + 16), &stick);

        if (stick.w <= 0.0f) {
            *(float*)(a1 + 28) = 0.0f;
            *(float*)(a1 + 16) = 0.0f;
            *(float*)(a1 + 20) = 0.0f;
            *(float*)(a1 + 24) = 0.0f;
        }
        else {
            int walk = *(int*)(a1 + 296);
            int run = *(int*)(a1 + 300);
            float outMag = *(float*)(a1 + 28);
            *(int*)(a1 + 28) = (outMag >= 0.80000001f) ? run : walk;
        }
    }


    // Keep your original multiplier
    float MoveSpeedMultiplier = *(float*)(moduleBase + 0x749808);
    *(float*)(a1 + 28) *= MoveSpeedMultiplier;
}

void AddOpcodeToHistory(uint32_t pc, uint16_t opcode) {
    if (OpcodeHistory.size() >= OPCODE_HISTORY_SIZE)
        OpcodeHistory.pop_front();
    OpcodeHistory.push_back({ pc, opcode });
}

void PrintOpcodeHistory() {
    for (const auto& entry : OpcodeHistory) {
        const OpcodeEntry* info = GetOpcodeInfo(entry.rawOpcode);
        if (info)
            printf("0x%X: %s\n", entry.pc, info->name);
        else
            printf("0x%X: UNKNOWN\n", entry.pc);
    }
}

void UpdateP2MovementOncePerFrame()
{
    if (!g_player2Obj)
        return;

    __int64 p2 = (std::int64_t)g_player2Obj;

    // Read PAD2 stick
    const float* pad2 = reinterpret_cast<const float*>(PAD2_ADDR + 48);

    float x = pad2[0];
    float y = pad2[1];

    float magRaw = std::sqrt(x * x + y * y);
    const float deadzone = 0.20f;

    Vector4 stick{};
    stick.x = x;
    stick.y = y;
    stick.z = 0.0f;
    stick.w = (magRaw <= deadzone)
        ? 0.0f
        : std::min(1.0f, (magRaw - deadzone) / (1.0f - deadzone));

    void* outMove = (void*)(p2 + 16); // x,y,z,w

    // Convert stick → world-space movement
    PlayerPadToMovementFunction(outMove, &stick);

    if (stick.w <= 0.0f)
    {
        // Idle (prevents walking-in-place)
        *(float*)(p2 + 28) = 0.0f;
        *(float*)(p2 + 16) = 0.0f;
        *(float*)(p2 + 20) = 0.0f;
        *(float*)(p2 + 24) = 0.0f;
    }
    else
    {
        int walk = *(int*)(p2 + 296);
        int run = *(int*)(p2 + 300);
        float outMag = *(float*)(p2 + 28);

        *(int*)(p2 + 28) = (outMag >= 0.80000001f) ? run : walk;
    }

    // Optional speed multiplier
    float mult = *(float*)(moduleBase + 0x749808);
    *(float*)(p2 + 28) *= mult;
}



/*__int64 __fastcall HookedVMEXEC(__int64 a1, __int64 a2)
{
    // Call the original constructor first
    __int64 result = oVMExec(a1, a2);

    VM* vm = (VM*)a1;
    //printf("[HOOK] New VM created at: 0x%llX\n", (uint64_t)vm);


   // uint32_t encodedVmObj = *(uint32_t*)((char*)vm + 0xB0);

    uint32_t encodedVmObj32 = *(uint32_t*)((char*)vm + 0xB0);

    if (encodedVmObj32 == 0 || encodedVmObj32 == 0xEFACCAFE)
    {
       // printf("[HOOK] VM at 0x%llX has no valid VMObj\n", (uint64_t)vm);
        return result; // skip decoding
    }

    uint64_t vmObj = CustomDecodePointer(encodedVmObj32, moduleBase);

    // Read actual owner/entity pointer
    uint32_t encodedOwner = *(uint32_t*)((char*)vmObj + 0x4);

    if (encodedOwner == 0 || encodedOwner == 0xEFACCAFE)
    {
       // printf("[HOOK] VMobj at 0x%llX has no valid owner\n", (uint64_t)vm);
        return result; // skip decoding
    }

    uint64_t owner = CustomDecodePointer(encodedOwner, moduleBase);

    if (!g_IsOBJValid || !g_IsOBJValid((YS::OBJ*)owner))
    {
       // printf("[HOOK] VMObj at 0x%llX points to invalid owner\n", (uint64_t)vm);
        return result;
    }
    
    const char* name = GetOBJEntryNameFunction((YS::OBJ*)owner);
    if (!name) name = "Unknown";

    wchar_t wname[128];
    mbstowcs_s(nullptr, wname, name ? name : "Unknown", _TRUNCATE);

    //LOG("[HOOK] VM: 0x%llX, VMObj: 0x%llX, Owner Entity: 0x%llX, Owner Name %s\n", (uint64_t)vm, vmObj, owner, wname);


    BD_PROCESS* p = &vm->Process;
    //DumpVMStack(p);
   // printf("=== VM PC DUMP ===\n");
    uintptr_t codeBase = (uintptr_t)p->top;

    // pc is actually a 32-bit offset (low part of the 64-bit value)
    uint64_t rawPc = (uint64_t)p->pc;
    uint32_t pcOffset = (uint32_t)(rawPc & 0xFFFFFFFF);

    uintptr_t pcAddr = codeBase + pcOffset;

    printf("=== VM PC DECODE ===\n");
    printf("Top: %p\n", p->top);
    printf("Raw PC : 0x%llX\n", rawPc);
    printf("PC Off : 0x%X\n", pcOffset);
    printf("PC Addr: %p\n", (void*)pcAddr);
    printf("==================\n");

    return result;
}*/

__int64 __fastcall HookedVMEXEC(__int64 a1, __int64 a2)
{
    __int64 result = oVMExec(a1, a2);

    VM* vm = (VM*)a1;
    BD_PROCESS* p = &vm->Process;

    unsigned short* codeBase = (unsigned short*)p->top;
   // unsigned short* instrAddr = (unsigned short*)p->pc;
    //size_t offset = (size_t)((uintptr_t)instrAddr - (uintptr_t)codeBase);
    //unsigned short* pcAddr = p->pc; // p->pc points to current instruction

    // Compute offset in bytes from code base
   // size_t pcOffset = (size_t)((uintptr_t)pcAddr - (uintptr_t)codeBase);

   // unsigned short* codeBase = (unsigned short*)p->top;
    //unsigned short* pcAddr = codeBase + pcOffset / 2;
    uint16_t opcode = codeBase[p->pc]; // p->pc is an index


    // record this opcode in history
    AddOpcodeToHistory(p->pc, opcode);

    return result;
}


__int64 __fastcall HookedSysCallCallBack(__int64 a1, int syscallID, __int64 args, int argc, __int64 current)
{

    VM* vm = (VM*)a1;
    BD_PROCESS* p = &vm->Process;
    unsigned short* codeBase = (unsigned short*)p->top;

    unsigned short* instrAddr = nullptr;
    size_t offset = 0;

    // --- Compute instruction pointer safely ---
    if ((uintptr_t)p->pc >= (uintptr_t)codeBase &&
        (uintptr_t)p->pc < (uintptr_t)codeBase + 0x1000000) // crude pointer check
    {
        // pc is a raw pointer
        instrAddr = (unsigned short*)p->pc;
        offset = (uintptr_t)instrAddr - (uintptr_t)codeBase;
    }
    else
    {
        // assume pc is an offset in bytes
        size_t pcOffset = (size_t)p->pc;
        instrAddr = codeBase + (pcOffset / 2); // 2 bytes per opcode
        offset = pcOffset;
    }


   // printf("=== Syscall triggered ===\n");
    LOG_CHANNEL("ALL", "Syscall ID=0x%X at address %p (offset 0x%zX) argc=%d\n", syscallID, instrAddr, offset, argc);
    LOG_CHANNEL("SYSCALL", "Syscall ID=0x%X at address %p (offset 0x%zX) argc=%d\n", syscallID, instrAddr, offset, argc);

    // print arguments
    BD_VALUE* bdArgs = (BD_VALUE*)args;
    for (int i = 0; i < argc; i++) {
        LOG_CHANNEL("ALL", " arg[%d]=0x%X\n", i, bdArgs[i].gap0);
        LOG_CHANNEL("SYSCALL", " arg[%d]=0x%X\n", i, bdArgs[i].gap0);
    }

    // print recent opcodes leading up to syscall
    PrintOpcodeHistory();

    // clear history after printing
    OpcodeHistory.clear();
    //printf("=========================\n");

    return OriginalSysCallCallBack(a1, syscallID, args, argc, current);
}


__int64 __fastcall HookedVMConstructor(__int64 a1, __int64 a2, __int64 a3, __int64 a4)
{
    // Call the original constructor first
    __int64 result = OriginalVMConstructor(a1, a2, a3, a4);

    VM* vm = (VM*)a1;
    
    YS::OBJ* owner = (YS::OBJ*)a4;

    // Register VM for tracing
    ActiveVMs.push_back({ vm, owner });
    const char* name = GetOBJEntryNameFunction(owner);

    wchar_t wname[128];
    mbstowcs_s(nullptr, wname, name ? name : "Unknown", _TRUNCATE);

    BD_PROCESS* p = &vm->Process;
    int pc = *(int*)&p->pc; // offset 8
    unsigned short opcode = *(unsigned short*)(*(uintptr_t*)p + pc * 2 + 16);
    //const char* opcodename = GetOpcodeName(opcode);

    LOG_CHANNEL("ALL", "New VM created for entity %p (%s)\n", owner, wname);
    LOG_CHANNEL("VM TRACE", "New VM created for entity %p (%s)\n", owner, wname);
    LOG_CHANNEL("ALL", "VM sp=%p, tp=%p, wp=%p\n", p->sp, p->tp, p->wp);
    LOG_CHANNEL("VM TRACE", "sp=%p, tp=%p, wp=%p\n", p->sp, p->tp, p->wp);
   // unsigned short* opcode_ptr = (unsigned short*)(*(uintptr_t*)vm + *(int*)&p->pc * 2 + 16);
    //unsigned short opcode = *opcode_ptr;

    //printf("[VM] PC=%p OPCODE=0x%X NAME=%s\n", (void*)pc, opcode, opcodename);


    return result;
}

bool SafeReadOpcode(unsigned int* wp, int pc, int termSize, unsigned int& outOpcode)
{
    if (!wp || pc < 0 || pc >= termSize)
        return false;

    // Optional sanity check for pointer
    if (reinterpret_cast<uintptr_t>(wp + pc) < 0x1000) // obviously invalid
        return false;

    outOpcode = wp[pc];
    return true;
}

__int64 __fastcall hkProjectionMatrix(float* a1, __int64 a2, int a3, int a4, float a5, int a6, int a7) {
    // Call original
    __int64 result = OriginalProjectionMatrix(a1, a2, a3, a4, a5, a6, a7);

    // Copy matrix to global
    std::memcpy(&g_ProjectionMatrix, (void*)a2, sizeof(Matrix4x4));

    return result;
}

uint64_t __fastcall hook_CreateViewMatrix(uint64_t a1, uint64_t a2, uint64_t a3, int a4)
{
    // a1 == RCX, a2 == RDX at the call site you showed
    // compute candidate vm using CE observation
    uintptr_t rcx = static_cast<uintptr_t>(a1);
    uintptr_t rdx = static_cast<uintptr_t>(a2);

    // CE computed vm = rcx + rdx + 0xE390
    uintptr_t vmCandidate = rcx + rdx + 0xE390; // 0xE390 == 58256 decimal

    // store runtime pointers for your other code to use
    g_camStructPtr.store(rdx);           // cam_struct == rdx (rbx + 0x08)
    g_vmPtr.store(vmCandidate);          // view matrix address

    // Optionally: log once (avoid spamming)
    static bool logged = false;
    if (!logged) {
        logged = true;
        char buf[256];
        sprintf_s(buf, "HOOK: rcx=0x%p rdx=0x%p vm=0x%p\n",
            reinterpret_cast<void*>(rcx),
            reinterpret_cast<void*>(rdx),
            reinterpret_cast<void*>(vmCandidate));
        printf(buf);
    }

    // Call original to preserve game behavior (or modify parameters before calling).
    return OriginalCreateViewMatrix(a1, a2, a3, a4);
}

__int64 __fastcall HookedLevelUpAddEXP(int exp)
{
    unsigned short* multiplier = reinterpret_cast<unsigned short*>(moduleBase + 0x74752C);
    int* totalEXP = reinterpret_cast<int*>(moduleBase + 0x9ACF90);

    // exactly PS3 behavior
    long long v2 = (long long)(*totalEXP) + ((long long)exp * (*multiplier));
    if (v2 > 9999999LL)
        v2 = 9999999LL;

    *totalEXP = (int)v2;

    // now just run level-up checks
    return OriginalLevelUpAddEXP(0);
}




/*void PollVMs()
{
    for (auto& v : ActiveVMs)
    {
        if (!v.vm)
            continue;

        BD_PROCESS* process = &v.vm->Process;
        if (!process || !process->wp)
            continue;

        int pc = process->pc;

        // If top is valid, read term_size; fallback to 512
        int termSize = 512;
        if (process->top)
        {
            BD_HEADER* header = reinterpret_cast<BD_HEADER*>(process->top);
            termSize = header->term_size;
        }

        unsigned int opcode;
        if (SafeReadOpcode(process->wp, pc, termSize, opcode))
        {
            LOG("Entity %p executing opcode 0x%08X (PC=%d)\n", v.owner, opcode, pc);
        }
        else
        {
            LOG("Entity %p has invalid PC/wp (PC=%d, term_size=%d), skipping\n", v.owner, pc, termSize);
        }
    }
}*/

void Cam2_OnRoomEnter()
{
    /*if (!g_cam2)
    {
        g_cam2 = (obj_camera*)_aligned_malloc(sizeof(obj_camera), 16);
        LOG("[Cam2] Allocated g_cam2=%p\n", (void*)g_cam2);
    }

    // per-room reset
    memset(g_cam2, 0, sizeof(obj_camera));
    g_player2Obj = nullptr;
    g_cam2Inited = false;
    g_cam2TaskCreated = false;

    SetKNCameraModeFunction(0);

    LOG("[Cam2] Request spawn NewObj(0x54,1)\n");*/
    NewObj(0x54, 1);
}



/*void camera_task2(TASK* task)
{
    if (!g_cam2 || !g_cam2->m_obj)
        return;

    static int counter = 0;
    if (++counter % 60 == 0)
        LOG("[Split] camera_task2 running, cam2 obj=%p\n", g_cam2->m_obj);

    const int prev = GetCameraNumberFunction();
    SetCameraNumberFunction(1);

    UpdateObjCameraFunction(g_cam2, 0);   // always = 0

    SetCameraNumberFunction(prev);
}*/

/*void Cam2_TickInit()
{
    if (!g_cam2)
        return;

    if (!g_cam2Inited)
    {
        if (!g_player2Obj) {
            static int t = 0;
            if (++t % 60 == 0)
                LOG("[Cam2] Waiting... g_player2Obj=%p\n", (void*)g_player2Obj);
            return;
        }

        g_cam2->m_obj = g_player2Obj;

        SetObjCameraParamFunction(g_cam2, 0);
        InitOBJCameraFunction(g_cam2);

        g_cam2Inited = true;
        LOG("[Cam2] Init OK cam2=%p obj=%p\n", (void*)g_cam2, (void*)g_cam2->m_obj);
    }


    if (g_cam2Inited && !g_cam2TaskCreated)
    {
        YS::GAME_MODE* gm = GetFieldModeFunction();
        LOG("[Cam2] FieldMode=%p\n", (void*)gm);

        TASK_MANAGER_11* mgr = *(TASK_MANAGER_11* const*)&(*((YS::GAME_MODE*)gm))[8];
        LOG("[Cam2] TaskMgr=%p\n", (void*)mgr);

        TaskManagerCreateTaskFunction(mgr, 1, 26001, (TASK_FUNC)camera_task2);
        g_cam2TaskCreated = true;
        LOG("[Cam2] camera_task2 created\n");
    }
}*/



void __cdecl hk_ObjCameraCreateTask() {
    LOG("[Split] player camera creation hook fired\n");
    OriginalCreateObjCameraTask();
    createmulticamera1();
    
}

void __fastcall hkDrawSceneTask(TASK* t)
{
    int cam = GetCameraNumberFunction() & 1;
    g_seenCamMask.fetch_or(1 << cam);
    g_drawSceneCount.fetch_add(1);

    oDrawSceneTask(t);
}


__int64* __fastcall hkGetCameraA(bool current)
{
    // Normal behavior unless we're inside our forced split pass
    if (g_renderPassAtomic < 0)
        return oGetCameraA(current);

    EnsureGameAddrsReady();
    if (!g_addrsReady.load(std::memory_order_acquire))
        return oGetCameraA(current);

    // Call original first (this also preserves current/old buffer logic)
    __int64* ret = oGetCameraA(current);
    if (!ret) return ret;

    const int camNum = GetCameraNumberFunction(); // 0/1 normally
    if ((unsigned)camNum >= 2) return ret;        // safety

    // Each camera slot is 16 qwords
    __int64* base = ret - 16LL * camNum;

    const int idx = (g_renderPassAtomic & 1);
    return base + 16LL * idx;
}

char* __fastcall hkGetProjA(char current)
{
    // Normal behavior unless we're inside our forced split pass
    if (g_renderPassAtomic < 0)
        return oGetProjA(current);

    EnsureGameAddrsReady();
    if (!g_addrsReady.load(std::memory_order_acquire))
        return oGetProjA(current);

    // Let the game choose current/old buffer
    char* ret = oGetProjA(current);
    if (!ret) return ret;

    const int camNum = GetCameraNumberFunction(); // 0/1
    if ((unsigned)camNum >= 2) return ret;

    // Recover base pointer: ret = base + stride * camNum
    char* base = ret - (388 * camNum);

    // Override index using our render pass
    const int idx = (g_renderPassAtomic & 1);
    return base + (388 * idx);
}

void __cdecl hkCreateSingleCameraTask() {
    oCreateSingleCameraTask();
    Cam2_OnRoomEnter();
    
}

static void __fastcall hkExecPlayerCommand(__int64 a1)
{
    if (!a1) { oExecPlayerCommand(a1); return; }

    auto** playerSlot = reinterpret_cast<void**>(a1 + 0xAE0);
    auto** padSlot = reinterpret_cast<void**>(a1 + 0xAE8);

    void* playerPtr = playerSlot ? *playerSlot : nullptr;
    void* oldPadPtr = padSlot ? *padSlot : nullptr;

    const bool isP2 = (g_player2Obj != nullptr && playerPtr == (void*)g_player2Obj);

    // Choose pad by which player this command object controls
    if (padSlot)
        *padSlot = reinterpret_cast<void*>(isP2 ? PAD2_ADDR : PAD1_ADDR);

    // Debug: confirm we ever see P2 here
    static int t = 0;
    if ((t++ % 120) == 0)
    {
        printf("[PCMD] this=%p player=%p isP2=%d oldPad=%p newPad=%p\n",
            (void*)a1, playerPtr, isP2 ? 1 : 0,
            oldPadPtr, padSlot ? *padSlot : nullptr);
    }

    oExecPlayerCommand(a1);

    // Restore to avoid leaking pad choice into other uses
    if (padSlot)
        *padSlot = oldPadPtr;
}

static inline YS::PAD* GetPad2()
{
    return reinterpret_cast<YS::PAD*>(moduleBase + 0xBF3208);
}

static void UpdateSticksFromXInput(YS::PAD* pad, const XINPUT_STATE& st)
{
    float lx = NormShort(st.Gamepad.sThumbLX);
    float ly = -NormShort(st.Gamepad.sThumbLY); // invert Y (your fix)
    float rx = NormShort(st.Gamepad.sThumbRX);
    float ry = -NormShort(st.Gamepad.sThumbRY); // invert Y

    // Your layout: Stick[0]=Right, Stick[1]=Left
    FillStick(pad->Stick[0], rx, ry);
    FillStick(pad->Stick[1], lx, ly);
}

static void UpdatePadFromXInput(YS::PAD* pad, int index)
{
    XINPUT_STATE st{};
    if (XInputGetState(index, &st) != ERROR_SUCCESS)
        return;

    ULONGLONG newButtons = MapXInputToHardpad(st);
    UpdateEdges(*pad, newButtons);

    float lx = NormShort(st.Gamepad.sThumbLX);
    float ly = -NormShort(st.Gamepad.sThumbLY);
    float rx = NormShort(st.Gamepad.sThumbRX);
    float ry = -NormShort(st.Gamepad.sThumbRY);

    // Your layout: Stick[0]=right, Stick[1]=left
    FillStick(pad->Stick[0], rx, ry);
    FillStick(pad->Stick[1], lx, ly);
}

static void TryCapturePad1Idle(const XINPUT_STATE& st, const YS::PAD* pad1)
{
    if (gPad1Idle == 0 && st.Gamepad.wButtons == 0 &&
        st.Gamepad.bLeftTrigger < 30 &&
        st.Gamepad.bRightTrigger < 30)
    {
        gPad1Idle = pad1->Button;
        std::printf("[MAP] PAD1 idle mask = 0x%llX\n", gPad1Idle);
    }
}

static void CalibrateTriggersFromPad1(const XINPUT_STATE& st, const YS::PAD* pad1)
{
    if (gPad1Idle == 0) return;

    const WORD w = st.Gamepad.wButtons;
    const bool noButtons = (w == 0);

    if (noButtons && gMaskL2 == 0 && st.Gamepad.bLeftTrigger > 30 && st.Gamepad.bRightTrigger < 30)
    {
        gMaskL2 = pad1->Button & ~gPad1Idle;
        std::printf("[MAP] learned LT(L2) -> gameMask=0x%llX\n", gMaskL2);
    }

    if (noButtons && gMaskR2 == 0 && st.Gamepad.bRightTrigger > 30 && st.Gamepad.bLeftTrigger < 30)
    {
        gMaskR2 = pad1->Button & ~gPad1Idle;
        std::printf("[MAP] learned RT(R2) -> gameMask=0x%llX\n", gMaskR2);
    }
}

static void CalibrateHardFromPad1(const XINPUT_STATE& st, const YS::PAD* pad1)
{
    if (gPad1Idle == 0) return;

    const std::uint64_t hard = MapXInputToHardpad(st);
    if (!OneHardBit(hard)) return;

    for (auto& e : gHardMap)
    {
        if (e.hardBit == hard && !e.learned)
        {
            e.gameMask = pad1->Button & ~gPad1Idle; // delta vs idle
            e.learned = true;
            std::printf("[MAP] learned hard=0x%llX -> game=0x%llX\n",
                (unsigned long long)e.hardBit,
                (unsigned long long)e.gameMask);
            break;
        }
    }
}


/*void __fastcall hkReadPadTask()
{
    oReadPadtask();

    UpdatePadFromXInput((YS::PAD*)PAD1_ADDR, 0); // force PAD1 = controller 1
    UpdatePadFromXInput((YS::PAD*)PAD2_ADDR, 1); // force PAD2 = controller 2
}*/

void __fastcall hkReadPadTask()
{
    // 1) run game input pipeline first
    oReadPadtask();

    auto* pad1 = reinterpret_cast<YS::PAD*>(PAD1_ADDR);
    auto* pad2 = reinterpret_cast<YS::PAD*>(PAD2_ADDR);

    // 2) learn mapping from controller 1 + PAD1 (only needs a few seconds)
    XINPUT_STATE st1{};
    if (XInputGetState(0, &st1) == ERROR_SUCCESS)
    {
        TryCapturePad1Idle(st1, pad1);
        CalibrateHardFromPad1(st1, pad1);

        // Optional: also force PAD1 sticks to be only controller 1
        UpdateSticksFromXInput(pad1, st1);
    }

    // 3) drive PAD2 from controller 2
    XINPUT_STATE st2{};
    if (XInputGetState(1, &st2) == ERROR_SUCCESS)
    {
        // sticks
        UpdateSticksFromXInput(pad2, st2);

        // buttons (game-domain)
        const std::uint64_t hard2 = MapXInputToHardpad(st2);
        const std::uint64_t game2 = HardpadToGameMask(hard2);

        // If idle isn't captured yet, don't inject buttons (prevents nonsense early)
        if (gPad1Idle != 0)
        {
            const std::uint64_t newButtons = gPad1Idle | game2;
            UpdateEdges(*pad2, newButtons);
        }
    }

    // 4) optional: once-per-second debug
    static int dbg = 0;
    if ((dbg++ % 60) == 0)
    {
        std::printf("[PAD2] Button=%016llX Trigger=%016llX\n",
            (unsigned long long)pad2->Button,
            (unsigned long long)pad2->Trigger);
    }
}


static bool IsXInputIdle(const XINPUT_STATE& st)
{
    return st.Gamepad.wButtons == 0
        && st.Gamepad.bLeftTrigger < 5
        && st.Gamepad.bRightTrigger < 5
        && std::abs((int)st.Gamepad.sThumbLX) < 4000
        && std::abs((int)st.Gamepad.sThumbLY) < 4000
        && std::abs((int)st.Gamepad.sThumbRX) < 4000
        && std::abs((int)st.Gamepad.sThumbRY) < 4000;
}

static inline bool IsP2Player(YS::PLAYER* self)
{
    return (g_player2Obj != nullptr && (void*)self == (void*)g_player2Obj);
}


void __fastcall hkPlayerCtor(
    YS::PLAYER* self,
    const OBJENTRY_0* entry,
    int form,
    YS::PAD* pad,
    const Vector4* pos,
    double rot
)
{
    // Save current global Player (P1)
    YS::OBJ* prevGlobalPlayer = (gPlayerSingleton && *gPlayerSingleton) ? *gPlayerSingleton : nullptr;

    // Call original constructor (game sets tons of fields)
    oPlayerCtor(self, entry, form, pad, pos, rot);

    if (!self) return;

    const bool isP2 = IsP2Player(self);

    if (isP2)
    {
        // Force P2 to use PAD2 at the per-player field
        *reinterpret_cast<YS::PAD**>((char*)self + 0xDB8) = (YS::PAD*)PAD2_ADDR;

        // Undo singleton hijack: restore global Player back to P1
        if (gPlayerSingleton)
            *gPlayerSingleton = prevGlobalPlayer;

        std::printf("[PCTOR] P2 self=%p forced Pad=%p restored GlobalPlayer=%p\n",
            (void*)self, (void*)PAD2_ADDR, (void*)prevGlobalPlayer);
    }
    else
    {
        std::printf("[PCTOR] P1 self=%p Pad=%p GlobalPlayer=%p\n",
            (void*)self,
            (void*)*reinterpret_cast<YS::PAD**>((char*)self + 0xDB8),
            gPlayerSingleton ? (void*)*gPlayerSingleton : nullptr);
    }
}


void HookRootMenuFocusFunc()
{
    MH_STATUS status = MH_Initialize();
    if (status != MH_OK && status != MH_ERROR_ALREADY_INITIALIZED)
    {

        LOG("[LevelUpAddEXP] MH_Initialize failed: 0x%X\n", status);
        return;
    }

    uintptr_t vmCtorAddr = moduleBase + 0x433ED0;

    status = MH_CreateHook((LPVOID)vmCtorAddr,
        &hk_rootmenu_focusfunc,
        reinterpret_cast<LPVOID*>(&Originalrootmenu_focusfunc));
    if (status != MH_OK && status != MH_ERROR_ALREADY_CREATED)
    {
        LOG("[HookRootMenuFocusFunc] Failed to create hook: 0x%X\n", status);
        return;
    }

    status = MH_EnableHook((LPVOID)vmCtorAddr);
    if (status != MH_OK && status != MH_ERROR_ENABLED)
    {
        LOG("[HookRootMenuFocusFunc] Failed to enable hook: 0x%X\n", status);
        return;
    }

    //LOG_CHANNEL("ALL", "[LevelUpAddEXP] execute hook installed\n");
   // LOG_CHANNEL("HOOKS", "[LevelUpAddEXP] execute hook installed\n");
}

void HookPlayerCtorFunc()
{
    MH_STATUS status = MH_Initialize();
    if (status != MH_OK && status != MH_ERROR_ALREADY_INITIALIZED)
    {

        LOG("[LevelUpAddEXP] MH_Initialize failed: 0x%X\n", status);
        return;
    }

    uintptr_t vmCtorAddr = moduleBase + 0x3A7A40;

    status = MH_CreateHook((LPVOID)vmCtorAddr,
        &hkPlayerCtor,
        reinterpret_cast<LPVOID*>(&oPlayerCtor));
    if (status != MH_OK && status != MH_ERROR_ALREADY_CREATED)
    {
        LOG("[HookRootMenuFocusFunc] Failed to create hook: 0x%X\n", status);
        return;
    }

    status = MH_EnableHook((LPVOID)vmCtorAddr);
    if (status != MH_OK && status != MH_ERROR_ENABLED)
    {
        LOG("[HookObjCameraCreateTaskFunc] Failed to enable hook: 0x%X\n", status);
        return;
    }

    //LOG_CHANNEL("ALL", "[LevelUpAddEXP] execute hook installed\n");
   // LOG_CHANNEL("HOOKS", "[LevelUpAddEXP] execute hook installed\n");
}


void HookReadPadTaskFunc()
{
    MH_STATUS status = MH_Initialize();
    if (status != MH_OK && status != MH_ERROR_ALREADY_INITIALIZED)
    {

        LOG("[LevelUpAddEXP] MH_Initialize failed: 0x%X\n", status);
        return;
    }

    uintptr_t vmCtorAddr = moduleBase + 0x39BF00;

    status = MH_CreateHook((LPVOID)vmCtorAddr,
        &hkReadPadTask,
        reinterpret_cast<LPVOID*>(&oReadPadtask));
    if (status != MH_OK && status != MH_ERROR_ALREADY_CREATED)
    {
        LOG("[HookRootMenuFocusFunc] Failed to create hook: 0x%X\n", status);
        return;
    }

    status = MH_EnableHook((LPVOID)vmCtorAddr);
    if (status != MH_OK && status != MH_ERROR_ENABLED)
    {
        LOG("[HookObjCameraCreateTaskFunc] Failed to enable hook: 0x%X\n", status);
        return;
    }

    //LOG_CHANNEL("ALL", "[LevelUpAddEXP] execute hook installed\n");
   // LOG_CHANNEL("HOOKS", "[LevelUpAddEXP] execute hook installed\n");
}



void HookExecutePlayerCommandFunc()
{
    MH_STATUS status = MH_Initialize();
    if (status != MH_OK && status != MH_ERROR_ALREADY_INITIALIZED)
    {

        LOG("[LevelUpAddEXP] MH_Initialize failed: 0x%X\n", status);
        return;
    }

    uintptr_t vmCtorAddr = moduleBase + 0x3B2340;

    status = MH_CreateHook((LPVOID)vmCtorAddr,
        &hkExecPlayerCommand,
        reinterpret_cast<LPVOID*>(&oExecPlayerCommand));
    if (status != MH_OK && status != MH_ERROR_ALREADY_CREATED)
    {
        LOG("[HookRootMenuFocusFunc] Failed to create hook: 0x%X\n", status);
        return;
    }

    status = MH_EnableHook((LPVOID)vmCtorAddr);
    if (status != MH_OK && status != MH_ERROR_ENABLED)
    {
        LOG("[HookObjCameraCreateTaskFunc] Failed to enable hook: 0x%X\n", status);
        return;
    }

    //LOG_CHANNEL("ALL", "[LevelUpAddEXP] execute hook installed\n");
   // LOG_CHANNEL("HOOKS", "[LevelUpAddEXP] execute hook installed\n");
}


void HookCreateSingleCameraTaskFunc()
{
    MH_STATUS status = MH_Initialize();
    if (status != MH_OK && status != MH_ERROR_ALREADY_INITIALIZED)
    {

        LOG("[LevelUpAddEXP] MH_Initialize failed: 0x%X\n", status);
        return;
    }

    uintptr_t vmCtorAddr = moduleBase + 0x162F20;

    status = MH_CreateHook((LPVOID)vmCtorAddr,
        &hkCreateSingleCameraTask,
        reinterpret_cast<LPVOID*>(&oCreateSingleCameraTask));
    if (status != MH_OK && status != MH_ERROR_ALREADY_CREATED)
    {
        LOG("[HookRootMenuFocusFunc] Failed to create hook: 0x%X\n", status);
        return;
    }

    status = MH_EnableHook((LPVOID)vmCtorAddr);
    if (status != MH_OK && status != MH_ERROR_ENABLED)
    {
        LOG("[HookObjCameraCreateTaskFunc] Failed to enable hook: 0x%X\n", status);
        return;
    }

    //LOG_CHANNEL("ALL", "[LevelUpAddEXP] execute hook installed\n");
   // LOG_CHANNEL("HOOKS", "[LevelUpAddEXP] execute hook installed\n");
}

void HookSetCameraNumberFunc()
{
    MH_STATUS status = MH_Initialize();
    if (status != MH_OK && status != MH_ERROR_ALREADY_INITIALIZED)
    {

        LOG("[LevelUpAddEXP] MH_Initialize failed: 0x%X\n", status);
        return;
    }

    uintptr_t vmCtorAddr = moduleBase + 0x19DDD0;

    status = MH_CreateHook((LPVOID)vmCtorAddr,
        &hkSetCamNum,
        reinterpret_cast<LPVOID*>(&oSetCamNum));
    if (status != MH_OK && status != MH_ERROR_ALREADY_CREATED)
    {
        LOG("[HookRootMenuFocusFunc] Failed to create hook: 0x%X\n", status);
        return;
    }

    status = MH_EnableHook((LPVOID)vmCtorAddr);
    if (status != MH_OK && status != MH_ERROR_ENABLED)
    {
        LOG("[HookObjCameraCreateTaskFunc] Failed to enable hook: 0x%X\n", status);
        return;
    }

    //LOG_CHANNEL("ALL", "[LevelUpAddEXP] execute hook installed\n");
   // LOG_CHANNEL("HOOKS", "[LevelUpAddEXP] execute hook installed\n");
}

void HookGetProjectionFunc()
{
    MH_STATUS status = MH_Initialize();
    if (status != MH_OK && status != MH_ERROR_ALREADY_INITIALIZED)
    {

        LOG("[LevelUpAddEXP] MH_Initialize failed: 0x%X\n", status);
        return;
    }

    uintptr_t vmCtorAddr = moduleBase + 0x19CFC0;

    status = MH_CreateHook((LPVOID)vmCtorAddr,
        &hkGetProjA,
        reinterpret_cast<LPVOID*>(&oGetProjA));
    if (status != MH_OK && status != MH_ERROR_ALREADY_CREATED)
    {
        LOG("[HookRootMenuFocusFunc] Failed to create hook: 0x%X\n", status);
        return;
    }

    status = MH_EnableHook((LPVOID)vmCtorAddr);
    if (status != MH_OK && status != MH_ERROR_ENABLED)
    {
        LOG("[HookObjCameraCreateTaskFunc] Failed to enable hook: 0x%X\n", status);
        return;
    }

    //LOG_CHANNEL("ALL", "[LevelUpAddEXP] execute hook installed\n");
   // LOG_CHANNEL("HOOKS", "[LevelUpAddEXP] execute hook installed\n");
}

void HookGetCameraFunc()
{
    MH_STATUS status = MH_Initialize();
    if (status != MH_OK && status != MH_ERROR_ALREADY_INITIALIZED)
    {

        LOG("[LevelUpAddEXP] MH_Initialize failed: 0x%X\n", status);
        return;
    }

    uintptr_t vmCtorAddr = moduleBase + 0x19CE40;

    status = MH_CreateHook((LPVOID)vmCtorAddr,
        &hkGetCameraA,
        reinterpret_cast<LPVOID*>(&oGetCameraA));
    if (status != MH_OK && status != MH_ERROR_ALREADY_CREATED)
    {
        LOG("[HookRootMenuFocusFunc] Failed to create hook: 0x%X\n", status);
        return;
    }

    status = MH_EnableHook((LPVOID)vmCtorAddr);
    if (status != MH_OK && status != MH_ERROR_ENABLED)
    {
        LOG("[HookObjCameraCreateTaskFunc] Failed to enable hook: 0x%X\n", status);
        return;
    }

    //LOG_CHANNEL("ALL", "[LevelUpAddEXP] execute hook installed\n");
   // LOG_CHANNEL("HOOKS", "[LevelUpAddEXP] execute hook installed\n");
}

void HookDrawSceneeTaskFunc()
{
    MH_STATUS status = MH_Initialize();
    if (status != MH_OK && status != MH_ERROR_ALREADY_INITIALIZED)
    {

        LOG("[LevelUpAddEXP] MH_Initialize failed: 0x%X\n", status);
        return;
    }

    uintptr_t vmCtorAddr = moduleBase + 0x170450;

    status = MH_CreateHook((LPVOID)vmCtorAddr,
        &hkDrawSceneTask,
        reinterpret_cast<LPVOID*>(&oDrawSceneTask));
    if (status != MH_OK && status != MH_ERROR_ALREADY_CREATED)
    {
        LOG("[HookRootMenuFocusFunc] Failed to create hook: 0x%X\n", status);
        return;
    }

    status = MH_EnableHook((LPVOID)vmCtorAddr);
    if (status != MH_OK && status != MH_ERROR_ENABLED)
    {
        LOG("[HookObjCameraCreateTaskFunc] Failed to enable hook: 0x%X\n", status);
        return;
    }

    //LOG_CHANNEL("ALL", "[LevelUpAddEXP] execute hook installed\n");
   // LOG_CHANNEL("HOOKS", "[LevelUpAddEXP] execute hook installed\n");
}




void HookObjCameraCreateTaskFunc()
{
    MH_STATUS status = MH_Initialize();
    if (status != MH_OK && status != MH_ERROR_ALREADY_INITIALIZED)
    {

        LOG("[LevelUpAddEXP] MH_Initialize failed: 0x%X\n", status);
        return;
    }

    uintptr_t vmCtorAddr = moduleBase + 0x164CB0;

    status = MH_CreateHook((LPVOID)vmCtorAddr,
        &hk_ObjCameraCreateTask,
        reinterpret_cast<LPVOID*>(&OriginalCreateObjCameraTask));
    if (status != MH_OK && status != MH_ERROR_ALREADY_CREATED)
    {
        LOG("[HookRootMenuFocusFunc] Failed to create hook: 0x%X\n", status);
        return;
    }

    status = MH_EnableHook((LPVOID)vmCtorAddr);
    if (status != MH_OK && status != MH_ERROR_ENABLED)
    {
        LOG("[HookObjCameraCreateTaskFunc] Failed to enable hook: 0x%X\n", status);
        return;
    }

    //LOG_CHANNEL("ALL", "[LevelUpAddEXP] execute hook installed\n");
   // LOG_CHANNEL("HOOKS", "[LevelUpAddEXP] execute hook installed\n");
}

void HookLevelUpAddEXP()
{
    MH_STATUS status = MH_Initialize();
    if (status != MH_OK && status != MH_ERROR_ALREADY_INITIALIZED)
    {

        LOG("[LevelUpAddEXP] MH_Initialize failed: 0x%X\n", status);
        return;
    }

    uintptr_t vmCtorAddr = moduleBase + 0x3ECF30;

    status = MH_CreateHook((LPVOID)vmCtorAddr,
        &HookedLevelUpAddEXP,
        reinterpret_cast<LPVOID*>(&OriginalLevelUpAddEXP));
    if (status != MH_OK && status != MH_ERROR_ALREADY_CREATED)
    {
        LOG("[LevelUpAddEXP] Failed to create hook: 0x%X\n", status);
        return;
    }

    status = MH_EnableHook((LPVOID)vmCtorAddr);
    if (status != MH_OK && status != MH_ERROR_ENABLED)
    {
        LOG("[LevelUpAddEXP] Failed to enable hook: 0x%X\n", status);
        return;
    }

    LOG_CHANNEL("ALL", "[LevelUpAddEXP] execute hook installed\n");
    LOG_CHANNEL("HOOKS", "[LevelUpAddEXP] execute hook installed\n");
}


void HookYS_LoaderRead()
{
    MH_STATUS status = MH_Initialize();
    if (status != MH_OK && status != MH_ERROR_ALREADY_INITIALIZED)
    {

        LOG("[YS::LOADER::Read] MH_Initialize failed: 0x%X\n", status);
        return;
    }

    uintptr_t vmCtorAddr = moduleBase + 0x3F9850;

    status = MH_CreateHook((LPVOID)vmCtorAddr,
        &HookedYS_LoaderRead,
        reinterpret_cast<LPVOID*>(&OriginalYS_LoaderRead));
    if (status != MH_OK && status != MH_ERROR_ALREADY_CREATED)
    {
        LOG("[YS::LOADER::Read] Failed to create hook: 0x%X\n", status);
        return;
    }

    status = MH_EnableHook((LPVOID)vmCtorAddr);
    if (status != MH_OK && status != MH_ERROR_ENABLED)
    {
        LOG("[YS::LOADER::Read] Failed to enable hook: 0x%X\n", status);
        return;
    }

    LOG_CHANNEL("ALL", "[YS::LOADER::Read] execute hook installed\n");
    LOG_CHANNEL("HOOKS", "[YS::LOADER::Read] execute hook installed\n");
}

void HookCreateViewMatrix()
{
    MH_STATUS status = MH_Initialize();
    if (status != MH_OK && status != MH_ERROR_ALREADY_INITIALIZED)
    {

        LOG("[ViewMatrix] MH_Initialize failed: 0x%X\n", status);
        return;
    }

    uintptr_t vmCtorAddr = moduleBase + 0x125D30;

    status = MH_CreateHook((LPVOID)vmCtorAddr,
        &hook_CreateViewMatrix,
        reinterpret_cast<LPVOID*>(&OriginalCreateViewMatrix));
    if (status != MH_OK && status != MH_ERROR_ALREADY_CREATED)
    {
        LOG("[ViewMatrix] Failed to create hook: 0x%X\n", status);
        return;
    }

    status = MH_EnableHook((LPVOID)vmCtorAddr);
    if (status != MH_OK && status != MH_ERROR_ENABLED)
    {
        LOG("[ViewMatrix] Failed to enable hook: 0x%X\n", status);
        return;
    }

    LOG_CHANNEL("ALL", "[ViewMatrix] execute hook installed\n");
    LOG_CHANNEL("HOOKS", "[ViewMatrix] execute hook installed\n");
}

void HookProjectionMatrix()
{
    MH_STATUS status = MH_Initialize();
    if (status != MH_OK && status != MH_ERROR_ALREADY_INITIALIZED)
    {

        LOG("[ViewScreenMatrix] MH_Initialize failed: 0x%X\n", status);
        return;
    }

    uintptr_t vmCtorAddr = moduleBase + 0x474440;

    status = MH_CreateHook((LPVOID)vmCtorAddr,
        &hkProjectionMatrix,
        reinterpret_cast<LPVOID*>(&OriginalProjectionMatrix));
    if (status != MH_OK && status != MH_ERROR_ALREADY_CREATED)
    {
        LOG("[ViewScreenMatrix] Failed to create hook: 0x%X\n", status);
        return;
    }

    status = MH_EnableHook((LPVOID)vmCtorAddr);
    if (status != MH_OK && status != MH_ERROR_ENABLED)
    {
        LOG("[ViewScreenMatrix] Failed to enable hook: 0x%X\n", status);
        return;
    }

    LOG_CHANNEL("ALL", "[ViewScreenMatrix] execute hook installed\n");
    LOG_CHANNEL("HOOKS", "[ViewScreenMatrix] execute hook installed\n");
}

void HookSysCallCallBack()
{
    MH_STATUS status = MH_Initialize();
    if (status != MH_OK && status != MH_ERROR_ALREADY_INITIALIZED)
    {

        LOG("[ACT START] MH_Initialize failed: 0x%X\n", status);
        return;
    }

    uintptr_t vmCtorAddr = moduleBase + 0x3E2000;

    status = MH_CreateHook((LPVOID)vmCtorAddr,
        &HookedSysCallCallBack,
        reinterpret_cast<LPVOID*>(&OriginalSysCallCallBack));
    if (status != MH_OK && status != MH_ERROR_ALREADY_CREATED)
    {
        LOG("[ACT START] Failed to create hook: 0x%X\n", status);
        return;
    }

    status = MH_EnableHook((LPVOID)vmCtorAddr);
    if (status != MH_OK && status != MH_ERROR_ENABLED)
    {
        LOG("[ACT START] Failed to enable hook: 0x%X\n", status);
        return;
    }

    LOG_CHANNEL("ALL", "[SYSCALL CALLBACK] execute hook installed\n");
    LOG_CHANNEL("HOOKS", "[SYSCALL CALLBACK] execute hook installed\n");
}

void HookActStart()
{
    MH_STATUS status = MH_Initialize();
    if (status != MH_OK && status != MH_ERROR_ALREADY_INITIALIZED)
    {

        LOG("[ACT START] MH_Initialize failed: 0x%X\n", status);
        return;
    }

    uintptr_t vmCtorAddr = moduleBase + 0x3CAD00;

    status = MH_CreateHook((LPVOID)vmCtorAddr,
        &Hooked_ActStart,
        reinterpret_cast<LPVOID*>(&OriginalActStart));
    if (status != MH_OK && status != MH_ERROR_ALREADY_CREATED)
    {
        LOG("[ACT START] Failed to create hook: 0x%X\n", status);
        return;
    }

    status = MH_EnableHook((LPVOID)vmCtorAddr);
    if (status != MH_OK && status != MH_ERROR_ENABLED)
    {
        LOG("[ACT START] Failed to enable hook: 0x%X\n", status);
        return;
    }

    LOG_CHANNEL("ALL", "[ACT START] execute hook installed\n");
    LOG_CHANNEL("HOOKS", "[ACT START] execute hook installed\n");
}

void HookCalcDamage()
{
    MH_STATUS status = MH_Initialize();
    if (status != MH_OK && status != MH_ERROR_ALREADY_INITIALIZED)
    {

        LOG("[CALC DAMAGE] MH_Initialize failed: 0x%X\n", status);
        return;
    }

    uintptr_t vmCtorAddr = moduleBase + 0x3C12F0;

    status = MH_CreateHook((LPVOID)vmCtorAddr,
        &Hooked_calc_damage,
        reinterpret_cast<LPVOID*>(&orig_calc_damage));
    if (status != MH_OK && status != MH_ERROR_ALREADY_CREATED)
    {
        LOG("[CALC DAMAGE] Failed to create hook: 0x%X\n", status);
        return;
    }

    status = MH_EnableHook((LPVOID)vmCtorAddr);
    if (status != MH_OK && status != MH_ERROR_ENABLED)
    {
        LOG("[CALC DAMAGE] Failed to enable hook: 0x%X\n", status);
        return;
    }

    LOG_CHANNEL("ALL", "[CALC DAMAGE] execute hook installed\n");
    LOG_CHANNEL("HOOKS", "[CALC DAMAGE] execute hook installed\n");
}


void HookPlayerMovement()
{
    MH_STATUS status = MH_Initialize();
    if (status != MH_OK && status != MH_ERROR_ALREADY_INITIALIZED)
    {

        LOG("[PLAYER MOVEMENT] MH_Initialize failed: 0x%X\n", status);
        return;
    }

    uintptr_t vmCtorAddr = moduleBase + 0x3A8FA0;

    status = MH_CreateHook((LPVOID)vmCtorAddr,
        &HookedPlayerMovement,
        reinterpret_cast<LPVOID*>(&OriginalPlayerMovement));
    if (status != MH_OK && status != MH_ERROR_ALREADY_CREATED)
    {
        LOG("[PLAYER MOVEMENT] Failed to create hook: 0x%X\n", status);
        return;
    }

    status = MH_EnableHook((LPVOID)vmCtorAddr);
    if (status != MH_OK && status != MH_ERROR_ENABLED)
    {
        LOG("[PLAYER MOVEMENT] Failed to enable hook: 0x%X\n", status);
        return;
    }

    LOG_CHANNEL("ALL", "[PLAYER MOVEMENT] execute hook installed\n");
    LOG_CHANNEL("HOOKS", "[PLAYER MOVEMENT] execute hook installed\n");
}



void HookVMExec()
{
    MH_STATUS status = MH_Initialize();
    if (status != MH_OK && status != MH_ERROR_ALREADY_INITIALIZED)
    {

        LOG("[VM TRACE] MH_Initialize failed: 0x%X\n", status);
        return;
    }

    uintptr_t vmCtorAddr = 0x1403E1C80;

    status = MH_CreateHook((LPVOID)vmCtorAddr,
        &HookedVMEXEC,
        reinterpret_cast<LPVOID*>(&oVMExec));
    if (status != MH_OK && status != MH_ERROR_ALREADY_CREATED)
    {
        LOG("[VM TRACE] Failed to create VM hook: 0x%X\n", status);
        return;
    }

    status = MH_EnableHook((LPVOID)vmCtorAddr);
    if (status != MH_OK && status != MH_ERROR_ENABLED)
    {
        LOG("[VM TRACE] Failed to enable VM hook: 0x%X\n", status);
        return;
    }

    LOG_CHANNEL("ALL", "[VM EXEC] VM execute hook installed\n");
    LOG_CHANNEL("HOOKS", "[VM EXEC] VM execute hook installed\n");
}




void HookVMConstructor()
{
    MH_STATUS status = MH_Initialize();
    if (status != MH_OK && status != MH_ERROR_ALREADY_INITIALIZED)
    {

        LOG("[VM TRACE] MH_Initialize failed: 0x%X\n", status);
        return;
    }

    uintptr_t vmCtorAddr = 0x1403E1240; // make sure this is correct for your build

    status = MH_CreateHook((LPVOID)vmCtorAddr,
        &HookedVMConstructor,
        reinterpret_cast<LPVOID*>(&OriginalVMConstructor));
    if (status != MH_OK && status != MH_ERROR_ALREADY_CREATED)
    {
        LOG("[VM TRACE] Failed to create VM hook: 0x%X\n", status);
        return;
    }

    status = MH_EnableHook((LPVOID)vmCtorAddr);
    if (status != MH_OK && status != MH_ERROR_ENABLED)
    {
        LOG("[VM TRACE] Failed to enable VM hook: 0x%X\n", status);
        return;
    }

    LOG_CHANNEL("ALL", "[VM TRACE] VM constructor hook installed\n");
    LOG_CHANNEL("HOOKS", "[VM TRACE] VM constructor hook installed\n");
}



// Operator overloads for ImVec2
inline ImVec2 operator+(const ImVec2& lhs, const ImVec2& rhs) {
    return ImVec2(lhs.x + rhs.x, lhs.y + rhs.y);
}

inline ImVec2 operator-(const ImVec2& lhs, const ImVec2& rhs) {
    return ImVec2(lhs.x - rhs.x, lhs.y - rhs.y);
}

inline ImVec2 operator*(const ImVec2& lhs, float rhs) {
    return ImVec2(lhs.x * rhs, lhs.y * rhs);
}

inline ImVec2 operator/(const ImVec2& lhs, float rhs) {
    return ImVec2(lhs.x / rhs, lhs.y / rhs);
}

enum BlinkPhase {
    Visible,
    Invisible
};


std::atomic<float> disco_time(0.0f);

struct Tile {
    ImVec2 verts[4];
    ImU32 color;
};

static YS::OBJ* cachedSelectedObject = nullptr;
static Entity* cachedSelectedEntity = nullptr;
static float timeSinceLastUpdate = 0.0f;
const float updateInterval = 0.1f; // update every 0.1 seconds (10 times per second)

static Vector4 lastCursorPos = { 0,0,0,0 };
static Vector4 lastCameraPos = { 0,0,0,0 };
static bool cacheValid = false;

// Cache screen positions for gizmo axis points (x1,x2,y1,y2,z1,z2)
static XMVECTOR cachedScreenPoints[6];

static XMMATRIX lastViewMatrix = XMMatrixIdentity();

static bool rotationModeActive = false;
static bool arrowGrabbed = false;

static Vector4 selectedEntityPos = { 0,0,0,1 };
static Vector4 rotationHandlePos = { 0,0,0,1 };


std::vector<Tile> discoTiles;

static uintptr_t DestroyObjFunctionAddress = 0;
static uintptr_t RealDestroyobjFunctionAddress = 0;
static FunctionCalls::tDestroyObj DestroyObjFunction = nullptr;
static FunctionCalls::tDestroyObj RealDestroyobjFunction = nullptr;
static bool destroyObjInitialized = false;

YS::OBJ* selectedObject = nullptr;
Entity* selectedEntity = nullptr;

int frame_counter = 0;

std::vector<uint64_t> HighBitsASLR = { 0x7FF7BC000000, 0x7FF7BE000000 }; // high bits for ASLR
std::vector<uint64_t> HighBitsNoASLR = { 0x140000000, 0x142000000 }; // high bits for non-ASLR

SAVERAM::SAVERAM* g_Saveram = nullptr;
bool showSaveramOverlay = false;

YS::ENEMY* g_enemyA = nullptr;
YS::ENEMY* g_enemyB = nullptr;
bool g_fightActive = false;

float g_currentFormTime = 0.0f;
float g_currentFormMaxTime = 0.0f;

using Microsoft::WRL::ComPtr;

bool ShowBuild = true;



static bool freeCameraCreated = false;

YS::OBJ* cachedEntityPointer = 0; // Cached entity pointer for optimization
Entity* cachedEntity = nullptr;    // Cached entity

bool g_waitingForEnemies = false;
static uint32_t g_spawnFrame = 0;
static bool g_spawnRequested = false;

Microsoft::WRL::ComPtr<ID3D12Resource> g_debugVertexBuffer;
D3D12_VERTEX_BUFFER_VIEW g_debugVertexBufferView;
UINT8* g_debugVertexDataBegin;
UINT8* g_debugVertexDataEnd;
Microsoft::WRL::ComPtr<ID3D12PipelineState> g_debugPipelineState;
Microsoft::WRL::ComPtr<ID3D12RootSignature> g_debugRootSignature;

Microsoft::WRL::ComPtr<ID3DBlob> g_debugVertexShader;
Microsoft::WRL::ComPtr<ID3DBlob> g_debugPixelShader;

Microsoft::WRL::ComPtr<ID3D12CommandAllocator> g_pd3dCommandAllocator;

//Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> g_pd3dCommandList;

Microsoft::WRL::ComPtr<ID3D12Resource> g_debugIndexBuffer;

std::unordered_map<Entity*, int> previousHPMap;

ComPtr<ID3D12CommandAllocator> g_commandAllocator;  // Allocator for recording commands
ComPtr<ID3D12GraphicsCommandList> g_commandList;     // Command list for recording drawing commands
ComPtr<IDXGISwapChain3> g_swapChain;       // Swap chain for presenting frames
ComPtr<ID3D12CommandQueue> g_commandQueue;   // Command queue for executing GPU commands



uintptr_t NopCutsceneCamera = 0;

uintptr_t viewMatrixAddress = 0;

uintptr_t CameraStruct = moduleBase + 0x718C68;

uintptr_t RealViewMatrixAddress = moduleBase + 0x8AED00;

float Matrix[16];
std::vector<DirectX::XMVECTOR> entityPositions;
std::vector<ImVec2> screenPositions;



// Camera variables
XMVECTOR cameraPosition = XMVectorSet(0.0f, 0.0f, -10.0f, 1.0f);  // Initial camera position
XMVECTOR cameraTarget = XMVectorSet(0.0f, 0.0f, 1.0f, 1.0f);     // Target point the camera is looking at
XMVECTOR cameraUp = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);         // Up vector
float cameraSpeed = 0.05f;  // Speed of camera movement

POINT prevMousePos = { 0, 0 };  // Previous mouse position, initially (0, 0)

float Freecam_sm_vec = 0.0f;
float Freecam_sm_pos = 0.0f;

float LevelEditorcamSpeed = 136.0f;

bool hasEnteredArea = false;
bool isInArea = false;



Vector4 LevelEditorcamPos = {};
Vector4 LevelEditorTargetPos = {};

std::chrono::time_point<std::chrono::high_resolution_clock> lastTime;
bool firstFrame = true;


std::vector<unsigned char> originalCutsceneCameraBytes;


static bool g_EventPlayerOpened = false;

const ImU32 RedColor = IM_COL32(255, 0, 0, 255);
const ImU32 LightGreen = IM_COL32(0, 255, 0, 159);
const ImU32 Green = IM_COL32(0, 255, 0, 255);
const ImU32 LightBlue = IM_COL32(0, 0, 255, 159);
const ImU32 Blue = IM_COL32(0, 0, 255, 255);
const ImU32 LightYellow = IM_COL32(255, 255, 0, 159);
const ImU32 Yellow = IM_COL32(255, 255, 0, 255);
const ImU32 LightCyan = IM_COL32(0, 255, 255, 159);
const ImU32 Cyan = IM_COL32(0, 255, 255, 255);
const ImU32 LightMagenta = IM_COL32(255, 0, 255, 159);
const ImU32 Magenta = IM_COL32(255, 0, 255, 255);
const ImU32 LightWhite = IM_COL32(255, 255, 255, 159);
const ImU32 White = IM_COL32(255, 255, 255, 255);
const ImU32 Gold = IM_COL32(255, 215, 0, 255);
const ImU32 Black = IM_COL32(0, 0, 0, 255);

const ImU32 BlackHalfOP = IM_COL32(0, 0, 0, 127);

int entityCount = 0;

ImFont* mainFont;

ImFont* secondFont;

ImFont* HugeSizeArialFont;

const UINT numVertices = 3;
const UINT numIndices = 3;

std::map<int, std::vector<int>> entityBoneIDs;

  // Store all active damage numbers
const float DAMAGE_TEXT_LIFETIME = 1.5f;  // Time in seconds for the damage text to stay on screen


ComPtr<ID3D12Resource> g_texture;
ComPtr<ID3D12DescriptorHeap> g_srvHeap;
static bool g_textureInitialized = false;
static ImTextureID myImguiTexID = nullptr;


static ID3D12Resource* g_MyTexture = nullptr;
static D3D12_GPU_DESCRIPTOR_HANDLE g_MyTextureSrvGpuHandle = {};
static int g_MyImageWidth = 0;
static int g_MyImageHeight = 0;

struct SimpleDescriptorAllocator {
    D3D12_CPU_DESCRIPTOR_HANDLE cpu_start;
    D3D12_GPU_DESCRIPTOR_HANDLE gpu_start;
    UINT handle_size;
    UINT current_index = 0;
    UINT capacity = 0;

    void Init(ID3D12Device* device, ID3D12DescriptorHeap* heap, UINT num_descriptors) {
        cpu_start = heap->GetCPUDescriptorHandleForHeapStart();
        gpu_start = heap->GetGPUDescriptorHandleForHeapStart();
        handle_size = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
        capacity = num_descriptors;
    }

    void Alloc(D3D12_CPU_DESCRIPTOR_HANDLE* cpu_out, D3D12_GPU_DESCRIPTOR_HANDLE* gpu_out) {
        IM_ASSERT(current_index < capacity);
        *cpu_out = { cpu_start.ptr + SIZE_T(current_index) * handle_size };
        *gpu_out = { gpu_start.ptr + SIZE_T(current_index) * handle_size };
        current_index++;
    }
};


static int const                    NUM_BACK_BUFFERS = 3;
static IDXGIFactory4* g_dxgiFactory = NULL;
static ID3D12Device* g_pd3dDevice = NULL;
static ID3D12DescriptorHeap* g_pd3dRtvDescHeap = NULL;
static ID3D12DescriptorHeap* g_pd3dSrvDescHeap = NULL;
static ID3D12CommandQueue* g_pd3dCommandQueue = NULL;
ID3D12CommandQueue* cmdQueue = g_pd3dCommandQueue;
static ID3D12GraphicsCommandList* g_pd3dCommandList = NULL;
static Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> g_pd3dCommandList1;
static IDXGISwapChain3* g_pSwapChain = NULL;
static ID3D12CommandAllocator* g_commandAllocators[NUM_BACK_BUFFERS] = {};
static ID3D12Resource* g_mainRenderTargetResource[NUM_BACK_BUFFERS] = {};
static D3D12_CPU_DESCRIPTOR_HANDLE  g_mainRenderTargetDescriptor[NUM_BACK_BUFFERS] = {};
static ComPtr<ID3D12Resource> g_pd3dVertexBuffer;
SimpleDescriptorAllocator g_pd3dSrvDescHeapAlloc;
static ID3D12Fence* g_fence = nullptr;
static HANDLE g_fenceEvent = nullptr;
static UINT64 g_fenceValue = 0;
static UINT64 g_frameFenceValues[NUM_BACK_BUFFERS] = {};





static inline bool ShouldSplitDx12Only()
{
    if (!g_forceSplit.load(std::memory_order_relaxed)) return false;
    if (g_inImGuiPass.load(std::memory_order_relaxed)) return false;
    if (!g_inMainBackbufferPass.load(std::memory_order_relaxed)) return false;
    return true;
}

static inline void BuildVerticalSplit(
    const D3D12_VIEWPORT& full,
    int cam01,
    D3D12_VIEWPORT& outVp,
    D3D12_RECT& outSc)
{
    const float halfW = full.Width * 0.5f;

    outVp = full;
    outVp.TopLeftX = full.TopLeftX + (cam01 ? halfW : 0.0f);
    outVp.Width = halfW;

    // Scissor expects integer pixel coords
    const LONG x0 = (LONG)outVp.TopLeftX;
    const LONG y0 = (LONG)outVp.TopLeftY;
    const LONG x1 = (LONG)(outVp.TopLeftX + outVp.Width);
    const LONG y1 = (LONG)(outVp.TopLeftY + outVp.Height);

    outSc.left = x0;
    outSc.top = y0;
    outSc.right = x1;
    outSc.bottom = y1;
}


static inline void TopHalfFromViewport(const D3D12_VIEWPORT& full, D3D12_VIEWPORT& outVp, D3D12_RECT& outSc)
{
    outVp = full;
    outVp.Height = full.Height * 0.5f;

    outSc.left = (LONG)outVp.TopLeftX;
    outSc.right = (LONG)(outVp.TopLeftX + outVp.Width);
    outSc.top = (LONG)outVp.TopLeftY;
    outSc.bottom = (LONG)(outVp.TopLeftY + outVp.Height);
}

static void BuildVerticalSplitFromSwapchain(int cam, D3D12_VIEWPORT& vp, D3D12_RECT& sc)
{
    DXGI_SWAP_CHAIN_DESC desc{};
    g_pSwapChain->GetDesc(&desc);

    const float W = (float)desc.BufferDesc.Width;
    const float H = (float)desc.BufferDesc.Height;

    vp.TopLeftY = 0.0f;
    vp.Height = H;
    vp.MinDepth = 0.0f;
    vp.MaxDepth = 1.0f;

    vp.Width = W * 0.5f;
    vp.TopLeftX = (cam == 0) ? 0.0f : vp.Width;

    sc.top = 0;
    sc.bottom = (LONG)desc.BufferDesc.Height;
    sc.left = (cam == 0) ? 0 : (LONG)(desc.BufferDesc.Width / 2);
    sc.right = (cam == 0) ? (LONG)(desc.BufferDesc.Width / 2) : (LONG)desc.BufferDesc.Width;
}

static void ApplySplitState(ID3D12GraphicsCommandList* cmd)
{
    if (g_inApplySplit) return;
    g_inApplySplit = true;

    if (!g_splitEnabled.load(std::memory_order_relaxed) ||
        !g_inMainBackbufferPass.load(std::memory_order_relaxed) ||
        !g_pSwapChain || !GetCameraNumberFunction)
    {
        g_inApplySplit = false;
        return;
    }

    const int cam = g_camNumAtomic.load(std::memory_order_relaxed) & 1;

    D3D12_VIEWPORT vp{};
    D3D12_RECT sc{};

    // use swapchain size (robust even if engine never calls RSSetViewports)
    BuildVerticalSplitFromSwapchain(cam, vp, sc);

    // IMPORTANT: call the ORIGINAL function pointers, not cmd->RSSetViewports
    if (oRSSetViewports)     oRSSetViewports(cmd, 1, &vp);
    if (oRSSetScissorRects)  oRSSetScissorRects(cmd, 1, &sc);

    g_inApplySplit = false;
}

static inline void BuildHalfVP_SC(int cam, D3D12_VIEWPORT& vp, D3D12_RECT& sc)
{
    DXGI_SWAP_CHAIN_DESC sd{};
    g_pSwapChain->GetDesc(&sd);

    const float W = (float)sd.BufferDesc.Width;
    const float H = (float)sd.BufferDesc.Height;
    const float halfW = W * 0.5f;

    vp.TopLeftY = 0.0f;
    vp.Width = halfW;
    vp.Height = H;
    vp.MinDepth = 0.0f;
    vp.MaxDepth = 1.0f;
    vp.TopLeftX = (cam == 0) ? 0.0f : halfW;

    sc.top = 0;
    sc.bottom = (LONG)sd.BufferDesc.Height;
    sc.left = (cam == 0) ? 0 : (LONG)(sd.BufferDesc.Width / 2);
    sc.right = (cam == 0) ? (LONG)(sd.BufferDesc.Width / 2) : (LONG)sd.BufferDesc.Width;
}

static inline void ForceSplitState(ID3D12GraphicsCommandList* cmd)
{
    if (!g_splitEnabled.load(std::memory_order_relaxed)) return;
    if (!g_pSwapChain || !GetCameraNumberFunction) return;

    // IMPORTANT: temporarily ignore your g_inMainBackbufferPass gate for debugging
    // If this fixes it, your backbuffer-pass detection is wrong.
    // if (!g_inMainBackbufferPass.load(std::memory_order_relaxed)) return;

    const int cam = GetCameraNumberFunction() & 1;

    D3D12_VIEWPORT vp{};
    D3D12_RECT sc{};
    BuildHalfVP_SC(cam, vp, sc);

    oRSSetViewports(cmd, 1, &vp);
    oRSSetScissorRects(cmd, 1, &sc);
}

static void ApplySplitForCurrentCamera(ID3D12GraphicsCommandList* cmd)
{
    if (g_inApplySplit) return;
    g_inApplySplit = true;

    // Need a “full viewport” baseline. Either cache it once from any RSSetViewports,
    // or fall back to swapchain size if you want.
    D3D12_VIEWPORT full{};

    if (g_haveCachedFullVp.load(std::memory_order_relaxed)) {
        full = g_cachedFullVp;
    }
    else {
        DXGI_SWAP_CHAIN_DESC sd{};
        g_pSwapChain->GetDesc(&sd);
        full.TopLeftX = 0;
        full.TopLeftY = 0;
        full.Width = (float)sd.BufferDesc.Width;
        full.Height = (float)sd.BufferDesc.Height;
        full.MinDepth = 0.0f;
        full.MaxDepth = 1.0f;
    }

    const int cam = g_camNumAtomic.load(std::memory_order_relaxed) & 1;

    D3D12_VIEWPORT vp = full;
    vp.Width *= 0.5f;
    vp.TopLeftX = full.TopLeftX + (cam ? vp.Width : 0.0f);

    D3D12_RECT sc{};
    sc.top = (LONG)full.TopLeftY;
    sc.bottom = (LONG)(full.TopLeftY + full.Height);
    const LONG half = (LONG)(full.Width * 0.5f);
    sc.left = (LONG)full.TopLeftX + (cam ? half : 0);
    sc.right = (LONG)full.TopLeftX + (cam ? (LONG)full.Width : half);

    // IMPORTANT: call ORIGINAL methods (oRS...), not cmd->RS... (avoids vtable recursion)
    if (oRSSetViewports)    oRSSetViewports(cmd, 1, &vp);
    if (oRSSetScissorRects) oRSSetScissorRects(cmd, 1, &sc);

    g_inApplySplit = false;
}


void STDMETHODCALLTYPE hkRSSetViewports(ID3D12GraphicsCommandList* cmd, UINT n, const D3D12_VIEWPORT* vps)
{
    if (vps && n > 0) {
        g_cachedFullVp = vps[0];
        g_haveCachedFullVp.store(true, std::memory_order_relaxed);
    }
    return oRSSetViewports(cmd, n, vps);
}


void STDMETHODCALLTYPE hkOMSetRenderTargets(
    ID3D12GraphicsCommandList* cmd,
    UINT NumRTVs,
    const D3D12_CPU_DESCRIPTOR_HANDLE* pRTVs,
    BOOL SingleHandleToDescriptorRange,
    const D3D12_CPU_DESCRIPTOR_HANDLE* pDSV)
{
    static uint64_t s = 0;
    if ((++s % 200) == 0) // low throttle for now
    {
        LOG("[DX12] OMSetRT cmd=%p NumRTVs=%u pRTVs=%p Single=%d pDSV=%p\n",
            cmd, NumRTVs, (void*)pRTVs, (int)SingleHandleToDescriptorRange, (void*)pDSV);

    }

    return oOMSetRenderTargets(cmd, NumRTVs, pRTVs, SingleHandleToDescriptorRange, pDSV);
}





void STDMETHODCALLTYPE hkRSSetScissorRects(ID3D12GraphicsCommandList* cmd, UINT n, const D3D12_RECT* rects)
{
    //if (g_inApplySplit) return oRSSetViewports(cmd, n, vps); // or scissor equivalent

    if (!g_splitEnabled.load(std::memory_order_relaxed) ||
        !g_inMainBackbufferPass.load(std::memory_order_relaxed))
    {
        return oRSSetScissorRects(cmd, n, rects);
    }

    // Ignore; viewport hook forces correct scissor for split
    return;
}



static void STDMETHODCALLTYPE hkResourceBarrier(ID3D12GraphicsCommandList* cmd, UINT num, const D3D12_RESOURCE_BARRIER* barriers)
{
    if (barriers) {
        for (UINT i = 0; i < num; ++i) {
            const auto& b = barriers[i];
            if (b.Type != D3D12_RESOURCE_BARRIER_TYPE_TRANSITION) continue;

            ID3D12Resource* r = b.Transition.pResource;
            if (!r) continue;

            for (int k = 0; k < NUM_BACK_BUFFERS; ++k) {
                if (g_mainRenderTargetResource[k] == r) {
                    if (b.Transition.StateAfter == D3D12_RESOURCE_STATE_RENDER_TARGET)
                        g_inMainBackbufferPass.store(true, std::memory_order_relaxed);
                    if (b.Transition.StateAfter == D3D12_RESOURCE_STATE_PRESENT)
                        g_inMainBackbufferPass.store(false, std::memory_order_relaxed);
                    break;
                }
            }
        }
    }
    return oResourceBarrier(cmd, num, barriers);
}



static inline D3D12_RECT MakeHalfRectFromSwapchain(int cam)
{
    DXGI_SWAP_CHAIN_DESC sd{};
    g_pSwapChain->GetDesc(&sd);

    const LONG W = (LONG)sd.BufferDesc.Width;
    const LONG H = (LONG)sd.BufferDesc.Height;
    const LONG half = W / 2;

    D3D12_RECT r{};
    r.top = 0;
    r.bottom = H;
    if (cam == 0) { r.left = 0;    r.right = half; }
    else { r.left = half; r.right = W; }
    return r;
}

using tClearRenderTargetView = void (STDMETHODCALLTYPE*)(
    ID3D12GraphicsCommandList*,
    D3D12_CPU_DESCRIPTOR_HANDLE,
    const FLOAT[4],
    UINT,
    const D3D12_RECT*);

static tClearRenderTargetView oClearRenderTargetView = nullptr;

void STDMETHODCALLTYPE hkClearRenderTargetView(
    ID3D12GraphicsCommandList* cmd,
    D3D12_CPU_DESCRIPTOR_HANDLE rtv,
    const FLOAT color[4],
    UINT numRects,
    const D3D12_RECT* rects)
{
    if (g_splitEnabled.load(std::memory_order_relaxed) &&
        g_inMainBackbufferPass.load(std::memory_order_relaxed) &&
        g_pSwapChain && GetCameraNumberFunction)
    {
        const int cam = g_camNumAtomic.load(std::memory_order_relaxed) & 1;
        const D3D12_RECT r = MakeHalfRectFromSwapchain(cam);
        return oClearRenderTargetView(cmd, rtv, color, 1, &r);
    }

    return oClearRenderTargetView(cmd, rtv, color, numRects, rects);
}



using tClearDepthStencilView = void (STDMETHODCALLTYPE*)(
    ID3D12GraphicsCommandList*,
    D3D12_CPU_DESCRIPTOR_HANDLE,
    D3D12_CLEAR_FLAGS,
    FLOAT,
    UINT8,
    UINT,
    const D3D12_RECT*);

static tClearDepthStencilView oClearDepthStencilView = nullptr;

void STDMETHODCALLTYPE hkClearDepthStencilView(
    ID3D12GraphicsCommandList* cmd,
    D3D12_CPU_DESCRIPTOR_HANDLE dsv,
    D3D12_CLEAR_FLAGS flags,
    FLOAT depth,
    UINT8 stencil,
    UINT numRects,
    const D3D12_RECT* rects)
{
    if (g_splitEnabled.load(std::memory_order_relaxed) &&
        g_inMainBackbufferPass.load(std::memory_order_relaxed) &&
        g_pSwapChain && GetCameraNumberFunction)
    {
        const int cam = g_camNumAtomic.load(std::memory_order_relaxed) & 1;
        const D3D12_RECT r = MakeHalfRectFromSwapchain(cam);
        return oClearDepthStencilView(cmd, dsv, flags, depth, stencil, 1, &r);
    }

    return oClearDepthStencilView(cmd, dsv, flags, depth, stencil, numRects, rects);
}

void STDMETHODCALLTYPE hkDrawInstanced(
    ID3D12GraphicsCommandList* cmd,
    UINT vtxPerInst, UINT instCount,
    UINT startVtx, UINT startInst)
{
    if (g_splitEnabled.load(std::memory_order_relaxed) &&
        g_inMainBackbufferPass.load(std::memory_order_relaxed))
    {
        ApplySplitForCurrentCamera(cmd);
    }
    return oDrawInstanced(cmd, vtxPerInst, instCount, startVtx, startInst);
}

void STDMETHODCALLTYPE hkDrawIndexedInstanced(
    ID3D12GraphicsCommandList* cmd,
    UINT IndexCountPerInstance,
    UINT InstanceCount,
    UINT StartIndexLocation,
    INT  BaseVertexLocation,
    UINT StartInstanceLocation)
{
    static uint64_t s = 0;
    if ((++s % 2000) == 0)
    {
        const int camGlobal = g_camNumAtomic.load(std::memory_order_relaxed) & 1;
        const bool inBB = g_inMainBackbufferPass.load(std::memory_order_relaxed);
        const uint64_t rtv = g_currentRTVPtr.load(std::memory_order_relaxed); // from OMSetRT hook

        LOG("[DX12] DI cmd=%p cam=%d inBB=%d rtv=%llx idx=%u inst=%u\n",
            cmd, camGlobal, (int)inBB, (unsigned long long)rtv,
            IndexCountPerInstance, InstanceCount);
    }

    // keep your split logic AFTER you confirm what inBB/rtv look like
    return oDrawIndexedInstanced(cmd, IndexCountPerInstance, InstanceCount,
        StartIndexLocation, BaseVertexLocation, StartInstanceLocation);
}







static void DumpDx12SplitStatsOncePerFrame()
{
    uint32_t di0 = g_drawIdxCount[0].exchange(0);
    uint32_t di1 = g_drawIdxCount[1].exchange(0);
    uint32_t d0 = g_drawCount[0].exchange(0);
    uint32_t d1 = g_drawCount[1].exchange(0);

    uint32_t vp0 = g_setVpCount[0].exchange(0);
    uint32_t vp1 = g_setVpCount[1].exchange(0);
    uint32_t sc0 = g_setScCount[0].exchange(0);
    uint32_t sc1 = g_setScCount[1].exchange(0);

    uint32_t cr0 = g_clearRTV[0].exchange(0);
    uint32_t cr1 = g_clearRTV[1].exchange(0);
    uint32_t cd0 = g_clearDSV[0].exchange(0);
    uint32_t cd1 = g_clearDSV[1].exchange(0);

    // print every 60 frames to avoid spam
    if ((++g_frameNo % 60) == 0) {
        LOG("[DX12] mainpass draws: cam0 DI=%u D=%u | cam1 DI=%u D=%u\n", di0, d0, di1, d1);
        LOG("[DX12] RSSetViewports: cam0=%u cam1=%u | Scissor: cam0=%u cam1=%u\n", vp0, vp1, sc0, sc1);
        LOG("[DX12] Clears: RTV cam0=%u cam1=%u | DSV cam0=%u cam1=%u\n", cr0, cr1, cd0, cd1);
    }
}


/*uintptr_t cachedGetPlayerAddress = 0;
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

FunctionCalls::tGetPlayer GetPlayerFunction = 0;
FunctionCalls::tEachObj EachOBJFunction = 0;
FunctionCalls::tGetObjEntryName GetOBJEntryNameFunction = 0;

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
FunctionCalls::tGetProjection GetProjectionMatrixFunction = 0;*/

bool LoadTextureFromMemory(const void* data, size_t data_size, ID3D12Device* d3d_device, D3D12_CPU_DESCRIPTOR_HANDLE srv_cpu_handle, ID3D12Resource** out_tex_resource, int* out_width, int* out_height)
{
    // Load from disk into a raw RGBA buffer
    int image_width = 0;
    int image_height = 0;
    unsigned char* image_data = stbi_load_from_memory((const unsigned char*)data, (int)data_size, &image_width, &image_height, NULL, 4);
    if (image_data == NULL)
        return false;

    // Create texture resource
    D3D12_HEAP_PROPERTIES props;
    memset(&props, 0, sizeof(D3D12_HEAP_PROPERTIES));
    props.Type = D3D12_HEAP_TYPE_DEFAULT;
    props.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
    props.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;

    D3D12_RESOURCE_DESC desc;
    ZeroMemory(&desc, sizeof(desc));
    desc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
    desc.Alignment = 0;
    desc.Width = image_width;
    desc.Height = image_height;
    desc.DepthOrArraySize = 1;
    desc.MipLevels = 1;
    desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    desc.SampleDesc.Count = 1;
    desc.SampleDesc.Quality = 0;
    desc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
    desc.Flags = D3D12_RESOURCE_FLAG_NONE;

    ID3D12Resource* pTexture = NULL;
    d3d_device->CreateCommittedResource(&props, D3D12_HEAP_FLAG_NONE, &desc,
        D3D12_RESOURCE_STATE_COPY_DEST, NULL, IID_PPV_ARGS(&pTexture));

    // Create a temporary upload resource to move the data in
    UINT uploadPitch = (image_width * 4 + D3D12_TEXTURE_DATA_PITCH_ALIGNMENT - 1u) & ~(D3D12_TEXTURE_DATA_PITCH_ALIGNMENT - 1u);
    UINT uploadSize = image_height * uploadPitch;
    desc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
    desc.Alignment = 0;
    desc.Width = uploadSize;
    desc.Height = 1;
    desc.DepthOrArraySize = 1;
    desc.MipLevels = 1;
    desc.Format = DXGI_FORMAT_UNKNOWN;
    desc.SampleDesc.Count = 1;
    desc.SampleDesc.Quality = 0;
    desc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
    desc.Flags = D3D12_RESOURCE_FLAG_NONE;

    props.Type = D3D12_HEAP_TYPE_UPLOAD;
    props.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
    props.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;

    ID3D12Resource* uploadBuffer = NULL;
    HRESULT hr = d3d_device->CreateCommittedResource(&props, D3D12_HEAP_FLAG_NONE, &desc,
        D3D12_RESOURCE_STATE_GENERIC_READ, NULL, IID_PPV_ARGS(&uploadBuffer));
    IM_ASSERT(SUCCEEDED(hr));

    // Write pixels into the upload resource
    void* mapped = NULL;
    D3D12_RANGE range = { 0, uploadSize };
    hr = uploadBuffer->Map(0, &range, &mapped);
    IM_ASSERT(SUCCEEDED(hr));
    for (int y = 0; y < image_height; y++)
        memcpy((void*)((uintptr_t)mapped + y * uploadPitch), image_data + y * image_width * 4, image_width * 4);
    uploadBuffer->Unmap(0, &range);

    // Copy the upload resource content into the real resource
    D3D12_TEXTURE_COPY_LOCATION srcLocation = {};
    srcLocation.pResource = uploadBuffer;
    srcLocation.Type = D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT;
    srcLocation.PlacedFootprint.Footprint.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    srcLocation.PlacedFootprint.Footprint.Width = image_width;
    srcLocation.PlacedFootprint.Footprint.Height = image_height;
    srcLocation.PlacedFootprint.Footprint.Depth = 1;
    srcLocation.PlacedFootprint.Footprint.RowPitch = uploadPitch;

    D3D12_TEXTURE_COPY_LOCATION dstLocation = {};
    dstLocation.pResource = pTexture;
    dstLocation.Type = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;
    dstLocation.SubresourceIndex = 0;

    D3D12_RESOURCE_BARRIER barrier = {};
    barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
    barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
    barrier.Transition.pResource = pTexture;
    barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
    barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;
    barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;

    // Create a temporary command queue to do the copy with
    ID3D12Fence* fence = NULL;
    hr = d3d_device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence));
    IM_ASSERT(SUCCEEDED(hr));

    HANDLE event = CreateEvent(0, 0, 0, 0);
    IM_ASSERT(event != NULL);

    D3D12_COMMAND_QUEUE_DESC queueDesc = {};
    queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
    queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
    queueDesc.NodeMask = 1;

    ID3D12CommandQueue* cmdQueue = NULL;
    hr = d3d_device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&cmdQueue));
    IM_ASSERT(SUCCEEDED(hr));

    ID3D12CommandAllocator* cmdAlloc = NULL;
    hr = d3d_device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&cmdAlloc));
    IM_ASSERT(SUCCEEDED(hr));

    ID3D12GraphicsCommandList* cmdList = NULL;
    hr = d3d_device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, cmdAlloc, NULL, IID_PPV_ARGS(&cmdList));
    IM_ASSERT(SUCCEEDED(hr));

    cmdList->CopyTextureRegion(&dstLocation, 0, 0, 0, &srcLocation, NULL);
    cmdList->ResourceBarrier(1, &barrier);

    hr = cmdList->Close();
    IM_ASSERT(SUCCEEDED(hr));

    // Execute the copy
    cmdQueue->ExecuteCommandLists(1, (ID3D12CommandList* const*)&cmdList);
    hr = cmdQueue->Signal(fence, 1);
    IM_ASSERT(SUCCEEDED(hr));

    // Wait for everything to complete
    fence->SetEventOnCompletion(1, event);
    WaitForSingleObject(event, INFINITE);

    // Tear down our temporary command queue and release the upload resource
    cmdList->Release();
    cmdAlloc->Release();
    cmdQueue->Release();
    CloseHandle(event);
    fence->Release();
    uploadBuffer->Release();

    // Create a shader resource view for the texture
    D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc;
    ZeroMemory(&srvDesc, sizeof(srvDesc));
    srvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
    srvDesc.Texture2D.MipLevels = desc.MipLevels;
    srvDesc.Texture2D.MostDetailedMip = 0;
    srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
    d3d_device->CreateShaderResourceView(pTexture, &srvDesc, srv_cpu_handle);

    // Return results
    *out_tex_resource = pTexture;
    *out_width = image_width;
    *out_height = image_height;
    stbi_image_free(image_data);

    return true;
}


bool LoadTextureFromFile(const char* file_name, ID3D12Device* d3d_device, D3D12_CPU_DESCRIPTOR_HANDLE srv_cpu_handle, ID3D12Resource** out_tex_resource, int* out_width, int* out_height)
{
    FILE* f = fopen(file_name, "rb");
    if (f == NULL)
        return false;
    fseek(f, 0, SEEK_END);
    size_t file_size = (size_t)ftell(f);
    if (file_size == -1)
        return false;
    fseek(f, 0, SEEK_SET);
    void* file_data = IM_ALLOC(file_size);
    fread(file_data, 1, file_size, f);
    fclose(f);
    bool ret = LoadTextureFromMemory(file_data, file_size, d3d_device, srv_cpu_handle, out_tex_resource, out_width, out_height);
    IM_FREE(file_data);
    return ret;
}

void DestroyTexture(ID3D12Resource** tex_resources)
{
    (*tex_resources)->Release();
    *tex_resources = NULL;
}



std::string GetHResultMessage(HRESULT hr)
{
    char* errorMsg = nullptr;
    FormatMessageA(
        FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
        nullptr,
        hr,
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        (LPSTR)&errorMsg,
        0,
        nullptr);

    std::string message = errorMsg ? errorMsg : "Unknown error";
    if (errorMsg)
        LocalFree(errorMsg);
    return message;
}

void WaitForGpu()
{
    static ComPtr<ID3D12Fence> fence;
    static HANDLE fenceEvent = nullptr;
    static UINT64 fenceValue = 0;

    if (!fenceEvent) {
        fenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
    }

    if (!fence) {
        g_pd3dDevice->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence));
    }

    // Signal fence
    fenceValue++;
    g_pd3dCommandQueue->Signal(fence.Get(), fenceValue);

    // Wait until fence is reached
    if (fence->GetCompletedValue() < fenceValue) {
        fence->SetEventOnCompletion(fenceValue, fenceEvent);
        WaitForSingleObject(fenceEvent, INFINITE);
    }
}

/*void InitCustomTexture(ID3D12Device* device, ID3D12GraphicsCommandList* cmdList)
{
    if (g_textureInitialized) {
        printf("[InitCustomTexture] Already initialized.\n");
        return;
    }

    if (!device || !cmdList) {
        printf("[InitCustomTexture] Invalid device (%p) or command list (%p)!\n", device, cmdList);
        return;
    }

    std::wstring texturePath = L"DebugMenu/CustomTextures/my_texture.png";
    printf("[InitCustomTexture] Loading texture: %ws\n", texturePath.c_str());

    DirectX::ScratchImage image;
    HRESULT hr = DirectX::LoadFromWICFile(texturePath.c_str(), DirectX::WIC_FLAGS_NONE, nullptr, image);
    if (FAILED(hr)) {
        printf("[InitCustomTexture] Failed LoadFromWICFile (HRESULT: 0x%08X)\n", hr);
        return;
    }

    const DirectX::Image* img = image.GetImage(0, 0, 0);
    if (!img) {
        printf("[InitCustomTexture] image.GetImage() returned NULL!\n");
        return;
    }

    printf("[InitCustomTexture] Image loaded: %dx%d, rowPitch: %zu, slicePitch: %zu\n", img->width, img->height, img->rowPitch, img->slicePitch);

    D3D12_RESOURCE_DESC texDesc = {};
    texDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
    texDesc.Width = img->width;
    texDesc.Height = img->height;
    texDesc.DepthOrArraySize = 1;
    texDesc.MipLevels = 1;
    texDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    texDesc.SampleDesc.Count = 1;
    texDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;

    CD3DX12_HEAP_PROPERTIES defaultHeapProps(D3D12_HEAP_TYPE_DEFAULT);

    printf("[InitCustomTexture] Creating default heap texture...\n");
    hr = device->CreateCommittedResource(
        &defaultHeapProps,
        D3D12_HEAP_FLAG_NONE,
        &texDesc,
        D3D12_RESOURCE_STATE_COPY_DEST,
        nullptr,
        IID_PPV_ARGS(&g_texture));
    if (FAILED(hr)) {
        printf("[InitCustomTexture] Failed to create committed texture resource (HRESULT: 0x%08X)\n", hr);
        return;
    }

    UINT64 uploadBufferSize = 0;
    device->GetCopyableFootprints(&texDesc, 0, 1, 0, nullptr, nullptr, nullptr, &uploadBufferSize);
    printf("[InitCustomTexture] Upload buffer size: %llu\n", uploadBufferSize);

    CD3DX12_HEAP_PROPERTIES uploadHeapProps(D3D12_HEAP_TYPE_UPLOAD);
    CD3DX12_RESOURCE_DESC uploadBufferDesc = CD3DX12_RESOURCE_DESC::Buffer(uploadBufferSize);

    ComPtr<ID3D12Resource> uploadHeap;
    printf("[InitCustomTexture] Creating upload heap...\n");
    hr = device->CreateCommittedResource(
        &uploadHeapProps,
        D3D12_HEAP_FLAG_NONE,
        &uploadBufferDesc,
        D3D12_RESOURCE_STATE_GENERIC_READ,
        nullptr,
        IID_PPV_ARGS(&uploadHeap));
    if (FAILED(hr)) {
        printf("[InitCustomTexture] Failed to create upload heap (HRESULT: 0x%08X)\n", hr);
        return;
    }

    D3D12_SUBRESOURCE_DATA textureData = {};
    textureData.pData = img->pixels;
    textureData.RowPitch = img->rowPitch;
    textureData.SlicePitch = img->slicePitch;


    int frameIndex = g_pSwapChain->GetCurrentBackBufferIndex();
    ID3D12CommandAllocator* allocator = g_commandAllocators[frameIndex];

    HRESULT hrAllocReset = allocator->Reset();
    if (FAILED(hrAllocReset)) {
        printf("[InitCustomTexture] Failed to reset command allocator (HRESULT: 0x%08X)\n", hrAllocReset);
        return;
    }

    HRESULT hrCmdListReset = cmdList->Reset(allocator, nullptr);
    if (FAILED(hrCmdListReset)) {
        printf("[InitCustomTexture] Failed to reset command list (HRESULT: 0x%08X)\n", hrCmdListReset);
        return;
    }


    printf("[InitCustomTexture] Uploading texture to GPU...\n");
    UpdateSubresources(cmdList, g_texture.Get(), uploadHeap.Get(), 0, 0, 1, &textureData);

    printf("[InitCustomTexture] Adding resource barrier...\n");
    CD3DX12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(
        g_texture.Get(),
        D3D12_RESOURCE_STATE_COPY_DEST,
        D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
    cmdList->ResourceBarrier(1, &barrier);

    HRESULT hrClose = cmdList->Close();
    if (FAILED(hrClose)) {
        printf("[InitCustomTexture] Failed to close command list (HRESULT: 0x%08X)\n", hrClose);
        return;
    }

    ID3D12CommandList* cmdsLists[] = { cmdList };
    g_pd3dCommandQueue->ExecuteCommandLists(1, cmdsLists);


    WaitForGpu();

    printf("[InitCustomTexture] Creating SRV descriptor heap...\n");
    D3D12_DESCRIPTOR_HEAP_DESC srvHeapDesc = {};
    srvHeapDesc.NumDescriptors = 1;
    srvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
    srvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;

    hr = device->CreateDescriptorHeap(&srvHeapDesc, IID_PPV_ARGS(&g_srvHeap));
    if (FAILED(hr)) {
        printf("[InitCustomTexture] Failed to create descriptor heap (HRESULT: 0x%08X)\n", hr);
        return;
    }

    D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
    srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
    srvDesc.Format = texDesc.Format;
    srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
    srvDesc.Texture2D.MipLevels = 1;

    printf("[InitCustomTexture] Creating SRV...\n");
    device->CreateShaderResourceView(g_texture.Get(), &srvDesc, g_srvHeap->GetCPUDescriptorHandleForHeapStart());

    g_textureInitialized = true;
    printf("[InitCustomTexture] Texture initialized successfully.\n");
}*/




// From Entity* to YS::OBJ*
YS::OBJ* ToYSOBJ(Entity* entity) {
    return reinterpret_cast<YS::OBJ*>(entity);
}



int debugprotectionLobotomiseAIOn() {

    unsigned char* Drive1 = reinterpret_cast<unsigned char*>(moduleBase + 0x3B447E);
    unsigned char* Drive2 = reinterpret_cast<unsigned char*>(moduleBase + 0x3B447F);


    DWORD flOldProtectDrive1, flOldProtectDrive2;
    VirtualProtect(Drive1, sizeof(unsigned char), PAGE_EXECUTE_READWRITE, &flOldProtectDrive1);
    VirtualProtect(Drive2, sizeof(unsigned char), PAGE_EXECUTE_READWRITE, &flOldProtectDrive2);



    *Drive1 = 0xEB;
    *Drive2 = 0x11;



    // Restore the original protection attributes of the memory locations
    VirtualProtect(Drive1, sizeof(unsigned char), flOldProtectDrive1, &flOldProtectDrive1);
    VirtualProtect(Drive2, sizeof(unsigned char), flOldProtectDrive2, &flOldProtectDrive2);

    return 0;

}

int debugprotectionLobotomiseAIOff() {

    unsigned char* Drive1 = reinterpret_cast<unsigned char*>(moduleBase + 0x3B447E);
    unsigned char* Drive2 = reinterpret_cast<unsigned char*>(moduleBase + 0x3B447F);


    DWORD flOldProtectDrive1, flOldProtectDrive2;
    VirtualProtect(Drive1, sizeof(unsigned char), PAGE_EXECUTE_READWRITE, &flOldProtectDrive1);
    VirtualProtect(Drive2, sizeof(unsigned char), PAGE_EXECUTE_READWRITE, &flOldProtectDrive2);



    *Drive1 = 0x75;
    *Drive2 = 0x11;



    // Restore the original protection attributes of the memory locations
    VirtualProtect(Drive1, sizeof(unsigned char), flOldProtectDrive1, &flOldProtectDrive1);
    VirtualProtect(Drive2, sizeof(unsigned char), flOldProtectDrive2, &flOldProtectDrive2);

    return 0;

}

enum ground_types {
    GROUND_NULL = 0,
    GROUND_STONE = 1,
    GROUND_STONE_SP = 2,
    GROUND_DIRT = 3,
    GROUND_DIRT_SP = 4,
    GROUND_SAND = 5,
    GROUND_SAND_SP = 6,
    GROUND_WOOD = 7,
    GROUND_BOARD = 8,
    GROUND_CREAKWOOD = 9,
    GROUND_LAWN = 10,
    GROUND_GRASS = 11,
    GROUND_DRYGRASS = 12,
    GROUND_WATER = 13,
    GROUND_DEEPWATER = 14,
    GROUND_SNOW = 15,
    GROUND_METAL = 16,
    GROUND_IRONWIRE = 17,
    GROUND_CARPET = 18,
    GROUND_CLOTH = 19,
    GROUND_SOFA = 20,
    GROUND_ROOF = 21,
    GROUND_ROOF_SP = 22,
    GROUND_ELECTRIC = 23,
    GROUND_ELECTRIC_SP = 24,
    GROUND_GLASS = 25,
    GROUND_RUBBER = 26,
    GROUND_BONE = 27,
    GROUND_PAPER = 28,
    GROUND_FRAGMENT = 29,
    GROUND_LAST = 30
};

std::map<int, std::string> groundTypeNames = {
{GROUND_NULL, "GROUND_NULL"},
{GROUND_STONE, "GROUND_STONE"},
{GROUND_STONE_SP, "GROUND_STONE_SP"},
{GROUND_DIRT, "GROUND_DIRT"},
{GROUND_DIRT_SP, "GROUND_DIRT_SP"},
{GROUND_SAND, "GROUND_SAND"},
{GROUND_SAND_SP, "GROUND_SAND_SP"},
{GROUND_WOOD, "GROUND_WOOD"},
{GROUND_BOARD, "GROUND_BOARD"},
{GROUND_CREAKWOOD, "GROUND_CREAKWOOD"},
{GROUND_LAWN, "GROUND_LAWN"},
{GROUND_GRASS, "GROUND_GRASS"},
{GROUND_DRYGRASS, "GROUND_DRYGRASS"},
{GROUND_WATER, "GROUND_WATER"},
{GROUND_DEEPWATER, "GROUND_DEEPWATER"},
{GROUND_SNOW, "GROUND_SNOW"},
{GROUND_METAL, "GROUND_METAL"},
{GROUND_IRONWIRE, "GROUND_IRONWIRE"},
{GROUND_CARPET, "GROUND_CARPET"},
{GROUND_CLOTH, "GROUND_CLOTH"},
{GROUND_SOFA, "GROUND_SOFA"},
{GROUND_ROOF, "GROUND_ROOF"},
{GROUND_ROOF_SP, "GROUND_ROOF_SP"},
{GROUND_ELECTRIC, "GROUND_ELECTRIC"},
{GROUND_ELECTRIC_SP, "GROUND_ELECTRIC_SP"},
{GROUND_GLASS, "GROUND_GLASS"},
{GROUND_RUBBER, "GROUND_RUBBER"},
{GROUND_BONE, "GROUND_BONE"},
{GROUND_PAPER, "GROUND_PAPER"},
{GROUND_FRAGMENT, "GROUND_FRAGMENT"},
{GROUND_LAST, "GROUND_LAST"}
};


std::vector<std::string> entityNames = { "P_EX100_XM" };

int selectedIndex = 0;  // Start with the first item selected

std::vector<u_int> entryIDs = {0x001,0x002,0x003,0x004,0x78,0x12E,0x12D};



std::map<uint32_t, std::vector<Caption>> activeCaptionsMap;
std::map<std::pair<std::string, int>, std::vector<Caption>> captionMap;
std::map<std::pair<std::string, int>, float> lastCaptionShownTime;

struct Vertex
{
    XMFLOAT3 position;
    XMFLOAT4 color;
};

// Define a simple triangle (in NDC space)
Vertex vertices[] =
{
    { XMFLOAT3(0.0f,  0.5f, 0.0f), XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f) }, // Top vertex (red)
    { XMFLOAT3(0.5f, -0.5f, 0.0f), XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f) }, // Bottom-right vertex (green)
    { XMFLOAT3(-0.5f, -0.5f, 0.0f), XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f) }, // Bottom-left vertex (blue)
};

// Define vertices for a simplified moose head
Vector3 mooseHeadVertices[] = {
    // Define a more detailed set of vertices for the moose head
    Vector3(0.0f, 1.0f, 0.0f),      // Top of the head
    Vector3(-0.5f, 0.6f, 0.0f),     // Left ear base
    Vector3(0.5f, 0.6f, 0.0f),      // Right ear base
    Vector3(-0.3f, 0.4f, 0.0f),     // Left antler base
    Vector3(0.3f, 0.4f, 0.0f),      // Right antler base
    Vector3(-0.6f, 0.3f, 0.0f),     // Left antler tip
    Vector3(0.6f, 0.3f, 0.0f),      // Right antler tip
    Vector3(-0.4f, 0.1f, 0.0f),     // Left side of the head
    Vector3(0.4f, 0.1f, 0.0f),      // Right side of the head
    Vector3(0.0f, -0.5f, 0.0f),     // Nose
    Vector3(-0.25f, -0.3f, 0.0f),   // Left eye
    Vector3(0.25f, -0.3f, 0.0f),    // Right eye
    Vector3(-0.4f, -0.2f, 0.0f),    // Left cheek
    Vector3(0.4f, -0.2f, 0.0f),     // Right cheek
    Vector3(-0.2f, 0.0f, 0.0f),     // Jaw left
    Vector3(0.2f, 0.0f, 0.0f),      // Jaw right
};

// Define edges connecting the vertices
std::vector<std::pair<int, int>> mooseHeadEdges = {
    {0, 1}, {0, 2}, // Head to ears
    {0, 3}, {0, 4}, // Head to antlers base
    {3, 5}, {4, 6}, // Antlers base to tips
    {0, 8}, {0, 9}, // Head to sides
    {0, 10}, // Head to nose
    {0, 11}, {0, 12}, // Head to eyes
    {1, 13}, {2, 14}, // Ears to cheeks
    {1, 15}, {2, 15}  // Ears to jaw
};

void DrawDebugPrimitivesInFrame();


struct StickMan {
    Vector3 head;
    Vector3 neck;
    Vector3 leftShoulder;
    Vector3 rightShoulder;
    Vector3 leftElbow;
    Vector3 rightElbow;
    Vector3 leftHand;
    Vector3 rightHand;
    Vector3 hips;
    Vector3 leftKnee;
    Vector3 rightKnee;
    Vector3 leftFoot;
    Vector3 rightFoot;
};

struct DamageText {
    Vector4 position;    // World position of the damage
    int damageAmount;    // Damage value to display
    float spawnTime;     // Time when the damage number was created
};

struct DebugVertex
{
    DirectX::XMFLOAT3 position;  // The position of the vertex
};

std::vector<DamageText> damageTextList;

Entity* GetCachedEntity() {

    
    // Only call GetMenuOBJFunction once per frame
    gGlobalObjPtr = GetMenuOBJFunction(); // This remains your source of truth

    // Check if the entity has changed (to avoid redundant casting)
    if (gGlobalObjPtr != cachedEntityPointer) {
        cachedEntityPointer = gGlobalObjPtr;

        // Update cached entity if the pointer is valid
        if (gGlobalObjPtr != 0) {
            cachedEntity = reinterpret_cast<Entity*>(gGlobalObjPtr);
        }
        else {
            //cachedEntity = nullptr;
        }
    }

    return cachedEntity;
}

Vector4* CopyFVector(Vector4* dest, const Vector4* source) {
    if (dest != source) {
        dest->x = source->x;
        dest->y = source->y;
        dest->z = source->z;
    }
    return dest;
}


float GetNowCurrentTime() {
    LARGE_INTEGER frequency;
    LARGE_INTEGER counter;
    QueryPerformanceFrequency(&frequency);  // Get frequency of high-performance timer
    QueryPerformanceCounter(&counter);      // Get current time

    return static_cast<float>(counter.QuadPart) / frequency.QuadPart;
}

// Hardcoded HLSL code for debug shaders
const char* debugVertexShaderSource = R"(
    struct VSOutput {
        float4 position : SV_POSITION;
    };

    VSOutput main(float4 position : POSITION) {
        VSOutput output;
        output.position = position;
        return output;
    }
)";

const char* debugPixelShaderSource = R"(
    float4 main() : SV_TARGET {
        return float4(1.0, 0.0, 0.0, 1.0); // Red color
    }
)";

// Encode function (with ASLR support)
unsigned int CustomEncodePointer(uint64_t fullAddress, uintptr_t moduleBase) {
    uint64_t highBits = fullAddress & 0xFFFFFFFF00000000;  // Get the static high bits (unchanged by ASLR)
    uint32_t lowBits = static_cast<uint32_t>(fullAddress);  // Get the low bits, which are affected by ASLR

    // Choose high bits table based on ASLR detection
    std::vector<uint64_t>& HighBitsStorage = ((uintptr_t)moduleBase != 0x140000000ULL) ? HighBitsASLR : HighBitsNoASLR;

    unsigned int index = lowBits & 0x1F;  // Mask to get a value within the range (0 to 31)

    unsigned int v5 = 0;
    while (HighBitsStorage[v5] != -1i64) {
        if (HighBitsStorage[v5] == highBits) {
            return lowBits | (v5 << 25) | 0x80000000;  // Found match, return encoded pointer
        }
        ++v5;
        if (v5 >= 0x40) {
            return lowBits | (v5 << 25) | 0x80000000;  // Add if not found
        }
    }

    HighBitsStorage[v5] = highBits;

    return lowBits | (v5 << 25) | 0x80000000;  // Return encoded pointer
}

// Decode function (with ASLR support)
uint64_t CustomDecodePointer(unsigned int encodedAddress, uintptr_t moduleBase) {
    uint32_t lowBits = encodedAddress & 0x1FFFFFF;  // Get the low 25 bits
    unsigned int upperBits = (encodedAddress >> 24) & 0xFF;  // Extract upper bits

    // Print out the extracted values for debugging
  //  LOG(L"Encoded Address: 0x%X\n", encodedAddress);
   // LOG(L"Extracted Upper Bits: 0x%X\n", upperBits);
   // LOG(L"Extracted Low Bits: 0x%X\n", lowBits);

    // Force index to be 1 if upperBits >= 0x82
    unsigned int index = (upperBits >= 0x82) ? 1 : 0;
   // LOG(L"Selected Index: %u\n", index);

    std::vector<uint64_t>& HighBitsStorage = (moduleBase == 0x140000000ULL) ? HighBitsNoASLR : HighBitsASLR;

    if (index >= HighBitsStorage.size()) {
     //   LOG(L"Invalid index: %u\n", index);
        return 0;
    }

    uint64_t highBits = HighBitsStorage[index];

    if (highBits == 0) {
     //   LOG(L"High bits are invalid (zero) for index: %u\n", index);
        return 0;
    }

    uint64_t decodedAddress = (highBits | lowBits);
   // LOG(L"Final Decoded Address: 0x%llX\n", decodedAddress);

    return decodedAddress;
}

kn_camera* GetCamera(bool current) {
    kn_camera* camera = GetKNCameraFunction(current);

    if (camera != nullptr) {
        // Log the _vptr_ pointer (if necessary)
        // LOG("Camera _vptr_: " + std::to_string(reinterpret_cast<std::uintptr_t>(camera->_vptr_)));

        // Log the Matrix4x4 components
        //LOG("Camera Matrix:\n");
      //  LOG("Matrix v0: %f\n", camera->Matrix.v0); // First row/column element
       // LOG("Matrix v1: %f\n", camera->Matrix.v1); // Second row/column element
       // LOG("Matrix v2: %f\n", camera->Matrix.v2); // Third row/column element
       // LOG("Matrix v3: %f\n", camera->Matrix.v3); // Fourth row/column element

        // Log the position vector
       // LOG("Camera Position (pos_): X: %f, Y: %f, Z: %f, W: %f\n",
        //    camera->Pos.x, camera->Pos.y, camera->Pos.z, camera->Pos.w);

        // Log the reference vector
        //LOG("Camera Reference (ref_): X: %f, Y: %f, Z: %f, W: %f\n",
        //    camera->Ref.x, camera->Ref.y, camera->Ref.z, camera->Ref.w);

        // Log the up vector
        //LOG("Camera Up (up_): X: %f, Y: %f, Z: %f, W: %f\n",
         //   camera->UP.x, camera->UP.y, camera->UP.z, camera->UP.w);

        // Log the roll
       // LOG("Camera Roll: %f\n", camera->Roll);
        Matrix4x4* maybeMatrix = reinterpret_cast<Matrix4x4*>((char*)camera + 0x08);

        printf("Memory at offset 0x08:\n");
        printf("v0: %f %f %f %f\n", maybeMatrix->v0.x, maybeMatrix->v0.y, maybeMatrix->v0.z, maybeMatrix->v0.w);
        printf("v1: %f %f %f %f\n", maybeMatrix->v1.x, maybeMatrix->v1.y, maybeMatrix->v1.z, maybeMatrix->v1.w);
        printf("v2: %f %f %f %f\n", maybeMatrix->v2.x, maybeMatrix->v2.y, maybeMatrix->v2.z, maybeMatrix->v2.w);
        printf("v3: %f %f %f %f\n", maybeMatrix->v3.x, maybeMatrix->v3.y, maybeMatrix->v3.z, maybeMatrix->v3.w);
    

    }
    else {
        //LOG("Camera is null\n");
    }

    return camera;
}

bool IsKeyPressed(int key) {
    return (GetAsyncKeyState(key) & 0x8000) != 0;
}

float GetMouseDeltaX() {
    POINT currentMousePos;
    GetCursorPos(&currentMousePos);  // Get the current mouse position

    float deltaX = static_cast<float>(currentMousePos.x - prevMousePos.x);  // Calculate delta X
    prevMousePos = currentMousePos;  // Update the previous position to the current

    return deltaX;  // Return the change in X
}

// Function to get mouse delta in Y-axis
float GetMouseDeltaY() {
    POINT currentMousePos;
    GetCursorPos(&currentMousePos);  // Get the current mouse position

    float deltaY = static_cast<float>(currentMousePos.y - prevMousePos.y);  // Calculate delta Y
    prevMousePos = currentMousePos;  // Update the previous position to the current

    return deltaY;  // Return the change in Y
}

void WriteViewMatrixToGameMemory(const XMMATRIX& newViewMatrix, uintptr_t viewMatrixAddress) {
    // Convert the XMMATRIX to a format that can be written back to memory
    XMFLOAT4X4 viewMatrixFloat;
    XMStoreFloat4x4(&viewMatrixFloat, newViewMatrix);

    SIZE_T bytesWritten;
    BOOL success = WriteProcessMemory(GetCurrentProcess(), (LPVOID)viewMatrixAddress, &viewMatrixFloat, sizeof(viewMatrixFloat), &bytesWritten);

    if (!success || bytesWritten != sizeof(viewMatrixFloat)) {
        DWORD error = GetLastError();
        LOG("Failed to write view matrix to memory. Error code: %lu", error);
    }
    else {
        LOG("Successfully wrote %lu bytes to view matrix address: %p", bytesWritten, (void*)viewMatrixAddress);
    }
}

float GetDeltaTime() {
    using namespace std::chrono;

    // Get current time
    auto currentTime = high_resolution_clock::now();

    // Calculate delta time
    duration<float> deltaTime;
    if (firstFrame) {
        // For the first frame, initialize lastTime and set deltaTime to 0
        lastTime = currentTime;
        deltaTime = duration<float>(0);
        firstFrame = false;
    }
    else {
        // Compute the time difference between the current frame and the previous frame
        deltaTime = currentTime - lastTime;
        lastTime = currentTime;
    }

    // Return delta time in seconds
    return deltaTime.count();
}

/*void UpdateFreeCamera(float deltaTime, XMMATRIX& viewMatrix, uintptr_t viewMatrixAddress) {
    InitializePointers();

    // Get user input (e.g., W, A, S, D for movement, and mouse input for rotation)
    // Example: Move forward with W

    if (DetectGameVersion() == "Steam") {
        uintptr_t cameraTargetAddress = SteamPointer - 0x3F4930;
        // Read the X, Y, Z values from the memory at the calculated address
        float cameraTargetX = *(float*)(cameraTargetAddress);
        float cameraTargetY = *(float*)(cameraTargetAddress + 0x4);
        float cameraTargetZ = *(float*)(cameraTargetAddress + 0x8);

        // Store the target as an XMVECTOR
        cameraTarget = XMVectorSet(cameraTargetX, cameraTargetY, cameraTargetZ, 1.0f);
        //LOG("Camera Target Address: %p\n", cameraTargetAddress);
    }
    else if (DetectGameVersion() == "Epic") {
        uintptr_t cameraTargetAddress = EpicPointer - 0x3F4930;
        // Read the X, Y, Z values from the memory at the calculated address
        float cameraTargetX = *(float*)(cameraTargetAddress);
        float cameraTargetY = *(float*)(cameraTargetAddress + 0x4);
        float cameraTargetZ = *(float*)(cameraTargetAddress + 0x8);

        // Store the target as an XMVECTOR
        cameraTarget = XMVectorSet(cameraTargetX, cameraTargetY, cameraTargetZ, 1.0f);
    }

    if (DetectGameVersion() == "Steam") {
        uintptr_t cameraPositionAddress = SteamPointer - 0x3F4920;
        // Read the X, Y, Z values from the memory at the calculated address
        float cameraPositionX = *(float*)(cameraPositionAddress);
        float cameraPositionY = *(float*)(cameraPositionAddress + 0x4);
        float cameraPositionZ = *(float*)(cameraPositionAddress + 0x8);

        // Store the target as an XMVECTOR
        cameraPosition = XMVectorSet(cameraPositionX, cameraPositionY, cameraPositionZ, 1.0f);

       // LOG("Camera Position Address: %p\n", cameraPositionAddress);
    }
    else if (DetectGameVersion() == "Epic") {
        uintptr_t cameraPositionAddress = EpicPointer - 0x3F4920;
        // Read the X, Y, Z values from the memory at the calculated address
        float cameraPositionX = *(float*)(cameraPositionAddress);
        float cameraPositionY = *(float*)(cameraPositionAddress + 0x4);
        float cameraPositionZ = *(float*)(cameraPositionAddress + 0x8);

        // Store the target as an XMVECTOR
        cameraPosition = XMVectorSet(cameraPositionX, cameraPositionY, cameraPositionZ, 1.0f);
    }

    
    

    

    if (IsKeyPressed('W')) {
        XMVECTOR forward = XMVector3Normalize(cameraTarget - cameraPosition);
        cameraPosition += forward * cameraSpeed * deltaTime;
        cameraTarget += forward * cameraSpeed * deltaTime;
    }

    // Move backward with S
    if (IsKeyPressed('S')) {
        XMVECTOR backward = XMVector3Normalize(cameraPosition - cameraTarget);
        cameraPosition += backward * cameraSpeed * deltaTime;
        cameraTarget += backward * cameraSpeed * deltaTime;
    }

    // Move left with A
    if (IsKeyPressed('A')) {
        XMVECTOR left = XMVector3Normalize(XMVector3Cross(cameraUp, cameraTarget - cameraPosition));
        cameraPosition += left * cameraSpeed * deltaTime;
        cameraTarget += left * cameraSpeed * deltaTime;
    }

    // Move right with D
    if (IsKeyPressed('D')) {
        XMVECTOR right = XMVector3Normalize(XMVector3Cross(cameraTarget - cameraPosition, cameraUp));
        cameraPosition += right * cameraSpeed * deltaTime;
        cameraTarget += right * cameraSpeed * deltaTime;
    }

    // Rotate camera with mouse input (horizontal and vertical)
    float mouseDeltaX = GetMouseDeltaX();  // Function to get mouse movement in X
    float mouseDeltaY = GetMouseDeltaY();  // Function to get mouse movement in Y

    if (mouseDeltaX != 0.0f || mouseDeltaY != 0.0f) {
        // Update camera target based on mouse movement (yaw and pitch)
        XMVECTOR cameraDirection = XMVector3Normalize(cameraTarget - cameraPosition);

        // Apply yaw (rotation around Y axis)
        XMMATRIX rotationMatrixYaw = XMMatrixRotationY(mouseDeltaX * 0.01f);
        cameraDirection = XMVector3Transform(cameraDirection, rotationMatrixYaw);

        // Apply pitch (rotation around X axis)
        XMVECTOR right = XMVector3Normalize(XMVector3Cross(cameraUp, cameraDirection));
        XMMATRIX rotationMatrixPitch = XMMatrixRotationAxis(right, mouseDeltaY * 0.01f);
        cameraDirection = XMVector3Transform(cameraDirection, rotationMatrixPitch);

        // Update camera target based on new direction
        cameraTarget = cameraPosition + cameraDirection;
    }

    // Create and update the new view matrix
    viewMatrix = XMMatrixLookAtLH(cameraPosition, cameraTarget, cameraUp);

    //LOG("View Matrix Address: %p\n", viewMatrixAddress);
    //LOG("View Matrix: ");
    //LOG("[%f, %f, %f, %f]\n", viewMatrix.r[0].m128_f32[0], viewMatrix.r[0].m128_f32[1], viewMatrix.r[0].m128_f32[2], viewMatrix.r[0].m128_f32[3]);
    //LOG("[%f, %f, %f, %f]\n", viewMatrix.r[1].m128_f32[0], viewMatrix.r[1].m128_f32[1], viewMatrix.r[1].m128_f32[2], viewMatrix.r[1].m128_f32[3]);
    //LOG("[%f, %f, %f, %f]\n", viewMatrix.r[2].m128_f32[0], viewMatrix.r[2].m128_f32[1], viewMatrix.r[2].m128_f32[2], viewMatrix.r[2].m128_f32[3]);
    //LOG("[%f, %f, %f, %f]\n", viewMatrix.r[3].m128_f32[0], viewMatrix.r[3].m128_f32[1], viewMatrix.r[3].m128_f32[2], viewMatrix.r[3].m128_f32[3]);

    // Write the updated view matrix back to the game memory
    WriteViewMatrixToGameMemory(viewMatrix,viewMatrixAddress);
}*/


/*void DebugCameraValues(kn_camera* camera, obj_camera* objcamera) {
    if (camera == nullptr) {
        LOG("Camera pointer is null!");
        return;
    }

    // Dereference the camera pointer to access its members
    //LOG("Camera Debug Info:\n");
    StatusPrintfFunction("FreecamPos: (%.2f, %.2f, %.2f)\n", FreecamPos.x, FreecamPos.y, FreecamPos.z);
    StatusPrintfFunction("FreecamTargetPos: (%.2f, %.2f, %.2f)\n", FreecamTargetPos.x, FreecamTargetPos.y, FreecamTargetPos.z);
    StatusPrintfFunction("FreecamSpeed: %.2f\n", FreecamPos.x, FreecamPos.y, FreecamPos.z);
    StatusPrintfFunction("eye: (%.2f %.2f %.2f)\n", objcamera->m_eye.x, objcamera->m_eye.y, objcamera->m_eye.z);
    StatusPrintfFunction("at: (%.2f %.2f %.2f)\n", objcamera->m_at.x, objcamera->m_at.y, objcamera->m_at.z);
    StatusPrintfFunction("m_Pitch: %.2f\n", objcamera->m_Pitch);
    StatusPrintfFunction("m_Yaw: %.2f\n", objcamera->m_Yaw);
    StatusPrintfFunction("m_eyeTarget: (%.2f, %.2f, %.2f)\n", objcamera->m_eyeTarget.x, objcamera->m_eyeTarget.y, objcamera->m_eyeTarget.z);
    StatusPrintfFunction("m_atTarget: (%.2f, %.2f, %.2f)\n", objcamera->m_atTarget.x, objcamera->m_atTarget.y, objcamera->m_atTarget.z);
    StatusPrintfFunction("m_fov: %.2f\n", objcamera->m_fov);
    StatusPrintfFunction("m_roll: %.2f\n", objcamera->m_roll);
}
void DebugOBJCameraValues(obj_camera* objcamera) {
    if (objcamera == nullptr) {
        LOG("Camera pointer is null!");
        return;
    }

    // Dereference the camera pointer to access its members
    LOG("Camera Debug Info:\n");
    LOG("m_at: (%.2f, %.2f, %.2f)\n", objcamera->m_at.x, objcamera->m_at.y, objcamera->m_at.z);
    LOG("m_eye: (%.2f, %.2f, %.2f)\n", objcamera->m_eye.x, objcamera->m_eye.y, objcamera->m_eye.z);
    LOG("m_fovV: (%.2f, %.2f, %.2f)\n", objcamera->m_fovV.x, objcamera->m_fovV.y, objcamera->m_fovV.z);
    LOG("m_fov: %.2f\n", objcamera->m_fov);
    LOG("m_roll: %.2f\n", objcamera->m_roll);
    LOG("m_fixed: %s\n", objcamera->m_fixed ? "true" : "false");
    LOG("m_homeQuick: %s\n", objcamera->m_homeQuick ? "true" : "false");
    LOG("m_kind: %s\n", objcamera->m_kind ? "true" : "false");
    LOG("m_lockon: %s\n", objcamera->m_lockon ? "true" : "false");
    LOG("m_type: %d\n", objcamera->m_type);
    LOG("m_mode: %d\n", objcamera->m_mode);
    LOG("m_radius: %.2f\n", objcamera->m_radius);
    LOG("m_yRot: %.2f\n", objcamera->m_yRot);
    LOG("m_backYRot: %.2f\n", objcamera->m_backYRot);
    LOG("m_eyeTarget: (%.2f, %.2f, %.2f)\n", objcamera->m_eyeTarget.x, objcamera->m_eyeTarget.y, objcamera->m_eyeTarget.z);
    LOG("m_eyeTargetPrev: (%.2f, %.2f, %.2f)\n", objcamera->m_eyeTargetPrev.x, objcamera->m_eyeTargetPrev.y, objcamera->m_eyeTargetPrev.z);
    LOG("m_atTarget: (%.2f, %.2f, %.2f)\n", objcamera->m_atTarget.x, objcamera->m_atTarget.y, objcamera->m_atTarget.z);
    LOG("m_atTargetPrev: (%.2f, %.2f, %.2f)\n", objcamera->m_atTargetPrev.x, objcamera->m_atTargetPrev.y, objcamera->m_atTargetPrev.z);
    LOG("m_fovVTarget: (%.2f, %.2f, %.2f)\n", objcamera->m_fovVTarget.x, objcamera->m_fovVTarget.y, objcamera->m_fovVTarget.z);
    LOG("m_fovVTargetPrev: (%.2f, %.2f, %.2f)\n", objcamera->m_fovVTargetPrev.x, objcamera->m_fovVTargetPrev.y, objcamera->m_fovVTargetPrev.z);
    LOG("m_Yaw: %.2f\n", objcamera->m_Yaw);
    LOG("m_Pitch: %.2f\n", objcamera->m_Pitch);
    LOG("m_objectiveInitRadius: %.2f\n", objcamera->m_objectiveInitRadius);
    LOG("m_objectiveLockRadius: %.2f\n", objcamera->m_objectiveLockRadius);
    LOG("m_objectiveRadiusMin: %.2f\n", objcamera->m_objectiveRadiusMin);
    LOG("m_objectiveRadiusMax: %.2f\n", objcamera->m_objectiveRadiusMax);
    LOG("m_objectiveUpCurve: %.2f\n", objcamera->m_objectiveUpCurve);
    LOG("m_defaultFov: %.2f\n", objcamera->m_defaultFov);
    LOG("m_defaultRoll: %.2f\n", objcamera->m_defaultRoll);
}



void UpdateFreeCamera(obj_camera* objcamera) {
    YS::PAD* SoftPad1;
    YS::PAD* HardPad1;
    YS::PAD* HardPad2;
    Matrix4x4 Matrix;
    Matrix4x4 Matrix1;
    Matrix4x4 Matrix2;
    Matrix4x4 Matrix3;
    Matrix4x4 Matrix4{};
    Matrix4x4 Matrix5;
    Vector4 Vector1;
    Vector4 Vector2;
    Vector4 Vector3;
    Vector4 Vector41;
    Vector4 Vector5;
    Vector4 Vector6;
    Vector4 Vector7;
    Vector4 Vector8;
    Vector4* FuncVector;
    kn_camera* camera;
    ULONGLONG Buttons;
    ULONGLONG Buttons2;
    ULONGLONG Buttons3;
    ULONGLONG Buttons4;
    ULONGLONG Buttons5;
    float SubjectiveMoveSpeed = 0.052359879f;
    float SubjectivePitchMax = 1.2217305f;
    float SubjectivePitchMin = -1.2217305f;
     //Freecam_sm_vec = 0.0f;
    //float Freecam_sm_pos = 0.0f;
    float gameSpeed = 1.0f;
    float gameSpeedPlaceHolder = 1.0f;
    float gameSpeedPlaceHolder1 = 1.0f;
    float gameSpeedPlaceHolder2 = 1.0f;
    float OddValue1 = 1.0f;
    float OddValue2 = 1.0f;
    double OddValue3 = 0.0;
    int IntOddValue = 0;
    float FreecamSpeedPlaceHolder = 0.0f;
    float FreecamSpeedPlaceHolder1 = 0.0f;
    //float FreecamSpeed = 0.0f;
    float CameraPitch = 0.0f;
    float CameraPitchPlaceHolder = 0.0f;
    float SubjectiveCameraPitchPlaceHolder = 0.0f;
    float CameraYaw = 0.0f;
    float FrameTime = 0.0f;


    SoftPad1 = GetSoftPadFunction(0);
    HardPad1 = GetHardPadFunction(0);
    HardPad2 = GetHardPadFunction(1);
    Matrix = {};

    FrameTime = GetFuncFrametimeFunction();

    // Calculate movement adjustments based on frame time
    float negativeFrameTimeAdjustment = 0.0f - FrameTime; // Negative frame time adjustment
    float positiveFrameTimeAdjustment = FrameTime - 0.0f; // Positive frame time adjustment

    camera = GetKNCameraFunction(1);
    if (camera) {
        Matrix = camera->Matrix;
    }

    //MatrixBitwiseNotFunction(&Matrix1, &Matrix2);

    MatrixBitwiseNotFunction(&Matrix, &Matrix2);

    Matrix3.v0.x = Matrix4.v0.x;
    Matrix3.v0.y = Matrix4.v0.y;
    Matrix3.v0.z = Matrix4.v0.z;
    Matrix3.v0.w = Matrix4.v0.w;
    Matrix3.v1.x = Matrix4.v1.x;
    Matrix3.v1.y = Matrix4.v1.y;
    Matrix3.v1.z = Matrix4.v1.z;
    Matrix3.v1.w = Matrix4.v1.w;
    Matrix3.v2.x = Matrix4.v2.x;
    Matrix3.v2.y = Matrix4.v2.y;
    Matrix3.v2.z = Matrix4.v2.z;
    Matrix3.v2.w = Matrix4.v2.w;
    Matrix3.v3.x = Matrix4.v3.x;
    Matrix3.v3.y = Matrix4.v3.y;
    Matrix3.v3.z = Matrix4.v3.z;
    Matrix3.v3.w = Matrix4.v3.w;

    AssignMatrixFunction(&Matrix2, &Matrix3);

    Vector1.z = 0.0f;
    Vector1.y = 0.0f;
    Vector1.x = 0.0f;
    Vector1.w = 1.0f;

    AddAssignFunction((Vector4*)&Freecam_sm_vec,&HardPad1->Stick[1]);

    if (HardPad1->Stick[1].w <= 0) {
        OddValue1 = 0;
        OddValue2 = 0;
        FreecamSpeedPlaceHolder = 0.0f;
        Freecam_sm_vec = 0.0f;
    }
    else {
        Vector1.y = 0.0f;
        Vector1.x = Freecam_sm_vec;
        Vector1.z = -FreecamSpeedPlaceHolder;
        Vector1.w = 0.0f;
        MatrixMultiplyVectorFunction(&Vector2, &Matrix2, &Vector1);
        Vector3.x = Vector2.x;
        Vector3.y = Vector2.y;
        Vector3.z = Vector2.z;
        Vector3.w = Vector2.w;
        CopyFVector((Vector4*)&Freecam_sm_vec, &Vector3);
        FreecamSpeedPlaceHolder = 0.0f;
        NormalizeVector4Function((Vector4*)&Freecam_sm_vec);
        MultiplyByScalarFunction((Vector4*)&Freecam_sm_vec, FreecamSpeed);
    }
    Buttons = HardPad1->Button;

    if ((HardPad1->Button & 0x400) != 0) {
        if ((Buttons & 0x800000) != 0) {
            OddValue1 = 0;
            OddValue2 = 0;
            Freecam_sm_vec = 0.0f;
            FreecamSpeedPlaceHolder = -FreecamSpeed;
        }
        else if ((Buttons & 0x200000) != 0)
        {
            OddValue1 = 0;
            OddValue2 = 0;
            Freecam_sm_vec = 0.0f;
            FreecamSpeedPlaceHolder = FreecamSpeed;
        }
    }
    Buttons2 = HardPad1->Button;
    if ((HardPad1->Button & 0x800) != 0)
    {
        if ((Buttons2 & 0x80000) != 0)
        {
            objcamera->m_roll = (float)(SoftPad1->Stick[0].w * (float)0.0099999998f) + objcamera->m_roll; // v2 = SoftPad1
        }
        else if ((Buttons2 & 0x20000) != 0)
        {
            objcamera->m_roll = -(float)((float)(SoftPad1->Stick[0].w * (float)0.0099999998f) - objcamera->m_roll);
        }
    }
    else
    {
        CameraPitch = objcamera->m_Pitch;
        OddValue3 = SoftPad1->Stick[0].x * SoftPad1->Stick[0].w * SubjectiveMoveSpeed + objcamera->m_Yaw;
        objcamera->m_Yaw = SoftPad1->Stick[0].x * SoftPad1->Stick[0].w * SubjectiveMoveSpeed + objcamera->m_Yaw;
        objcamera->m_Pitch = -SoftPad1->Stick[0].y * SoftPad1->Stick[0].w * SubjectiveMoveSpeed - CameraPitch;
        CameraYaw = NormalizeRadianFunction(OddValue3);
        CameraPitchPlaceHolder = objcamera->m_Pitch;
        objcamera->m_Yaw = CameraYaw;
        SubjectiveCameraPitchPlaceHolder = NormalizeRadianFunction(CameraPitchPlaceHolder);
        objcamera->m_Pitch = SubjectiveCameraPitchPlaceHolder;
        if (SubjectiveCameraPitchPlaceHolder <= SubjectivePitchMax) {
            if (SubjectiveCameraPitchPlaceHolder < SubjectivePitchMin)
                objcamera->m_Pitch = SubjectivePitchMin;
        }
        else
        {
            objcamera->m_Pitch = SubjectivePitchMax;
        }
    }
    Buttons3 = HardPad1->Repeat;
    if ((Buttons3 & 0x10) != 0) {
        objcamera->m_fov = objcamera->m_fov + (float)0.0099999998f;
        Buttons3 = HardPad1->Repeat;
    }
    Buttons4 = Buttons3;
    if ((Buttons3 & 0x40) != 0) {
        objcamera->m_fov = objcamera->m_fov - (float)0.0099999998f;
        Buttons4 = HardPad1->Repeat;
    }
    Buttons3 = Buttons4;
    if ((Buttons4 & 0x80) != 0) {
        FreecamSpeed = FreecamSpeed - (float)4.0f;
        Buttons4 = HardPad1->Repeat;
        Buttons3 = Buttons4;
    }
    if ((Buttons4 & 0x20) != 0) {
        FreecamSpeed = FreecamSpeed + (float)4.0f;
        Buttons3 = HardPad1->Repeat;
    }
    if ((Buttons4 & 0x100) != 0) {
        FreecamSpeed = FreecamSpeed - (float)32.0f; // v26 = Buttons3 && v27 = Buttons4
        Buttons4 = HardPad1->Repeat;
    }
    if ((Buttons4 & 0x200) != 0)
        FreecamSpeed = FreecamSpeed + (float)32.0f;
    if ((HardPad1->Trigger & 0x8000) != 0)
    {
        IntOddValue = 0;
        objcamera->m_Pitch = 0.0;
    }
    FreecamSpeedPlaceHolder1 = FreecamSpeed;
    if (FreecamSpeed < 8.0f) {
        FreecamSpeedPlaceHolder1 = 8.0f;
        FreecamSpeed = 8.0f;
    }
    if (FreecamSpeedPlaceHolder1 > 320.0f)
        FreecamSpeed = 320.0f;
    if ((HardPad1->Trigger & 4) != 0)
    {
        if ((HardPad1->Button & 0x800) != 0)
            objcamera->m_roll = 0.0f;
        else
            objcamera->m_fov = 1.5f;
    }
    AddAssignFunction((Vector4*)&Freecam_sm_pos, (Vector4*)&Freecam_sm_vec);
    CopyFVector((Vector4*)&objcamera->m_eyeTarget, (Vector4*)&Freecam_sm_pos);
    Buttons5 = HardPad2->Repeat;
    if ((Buttons5 & 0x10) != 0)
    {
        Buttons5 = HardPad2->Repeat;
        gameSpeed = 1.0f;
        gameSpeed = *(float*)&HardPad2->Repeat;
    }
    gameSpeedPlaceHolder = gameSpeed;
    if ((Buttons5 & 0x40) != 0)
    {
        gameSpeed = 0.0f;
        Buttons5 = HardPad2->Repeat;
        gameSpeedPlaceHolder = 0.0f;
    }
    gameSpeedPlaceHolder1 = gameSpeedPlaceHolder;
    if ((Buttons5 & 0x80) != 0)
    {
        Buttons5 = HardPad2->Repeat;
        gameSpeedPlaceHolder1 = (float)((float)gameSpeedPlaceHolder - (float)0.1);
        gameSpeed = gameSpeedPlaceHolder1;
        gameSpeed = (float)gameSpeedPlaceHolder - (float)0.1;
        gameSpeedPlaceHolder = gameSpeedPlaceHolder1;
    }
    gameSpeedPlaceHolder2 = gameSpeedPlaceHolder1;
    if ((Buttons5 & 0x20) != 0)
    {
        gameSpeedPlaceHolder2 = (float)((float)gameSpeedPlaceHolder1 + (float)0.1);
        gameSpeed = gameSpeedPlaceHolder2;
        gameSpeed = (float)gameSpeedPlaceHolder1 + (float)0.1;
        gameSpeedPlaceHolder = gameSpeedPlaceHolder2;
    }
    if (gameSpeedPlaceHolder2 > 1.0)
    {
        gameSpeed = 1.0;
        gameSpeed = 1.0;
        gameSpeedPlaceHolder = 1.0;
    }
    if (gameSpeedPlaceHolder < 0.0)
    {
        gameSpeed = 0.0;
        //gameSpeed = 0.0;
    }
    Vector41.x = 0.0f;
    Vector41.y = 0.0f;
    Vector41.z = 350.0f;
    Vector41.w = 0.0f;
    InitializeAsIdentityMatrixFunction(&Matrix5);
    InitializeAsIdentityMatrixFunction(&Matrix5);
    ApplyMatrixRotationXFunction(&Matrix5, objcamera->m_Pitch);
    ApplyMatrixRotationYFunction(&Matrix5, objcamera->m_Yaw);
    MatrixMultiplyVectorFunction(&Vector5, &Matrix5, &Vector41);
    Vector6.x = Vector5.x;
    Vector6.y = Vector5.y;
    Vector6.z = Vector5.z;
    Vector6.w = Vector5.w;
    CopyFVector(&Vector41, &Vector6);
    Vector7.x = objcamera->m_eyeTarget.x;
    Vector7.y = objcamera->m_eyeTarget.y;
    Vector7.z = objcamera->m_eyeTarget.z;
    Vector7.w = objcamera->m_eyeTarget.w;
    FuncVector = AddAssignFunction(&Vector7, &Vector41);
    Vector8.x = FuncVector->x;
    Vector8.y = FuncVector->y;
    Vector8.z = FuncVector->z;
    Vector8.w = FuncVector->w;
    CopyFVector(&objcamera->m_atTarget, &Vector8);
    AdjustEyeAndAtTargetsFunction(objcamera);

    //DebugCameraValues(camera, objcamera);
}*/






void UpdateLevelEditorCamera(obj_camera* objcamera) {
    // Get the state of the left and right analog sticks
    YS::PAD* leftStickPad = GetHardPadFunction(0);  // Left stick controls movement
    YS::PAD* rightStickPad = GetSoftPadFunction(0); // Right stick controls camera orientation
    kn_camera* camera = GetKNCameraFunction(1);
    OBJENTRY_0* entry = nullptr;

    if (entry == nullptr) {
        entry = GetOBJENTRYFunction(entryIDs[selectedIndex]);
    }

    // Initialize position variables
    LevelEditorcamPos = objcamera->m_eyeTarget;  // Camera position (eye)
    LevelEditorTargetPos = objcamera->m_atTarget;  // Camera target position (at)
    //FreecamPos = camera->Pos;
    //FreecamTargetPos = camera->Ref;

    float frameTime = GetFuncFrametimeFunction();  // Frame time
    float deadZone = 0.05f;  // Dead zone for stick input
    float movementSpeedModifier = 0.052359879f;  // Multiplier for smooth speed
    //float freecamSpeed = 64.0f;  // Constant camera movement speed
    //float rightStickSensitivity = 0.5f;  // Sensitivity for right stick rotation

    // Read the current values from the left and right sticks
    float leftStickX = leftStickPad->Stick[1].x; // Left stick X-axis (strafe left/right)
    float leftStickY = -leftStickPad->Stick[1].y; // Left stick Y-axis (move forward/backward) (FIXED DIRECTION)
    float rightStickX = rightStickPad->Stick[0].x; // Right stick X-axis (camera yaw)
    float rightStickY = rightStickPad->Stick[0].y; // Right stick Y-axis (camera pitch)
    float rightStickW = rightStickPad->Stick[0].w;  // Assuming there's a W axis

    // Apply dead zone to prevent unintentional movement when sticks are near neutral
    if (fabs(leftStickX) < deadZone) leftStickX = 0.0f;
    if (fabs(leftStickY) < deadZone) leftStickY = 0.0f;
    if (fabs(rightStickX) < deadZone) rightStickX = 0.0f;
    if (fabs(rightStickY) < deadZone) rightStickY = 0.0f;

    // Handle camera rotation (right analog stick controls yaw and pitch)
    float MaxCameraPitch = 1.2217305f;  // ~90 degrees in radians (pi/2)
    float MinCameraPitch = -1.2217305f;  // ~-90 degrees in radians (-pi/2)

    // Update yaw (horizontal rotation) using right stick X-axis input
    if (fabs(rightStickW) < deadZone) rightStickW = 0.0f;

    // Update yaw based on both X and W axes
    if (fabs(rightStickX) > deadZone && rightStickW > 0.0f) {
        // Calculate yaw change based on both X-axis and W-axis inputs
        objcamera->m_Yaw -= rightStickX * frameTime * movementSpeedModifier;
        //objcamera->m_Yaw = NormalizeRadianFunction(objcamera->m_Yaw);

    }

    // Update pitch (vertical rotation) using right stick Y-axis input
    objcamera->m_Pitch += rightStickY * frameTime * movementSpeedModifier;



    // Enforce pitch limits
    if (objcamera->m_Pitch > MaxCameraPitch) {
        objcamera->m_Pitch = MaxCameraPitch; // Cap at maximum
    }
    else if (objcamera->m_Pitch < MinCameraPitch) {
        objcamera->m_Pitch = MinCameraPitch; // Cap at minimum
    }





    // Log the right stick values for debugging
    //LOG("Right Stick X: %.2f, Right Stick Y: %.2f", rightStickX, rightStickY); // pitch = m_radius && yaw = m_yRot

    // Calculate forward direction based on current yaw and pitch (camera orientation)
    Vector4 forwardDir;
    forwardDir.x = cosf(objcamera->m_Yaw) * cosf(objcamera->m_Pitch);

    forwardDir.y = sinf(objcamera->m_Pitch);

    forwardDir.z = sinf(objcamera->m_Yaw) * cosf(objcamera->m_Pitch);

    NormalizeVector4Function(&forwardDir);  // Normalize forward direction vector

    // Calculate right direction (perpendicular to forward direction)
    Vector4 rightDir;
    rightDir.x = forwardDir.z;
    rightDir.z = -forwardDir.x;
    rightDir.y = 0;
    NormalizeVector4Function(&rightDir);

    // Log the camera position before updating
   // LOG("Before Update FreecamPos: X: %f, Y: %f, Z: %f", FreecamPos.x, FreecamPos.y, FreecamPos.z);

    // Handle movement (left analog stick controls movement, but not camera rotation)
    LevelEditorcamPos.x += (leftStickY * forwardDir.x + leftStickX * rightDir.x) * LevelEditorcamSpeed * frameTime * movementSpeedModifier;

    LevelEditorcamPos.y += (leftStickY * forwardDir.y) * LevelEditorcamSpeed * frameTime * movementSpeedModifier;  // Vertical movement if moving forward/backward


    LevelEditorcamPos.z += (leftStickY * forwardDir.z + leftStickX * rightDir.z) * LevelEditorcamSpeed * frameTime * movementSpeedModifier;


    // Log the camera position after updating
   // LOG("After Update FreecamPos: X: %f, Y: %f, Z: %f", FreecamPos.x, FreecamPos.y, FreecamPos.z);

    // Reset camera if the trigger is pressed
    if ((leftStickPad->Trigger & 0x2000) != 0) { //square button to reset position and pitch and yaw and fov
        objcamera->m_Pitch = 0.0f;
        objcamera->m_Yaw = 0.0f;
        LevelEditorcamPos = (Vector4)(0.0f, 0.0f, 0.0f, 0.0f);
        LevelEditorTargetPos = (Vector4)(0.0f, 0.0f, 0.0f, 0.0f);
        objcamera->m_fov = 1.50f;
    }

    if ((leftStickPad->Repeat & 0x100) != 0) { //L2 pressed repeatedly to decrease FreecamSpeed
        LevelEditorcamSpeed = LevelEditorcamSpeed - 32.0f;
    }

    if ((leftStickPad->Repeat & 0x200) != 0) { //R2 pressed repeatedly to increase FreecamSpeed
        LevelEditorcamSpeed = LevelEditorcamSpeed + 32.0f;
    }

    if ((leftStickPad->Button & 0x10) != 0) {
        objcamera->m_fov = objcamera->m_fov + (float)0.0099999998f;
    }

    if ((leftStickPad->Button & 0x40) != 0) {
        objcamera->m_fov = objcamera->m_fov - (float)0.0099999998f;
    }

    if ((leftStickPad->Button & 0x400) != 0) { // Assuming L1 button to decrease height
        LevelEditorcamPos.y += LevelEditorcamSpeed * frameTime * movementSpeedModifier;
    }

    if ((leftStickPad->Button & 0x800) != 0) { // Assuming R1 button to increase height
        LevelEditorcamPos.y -= LevelEditorcamSpeed * frameTime * movementSpeedModifier;
    }

    if ((leftStickPad->Trigger & 0x20) != 0) {
        selectedIndex = (selectedIndex + 1) % entryIDs.size();

        // Set selectedID to the new value
        uint32_t selectedID = entryIDs[selectedIndex];
        entry = GetOBJENTRYFunction(selectedID);
        
    }

    if ((leftStickPad->Trigger & 0x80) != 0) {
        selectedIndex = (selectedIndex - 1) % entryIDs.size();

        // Set selectedID to the new value
        uint32_t selectedID = entryIDs[selectedIndex];
        entry = GetOBJENTRYFunction(selectedID);

    }

    





    if (LevelEditorcamSpeed < 8.0f) {
        LevelEditorcamSpeed = 8.0f;
    }
    if (LevelEditorcamSpeed > 640.0f) {
        LevelEditorcamSpeed = 640.0f;
    }

    if (objcamera->m_fov < 0.0f) {
        objcamera->m_fov = 0.0f;

    }
    if (objcamera->m_fov > 3.0f) {
        objcamera->m_fov = 3.0f;

    }


    // Update the camera's target position based on the new yaw and pitch
    LevelEditorTargetPos.x = LevelEditorcamPos.x + forwardDir.x;
    LevelEditorTargetPos.y = LevelEditorcamPos.y + forwardDir.y;
    LevelEditorTargetPos.z = LevelEditorcamPos.z + forwardDir.z;

    // Update the camera's eye (position) and at (target) values
    objcamera->m_eye = LevelEditorcamPos;  // Camera position
    objcamera->m_at = LevelEditorTargetPos;  // Camera target
    objcamera->m_eyeTarget = LevelEditorcamPos;  // Camera position (for smooth transitions)
    objcamera->m_atTarget = LevelEditorTargetPos;  // Camera target (for smooth transitions)
    
    if (entry != nullptr) {
        if ((leftStickPad->Trigger & 0x1000) != 0) { //Triangle button to spawn entry
            NewWorldPosObj(entry->Id, 1, LevelEditorcamPos);
        }
    }
    //camera->Pos = FreecamPos;
   // camera->Ref = FreecamTargetPos;

   // LOG("Forward Dir X: %.2f, Y: %.2f, Z: %.2f\n", forwardDir.x, forwardDir.y, forwardDir.z);
    //LOG("Right Dir X: %.2f, Y: %.2f, Z: %.2f\n", rightDir.x, rightDir.y, rightDir.z);
   // LOG("Left Stick X: %.2f, Left Stick Y: %.2f\n", leftStickX, leftStickY);
    StatusPrintfFunction("LevelEditorcamPos X: %.2f, Y: %.2f, Z: %.2f\n", LevelEditorcamPos.x, LevelEditorcamPos.y, LevelEditorcamPos.z);
    StatusPrintfFunction("LevelEditorcamAt X: %.2f, Y: %.2f, Z: %.2f\n", LevelEditorTargetPos.x, LevelEditorTargetPos.y, LevelEditorTargetPos.z);
    //LOG("Yaw: %.2f, Pitch: %.2f\n", objcamera->m_Yaw, objcamera->m_Pitch);
    //LOG("Frame Time: %.2f\n", frameTime);
    StatusPrintfFunction("LevelEditorCamSpeed: %.2f\n", LevelEditorcamSpeed);
    StatusPrintfFunction("FOV: %.2f\n", objcamera->m_fov);
    if (entry != nullptr) {
        StatusPrintfFunction("Spawn Entry: %s\n", entry->EntryName);
    }
    
    
    StatusPrintfFunction("@066LevelEditorcam Controls: \n");
    StatusPrintfFunction("@660Circle button to reset position and pitch and yaw and fov\n");
    StatusPrintfFunction("L2 to decrease CamSpeed and R2 to increase FreeCamSpeed by 32.0 float\n");
    StatusPrintfFunction("DPAD-UP to increase FOV and DPAD-DOWN to decrease FOV\n");
    StatusPrintfFunction("L1 to decrease cam height and R1 to increase cam height\n");
    StatusPrintfFunction("Triangle to Pause the Game");
}



















void CreateLevelEditorCamera(obj_camera* objcamera) {
    uintptr_t objcameraMode = 0;

    if (DetectGameVersion() == "Steam") {
        objcameraMode = SteamPointer - 0x3F48F0;

    }
    else if (DetectGameVersion() == "Epic") {
        objcameraMode = EpicPointer - 0x3F48F0;
    }

    CancelSubjectiveFunction(30.0);
    //FreecamSpeed = 32.0f;
    *(int*)objcameraMode = 11;
    //LOG("Camera m_type set to: %d", GetOBJCameraInstanceFunction()->m_type);
    //camera->m_Pitch = 0.0f;
    GetOBJControlOFFunction(0);
    DBGWINPauseFunction();
    CommonLayerOFFFunction();
}




void DestroyLevelEditorCamera(obj_camera* objcamera) {
    uintptr_t objcameraMode = 0;

    if (DetectGameVersion() == "Steam") {
        objcameraMode = SteamPointer - 0x3F48F0;

    }
    else if (DetectGameVersion() == "Epic") {
        objcameraMode = EpicPointer - 0x3F48F0;
    }

    *(int*)objcameraMode = 0;
    GetOBJControlONFunction(0);
    DBGWINPauseOffFunction();
    CommonLayerONFunction();

}




HRESULT CompileShader(const char* shaderSource, const char* entryPoint, const char* target, Microsoft::WRL::ComPtr<ID3DBlob>& outBlob) {
    UINT compileFlags = D3DCOMPILE_ENABLE_STRICTNESS;

    Microsoft::WRL::ComPtr<ID3DBlob> errorBlob;
    HRESULT hr = D3DCompile(
        shaderSource,
        strlen(shaderSource),
        NULL,
        NULL,
        NULL,
        entryPoint,
        target,
        compileFlags,
        0,
        &outBlob,
        &errorBlob
    );

    if (FAILED(hr)) {
        if (errorBlob) {
            OutputDebugStringA((char*)errorBlob->GetBufferPointer());
        }
        return hr;
    }

    return S_OK;
}

// Example usage
void LoadDebugShaders() {
    HRESULT hr = CompileShader(debugVertexShaderSource, "main", "vs_5_0", g_debugVertexShader);
    if (FAILED(hr)) {
        OutputDebugStringA("Failed to compile vertex shader.\n");
        return;
    }

    hr = CompileShader(debugPixelShaderSource, "main", "ps_5_0", g_debugPixelShader);
    if (FAILED(hr)) {
        OutputDebugStringA("Failed to compile pixel shader.\n");
        return;
    }
}





StickMan CreateStickManInTpose(const Vector3& worldPos, float height) {
    StickMan stickMan;

    // Proportion constants relative to the total height of the stickman
    float armLength = height * 0.3f; // Arms should be about 30% of height
    float legLength = height * 0.4f; // Legs should be about 40% of height
    float bodyLength = height * 0.2f; // Body (torso) should be about 20% of height

    // Head position (top of the stickman)
    stickMan.head = { worldPos.x, worldPos.y + height, worldPos.z };

    // Neck position just below the head
    stickMan.neck = { worldPos.x, worldPos.y + height - 1.0f, worldPos.z };

    // Hips position (bottom of the torso)
    stickMan.hips = { worldPos.x, worldPos.y + height - bodyLength - 2.0f, worldPos.z };

    // Arm positions (T-pose: arms stretched horizontally)
    stickMan.leftShoulder = { worldPos.x - armLength, worldPos.y + height - 1.0f, worldPos.z };
    stickMan.rightShoulder = { worldPos.x + armLength, worldPos.y + height - 1.0f, worldPos.z };
    stickMan.leftElbow = { worldPos.x - armLength - 1.0f, worldPos.y + height - 1.0f, worldPos.z };
    stickMan.rightElbow = { worldPos.x + armLength + 1.0f, worldPos.y + height - 1.0f, worldPos.z };
    stickMan.leftHand = { worldPos.x - armLength - 2.0f, worldPos.y + height - 1.0f, worldPos.z };
    stickMan.rightHand = { worldPos.x + armLength + 2.0f, worldPos.y + height - 1.0f, worldPos.z };

    // Leg positions (slightly spread legs, for X-axis horizontal offset)
    stickMan.leftKnee = { worldPos.x - 0.5f, worldPos.y + height - bodyLength - legLength + 0.5f, worldPos.z };
    stickMan.rightKnee = { worldPos.x + 0.5f, worldPos.y + height - bodyLength - legLength + 0.5f, worldPos.z };
    stickMan.leftFoot = { worldPos.x - 0.5f, worldPos.y + height - bodyLength - legLength, worldPos.z };
    stickMan.rightFoot = { worldPos.x + 0.5f, worldPos.y + height - bodyLength - legLength, worldPos.z };

    return stickMan;
}




DAMAGE GetEntityDamageStruct() {
    InitializePointers();  // Assuming this initializes necessary pointers like SteamPointer or EpicPointer

    uintptr_t DamageStructureAddress = 0;

    // Detect the game version and assign the correct base pointer
    if (DetectGameVersion() == "Steam") {
        DamageStructureAddress = SteamPointer + 0x1F17458;
    }
    else if (DetectGameVersion() == "Epic") {
        DamageStructureAddress = EpicPointer + 0x1F17458;
    }

    // Ensure the address is valid (non-zero)
    if (DamageStructureAddress == 0) {
        // Handle error (could log, return a default, etc.)
        throw std::runtime_error("Invalid damage structure address");
    }

    // Read the memory at the address and return the structure
    DAMAGE* damagePtr = reinterpret_cast<DAMAGE*>(DamageStructureAddress);  // Cast the address to a pointer to DAMAGE

    return *damagePtr;  // Dereference the pointer and return the actual structure
}


std::vector<Entity*> GetParty() {
    InitializePointers();
    uintptr_t pPartyBase = 0;

    if (DetectGameVersion() == "Steam") {
        pPartyBase = (SteamPointer - 0x36B80);

    }
    else if (DetectGameVersion() == "Epic") {
        pPartyBase = (EpicPointer - 0x36B80);
    }

    
    Entity** pParty = (Entity**)(pPartyBase);

    std::vector<Entity*> partyMembers;

    if (!pParty)
        return partyMembers;

    // Assuming there are always 3 party members
    const size_t partySize = 3;
    for (size_t i = 0; i < partySize; ++i) {
        Entity* member = *(pParty + i);
        if (member) { // Check if the pointer is valid
            partyMembers.push_back(member);
        }
    }

    return partyMembers;
}

/*void UpdateFunctionAddresses() {
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
        cachedGetProjectionMatrixAddress = ReadLEA32(Patterns::pattern_for_GetProjectionMatrix, L"Get Projection Matrix", 0,1,5);
        if (cachedGetProjectionMatrixAddress) {
            GetProjectionMatrixFunction = reinterpret_cast<FunctionCalls::tGetProjection>(cachedGetProjectionMatrixAddress);
        }
        else {
            LOG(L"[ERROR] Failed to find Get Projection Matrix address.\n");
        }
    }
}*/

ProjectionMatrix* GetProjectionMatrix() {
    ProjectionMatrix* projection = nullptr;
    int cameraNumber = 0;

    if (DetectGameVersion() == "Steam") {
        projection = (ProjectionMatrix*)(SteamPointer - 0x4BD58);
    }
    else if (DetectGameVersion() == "Epic") {
        projection = (ProjectionMatrix*)(EpicPointer - 0x4BD58);
    }

    if (DetectGameVersion() == "Steam") {
        cameraNumber = (SteamPointer - 0x4BD60);
    }
    else if (DetectGameVersion() == "Epic") {
        cameraNumber = (EpicPointer - 0x4BD60);
    }

    GetCalcProjectionMatrixFunction(projection + 0x184);

    return projection; // Return the projection matrix pointer
}

void ReadMDLXbonesAmount(Entity* entity, int entityID) {
    if (!entity) {
        // printf("Error: Entity is null\n");
        return;
    }

    uint64_t mdlxPointer = entity->mdlxPointer;
    char* mdlxAddress = reinterpret_cast<char*>(mdlxPointer);
    // printf("Debug: mdlxPointer = %llu, mdlxAddress = %p\n", mdlxPointer, (void*)mdlxAddress);

    int valueAt10Offset = *reinterpret_cast<int*>(mdlxAddress + 0x10);
    // printf("Debug: valueAt10Offset = %d\n", valueAt10Offset);

    if (valueAt10Offset == 4) {
        char* offsetAddress = mdlxAddress + 0x18;
        int valueAtOffset = *reinterpret_cast<int*>(offsetAddress);
        //  printf("Debug: offsetAddress = %p, valueAtOffset (hex) = 0x%X\n", (void*)offsetAddress, (unsigned int)valueAtOffset);

        uintptr_t resultAddress = 0;

        if (valueAtOffset != 0) {
            // Define the base module address (adjust as needed for your application).
            //uintptr_t moduleBase = reinterpret_cast<uintptr_t>(baseModule);

            // Decode the pointer using CustomDecodePointer
            resultAddress = DecodePointerFunction(valueAtOffset);

            // Check if decoding was successful
            if (resultAddress != 0) {
                // Perform actions with the decoded resultAddress
                // Example: Debugging output
               // printf("Decoded Address: %p\n", (void*)resultAddress);
            }
            else {
                // Handle decoding failure
                printf("Failed to decode address: 0x%08X\n", valueAtOffset);
            }
        }

        unsigned char bonesAmount = *reinterpret_cast<unsigned char*>(resultAddress + 0xA0);
        // printf("Debug: bonesAmount (hex) = 0x%02X\n", bonesAmount);

         // Populate the bone IDs for this specific entity
        std::vector<int> boneIDs;
        for (int i = 0; i < bonesAmount; ++i) {
            boneIDs.push_back(i);
        }
        entityBoneIDs[entityID] = boneIDs;
    }
    else {
        // printf("Debug: Condition valueAt10Offset == 4 not met\n");
    }
}

short GetUniqueIDFromEntity(Entity* entity) {
    if (!entity || !entity->Entry) {
        // printf("Invalid entity or entity Entry is null\n");
        return -1;  // Return an invalid ID or handle this case as appropriate
    }

    uint32_t entryPtr = entity->Entry;
    //uint32_t entryValue = *entryPtr;
    uintptr_t resultAddress = 0;

    //uintptr_t moduleBase = reinterpret_cast<uintptr_t>(baseModule);

    // Decode the pointer using CustomDecodePointer
     resultAddress = DecodePointerFunction(entryPtr);

    

    if (resultAddress != 0) {
        short* uniqueIDPtr = reinterpret_cast<short*>(resultAddress);
        return *uniqueIDPtr;
    }
    else {
        //printf("resultAddress is null or invalid\n");
        return -1;  // Return an invalid ID or handle this case as appropriate
    }
}

void UpdateViewMatrix() {
    

    if (DetectGameVersion() == "Steam") {
        viewMatrixAddress = (SteamPointer - 0x25E898);

    }


    else if (DetectGameVersion() == "Epic") {
        viewMatrixAddress = (EpicPointer - 0x25E898);
    }
   
}



XMMATRIX RemoveRotationFromView(const XMMATRIX& view)
{
    // Extract translation
    XMVECTOR camPos = view.r[3];

    // Extract forward direction but zero out Y for up/down
    XMVECTOR forward = XMVectorSet(view.r[2].m128_f32[0], 0.0f, view.r[2].m128_f32[2], 0.0f);
    forward = XMVector3Normalize(forward);

    // Rebuild right and up vectors
    XMVECTOR up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
    XMVECTOR right = XMVector3Cross(up, forward);

    // Reconstruct view matrix
    XMMATRIX newView;
    newView.r[0] = right;
    newView.r[1] = up;
    newView.r[2] = forward;
    newView.r[3] = camPos;

    return newView;
}




/*bool WorldToScreen(const XMVECTOR& worldPos, const XMMATRIX& viewMatrix, float fov, float aspectRatio, float nearZ, float farZ, float viewportWidth, float viewportHeight, XMVECTOR& screenPosOut) {
    // Create the projection matrix based on FOV, aspect ratio, nearZ, and farZ

    //Assert::always_assert_(myViewport.x > 0 && myViewport.y > 0);
    XMMATRIX projectionMatrix = XMMatrixPerspectiveFovLH(fov, aspectRatio, nearZ, farZ);

    // Transform world to view space
    XMVECTOR viewPos = XMVector3TransformCoord(worldPos, viewMatrix);

    // Transform view to projection space
    XMVECTOR projPos = XMVector3TransformCoord(viewPos, projectionMatrix);

    // Perform perspective divide (homogeneous divide) to get NDC
    XMVECTOR projPosDiv = XMVectorDivide(projPos, XMVectorReplicate(XMVectorGetW(projPos)));

    // Check if the point is within the view frustum
    float z = XMVectorGetZ(projPosDiv);
    if (z < 0.0f || z > 1.0f) {
        return false; // The point is outside the view frustum
    }

    // Map from [-1, 1] NDC space to [0, 1] screen space
    XMVECTOR screenPos = (projPosDiv + XMVectorSet(1.0f, 1.0f, 0.0f, 0.0f)) * 0.5f;

    // Adjust for the viewport to get the final screen position

    float screenX = XMVectorGetX(screenPos) * viewportWidth;
    float screenY = (1.0f - XMVectorGetY(screenPos)) * viewportHeight;

    screenPosOut = XMVectorSet(
        XMVectorGetX(screenPos) * viewportWidth,
        XMVectorGetY(screenPos) * viewportHeight, // No inversion
        0.0f, // The z-coordinate is not needed for 2D screen coordinates
        1.0f
    );



    return true; // The point is within the view frustum
}*/

bool WorldToScreen(
    const XMVECTOR& worldPos,
    const XMMATRIX& viewMatrix,   // pass camMat DIRECTLY, no inversion
    float fov,
    float aspectRatio,
    float nearZ,
    float farZ,
    float viewportWidth,
    float viewportHeight,
    XMVECTOR& screenPosOut)
{
    XMMATRIX projMatrix = XMMatrixPerspectiveFovLH(fov, aspectRatio, nearZ, farZ);

    // view * proj  (no inversion of the camera matrix)
    XMMATRIX viewProj = XMMatrixMultiply(viewMatrix, projMatrix);

    XMVECTOR pos = XMVectorSetW(worldPos, 1.0f);
    XMVECTOR clipPos = XMVector4Transform(pos, viewProj);

    float w = XMVectorGetW(clipPos);
    if (w <= 0.001f)
        return false;

    float x = XMVectorGetX(clipPos) / w;
    float y = XMVectorGetY(clipPos) / w;
    float z = XMVectorGetZ(clipPos) / w;

    if (z < 0.0f || z > 1.0f)
        return false;

    float screenX = (x * 0.5f + 0.5f) * viewportWidth;
    float screenY = (1.0f - (y * 0.5f + 0.5f)) * viewportHeight;

    screenPosOut = XMVectorSet(screenX, screenY, 0.0f, 1.0f);
    return true;
}

bool NewWorldToScreen(
    const XMVECTOR& worldPos,
    XMVECTOR& screenPosOut,
    float viewportWidth,
    float viewportHeight)
{
    int camNum = *reinterpret_cast<int*>(moduleBase + 0xAC1838);
    uintptr_t projStructBase = moduleBase + 0xAC1840 + 388 * camNum;

    // viewClip_ is at offset 0x90 in kn::Projection struct
    XMMATRIX viewClip = *reinterpret_cast<XMMATRIX*>(projStructBase + 0x90);
    XMMATRIX viewMatrix = *reinterpret_cast<XMMATRIX*>(moduleBase + 0x8AED00);

    // Game order confirmed: cam * viewClip
    XMMATRIX viewProj = XMMatrixMultiply(viewMatrix, viewClip);

    XMVECTOR pos = XMVectorSetW(worldPos, 1.0f);
    XMVECTOR clipPos = XMVector4Transform(pos, viewProj);

    float w = XMVectorGetW(clipPos);
    if (w <= 0.001f) return false;

    float x = XMVectorGetX(clipPos) / w;
    float y = XMVectorGetY(clipPos) / w;
    float z = XMVectorGetZ(clipPos) / w;

    if (z < 0.0f || z > 1.0f) return false;

    // NDC to screen
    float screenX = (x * 0.5f + 0.5f) * viewportWidth;
    float screenY = (1.0f - (y * 0.5f + 0.5f)) * viewportHeight;

    screenPosOut = XMVectorSet(screenX, screenY, 0.0f, 1.0f);
    return true;
}


/*bool WorldToScreen(
    const XMVECTOR& worldPos,
    const XMMATRIX& viewMatrix,
    float fov, float aspectRatio, float nearZ, float farZ,
    float viewportWidth, float viewportHeight,
    XMVECTOR& screenPosOut)
{
    // Create projection matrix
    XMMATRIX projMatrix = XMMatrixPerspectiveFovLH(fov, aspectRatio, nearZ, farZ);

    // Remove rotation from the view matrix
    XMMATRIX nonRotatedView = RemoveRotationFromView(viewMatrix);

    // Transform world position by non-rotated view matrix
    XMVECTOR viewPos = XMVector3TransformCoord(worldPos, nonRotatedView);

    // Transform to clip space
    XMVECTOR clipPos = XMVector3TransformCoord(viewPos, projMatrix);

    // NDC coordinates
    float ndcX = XMVectorGetX(clipPos);
    float ndcY = XMVectorGetY(clipPos);
    float ndcZ = XMVectorGetZ(clipPos);

    // Check if behind camera
    if (ndcZ < 0.0f || ndcZ > 1.0f)
        return false;

    // Map NDC [-1,1] -> screen coordinates
    float screenX = (ndcX + 1.0f) * 0.5f * viewportWidth;
    float screenY = (1.0f - ndcY) * 0.5f * viewportHeight; // invert Y

    screenPosOut = XMVectorSet(screenX, screenY, 0.0f, 1.0f);
    return true;
}*/








bool WorldToScreenNoClip(
    const XMVECTOR& worldPos,
    const XMMATRIX& viewMatrix,
    float fov,
    float aspectRatio,
    float nearZ,
    float farZ,
    float viewportWidth,
    float viewportHeight,
    XMVECTOR& screenPosOut)
{
    // Build projection matrix (LH to match the game)
    XMMATRIX projMatrix = XMMatrixPerspectiveFovLH(fov, aspectRatio, nearZ, farZ);

    // Correct order: view first, then projection (row-major: v * V * P)
    XMMATRIX viewProj = XMMatrixMultiply(viewMatrix, projMatrix);

    // Transform world position into clip space
    XMVECTOR clipPos = XMVector4Transform(worldPos, viewProj);

    float w = XMVectorGetW(clipPos);
    if (w <= 0.001f)
        return false;

    // Perspective divide -> NDC
    float x = XMVectorGetX(clipPos) / w;
    float y = XMVectorGetY(clipPos) / w;

    // NDC to screen (y flipped because screen Y grows downward)
    float screenX = (x * 0.5f + 0.5f) * viewportWidth;
    float screenY = (1.0f - (y * 0.5f + 0.5f)) * viewportHeight;

    screenPosOut = XMVectorSet(screenX, screenY, 0.0f, 1.0f);
    return true;
}


















void DrawTextAtWorldPos(ImDrawList* drawList,
    const std::string& text,
    const DirectX::XMVECTOR& worldPos,
    const DirectX::XMMATRIX& viewMatrix,
    float fov,
    float aspectRatio,
    float nearZ,
    float farZ,
    float viewportWidth,
    float viewportHeight,
    ImFont* font, // New parameter for font
    float fontSize) // New parameter for font size
{
    DirectX::XMVECTOR screenPos;
    if (WorldToScreen(worldPos, viewMatrix, fov, aspectRatio, nearZ, farZ, viewportWidth, viewportHeight, screenPos)) {
        ImVec2 screenPos2D = ImVec2(DirectX::XMVectorGetX(screenPos), DirectX::XMVectorGetY(screenPos));

        // Set font and size before adding text
        drawList->PushTextureID(font->ContainerAtlas->TexID);
        drawList->AddText(font, fontSize, screenPos2D, IM_COL32(255, 255, 255, 255), text.c_str());
        drawList->PopTextureID();
    }
}

void DrawCinematicCameraInfoBox()
{

    if (!CinematicCameraToggle)
        return;

    if (!g_cineShowSpline)
        return;

    ImGui::PushFont(g_ArialFont);

    ImVec2 viewportSize = ImGui::GetIO().DisplaySize;
    ImGui::SetNextWindowPos(ImVec2(viewportSize.x - 10, viewportSize.y - 10),
        ImGuiCond_Always,
        ImVec2(1.0f, 1.0f));
    ImGui::SetNextWindowSize(ImVec2(350, 0), ImGuiCond_Always);

    ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.6f, 0.6f, 0.6f, 1.f)); // Gray background
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0, 0, 0, 1));                // Black text

    ImGuiWindowFlags flags =
        ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoCollapse |
        ImGuiWindowFlags_NoTitleBar |
        ImGuiWindowFlags_NoScrollbar |
        ImGuiWindowFlags_NoScrollWithMouse;

    ImGui::Begin("CinematicCameraInfoBox", nullptr, flags);

    // Custom title bar
    ImDrawList* draw_list = ImGui::GetWindowDrawList();
    ImVec2 p = ImGui::GetWindowPos();
    ImVec2 size = ImGui::GetWindowSize();
    float titleBarHeight = ImGui::GetFrameHeight();

    draw_list->AddRectFilled(
        p,
        ImVec2(p.x + size.x, p.y + titleBarHeight),
        IM_COL32(0, 0, 0, 255)
    );

    ImVec2 textPos = ImVec2(p.x + 10, p.y + 2);
    draw_list->AddText(textPos, IM_COL32(255, 255, 255, 255), "Cinematic Camera");

    ImGui::Dummy(ImVec2(0.0f, titleBarHeight)); // Space for title bar

    // --------- Cinematic Camera Status Text ---------

    const auto& keys = g_cinematicCamera.getKeyframes();

    ImGui::Text("Keyframes: %d", (int)keys.size());
    ImGui::Text("Timeline Time: %.2f s", g_cineTimelineTime);

    const char* modeText = "Idle";

    if (g_cinematicCamera.isPlaying())
    {
        modeText = g_cinematicCamera.isLooping() ? "Playing (Loop)" : "Playing (Once)";
    }
    else if (g_cinematicCamera.isEditing())
    {
        modeText = "Editing";
    }

    ImGui::Text("Playback: %s", modeText);


    ImGui::Separator();

    // --- Selected keyframe display ---
    if (!keys.empty())
    {
        int sel = g_cineSelectedKeyFrame;
        if (sel < 0 || sel >= (int)keys.size())
            ImGui::Text("Selected Keyframe: None");
        else
            ImGui::Text("Selected Keyframe: %d / %d   (t = %.2f)",
                sel + 1,
                (int)keys.size(),
                keys[sel].time
            );
    }
    else
    {
        ImGui::Text("Selected Keyframe: None");
    }

    ImGui::Separator();

    // Controls
    ImGui::Text("Controls:");
    ImGui::Text("  DPAD LEFT / RIGHT: Move timeline cursor");
    ImGui::Text("  R3: Add keyframe");
    ImGui::Text("  CROSS: Play cinematic path");
    ImGui::Text("  TRIANGLE: Play (loop)");
    ImGui::Text("  CIRCLE: Stop playback");
    ImGui::Text("  SQUARE: Hide Spline Render");
    ImGui::Text("  SELECT: Open Cinematic Camera Settings Window");
    ImGui::Text("  L3: Delete Newest Keyframe");
    ImGui::Text("  Freecam sticks: Move camera while editing");

    ImGui::End();

    ImGui::PopStyleColor(2);
    ImGui::PopFont();
}





void DrawCinematicPathAndKeyframes(
    const XMMATRIX& viewMatrix,
    float fov,
    float aspectRatio,
    float nearZ,
    float farZ)
{
    if (!g_cineShowSpline)
        return;

    const auto& keyframes = g_cinematicCamera.getKeyframes();
    if (keyframes.empty())
        return;

    ImDrawList* dl = ImGui::GetForegroundDrawList();
    ImVec2 viewportSize = ImGui::GetIO().DisplaySize;
    float viewportWidth = viewportSize.x;
    float viewportHeight = viewportSize.y;

    const int samplesPerSegment = 16;

    // -------------------------------------------------------
    // 1) Draw spline path ONLY if there are 2 or more keyframes
    // -------------------------------------------------------
    if (keyframes.size() >= 2)
    {
        for (size_t i = 0; i + 1 < keyframes.size(); ++i)
        {
            const auto& kfA = keyframes[i];
            const auto& kfB = keyframes[i + 1];

            XMVECTOR pA = XMVectorSet(kfA.state.eye.x, kfA.state.eye.y, kfA.state.eye.z, 1.0f);
            XMVECTOR pB = XMVectorSet(kfB.state.eye.x, kfB.state.eye.y, kfB.state.eye.z, 1.0f);

            ImVec2 prevScreen;
            bool prevValid = false;

            for (int s = 0; s <= samplesPerSegment; ++s)
            {
                float t = (float)s / (float)samplesPerSegment;
                XMVECTOR worldPos = XMVectorLerp(pA, pB, t);

                XMVECTOR screenVec;
                if (!WorldToScreen(worldPos, viewMatrix, fov, aspectRatio,
                    nearZ, farZ, viewportWidth, viewportHeight, screenVec))
                {
                    prevValid = false;
                    continue;
                }

                ImVec2 screen(XMVectorGetX(screenVec), XMVectorGetY(screenVec));

                if (prevValid)
                {
                    dl->AddLine(prevScreen, screen, IM_COL32(0, 0, 255, 255), 2.0f);
                }

                prevScreen = screen;
                prevValid = true;
            }
        }
    }

    // -------------------------------------------------------
    // 2) Draw balls + labels for EVERY keyframe (even if only 1)
    // -------------------------------------------------------
    // persistent hover state
    static int   s_lastHoveredKF = -1;
    static int   s_lastTeleportedKF = -1;
    static float s_hoverTime = 0.0f;

    // how long you must hover before teleport (seconds)
    const float hoverTeleportDelay = 1.0f;

    // get mouse position once
    ImVec2 mouse = ImGui::GetIO().MousePos;

    // get frame time (same as in UpdateFreeCamera)
    float frameTime = GetFuncFrametimeFunction();

    int hoveredKF = -1;

    for (size_t i = 0; i < keyframes.size(); ++i)
    {
        const auto& kf = keyframes[i];

        XMVECTOR worldPos = XMVectorSet(
            kf.state.eye.x,
            kf.state.eye.y,
            kf.state.eye.z,
            1.0f
        );

        XMVECTOR screenVec;
        if (!WorldToScreen(
            worldPos,
            viewMatrix, fov, aspectRatio,
            nearZ, farZ,
            viewportWidth, viewportHeight,
            screenVec))
        {
            continue;
        }

        ImVec2 screen(XMVectorGetX(screenVec), XMVectorGetY(screenVec));

        float radius = 6.0f;

        // distance from mouse to this keyframe on screen
        float dx = mouse.x - screen.x;
        float dy = mouse.y - screen.y;
        float dist = sqrtf(dx * dx + dy * dy);

        bool isHovered = (dist < radius + 4.0f);
        if (isHovered)
            hoveredKF = (int)i;

        // color: yellow if hovered, red otherwise
        ImU32 circleColor = isHovered
            ? IM_COL32(255, 255, 0, 255)
            : IM_COL32(255, 0, 0, 255);

        dl->AddCircle(screen, radius, circleColor, 20, 2.0f);

        // label: Keyframe N + time
        char label[64];
        snprintf(label, sizeof(label),
            "Keyframe %d\n t = %.2f s",
            (int)i + 1,
            kf.time
        );

        ImVec2 textPos(screen.x + radius + 6.0f, screen.y - radius);
        dl->AddText(textPos, IM_COL32(255, 255, 255, 255), label);
    }

    // ----- Hover timing / teleport logic -----
    if (hoveredKF != -1)
    {
        if (hoveredKF == s_lastHoveredKF)
        {
            // still hovering the same keyframe → accumulate time
            s_hoverTime += frameTime;
        }
        else
        {
            // started hovering a *different* keyframe → reset timer
            s_hoverTime = 0.0f;
        }

        // when dwell time exceeds delay AND we haven’t teleported to this one yet
        if (s_hoverTime >= hoverTeleportDelay && hoveredKF != s_lastTeleportedKF)
        {
            g_cinematicCamera.applyKeyframeInstant((size_t)hoveredKF);

            const auto& kf = keyframes[hoveredKF];
            g_cineTimelineTime = kf.time;

            s_lastTeleportedKF = hoveredKF;
        }
    }
    else
    {
        // not hovering any keyframe → reset timer
        s_hoverTime = 0.0f;
    }

    // remember for next frame
    s_lastHoveredKF = hoveredKF;
}


void AddDamageNumber(Vector4 damagePosition, int damageAmount, const XMMATRIX& viewMatrix, float fov,
    float aspectRatio, float nearZ, float farZ, float viewportWidth, float viewportHeight,
    ImDrawList* drawList) {

    // Get current time using high precision timer
    float currentTime = GetNowCurrentTime();

    // Convert your Vector4 to DirectX::XMFLOAT4
    DirectX::XMFLOAT4 worldPosFloat4(damagePosition.x, damagePosition.y, damagePosition.z, damagePosition.w);

    // Load the XMFLOAT4 into an XMVECTOR
    XMVECTOR worldPosVec = XMLoadFloat4(&worldPosFloat4);

    // Extract the camera position from the view matrix
    XMVECTOR cameraPosition = viewMatrix.r[3]; // The camera position is stored in the last row (r[3]) of the view matrix

    // Convert world position to screen space
    XMVECTOR screenPos;
    bool isOnScreen = WorldToScreen(worldPosVec, viewMatrix, fov, aspectRatio, nearZ, farZ, viewportWidth, viewportHeight, screenPos);

    // If it's on-screen, add it to the damageTextList
    if (isOnScreen) {
        // Add damage text to the list if it's a new attack (or update the position, etc.)
        DamageText newDamageText;
        newDamageText.position = damagePosition;
        newDamageText.damageAmount = damageAmount;
        newDamageText.spawnTime = currentTime;  // Store the current time when the damage text is created
        damageTextList.push_back(newDamageText);
    }

    // Iterate over the damage text list to update and draw all active damage texts
    for (size_t i = 0; i < damageTextList.size(); ++i) {
        DamageText& damageText = damageTextList[i];

        // Calculate the elapsed time since the damage was dealt
        float elapsedTime = currentTime - damageText.spawnTime;

        // If the damage number is still within the desired lifespan, draw it
        if (elapsedTime < DAMAGE_TEXT_LIFETIME) {
            // Convert world position to screen space again (you may want to cache this if it doesn't change)
            DirectX::XMFLOAT4 worldPosFloat4(damageText.position.x, damageText.position.y, damageText.position.z, damageText.position.w);
            XMVECTOR worldPosVec = XMLoadFloat4(&worldPosFloat4);
            XMVECTOR screenPos;
            bool isOnScreen = WorldToScreen(worldPosVec, viewMatrix, fov, aspectRatio, nearZ, farZ, viewportWidth, viewportHeight, screenPos);

            if (isOnScreen) {
                // Get the screen X and Y
                float screenX = XMVectorGetX(screenPos);
                float screenY = XMVectorGetY(screenPos) - 20.0f;  // Adjust position if necessary

                // Create the text position
                ImVec2 textPos(screenX, screenY);
                std::string damageTextString = "-" + std::to_string(damageText.damageAmount);

                // Draw the damage text
                drawList->AddText(textPos, IM_COL32(255, 0, 0, 255), damageTextString.c_str());
            }
        }
        else {
            // Remove damage text from the list if it has expired
            damageTextList.erase(damageTextList.begin() + i);
            --i;  // Adjust the index after removal
        }
    }
}


void UpdateAndDrawDamage(DAMAGE& damageStruct, float currentTime, const XMMATRIX& viewMatrix,
    float fov, float aspectRatio, float nearZ, float farZ, float viewportWidth,
    float viewportHeight, ImDrawList* drawList) {

    // Check if the damageStruct contains valid data
    if (damageStruct.Damage == 0 || damageStruct.Dir.x == 0.0f && damageStruct.Dir.y == 0.0f &&
        damageStruct.Dir.z == 0.0f) {
        // If damage is zero or the direction is invalid (0, 0, 0), skip processing
        return;
    }

    // Step 1: Use the direction from the damage structure (world position)
    Vector4 damagePosition = damageStruct.Dir;  // World position where the damage occurred

    // Step 2: Store the damage text info in the damageTextList
    DamageText damageTextInfo;
    damageTextInfo.position = damagePosition;
    damageTextInfo.damageAmount = damageStruct.Damage;

    // Use the high-precision time to track when the damage text is created
    damageTextInfo.spawnTime = GetNowCurrentTime();  // Get the current time with high precision

    // Add the damage text info to the list
    damageTextList.push_back(damageTextInfo);
}

void RenderDamageTexts(float currentTime, const XMMATRIX& viewMatrix, float fov, float aspectRatio,
    float nearZ, float farZ, float viewportWidth, float viewportHeight, ImDrawList* drawList) {

    // Step 1: Iterate through the active damage text list
    for (auto it = damageTextList.begin(); it != damageTextList.end(); ) {
        // Calculate the elapsed time since the damage text was spawned
        float elapsedTime = currentTime - it->spawnTime;

        // Step 2: If the damage text has exceeded the lifetime, remove it from the list
        if (elapsedTime > DAMAGE_TEXT_LIFETIME) {
            it = damageTextList.erase(it);  // Remove the expired damage text
        }
        else {
            // Step 3: Otherwise, draw the damage text at its screen position
            AddDamageNumber(it->position, it->damageAmount, viewMatrix, fov, aspectRatio,
                nearZ, farZ, viewportWidth, viewportHeight, drawList);
            ++it;  // Move to the next damage text
        }
    }
}


bool StickmanWorldToScreen(XMVECTOR worldPos, const XMMATRIX& viewMatrix, float fov, float aspectRatio, float nearZ, float farZ, float viewportWidth, float viewportHeight, XMVECTOR& screenPos) {
    // Apply the view and projection matrix to the world position to get the normalized device coordinates
    XMVECTOR projPos = XMVector3TransformCoord(worldPos, viewMatrix);

    // Perspective division (convert to clip space coordinates)
    float w = XMVectorGetW(projPos);
    if (w == 0.0f) {
        return false;
    }
    projPos = XMVectorDivide(projPos, XMVectorSet(w, w, w, w));

    // Convert to screen space coordinates
    float x = (XMVectorGetX(projPos) + 1.0f) * 0.5f * viewportWidth;
    float y = (1.0f - XMVectorGetY(projPos)) * 0.5f * viewportHeight; // Flip the Y-coordinate
    screenPos = XMVectorSet(x, y, 0.0f, 1.0f);

    return true;
}

void DrawStickMan(const StickMan& stickMan, const XMMATRIX& viewMatrix, float fov, float aspectRatio, float nearZ, float farZ, float viewportWidth, float viewportHeight) {
    ImDrawList* drawList = ImGui::GetBackgroundDrawList();

    // Declare XMVECTOR variables to store the world positions of each joint
    XMVECTOR headVec, neckVec, leftShoulderVec, rightShoulderVec, leftElbowVec, rightElbowVec;
    XMVECTOR leftHandVec, rightHandVec, hipsVec, leftKneeVec, rightKneeVec, leftFootVec, rightFootVec;

    // Convert 3D coordinates to XMVECTOR
    headVec = XMVectorSet(stickMan.head.x, stickMan.head.y, stickMan.head.z, 1.0f);
    neckVec = XMVectorSet(stickMan.neck.x, stickMan.neck.y, stickMan.neck.z, 1.0f);
    leftShoulderVec = XMVectorSet(stickMan.leftShoulder.x, stickMan.leftShoulder.y, stickMan.leftShoulder.z, 1.0f);
    rightShoulderVec = XMVectorSet(stickMan.rightShoulder.x, stickMan.rightShoulder.y, stickMan.rightShoulder.z, 1.0f);
    leftElbowVec = XMVectorSet(stickMan.leftElbow.x, stickMan.leftElbow.y, stickMan.leftElbow.z, 1.0f);
    rightElbowVec = XMVectorSet(stickMan.rightElbow.x, stickMan.rightElbow.y, stickMan.rightElbow.z, 1.0f);
    leftHandVec = XMVectorSet(stickMan.leftHand.x, stickMan.leftHand.y, stickMan.leftHand.z, 1.0f);
    rightHandVec = XMVectorSet(stickMan.rightHand.x, stickMan.rightHand.y, stickMan.rightHand.z, 1.0f);
    hipsVec = XMVectorSet(stickMan.hips.x, stickMan.hips.y, stickMan.hips.z, 1.0f);
    leftKneeVec = XMVectorSet(stickMan.leftKnee.x, stickMan.leftKnee.y, stickMan.leftKnee.z, 1.0f);
    rightKneeVec = XMVectorSet(stickMan.rightKnee.x, stickMan.rightKnee.y, stickMan.rightKnee.z, 1.0f);
    leftFootVec = XMVectorSet(stickMan.leftFoot.x, stickMan.leftFoot.y, stickMan.leftFoot.z, 1.0f);
    rightFootVec = XMVectorSet(stickMan.rightFoot.x, stickMan.rightFoot.y, stickMan.rightFoot.z, 1.0f);

    // Declare ImVec2 variables for 2D screen positions
    ImVec2 headPos, neckPos, leftShoulderPos, rightShoulderPos, leftElbowPos, rightElbowPos;
    ImVec2 leftHandPos, rightHandPos, hipsPos, leftKneePos, rightKneePos, leftFootPos, rightFootPos;

    // Project the 3D joint positions to screen space using WorldToScreen
    if (!StickmanWorldToScreen(headVec, viewMatrix, fov, aspectRatio, nearZ, farZ, viewportWidth, viewportHeight, headVec)) return;
    if (!StickmanWorldToScreen(neckVec, viewMatrix, fov, aspectRatio, nearZ, farZ, viewportWidth, viewportHeight, neckVec)) return;
    if (!StickmanWorldToScreen(leftShoulderVec, viewMatrix, fov, aspectRatio, nearZ, farZ, viewportWidth, viewportHeight, leftShoulderVec)) return;
    if (!StickmanWorldToScreen(rightShoulderVec, viewMatrix, fov, aspectRatio, nearZ, farZ, viewportWidth, viewportHeight, rightShoulderVec)) return;
    if (!StickmanWorldToScreen(leftElbowVec, viewMatrix, fov, aspectRatio, nearZ, farZ, viewportWidth, viewportHeight, leftElbowVec)) return;
    if (!StickmanWorldToScreen(rightElbowVec, viewMatrix, fov, aspectRatio, nearZ, farZ, viewportWidth, viewportHeight, rightElbowVec)) return;
    if (!StickmanWorldToScreen(leftHandVec, viewMatrix, fov, aspectRatio, nearZ, farZ, viewportWidth, viewportHeight, leftHandVec)) return;
    if (!StickmanWorldToScreen(rightHandVec, viewMatrix, fov, aspectRatio, nearZ, farZ, viewportWidth, viewportHeight, rightHandVec)) return;
    if (!StickmanWorldToScreen(hipsVec, viewMatrix, fov, aspectRatio, nearZ, farZ, viewportWidth, viewportHeight, hipsVec)) return;
    if (!StickmanWorldToScreen(leftKneeVec, viewMatrix, fov, aspectRatio, nearZ, farZ, viewportWidth, viewportHeight, leftKneeVec)) return;
    if (!StickmanWorldToScreen(rightKneeVec, viewMatrix, fov, aspectRatio, nearZ, farZ, viewportWidth, viewportHeight, rightKneeVec)) return;
    if (!StickmanWorldToScreen(leftFootVec, viewMatrix, fov, aspectRatio, nearZ, farZ, viewportWidth, viewportHeight, leftFootVec)) return;
    if (!StickmanWorldToScreen(rightFootVec, viewMatrix, fov, aspectRatio, nearZ, farZ, viewportWidth, viewportHeight, rightFootVec)) return;

    // Convert the screen-space XMVECTOR to ImVec2 for drawing
    headPos = ImVec2(XMVectorGetX(headVec), XMVectorGetY(headVec));
    neckPos = ImVec2(XMVectorGetX(neckVec), XMVectorGetY(neckVec));
    leftShoulderPos = ImVec2(XMVectorGetX(leftShoulderVec), XMVectorGetY(leftShoulderVec));
    rightShoulderPos = ImVec2(XMVectorGetX(rightShoulderVec), XMVectorGetY(rightShoulderVec));
    leftElbowPos = ImVec2(XMVectorGetX(leftElbowVec), XMVectorGetY(leftElbowVec));
    rightElbowPos = ImVec2(XMVectorGetX(rightElbowVec), XMVectorGetY(rightElbowVec));
    leftHandPos = ImVec2(XMVectorGetX(leftHandVec), XMVectorGetY(leftHandVec));
    rightHandPos = ImVec2(XMVectorGetX(rightHandVec), XMVectorGetY(rightHandVec));
    hipsPos = ImVec2(XMVectorGetX(hipsVec), XMVectorGetY(hipsVec));
    leftKneePos = ImVec2(XMVectorGetX(leftKneeVec), XMVectorGetY(leftKneeVec));
    rightKneePos = ImVec2(XMVectorGetX(rightKneeVec), XMVectorGetY(rightKneeVec));
    leftFootPos = ImVec2(XMVectorGetX(leftFootVec), XMVectorGetY(leftFootVec));
    rightFootPos = ImVec2(XMVectorGetX(rightFootVec), XMVectorGetY(rightFootVec));

    // Draw lines between the joints to form the stickman
    drawList->AddLine(headPos, neckPos, IM_COL32(255, 255, 255, 255), 2.0f); // Head to neck
    drawList->AddLine(neckPos, leftShoulderPos, IM_COL32(255, 255, 255, 255), 2.0f); // Neck to left shoulder
    drawList->AddLine(neckPos, rightShoulderPos, IM_COL32(255, 255, 255, 255), 2.0f); // Neck to right shoulder
    drawList->AddLine(leftShoulderPos, leftElbowPos, IM_COL32(255, 255, 255, 255), 2.0f); // Left shoulder to left elbow
    drawList->AddLine(rightShoulderPos, rightElbowPos, IM_COL32(255, 255, 255, 255), 2.0f); // Right shoulder to right elbow
    drawList->AddLine(leftElbowPos, leftHandPos, IM_COL32(255, 255, 255, 255), 2.0f); // Left elbow to left hand
    drawList->AddLine(rightElbowPos, rightHandPos, IM_COL32(255, 255, 255, 255), 2.0f); // Right elbow to right hand
    drawList->AddLine(neckPos, hipsPos, IM_COL32(255, 255, 255, 255), 2.0f); // Neck to hips
    drawList->AddLine(hipsPos, leftKneePos, IM_COL32(255, 255, 255, 255), 2.0f); // Hips to left knee
    drawList->AddLine(hipsPos, rightKneePos, IM_COL32(255, 255, 255, 255), 2.0f); // Hips to right knee
    drawList->AddLine(leftKneePos, leftFootPos, IM_COL32(255, 255, 255, 255), 2.0f); // Left knee to left foot
    drawList->AddLine(rightKneePos, rightFootPos, IM_COL32(255, 255, 255, 255), 2.0f); // Right knee to right foot

    // Draw the head as a circle (improved)
    float headRadius = 10.0f; // Adjust the radius as needed
    drawList->AddCircle(headPos, headRadius, IM_COL32(255, 255, 255, 255), 12, 2.0f); // Circle for the head
}








ImU32 ColorBasedOnDistance(float distance) {
    // Define ranges for distances
    const float closeRange = 100.0f; // Example value, define as needed
    const float farRange = 1000.0f; // Example value, define as needed

    // Define colors for each range
    const ImU32 closeColor = IM_COL32(0, 255, 0, 255); // Green
    const ImU32 mediumColor = IM_COL32(255, 255, 0, 255); // Yellow
    const ImU32 farColor = IM_COL32(255, 0, 0, 255); // Red

    if (distance < closeRange) {
        return closeColor;
    }
    else if (distance < farRange) {
        // Optionally, blend between colors for a gradient effect
        float t = (distance - closeRange) / (farRange - closeRange);
        return ImColor::HSV(0.1f + t * (1.0f - 0.1f), 0.7f, 0.7f);
    }
    else {
        return farColor;
    }
}

Entity* GetPlayer1() {
    InitializePointers();
    uintptr_t pPlayerBase = 0;
    if (DetectGameVersion() == "Steam") {
        pPlayerBase = (SteamPointer + 0x1F03038);

    }
    else if (DetectGameVersion() == "Epic") {
        pPlayerBase = (EpicPointer + 0x1F03038);
    }

    Entity** pPlayer = (Entity**)(pPlayerBase);

    if (!pPlayer)
        return nullptr;

    return *pPlayer;
}

Vector4 GetCameraPosition() {
    InitializePointers();
    Vector4* CameraEye = nullptr; // Use nullptr instead of 0 for clarity

    if (DetectGameVersion() == "Steam") {
        CameraEye = reinterpret_cast<Vector4*>(SteamPointer - 0x3F4920);
    }
    else if (DetectGameVersion() == "Epic") {
        CameraEye = reinterpret_cast<Vector4*>(EpicPointer - 0x3F4920);
    }

    if (CameraEye != nullptr) {
        // Assuming Vector4 has x, y, z, and w members
        return *CameraEye; // Return the camera position
    }

    // Return a default or zero vector if CameraEye is null
    return Vector4{ 0.0f, 0.0f, 0.0f, 0.0f }; // Adjust as necessary
}

float CalculateDistanceToCamera(const Vector3& entityPosition) {
    // Get the camera position by calling GetCameraPosition
    Vector4 cameraPosition = GetCameraPosition(); // Get camera position (x, y, z, w)

    // Create a Vector3 from the camera position (assuming you only need x, y, z)
    Vector3 cameraPos3D = { cameraPosition.x, cameraPosition.y, cameraPosition.z };

    // Calculate the distance between the camera and the entity
    float dx = cameraPos3D.x - entityPosition.x;
    float dy = cameraPos3D.y - entityPosition.y;
    float dz = cameraPos3D.z - entityPosition.z;

    // Return the Euclidean distance
    return std::sqrt(dx * dx + dy * dy + dz * dz);
}


float CalculateDistance(const XMVECTOR& cameraPos, const XMVECTOR& entityPos) {
    // Check for null pointers
    if (XMVector3IsNaN(cameraPos) || XMVector3IsNaN(entityPos)) {
        std::cerr << "Error: Null pointer detected in CalculateDistance function." << std::endl;
        return 0.0f; // Or any other appropriate value
    }

    XMVECTOR distanceVec = entityPos - cameraPos;
    return XMVectorGetX(XMVector3Length(distanceVec)); // Return the length of the vector
}



void DrawWireframeSphere(Entity* entity, const ImU32 color, const XMMATRIX& viewMatrix, float fov, float aspectRatio, float nearZ, float farZ, float viewportWidth, float viewportHeight)
{
    if (!entity) return; // Check for null pointer

    // Assuming you have access to the AABB values
    const Vector3 aabbmin = entity->RBBOXMIN;
    const Vector3 aabbmax = entity->RBBOXMAX;
    Vector3 center = {
        (aabbmin.x + aabbmax.x) / 2.0f,
        (aabbmin.y + aabbmax.y) / 2.0f,
        (aabbmin.z + aabbmax.z) / 2.0f
    };

    // Calculate the radius based on the AABB dimensions
    float width = aabbmax.x - aabbmin.x;
    float height = aabbmax.y - aabbmin.y;
    float depth = aabbmax.z - aabbmin.z;

    // Use the average of the dimensions as the radius or take the maximum dimension
    float radius = std::max({ width, height, depth }) / 2.0f;

    // Define sphere properties
    const int latitudeCount = 16; // Number of horizontal segments
    const int longitudeCount = 16; // Number of vertical segments

    ImDrawList* drawList = ImGui::GetBackgroundDrawList();

    // Generate vertices for the sphere
    for (int lat = 0; lat <= latitudeCount; ++lat) {
        float theta = lat * XM_PI / latitudeCount; // Angle for latitude
        float sinTheta = sin(theta);
        float cosTheta = cos(theta);

        for (int lon = 0; lon <= longitudeCount; ++lon) {
            float phi = lon * 2 * XM_PI / longitudeCount; // Angle for longitude
            float sinPhi = sin(phi);
            float cosPhi = cos(phi);

            // Calculate the 3D position of the vertex based on AABB center
            Vector3 position = {
                center.x + radius * cosPhi * sinTheta, // X
                center.y + radius * sinPhi * sinTheta, // Y
                center.z + radius * cosTheta             // Z
            };

            // Convert to screen space
            XMVECTOR worldPos = XMVectorSet(position.x, position.y, position.z, 1.0f);
            XMVECTOR screenPos;

            // Call the WorldToScreen function
            if (WorldToScreen(worldPos, viewMatrix, fov, aspectRatio, nearZ, farZ, viewportWidth, viewportHeight, screenPos)) {
            
                // Get screen position
                ImVec2 screenPosition(XMVectorGetX(screenPos), XMVectorGetY(screenPos));

                // Draw lines to connect to the next longitude segment (within the same latitude)
                if (lon > 0) {
                    drawList->AddLine(screenPosition, screenPosition, color);
                }

                // Draw lines to connect to the next latitude segment (within the same longitude)
                if (lat > 0) {
                    // Calculate the previous latitude position for the line
                    Vector3 prevPosition = {
                        center.x + radius * cosPhi * sin(theta - XM_PI / latitudeCount), // X
                        center.y + radius * sinPhi * sin(theta - XM_PI / latitudeCount), // Y
                        center.z + radius * cos(theta - XM_PI / latitudeCount)            // Z
                    };
                    XMVECTOR prevWorldPos = XMVectorSet(prevPosition.x, prevPosition.y, prevPosition.z, 1.0f);
                    XMVECTOR prevScreenPos;

                    // Call the WorldToScreen function for previous position
                    if (WorldToScreen(prevWorldPos, viewMatrix, fov, aspectRatio, nearZ, farZ, viewportWidth, viewportHeight, prevScreenPos)) {
                    
                        ImVec2 prevScreenPosition(XMVectorGetX(prevScreenPos), XMVectorGetY(prevScreenPos));
                        // Draw line to the previous latitude position
                        drawList->AddLine(prevScreenPosition, screenPosition, color);
                    }
                }
            }
        }
    }

    // Connect latitude segments
    for (int lat = 0; lat < latitudeCount; ++lat) {
        for (int lon = 0; lon < longitudeCount; ++lon) {
            // Calculate positions for each segment
            float theta1 = lat * XM_PI / latitudeCount; // First latitude
            float theta2 = (lat + 1) * XM_PI / latitudeCount; // Second latitude

            float phi1 = lon * 2 * XM_PI / longitudeCount; // First longitude
            float phi2 = (lon + 1) * 2 * XM_PI / longitudeCount; // Second longitude

            // Get the four vertices for the current quad
            Vector3 positions[4] = {
                { center.x + radius * cos(phi1) * sin(theta1), center.y + radius * sin(phi1) * sin(theta1), center.z + radius * cos(theta1) },
                { center.x + radius * cos(phi2) * sin(theta1), center.y + radius * sin(phi2) * sin(theta1), center.z + radius * cos(theta1) },
                { center.x + radius * cos(phi2) * sin(theta2), center.y + radius * sin(phi2) * sin(theta2), center.z + radius * cos(theta2) },
                { center.x + radius * cos(phi1) * sin(theta2), center.y + radius * sin(phi1) * sin(theta2), center.z + radius * cos(theta2) }
            };

            // Convert the vertices to screen space and draw lines
            ImVec2 screenPositions[4];
            for (int i = 0; i < 4; ++i) {
                XMVECTOR worldPos = XMVectorSet(positions[i].x, positions[i].y, positions[i].z, 1.0f);
                XMVECTOR screenPos;

                // Call the WorldToScreen function
                if (WorldToScreen(worldPos, viewMatrix, fov, aspectRatio, nearZ, farZ, viewportWidth, viewportHeight, screenPos)) {
                
                    screenPositions[i] = ImVec2(XMVectorGetX(screenPos), XMVectorGetY(screenPos));
                }
            }

            // Draw lines between the vertices to form a wireframe quad
            drawList->AddLine(screenPositions[0], screenPositions[1], color);
            drawList->AddLine(screenPositions[1], screenPositions[2], color);
            drawList->AddLine(screenPositions[2], screenPositions[3], color);
            drawList->AddLine(screenPositions[3], screenPositions[0], color);
        }
    }
}

void DrawBillboardUsingMatrix(Entity* entity, const ImU32 baseColor, int currentHealth, int maxHealth, uint8_t* level, const char* EntryName, int* exp, const XMMATRIX& viewMatrix, float fov, float aspectRatio, float nearZ, float farZ, float viewportWidth, float viewportHeight)
{
    if (!entity) return;

    ImDrawList* drawList = ImGui::GetBackgroundDrawList();

    // Convert the entity's Matrix4x4 to XMMATRIX
    XMMATRIX transformMatrix = XMMATRIX(
        entity->Matrix.v0.x, entity->Matrix.v1.x, entity->Matrix.v2.x, entity->Matrix.v3.x,
        entity->Matrix.v0.y, entity->Matrix.v1.y, entity->Matrix.v2.y, entity->Matrix.v3.y,
        entity->Matrix.v0.z, entity->Matrix.v1.z, entity->Matrix.v2.z, entity->Matrix.v3.z,
        entity->Matrix.v0.w, entity->Matrix.v1.w, entity->Matrix.v2.w, entity->Matrix.v3.w
    );

    // Decompose the entity's matrix into scale, rotation, and translation
    XMVECTOR scale, rotation, translation;
    XMMatrixDecompose(&scale, &rotation, &translation, transformMatrix);

    // Extract the scale factors (width, height, and depth)
    float width = XMVectorGetX(scale);   // X-axis scale (width)
    float height = XMVectorGetY(scale);  // Y-axis scale (height)

    // Project the entity's position (translation vector) to screen space
    XMVECTOR entityWorldPosition = translation;
    XMVECTOR screenPos;
    if (!WorldToScreen(entityWorldPosition, viewMatrix, fov, aspectRatio, nearZ, farZ, viewportWidth, viewportHeight, screenPos)) {
        return;  // Skip rendering if projection fails
    }

    ImVec2 screenPosition = ImVec2(XMVectorGetX(screenPos), XMVectorGetY(screenPos));

    // Use the width and height to scale the billboard box
    float boxWidth = width * 100.0f;   // Adjust multiplier as needed to fit screen scaling
    float boxHeight = height * 100.0f; // Adjust multiplier as needed to fit screen scaling

    ImVec2 minBoxPos = ImVec2(screenPosition.x - boxWidth / 2, screenPosition.y - boxHeight / 2);
    ImVec2 maxBoxPos = ImVec2(screenPosition.x + boxWidth / 2, screenPosition.y + boxHeight / 2);

    // Draw the outline of the billboard
    drawList->AddRect(minBoxPos, maxBoxPos, baseColor, 0.0f, 0, 3.0f);

    // Calculate health percentage
    float healthPercentage = (float)currentHealth / (float)maxHealth;

    // Draw health bar (example)
    float healthBarWidth = 20.0f;
    float healthBarHeight = boxHeight;

    ImVec2 healthBarStart = { maxBoxPos.x + 5.0f, maxBoxPos.y };
    ImVec2 healthBarEnd = { maxBoxPos.x + 5.0f + healthBarWidth, minBoxPos.y };

    float filledHeight = healthBarHeight * healthPercentage;
    ImVec2 filledHealthBarStart = { healthBarStart.x, healthBarStart.y - filledHeight };
    ImVec2 filledHealthBarEnd = { healthBarEnd.x, healthBarStart.y };

    drawList->AddRectFilled(filledHealthBarStart, filledHealthBarEnd, IM_COL32(0, 255, 0, 127));
    drawList->AddRect(healthBarStart, healthBarEnd, IM_COL32(255, 255, 255, 255));

    // Add entity info (name, level, etc.)
    char EntryNameStr[32];
    char LevelStr[68];
    char expStr[68];
    snprintf(LevelStr, sizeof(LevelStr), "Level %u", *level);
    snprintf(EntryNameStr, sizeof(EntryNameStr), "%s", EntryName);
    snprintf(expStr, sizeof(expStr), "EXP %u", *exp);

    float textSpacing = 24.0f;
    ImVec2 textPos = ImVec2(minBoxPos.x, minBoxPos.y - textSpacing * 3);
    drawList->AddText(textPos, IM_COL32(255, 255, 255, 255), EntryNameStr);
    drawList->AddText(ImVec2(textPos.x, textPos.y + textSpacing), IM_COL32(255, 255, 255, 255), LevelStr);
    drawList->AddText(ImVec2(textPos.x, textPos.y + 2 * textSpacing), IM_COL32(255, 255, 255, 255), expStr);
}


void DrawGlowingRect(ImDrawList* list, ImVec2 min, ImVec2 max, ImColor color, float radius = 0.0f, int glow_layers = 8, float glow_spread = 1.0f)
{
    const float baseAlpha = color.Value.w;
    const float falloffFactor = 0.6f;

    for (int i = glow_layers; i > 0; --i)
    {
        float growth = i * glow_spread;
        float alpha = baseAlpha * powf(falloffFactor, (float)(i));

        ImColor glowColor = ImColor(color.Value.x, color.Value.y, color.Value.z, alpha);

        ImVec2 glowMin = ImVec2(min.x - growth, min.y - growth);
        ImVec2 glowMax = ImVec2(max.x + growth, max.y + growth);

        list->AddRect(glowMin, glowMax, glowColor, radius, 0, 1.0f);
    }

    list->AddRect(min, max, color, radius, 0, 2.0f);
}


void DrawGlowingText(ImDrawList* list, ImVec2 pos, ImColor color, const char* text)
{
    const float offset = 1.5f;
    const int passes = 8;
    float alpha = color.Value.w * 0.25f;

    for (int i = 0; i < passes; ++i)
    {
        float angle = (i / (float)passes) * IM_PI * 2;
        ImVec2 offsetVec = ImVec2(cosf(angle) * offset, sinf(angle) * offset);
        ImColor glowColor(color.Value.x, color.Value.y, color.Value.z, alpha);
        list->AddText(ImVec2(pos.x + offsetVec.x, pos.y + offsetVec.y), glowColor, text);
    }

    list->AddText(pos, color, text); // Core
}

void DrawGlowingFilledRect(ImDrawList* list, ImVec2 min, ImVec2 max, ImColor color, int glow_layers = 8, float glow_spread = 1.0f)
{
    const int baseAlpha = color.Value.w * 255.0f; // Alpha in 0-255
    const float falloffFactor = 0.6f;             // Glow falloff per layer

    for (int i = glow_layers; i > 0; --i)
    {
        float growth = i * glow_spread;
        float alphaScale = powf(falloffFactor, (float)i);
        int layerAlpha = static_cast<int>(baseAlpha * alphaScale);

        // Clamp alpha to [0, 255]
        layerAlpha = std::clamp(layerAlpha, 0, 255);

        ImColor glowColor(color.Value.x, color.Value.y, color.Value.z, layerAlpha / 255.0f);

        ImVec2 glowMin = ImVec2(min.x - growth, min.y - growth);
        ImVec2 glowMax = ImVec2(max.x + growth, max.y + growth);

        list->AddRectFilled(glowMin, glowMax, glowColor);
    }

    // Solid main rect with full alpha
    list->AddRectFilled(min, max, color);
}


void DrawGlowingRectRainbow(ImDrawList* list, ImVec2 min, ImVec2 max, float radius = 0.0f, int glow_layers = 8, float glow_spread = 1.0f)
{
    float time = ImGui::GetTime();
    const float falloffFactor = 0.6f;

    for (int i = glow_layers; i > 0; --i)
    {
        float growth = i * glow_spread;
        float layerHue = fmodf(time * 0.1f + (float)i / glow_layers, 1.0f);
        float alpha = powf(falloffFactor, (float)(i)); // fade

        ImColor glowColor = ImColor::HSV(layerHue, 1.0f, 1.0f, alpha);

        ImVec2 glowMin = ImVec2(min.x - growth, min.y - growth);
        ImVec2 glowMax = ImVec2(max.x + growth, max.y + growth);

        list->AddRect(glowMin, glowMax, glowColor, radius, 0, 1.0f);
    }

    // Main outline: white or fixed color, or rainbow if you prefer
    ImColor mainColor = ImColor::HSV(fmodf(time * 0.1f, 1.0f), 1.0f, 1.0f);
    list->AddRect(min, max, mainColor, radius, 0, 2.0f);
}

void DrawSpikyGlowingRect(ImDrawList* drawList, ImVec2 min, ImVec2 max, float time, int segments = 64, float intensity = 10.0f, float freq = 14.0f)
{
    ImVec2 center = ImVec2((min.x + max.x) * 0.5f, (min.y + max.y) * 0.5f);
    float width = max.x - min.x;
    float height = max.y - min.y;

    std::vector<ImVec2> points;

    for (int i = 0; i < segments; ++i)
    {
        float t = (float)i / (float)segments;
        float angle = t * 2.0f * IM_PI;
        float baseX = cosf(angle);
        float baseY = sinf(angle);

        // Base radius (ellipse-shaped around rect)
        float radiusX = width * 0.5f;
        float radiusY = height * 0.5f;

        // Add sine-based displacement for spikiness
        float offset = sinf(time * 2.0f + angle * freq) * intensity;

        ImVec2 point = {
            center.x + (radiusX + offset) * baseX,
            center.y + (radiusY + offset) * baseY
        };

        points.push_back(point);
    }

    // Glow layers
    for (int i = 6; i > 0; --i)
    {
        float alpha = powf(0.5f, (float)i);
        float scale = 1.0f + i * 0.02f;

        std::vector<ImVec2> scaled;
        for (const auto& p : points)
        {
            ImVec2 delta = ImVec2(p.x - center.x, p.y - center.y);
            ImVec2 s = ImVec2(center.x + delta.x * scale, center.y + delta.y * scale);
            scaled.push_back(s);
        }

        drawList->AddPolyline(scaled.data(), scaled.size(), ImColor::HSV(fmodf(time * 0.1f, 1.0f), 1.0f, 1.0f, alpha), true, 1.5f);
    }

    // Main visible line
    drawList->AddPolyline(points.data(), points.size(), ImColor::HSV(fmodf(time * 0.1f, 1.0f), 1.0f, 1.0f), true, 2.0f);
}

/*void DrawOrganicSpikyGlow(ImDrawList* drawList, ImVec2 center, float radius, float time, float amplitude)
{
    constexpr int segments = 64;
    constexpr int glowLayers = 6;
    constexpr float glowSpread = 1.5f;
    constexpr float spikeIntensity = 1000.0f;

    constexpr float spikeDecay = 0.95f;
    static float spikes[segments] = { 0 };

    // Animate spikes based on amplitude
    for (int i = 0; i < segments; ++i)
    {
        float noise = (rand() % 100) / 100.0f;
        float newSpike = spikeIntensity * amplitude * noise;
        spikes[i] = (spikes[i] * spikeDecay) + (newSpike * (1.0f - spikeDecay));
    }

    std::vector<ImVec2> points;
    for (int i = 0; i < segments; ++i)
    {
        float t = (float)i / segments;
        float angle = t * 2.0f * IM_PI;
        float r = radius + spikes[i];

        ImVec2 p = ImVec2(center.x + cosf(angle) * r, center.y + sinf(angle) * r);
        points.push_back(p);
    }

    // Draw glow layers
    for (int i = glowLayers; i > 0; --i)
    {
        float scale = 1.0f + i * 0.04f;
        float alpha = powf(0.6f, (float)i);
        ImU32 color = ImColor::HSV(fmodf(time * 0.1f, 1.0f), 1.0f, 1.0f, alpha);

        std::vector<ImVec2> scaled;
        for (const ImVec2& p : points)
        {
            ImVec2 delta = ImVec2(p.x - center.x, p.y - center.y);
            scaled.push_back(ImVec2(center.x + delta.x * scale, center.y + delta.y * scale));
        }

        drawList->AddPolyline(scaled.data(), scaled.size(), color, true, 1.5f);
    }

    ImU32 mainColor = ImColor::HSV(fmodf(time * 0.1f, 1.0f), 1.0f, 1.0f);
    drawList->AddPolyline(points.data(), points.size(), mainColor, true, 2.5f);
}*/


void DrawOrganicSpikyGlowWithGlowingRect(
    ImDrawList* drawList, ImVec2 center, float radius, float time, float amplitude,
    float health, int currentHealth,
    const char* EntryName, uint8_t* level, int* exp)
{
    constexpr int segments = 64;
    constexpr int glowLayers = 6;
    constexpr float glowSpread = 1.5f;
    constexpr float spikeIntensity = 1000.0f;
    constexpr float spikeDecay = 0.95f;
    static float spikes[segments] = { 0 };

    // Animate spikes
    for (int i = 0; i < segments; ++i)
    {
        float noise = (rand() % 100) / 100.0f;
        float newSpike = spikeIntensity * amplitude * noise;
        spikes[i] = (spikes[i] * spikeDecay) + (newSpike * (1.0f - spikeDecay));
    }

    // Build base points for the spiky ring
    std::vector<ImVec2> points;
    int longestSpikeIndex = 0;
    float maxSpikeLength = 0;

    for (int i = 0; i < segments; ++i)
    {
        float t = (float)i / segments;
        float angle = t * 2.0f * IM_PI;
        float r = radius + spikes[i];

        if (spikes[i] > maxSpikeLength)
        {
            maxSpikeLength = spikes[i];
            longestSpikeIndex = i;
        }

        ImVec2 p = ImVec2(center.x + cosf(angle) * r, center.y + sinf(angle) * r);
        points.push_back(p);
    }

    // Draw glow ring layers
    for (int i = glowLayers; i > 0; --i)
    {
        float scale = 1.0f + i * 0.04f;
        float alpha = powf(0.6f, (float)i);
        ImU32 color = ImColor::HSV(fmodf(time * 0.1f, 1.0f), 1.0f, 1.0f, alpha);

        std::vector<ImVec2> scaled;
        for (const ImVec2& p : points)
        {
            ImVec2 delta = ImVec2(p.x - center.x, p.y - center.y);
            scaled.push_back(ImVec2(center.x + delta.x * scale, center.y + delta.y * scale));
        }

        drawList->AddPolyline(scaled.data(), scaled.size(), color, true, 1.5f);
    }

    // Final spiky shape
    ImU32 mainColor = ImColor::HSV(fmodf(time * 0.1f, 1.0f), 1.0f, 1.0f);
    drawList->AddPolyline(points.data(), points.size(), mainColor, true, 2.5f);

    // -------- Draw Curved Health Bar --------
    int healthSegments = (int)(segments * health);
    if (healthSegments <= 1) healthSegments = 1;

    ImVec2 lastSegmentStartPos;  // To store start position of last health segment

    for (int i = 0; i < healthSegments - 1; ++i)
    {
        float t0 = (float)i / segments;
        float t1 = (float)(i + 1) / segments;

        float angle0 = t0 * 2.0f * IM_PI;
        float angle1 = t1 * 2.0f * IM_PI;

        float r0 = radius + spikes[i];
        float r1 = radius + spikes[i + 1];

        ImVec2 p0 = ImVec2(center.x + cosf(angle0) * r0, center.y + sinf(angle0) * r0);
        ImVec2 p1 = ImVec2(center.x + cosf(angle1) * r1, center.y + sinf(angle1) * r1);

        if (i == healthSegments - 2)
            lastSegmentStartPos = p0;

        // Use hue cycling color for smooth RGB glow
        float hue = fmodf(time * 0.1f + ((float)i / segments), 1.0f);
        ImU32 color = ImColor::HSV(hue, 1.0f, 1.0f);

        drawList->AddLine(p0, p1, color, 5.0f);
    }

    // Draw health text at the start of last health segment (head of the health bar)
    if (currentHealth > 0)
    {
        char healthText[32];
        sprintf(healthText, "%d", currentHealth);

        ImVec2 textPos = lastSegmentStartPos;

        // Offset text a bit outward from the circle edge
        ImVec2 centerToText = ImVec2(textPos.x - center.x, textPos.y - center.y);
        float length = sqrtf(centerToText.x * centerToText.x + centerToText.y * centerToText.y);
        float offset = 15.0f; // pixels outward from the edge
        if (length > 0.001f)
        {
            textPos.x += (centerToText.x / length) * offset;
            textPos.y += (centerToText.y / length) * offset;
        }

        ImVec2 textSize = ImGui::CalcTextSize(healthText);
        textPos.x -= textSize.x * 0.5f;
        textPos.y -= textSize.y * 0.5f;

        DrawGlowingText(drawList, textPos, ImColor(255, 255, 255, 255), healthText);
    }

    // -------- Draw NAME, LEVEL, EXP above the circle, following the top spike --------
    char entryStr[64], levelStr[64], expStr[64];
    snprintf(entryStr, sizeof(entryStr), "%s", EntryName);
    snprintf(levelStr, sizeof(levelStr), "Level %u", *level);
    snprintf(expStr, sizeof(expStr), "EXP %u", *exp);

    // Position at top spike (270 degrees, or 0.75*segments)
    float topSpikeAngle = 1.5f * IM_PI; // 270 degrees
    int topSpikeIndex = (int)(segments * 0.75f) % segments;
    float rTop = radius + spikes[topSpikeIndex];

    ImVec2 dirTop = ImVec2(cosf(topSpikeAngle), sinf(topSpikeAngle));
    float textBaseOffset = 20.0f;

    ImVec2 baseTextPos = ImVec2(center.x + dirTop.x * (rTop + textBaseOffset), center.y + dirTop.y * (rTop + textBaseOffset));

    float spacing = 20.0f;

    ImVec2 posName = ImVec2(baseTextPos.x, baseTextPos.y - spacing * 2);
    ImVec2 posLevel = ImVec2(baseTextPos.x, baseTextPos.y - spacing);
    ImVec2 posExp = baseTextPos;

    ImVec2 sizeName = ImGui::CalcTextSize(entryStr);
    ImVec2 sizeLevel = ImGui::CalcTextSize(levelStr);
    ImVec2 sizeExp = ImGui::CalcTextSize(expStr);

    posName.x -= sizeName.x * 0.5f;
    posLevel.x -= sizeLevel.x * 0.5f;
    posExp.x -= sizeExp.x * 0.5f;

    ImColor textColor = ImColor(255, 255, 255, 255);

    DrawGlowingText(drawList, posName, textColor, entryStr);
    DrawGlowingText(drawList, posLevel, textColor, levelStr);
    DrawGlowingText(drawList, posExp, textColor, expStr);
}






void DrawHypnoticEffect(ImDrawList* drawList, ImVec2 center, float radius, float time, int rings = 32)
{
    float spinSpeed = 1.5f; // controls rotation speed
    float phaseShift = time * spinSpeed;

    for (int i = 0; i < rings; ++i)
    {
        float t = (float)i / (float)rings;
        float ringRadius = radius * t;

        int segments = 64;
        for (int j = 0; j < segments; ++j)
        {
            float angle1 = (j / (float)segments) * 2 * IM_PI + phaseShift + t * 10;
            float angle2 = ((j + 1) / (float)segments) * 2 * IM_PI + phaseShift + t * 10;

            ImVec2 p1 = ImVec2(center.x + cosf(angle1) * ringRadius, center.y + sinf(angle1) * ringRadius);
            ImVec2 p2 = ImVec2(center.x + cosf(angle2) * ringRadius, center.y + sinf(angle2) * ringRadius);

            // Optional: pulsating color
            ImColor color = ImColor::HSV(fmodf(t + time * 0.1f, 1.0f), 0.8f, 1.0f);
            drawList->AddLine(p1, p2, color, 1.5f);
        }
    }
}

void DrawHypnoticBW(ImDrawList* dl, ImVec2 center, float baseRadius, float time, int rings = 6, int segs = 64)
{
    float rotationSpeed = 0.5f;
    float t = time * rotationSpeed;

    dl->AddCircleFilled(center, baseRadius * 1.05f, IM_COL32(0, 0, 0, 200), segs); // Semi-transparent black backdrop

    for (int r = 1; r <= rings; ++r)
    {
        float radius = baseRadius * r / rings;
        float alpha = (1.0f - (float)r / rings) * 200 + 55; // Fade outer rings

        for (int i = 0; i < segs; ++i)
        {
            float a1 = (i / (float)segs) * IM_PI * 2 + t * (r / rings);
            float a2 = ((i + 1) / (float)segs) * IM_PI * 2 + t * (r / rings);

            ImVec2 p1 = ImVec2(center.x + cosf(a1) * radius, center.y + sinf(a1) * radius);
            ImVec2 p2 = ImVec2(center.x + cosf(a2) * radius, center.y + sinf(a2) * radius);

            dl->AddLine(p1, p2, IM_COL32(255, 255, 255, (int)alpha), 1.2f);
        }
    }
}

void DrawDeepHypnoticTunnel(ImDrawList* dl, ImVec2 center, float baseRadius, float time, int rings = 30, int segs = 64)
{
    float swirlIntensity = 8.0f;
    float rotationSpeed = 3.5f;
    float t = time * rotationSpeed;

    float minRadius = 1.5f;

    for (int r = 0; r < rings - 1; ++r)
    {
        float depthFactor1 = (float)r / rings;
        float depthFactor2 = (float)(r + 1) / rings;

        float radiusOuter = baseRadius * powf(0.85f, r);
        radiusOuter = fmaxf(radiusOuter, minRadius);

        float radiusInner = baseRadius * powf(0.85f, r + 1);
        radiusInner = fmaxf(radiusInner, minRadius);

        int alpha = (int)(255 * (1.0f - depthFactor1));
        alpha = std::clamp(alpha, 0, 255);

        // Skip drawing rings that are too small or invisible
        if (radiusOuter <= minRadius || alpha == 0)
            continue;

        ImU32 color = (r % 2 == 0) ? IM_COL32(255, 255, 255, alpha) : IM_COL32(0, 0, 0, alpha);

        ImVector<ImVec2> vertices;
        for (int i = 0; i <= segs; ++i)
        {
            float angle = (i / (float)segs) * IM_PI * 2 + t + swirlIntensity * depthFactor1;

            ImVec2 outer = ImVec2(
                center.x + cosf(angle) * radiusOuter,
                center.y + sinf(angle) * radiusOuter);

            ImVec2 inner = ImVec2(
                center.x + cosf(angle) * radiusInner,
                center.y + sinf(angle) * radiusInner);

            vertices.push_back(outer);
            vertices.push_back(inner);
        }

        dl->AddConvexPolyFilled(vertices.Data, vertices.Size, color);
    }
}


void DrawDeepHypnoticTunnel_NoSpikes(ImDrawList* dl, ImVec2 center, float baseRadius, float time, int rings = 30, int segs = 64)
{
    float swirlIntensity = 8.0f;
    float rotationSpeed = 3.5f;
    float t = time * rotationSpeed;

    for (int r = 0; r < rings - 1; ++r)
    {
        float depthFactor1 = (float)r / rings;
        float depthFactor2 = (float)(r + 1) / rings;

        float radiusOuter = baseRadius * powf(0.85f, r);
        float radiusInner = baseRadius * powf(0.85f, r + 1);

        int alpha = (int)(255 * (1.0f - depthFactor1));
        alpha = std::clamp(alpha, 40, 255);

        ImU32 color = (r % 2 == 0) ? IM_COL32(255, 255, 255, alpha) : IM_COL32(0, 0, 0, alpha);

        for (int i = 0; i < segs; ++i)
        {
            float angle1 = (i / (float)segs) * IM_PI * 2 + t + swirlIntensity * depthFactor1;
            float angle2 = ((i + 1) / (float)segs) * IM_PI * 2 + t + swirlIntensity * depthFactor1;

            ImVec2 outer1 = ImVec2(center.x + cosf(angle1) * radiusOuter, center.y + sinf(angle1) * radiusOuter);
            ImVec2 outer2 = ImVec2(center.x + cosf(angle2) * radiusOuter, center.y + sinf(angle2) * radiusOuter);

            ImVec2 inner1 = ImVec2(center.x + cosf(angle1) * radiusInner, center.y + sinf(angle1) * radiusInner);
            ImVec2 inner2 = ImVec2(center.x + cosf(angle2) * radiusInner, center.y + sinf(angle2) * radiusInner);

            // Draw the quad as two triangles
            dl->AddTriangle(outer1, outer2, inner2, color);
            dl->AddTriangle(outer1, inner2, inner1, color);
        }
    }
}

void DrawGlowingSpikyHealthShape(ImDrawList* drawList, ImVec2 center, float radius, float time, float amplitude, float healthRatio)
{
    constexpr int segments = 64;
    constexpr int glowLayers = 6;
    constexpr float glowSpread = 1.5f;
    constexpr float spikeIntensity = 1000.0f;
    constexpr float spikeDecay = 0.95f;

    static float spikes[segments] = { 0 };

    // Animate spikes
    for (int i = 0; i < segments; ++i)
    {
        float noise = (rand() % 100) / 100.0f;
        float newSpike = spikeIntensity * amplitude * noise;
        spikes[i] = (spikes[i] * spikeDecay) + (newSpike * (1.0f - spikeDecay));
    }

    // Health-based color (green → red)
    ImColor baseColor;
    float t = std::clamp(healthRatio, 0.0f, 1.0f);
    if (t > 0.5f)
        baseColor = ImColor((int)((1.0f - t) * 2.0f * 255), 255, 0);
    else
        baseColor = ImColor(255, (int)(t * 2.0f * 255), 0);

    // Construct spiky shape
    std::vector<ImVec2> shapePoints;
    shapePoints.push_back(center); // Center for triangle fan

    for (int i = 0; i <= segments; ++i)
    {
        float segmentT = (float)i / segments;
        float angle = segmentT * 2.0f * IM_PI;
        float r = radius + spikes[i % segments];
        ImVec2 p = ImVec2(center.x + cosf(angle) * r, center.y + sinf(angle) * r);
        shapePoints.push_back(p);
    }

    // Glow fill layers
    for (int i = glowLayers; i > 0; --i)
    {
        float growth = i * glowSpread;
        float alphaScale = powf(0.6f, (float)i);
        float alpha = baseColor.Value.w * alphaScale;

        ImColor glowColor(baseColor.Value.x, baseColor.Value.y, baseColor.Value.z, alpha);

        std::vector<ImVec2> scaled;
        scaled.push_back(center);

        for (int j = 1; j < shapePoints.size(); ++j)
        {
            ImVec2 delta = ImVec2(shapePoints[j].x - center.x, shapePoints[j].y - center.y);
            scaled.push_back(ImVec2(center.x + delta.x * (1.0f + growth / radius), center.y + delta.y * (1.0f + growth / radius)));
        }

        drawList->AddConvexPolyFilled(scaled.data(), scaled.size(), glowColor);
    }

    // Final solid fill
    drawList->AddConvexPolyFilled(shapePoints.data(), shapePoints.size(), baseColor);
}



void DrawHorizontalAmplitudeBars(
    ImDrawList* drawList, ImVec2 startPos, float width, float maxHeight, float time, float amplitude)
{
    constexpr int segments = 64;
    constexpr int glowLayers = 6;
    constexpr float glowSpread = 1.5f;
    constexpr float spikeIntensity = 1000.0f;
    constexpr float spikeDecay = 0.95f;
    static float spikes[segments] = { 0 };

    float segmentWidth = width / segments;

    // Animate spikes
    for (int i = 0; i < segments; ++i)
    {
        float noise = (rand() % 100) / 100.0f;
        float newSpike = spikeIntensity * amplitude * noise;
        spikes[i] = (spikes[i] * spikeDecay) + (newSpike * (1.0f - spikeDecay));
    }

    // Normalize spike heights to maxHeight
    float maxSpike = 0.0001f;
    for (int i = 0; i < segments; ++i)
        if (spikes[i] > maxSpike) maxSpike = spikes[i];

    // Draw bars with glow effect
    for (int i = 0; i < segments; ++i)
    {
        float normalizedHeight = (spikes[i] / maxSpike) * maxHeight;
        float x = startPos.x + i * segmentWidth;
        float y = startPos.y;

        // Glow layers: draw thicker bars with lower alpha behind main bar
        for (int layer = glowLayers; layer > 0; --layer)
        {
            float alpha = powf(0.6f, (float)layer);
            float thickness = 1.0f + layer * 1.5f;

            // Hue cycles over time + segment index for smooth rainbow effect
            float hue = fmodf(time * 0.1f + ((float)i / segments), 1.0f);
            ImU32 glowColor = ImColor::HSV(hue, 1.0f, 1.0f, alpha);

            drawList->AddLine(
                ImVec2(x + segmentWidth * 0.5f, y),
                ImVec2(x + segmentWidth * 0.5f, y - normalizedHeight),
                glowColor,
                thickness
            );
        }
    }
}



/*void DrawBillboard(Entity* entity, const ImU32 baseColor, int currentHealth, int maxHealth, uint8_t* level, const char* EntryName, int* exp, const XMMATRIX& viewMatrix, float fov, float aspectRatio, float nearZ, float farZ, float viewportWidth, float viewportHeight)
{
    if (!entity) return;

    // Get the entity's team
    int team = *(reinterpret_cast<int*>(reinterpret_cast<char*>(entity) + 0x4DC)); // Adjust offset if necessary

    // Use the baseColor, but change it based on the team within the function
    ImU32 color = baseColor;  // Default base color
    switch (team) {
    case 1:
        color = IM_COL32(0, 0, 255, 255); // Green for player
        break;
    case 2:
        color = IM_COL32(255, 0, 0, 255); // Red for enemy
        break;
    case 3:
        color = IM_COL32(255, 255, 0, 255); // Yellow for gimmick
        break;
    case 4:
        color = IM_COL32(0, 0, 255, 255); // Blue for other
        break;
    default:
        color = IM_COL32(255, 255, 255, 255); // White for neutral/unknown
        break;
    }

    ImDrawList* drawList = ImGui::GetBackgroundDrawList();

    // Calculate AABB center
    const Vector3 aabbMin = entity->RBBOXMIN;
    const Vector3 aabbMax = entity->RBBOXMAX;

    if (std::isnan(aabbMin.x) || std::isnan(aabbMin.y) || std::isnan(aabbMin.z) ||
        std::isnan(aabbMax.x) || std::isnan(aabbMax.y) || std::isnan(aabbMax.z) ||
        std::isinf(aabbMin.x) || std::isinf(aabbMin.y) || std::isinf(aabbMin.z) ||
        std::isinf(aabbMax.x) || std::isinf(aabbMax.y) || std::isinf(aabbMax.z)) {
        return; // Skip drawing if AABB values are invalid
    }

    Vector3 center = {
        (aabbMin.x + aabbMax.x) * 0.5f,
        (aabbMin.y + aabbMax.y) * 0.5f,
        (aabbMin.z + aabbMax.z) * 0.5f
    };

    // Define AABB corners in world space
    Vector3 corners[8] = {
        { aabbMin.x, aabbMin.y, aabbMin.z },
        { aabbMax.x, aabbMin.y, aabbMin.z },
        { aabbMin.x, aabbMax.y, aabbMin.z },
        { aabbMax.x, aabbMax.y, aabbMin.z },
        { aabbMin.x, aabbMin.y, aabbMax.z },
        { aabbMax.x, aabbMin.y, aabbMax.z },
        { aabbMin.x, aabbMax.y, aabbMax.z },
        { aabbMax.x, aabbMax.y, aabbMax.z }
    };

    ImVec2 screenPos[8];
    bool validProjection = true;

    // Project AABB corners into screen space
    for (int i = 0; i < 8; ++i) {
        XMVECTOR worldPos = XMVectorSet(corners[i].x, corners[i].y, corners[i].z, 1.0f);
        XMVECTOR projectedPos;
        if (!WorldToScreen(worldPos, viewMatrix, fov, aspectRatio, nearZ, farZ, viewportWidth, viewportHeight, projectedPos)) {
            validProjection = false;
            break;
        }
        screenPos[i] = ImVec2(XMVectorGetX(projectedPos), XMVectorGetY(projectedPos));
    }

    if (!validProjection) return;

    // Find the min and max screen positions for the AABB
    ImVec2 minScreenPos = screenPos[0];
    ImVec2 maxScreenPos = screenPos[0];
    for (int i = 1; i < 8; ++i) {
        minScreenPos.x = std::min(minScreenPos.x, screenPos[i].x);
        minScreenPos.y = std::min(minScreenPos.y, screenPos[i].y);
        maxScreenPos.x = std::max(maxScreenPos.x, screenPos[i].x);
        maxScreenPos.y = std::max(maxScreenPos.y, screenPos[i].y);
    }

    // Draw the outline of the billboard using the team-based color
    //drawList->AddRectFilled(minScreenPos, maxScreenPos, IM_COL32(7,211,235,127), 0.0f, 0.0f);  // Semi-transparent filled box
    drawList->AddRect(minScreenPos, maxScreenPos, color, 0.0f, 0, 3.0f); // Outline

    

    // Get the current screen size
    ImVec2 screenSize = ImGui::GetIO().DisplaySize;

    // Draw the outline of the billboard using the team-based color
    drawList->AddRect(minScreenPos, maxScreenPos, color, 0.0f, 0, 3.0f); // Outline

    // Calculate health percentage
    float healthPercentage = (float)currentHealth / (float)maxHealth;

    // Determine health bar position and size
    float healthBarWidth = screenSize.x * 0.005f; // Width of the health bar

    // Get font size based on screen size
    float fontSize = screenSize.x * 0.005f;

   

    // Set health bar start and end positions
    ImVec2 healthBarStart = { maxScreenPos.x + 5.0f, maxScreenPos.y }; // Start from the bottom of the screen
    ImVec2 healthBarEnd = { maxScreenPos.x + 5.0f + healthBarWidth, minScreenPos.y }; // End at the top of the screen

    // Calculate the height of the filled portion of the health bar based on the current health
    float healthBarHeight = maxScreenPos.y - minScreenPos.y;  // Full height of the health bar
    float filledHeight = healthBarHeight * healthPercentage;   // Amount to fill based on current health

    // The top of the fill should start from the top of the health bar, and move downwards as health decreases
    //float fillTop = healthBarStart.y;  // The top of the health bar (start position for filled area)

    // Draw the filled portion of the health bar (current health) -- green fill
    ImVec2 filledHealthBarStart = { healthBarStart.x, healthBarStart.y - filledHeight }; // Start of the filled area (top)
    ImVec2 filledHealthBarEnd = { healthBarEnd.x, healthBarStart.y };  // End of the filled area (bottom)
    drawList->AddRectFilled(filledHealthBarStart, filledHealthBarEnd, IM_COL32(0, 255, 0, 255));  // Green filled area

    // Draw the outline of the health bar
    drawList->AddRect(healthBarStart, healthBarEnd, IM_COL32(255, 255, 255, 255));  // Outline of the health bar

    

    // Text to show health value
    ImVec2 HealthtextPos;

    // Check if there's health
    if (currentHealth > 0) {

        // Text showing the health value
        char healthText[32];
        sprintf(healthText, "%d", currentHealth);
        ImVec2 textSize = ImGui::CalcTextSize(healthText, nullptr, false, fontSize);

        //float healthFillHeight = healthBarHeight * (float(currentHealth) / float(maxHealth));

        //ImVec2 HealthTextPos = { healthBarStart.x + healthBarWidth / 2 - textSize.x / 2, healthBarStart.y + (healthBarHeight * 0.5f) - (textSize.y * 0.5f) };

        ImVec2 HealthTextPos = ImVec2(healthBarStart.x + (0.1f * 0.5f), healthBarStart.y + (healthBarHeight * 0.5f));

        

        // Draw the health value text at the appropriate position
        drawList->AddText(HealthtextPos, IM_COL32(255, 255, 255, 255), healthText);
        //ImFont* font = ImGui::GetFont();
        //drawList->AddText(font, fontSize, HealthTextPos, IM_COL32(255, 255, 255, 255), healthText);
    }

    

    //char LevelStr[68];
    char EntryNameStr[32];
    char LevelStr[68];
    char expStr[68];
    snprintf(LevelStr, sizeof(LevelStr), "Level %u", *level);
    snprintf(EntryNameStr, sizeof(EntryNameStr), "%s", EntryName);
    snprintf(expStr, sizeof(expStr), "EXP %u", *exp);

    // Define the vertical spacing between text labels
    float textSpacing = 24.0f;

    // Define the position for the first line of text (above the 2D box)
    ImVec2 textPos = ImVec2(minScreenPos.x, minScreenPos.y - textSpacing * 3); // Adjust the y-offset to place it above the ESP box

    // Draw the text labels in order (EntryName, Level, and EXP)
    drawList->AddText(textPos, White, EntryNameStr);
    drawList->AddText(ImVec2(textPos.x - fontSize * 0.5f, textPos.y - fontSize * 0.5f + textSpacing), White, LevelStr);
    drawList->AddText(ImVec2(textPos.x - fontSize * 0.5f, textPos.y - fontSize * 0.5f + 2 * textSpacing), White, expStr);
    //char expStr[68];
    


    // Define the vertical spacing between text labels
   // float textSpacing = 24.0f;

    // Define the position for the first line of text (above the 2D box)
    //ImVec2 textPos = ImVec2(minScreenPos.x, minScreenPos.y - textSpacing * 3); // Adjust the y-offset to place it above the ESP box

    // Draw the text labels in order (EntryName, Level, and EXP)
    //drawList->AddText(textPos, White, EntryNameStr);
    //drawList->AddText(ImVec2(textPos.x, textPos.y + textSpacing), White, LevelStr);
    //drawList->AddText(ImVec2(textPos.x, textPos.y + 2 * textSpacing), White, expStr);

}*/

void DrawBillboard(Entity* entity, const ImU32 baseColor, int currentHealth, int maxHealth, uint8_t* level, const char* EntryName, int* exp, const XMMATRIX& viewMatrix, float fov, float aspectRatio, float nearZ, float farZ, float viewportWidth, float viewportHeight)
{
    if (!entity) return;

    int team = *(reinterpret_cast<int*>(reinterpret_cast<char*>(entity) + 0x4DC));
    ImU32 color = baseColor;
    switch (team)
    {
    case 1: color = IM_COL32(0, 255, 0, 255); break;
    case 2: color = IM_COL32(255, 0, 0, 255); break;
    case 3: color = IM_COL32(255, 255, 0, 255); break;
    case 4: color = IM_COL32(0, 0, 255, 255); break;
    default: color = IM_COL32(255, 255, 255, 255); break;
    }

    ImDrawList* drawList = ImGui::GetBackgroundDrawList();

    Vector3 aabbMin = entity->RBBOXMIN;
    Vector3 aabbMax = entity->RBBOXMAX;
    if (std::isnan(aabbMin.x) || std::isinf(aabbMin.x)) return;

    Vector3 center = {
        (aabbMin.x + aabbMax.x) * 0.5f,
        (aabbMin.y + aabbMax.y) * 0.5f,
        (aabbMin.z + aabbMax.z) * 0.5f
    };

    Vector3 corners[8] = {
        { aabbMin.x, aabbMin.y, aabbMin.z }, { aabbMax.x, aabbMin.y, aabbMin.z },
        { aabbMin.x, aabbMax.y, aabbMin.z }, { aabbMax.x, aabbMax.y, aabbMin.z },
        { aabbMin.x, aabbMin.y, aabbMax.z }, { aabbMax.x, aabbMin.y, aabbMax.z },
        { aabbMin.x, aabbMax.y, aabbMax.z }, { aabbMax.x, aabbMax.y, aabbMax.z }
    };

    ImVec2 screenPos[8];
    for (int i = 0; i < 8; ++i)
    {
        XMVECTOR worldPos = XMVectorSet(corners[i].x, corners[i].y, corners[i].z, 1.0f);
        XMVECTOR projectedPos;
        if (!WorldToScreen(worldPos, viewMatrix, fov, aspectRatio, nearZ, farZ, viewportWidth, viewportHeight, projectedPos))
            return;
        screenPos[i] = ImVec2(XMVectorGetX(projectedPos), XMVectorGetY(projectedPos));
    }

    ImVec2 minScreen = screenPos[0], maxScreen = screenPos[0];
    for (int i = 1; i < 8; ++i)
    {
        minScreen.x = std::min(minScreen.x, screenPos[i].x);
        minScreen.y = std::min(minScreen.y, screenPos[i].y);
        maxScreen.x = std::max(maxScreen.x, screenPos[i].x);
        maxScreen.y = std::max(maxScreen.y, screenPos[i].y);
    }

    // Draw GLOWING BOX
    //DrawGlowingRectRainbow(drawList, minScreen, maxScreen, ImColor(color));
    ImVec2 center1 = ImVec2((minScreen.x + maxScreen.x) * 0.5f, (minScreen.y + maxScreen.y) * 0.5f);

    // Compute radius based on box size
    float boxWidth = maxScreen.x - minScreen.x;
    float boxHeight = maxScreen.y - minScreen.y;
    float radius = fminf(boxWidth, boxHeight) * 0.5f; // Optional: scale smaller if needed



    // Get time
    float time = ImGui::GetTime();

    // Draw the animated spiky glow around the center
    //DrawDeepHypnoticTunnel(drawList, center1, 150.0f, time);
    float baseRadius = 200.0f;
    //float time = ImGui::GetTime();

    //DrawDeepHypnoticTunnel(drawList, center1, 150.0f, time);

    float amplitude = GetCurrentAmplitude();

    //LOG(L"Current amplitude: %f\n", amplitude);

    //DrawOrganicSpikyGlow(drawList, center1, 100.0f, time, amplitude);

    //DrawDeepHypnoticTunnel_NoSpikes(drawList, center1, baseRadius, time);

    // HEALTH BAR
    ImVec2 screenSize = ImGui::GetIO().DisplaySize;
    float barWidth = screenSize.x * 0.005f;
    float fontSize = screenSize.x * 0.005f;

    ImVec2 barStart = { maxScreen.x + 5.0f, maxScreen.y };
    ImVec2 barEnd = { maxScreen.x + 5.0f + barWidth, minScreen.y };
    float healthRatio = (float)currentHealth / (float)maxHealth;
    float barHeight = maxScreen.y - minScreen.y;
    float filledHeight = barHeight * healthRatio;

    ImVec2 filledStart = { barStart.x, barStart.y - filledHeight };
    ImVec2 filledEnd = { barEnd.x, barStart.y };

    // If above 50%, interpolate from green to yellow
// If below 50%, interpolate from yellow to red
    float healthPerc = std::clamp((float)currentHealth / (float)maxHealth, 0.0f, 1.0f);

    ImColor gradientColor;

    if (healthPerc > 0.5f) {
        float t = (healthPerc - 0.5f) * 2.0f;  // 0.0 → 1.0
        int red = (int)(255 * (1.0f - t));
        int green = 255;
        gradientColor = ImColor(red, green, 0, 255);
    }
    else {
        float t = healthPerc * 2.0f;  // 0.0 → 1.0
        int red = 255;
        int green = (int)(255 * t);
        gradientColor = ImColor(red, green, 0, 255);
    }

    // Glowing fill
    //DrawGlowingRect(drawList, filledStart, filledEnd, ImColor(0, 255, 0, 255));
    //DrawGlowingFilledRect(drawList,filledStart, filledEnd, gradientColor);
    DrawOrganicSpikyGlowWithGlowingRect(drawList, center1, 100.0f, time, amplitude, healthPerc,currentHealth,EntryName,level,exp);
    //DrawGlowingRect(drawList,barStart, barEnd, IM_COL32(255, 255, 255, 255));

    // HEALTH TEXT
    /*if (currentHealth > 0)
    {
        char healthText[32];
        sprintf(healthText, "%d", currentHealth);
        ImVec2 textSize = ImGui::CalcTextSize(healthText);
        ImVec2 textPos = ImVec2(barStart.x + (barWidth - textSize.x) * 0.5f, filledStart.y - textSize.y);
        DrawGlowingText(drawList, textPos, ImColor(255, 255, 255), healthText);
    }*/

    // NAME, LEVEL, EXP
    /*char entryStr[64], levelStr[64], expStr[64];
    snprintf(entryStr, sizeof(entryStr), "%s", EntryName);
    snprintf(levelStr, sizeof(levelStr), "Level %u", *level);
    snprintf(expStr, sizeof(expStr), "EXP %u", *exp);

    float spacing = 24.0f;
    ImVec2 textPos = ImVec2(minScreen.x, minScreen.y - spacing * 3);
    DrawGlowingText(drawList, textPos, ImColor(255, 255, 255), entryStr);
    DrawGlowingText(drawList, ImVec2(textPos.x, textPos.y + spacing), ImColor(255, 255, 255), levelStr);
    DrawGlowingText(drawList, ImVec2(textPos.x, textPos.y + spacing * 2), ImColor(255, 255, 255), expStr);*/
}





void DrawBillboardForNonEnemy(Entity* entity, const ImU32 baseColor, int currentHealth, int maxHealth, const char* EntryName, const XMMATRIX& viewMatrix, float fov, float aspectRatio, float nearZ, float farZ, float viewportWidth, float viewportHeight)
{
    if (!entity) return;

    // Get the entity's team
    int team = *(reinterpret_cast<int*>(reinterpret_cast<char*>(entity) + 0x4DC)); // Adjust offset if necessary

    // Use the baseColor, but change it based on the team within the function
    ImU32 color = baseColor;  // Default base color
    switch (team) {
    case 1:
        color = IM_COL32(0, 0, 255, 255); // Green for player
        break;
    case 2:
        color = IM_COL32(255, 0, 0, 255); // Red for enemy
        break;
    case 3:
        color = IM_COL32(255, 255, 0, 255); // Yellow for gimmick
        break;
    case 4:
        color = IM_COL32(0, 0, 255, 255); // Blue for other
        break;
    default:
        color = IM_COL32(255, 255, 255, 255); // White for neutral/unknown
        break;
    }

    ImDrawList* drawList = ImGui::GetBackgroundDrawList();

    // Calculate AABB center
    const Vector3 aabbMin = entity->RBBOXMIN;
    const Vector3 aabbMax = entity->RBBOXMAX;

    if (std::isnan(aabbMin.x) || std::isnan(aabbMin.y) || std::isnan(aabbMin.z) ||
        std::isnan(aabbMax.x) || std::isnan(aabbMax.y) || std::isnan(aabbMax.z) ||
        std::isinf(aabbMin.x) || std::isinf(aabbMin.y) || std::isinf(aabbMin.z) ||
        std::isinf(aabbMax.x) || std::isinf(aabbMax.y) || std::isinf(aabbMax.z)) {
        return; // Skip drawing if AABB values are invalid
    }

    Vector3 center = {
        (aabbMin.x + aabbMax.x) * 0.5f,
        (aabbMin.y + aabbMax.y) * 0.5f,
        (aabbMin.z + aabbMax.z) * 0.5f
    };

    // Define AABB corners in world space
    Vector3 corners[8] = {
        { aabbMin.x, aabbMin.y, aabbMin.z },
        { aabbMax.x, aabbMin.y, aabbMin.z },
        { aabbMin.x, aabbMax.y, aabbMin.z },
        { aabbMax.x, aabbMax.y, aabbMin.z },
        { aabbMin.x, aabbMin.y, aabbMax.z },
        { aabbMax.x, aabbMin.y, aabbMax.z },
        { aabbMin.x, aabbMax.y, aabbMax.z },
        { aabbMax.x, aabbMax.y, aabbMax.z }
    };

    ImVec2 screenPos[8];
    bool validProjection = true;

    // Project AABB corners into screen space
    for (int i = 0; i < 8; ++i) {
        XMVECTOR worldPos = XMVectorSet(corners[i].x, corners[i].y, corners[i].z, 1.0f);
        XMVECTOR projectedPos;
        if (!WorldToScreen(worldPos, viewMatrix, fov, aspectRatio, nearZ, farZ, viewportWidth, viewportHeight, projectedPos)) {
            validProjection = false;
            break;
        }
        screenPos[i] = ImVec2(XMVectorGetX(projectedPos), XMVectorGetY(projectedPos));
    }

    if (!validProjection) return;

    // Find the min and max screen positions for the AABB
    ImVec2 minScreenPos = screenPos[0];
    ImVec2 maxScreenPos = screenPos[0];
    for (int i = 1; i < 8; ++i) {
        minScreenPos.x = std::min(minScreenPos.x, screenPos[i].x);
        minScreenPos.y = std::min(minScreenPos.y, screenPos[i].y);
        maxScreenPos.x = std::max(maxScreenPos.x, screenPos[i].x);
        maxScreenPos.y = std::max(maxScreenPos.y, screenPos[i].y);
    }

    // Draw the outline of the billboard using the team-based color
    //drawList->AddRectFilled(minScreenPos, maxScreenPos, IM_COL32(7,211,235,127), 0.0f, 0.0f);  // Semi-transparent filled box
    drawList->AddRect(minScreenPos, maxScreenPos, color, 0.0f, 0, 3.0f); // Outline

    // Calculate health percentage
    float healthPercentage = (float)currentHealth / (float)maxHealth;

    // Determine health bar position and size
    float healthBarWidth = 5.0f; // Width of the health bar

    ImVec2 healthBarStart = { maxScreenPos.x + 5.0f, minScreenPos.y };
    ImVec2 healthBarEnd = { maxScreenPos.x + 5.0f + healthBarWidth, maxScreenPos.y };

    // Calculate filled portion of the health bar based on the current health
    float healthHeight = (maxScreenPos.y - minScreenPos.y) * healthPercentage;

    // Draw the filled portion of the health bar (current health)
    ImVec2 filledHealthBarEnd = { healthBarEnd.x, healthBarStart.y + healthHeight };
    drawList->AddRectFilled(healthBarStart, filledHealthBarEnd, IM_COL32(0, 255, 0, 255));  // Green filled area
    drawList->AddRect(healthBarStart, healthBarEnd, IM_COL32(255, 255, 255, 255));  // Outline of the health bar

    //char LevelStr[68];
    char EntryNameStr[32];
    //char expStr[68];
    snprintf(EntryNameStr, sizeof(EntryNameStr), "%s", EntryName);


    // Define the vertical spacing between text labels
    float textSpacing = 24.0f;

    // Define the position for the first line of text (above the 2D box)
    ImVec2 textPos = ImVec2(minScreenPos.x, minScreenPos.y - textSpacing * 3); // Adjust the y-offset to place it above the ESP box

    // Draw the text labels in order (EntryName, Level, and EXP)
    drawList->AddText(textPos, White, EntryNameStr);
    //drawList->AddText(ImVec2(textPos.x, textPos.y + textSpacing), White, LevelStr);
    //drawList->AddText(ImVec2(textPos.x, textPos.y + 2 * textSpacing), White, expStr);

}

void DrawBillboardForNpcs(Entity* entity, const ImU32 baseColor, const char* EntryName, const XMMATRIX& viewMatrix, float fov, float aspectRatio, float nearZ, float farZ, float viewportWidth, float viewportHeight)
{
    if (!entity) return;

    // Get the entity's team
    int team = *(reinterpret_cast<int*>(reinterpret_cast<char*>(entity) + 0x4DC)); // Adjust offset if necessary

    // Use the baseColor, but change it based on the team within the function
    ImU32 color = baseColor;  // Default base color
    switch (team) {
    case 1:
        color = IM_COL32(0, 0, 255, 255); // Green for player
        break;
    case 2:
        color = IM_COL32(255, 0, 0, 255); // Red for enemy
        break;
    case 3:
        color = IM_COL32(255, 255, 0, 255); // Yellow for gimmick
        break;
    case 4:
        color = IM_COL32(0, 0, 255, 255); // Blue for other
        break;
    default:
        color = IM_COL32(255, 255, 255, 255); // White for neutral/unknown
        break;
    }

    ImDrawList* drawList = ImGui::GetBackgroundDrawList();

    // Calculate AABB center
    const Vector3 aabbMin = entity->RBBOXMIN;
    const Vector3 aabbMax = entity->RBBOXMAX;

    if (std::isnan(aabbMin.x) || std::isnan(aabbMin.y) || std::isnan(aabbMin.z) ||
        std::isnan(aabbMax.x) || std::isnan(aabbMax.y) || std::isnan(aabbMax.z) ||
        std::isinf(aabbMin.x) || std::isinf(aabbMin.y) || std::isinf(aabbMin.z) ||
        std::isinf(aabbMax.x) || std::isinf(aabbMax.y) || std::isinf(aabbMax.z)) {
        return; // Skip drawing if AABB values are invalid
    }

    Vector3 center = {
        (aabbMin.x + aabbMax.x) * 0.5f,
        (aabbMin.y + aabbMax.y) * 0.5f,
        (aabbMin.z + aabbMax.z) * 0.5f
    };

    // Define AABB corners in world space
    Vector3 corners[8] = {
        { aabbMin.x, aabbMin.y, aabbMin.z },
        { aabbMax.x, aabbMin.y, aabbMin.z },
        { aabbMin.x, aabbMax.y, aabbMin.z },
        { aabbMax.x, aabbMax.y, aabbMin.z },
        { aabbMin.x, aabbMin.y, aabbMax.z },
        { aabbMax.x, aabbMin.y, aabbMax.z },
        { aabbMin.x, aabbMax.y, aabbMax.z },
        { aabbMax.x, aabbMax.y, aabbMax.z }
    };

    ImVec2 screenPos[8];
    bool validProjection = true;

    // Project AABB corners into screen space
    for (int i = 0; i < 8; ++i) {
        XMVECTOR worldPos = XMVectorSet(corners[i].x, corners[i].y, corners[i].z, 1.0f);
        XMVECTOR projectedPos;
        if (!WorldToScreen(worldPos, viewMatrix, fov, aspectRatio, nearZ, farZ, viewportWidth, viewportHeight, projectedPos)) {
            validProjection = false;
            break;
        }
        screenPos[i] = ImVec2(XMVectorGetX(projectedPos), XMVectorGetY(projectedPos));
    }

    if (!validProjection) return;

    // Find the min and max screen positions for the AABB
    ImVec2 minScreenPos = screenPos[0];
    ImVec2 maxScreenPos = screenPos[0];
    for (int i = 1; i < 8; ++i) {
        minScreenPos.x = std::min(minScreenPos.x, screenPos[i].x);
        minScreenPos.y = std::min(minScreenPos.y, screenPos[i].y);
        maxScreenPos.x = std::max(maxScreenPos.x, screenPos[i].x);
        maxScreenPos.y = std::max(maxScreenPos.y, screenPos[i].y);
    }

    // Draw the outline of the billboard using the team-based color
    //drawList->AddRectFilled(minScreenPos, maxScreenPos, IM_COL32(7,211,235,127), 0.0f, 0.0f);  // Semi-transparent filled box
    drawList->AddRect(minScreenPos, maxScreenPos, color, 0.0f, 0, 3.0f); // Outline

    //char LevelStr[68];
    char EntryNameStr[32];
    //char expStr[68];
    snprintf(EntryNameStr, sizeof(EntryNameStr), "%s", EntryName);


    // Define the vertical spacing between text labels
    float textSpacing = 24.0f;

    // Define the position for the first line of text (above the 2D box)
    ImVec2 textPos = ImVec2(minScreenPos.x, minScreenPos.y - textSpacing * 3); // Adjust the y-offset to place it above the ESP box

    // Draw the text labels in order (EntryName, Level, and EXP)
    drawList->AddText(textPos, White, EntryNameStr);
    //drawList->AddText(ImVec2(textPos.x, textPos.y + textSpacing), White, LevelStr);
    //drawList->AddText(ImVec2(textPos.x, textPos.y + 2 * textSpacing), White, expStr);

}

void DrawPngBillboardForEntity(
    Entity* entity,
    const Dx12ImguiTexture& tex,   // your loaded PNG texture (gpuHandle, width, height)
    const XMMATRIX& viewMatrix,
    float fov, float aspectRatio, float nearZ, float farZ,
    float viewportWidth, float viewportHeight)
{
    if (!entity) return;
    if (!tex.gpuHandle.ptr) return;

    ImDrawList* dl = ImGui::GetForegroundDrawList();

    // --- world AABB ---
    const Vector3 aabbMin = entity->RBBOXMIN;
    const Vector3 aabbMax = entity->RBBOXMAX;

    auto bad = [](float v) { return std::isnan(v) || std::isinf(v); };
    if (bad(aabbMin.x) || bad(aabbMin.y) || bad(aabbMin.z) || bad(aabbMax.x) || bad(aabbMax.y) || bad(aabbMax.z))
        return;

    Vector3 corners[8] = {
        { aabbMin.x, aabbMin.y, aabbMin.z },
        { aabbMax.x, aabbMin.y, aabbMin.z },
        { aabbMin.x, aabbMax.y, aabbMin.z },
        { aabbMax.x, aabbMax.y, aabbMin.z },
        { aabbMin.x, aabbMin.y, aabbMax.z },
        { aabbMax.x, aabbMin.y, aabbMax.z },
        { aabbMin.x, aabbMax.y, aabbMax.z },
        { aabbMax.x, aabbMax.y, aabbMax.z }
    };

    ImVec2 screenPos[8];
    bool ok = true;

    for (int i = 0; i < 8; ++i)
    {
        XMVECTOR worldPos = XMVectorSet(corners[i].x, corners[i].y, corners[i].z, 1.0f);
        XMVECTOR projectedPos;

        if (!WorldToScreen(worldPos, viewMatrix, fov, aspectRatio, nearZ, farZ,
            viewportWidth, viewportHeight, projectedPos))
        {
            ok = false;
            break;
        }

        screenPos[i] = ImVec2(XMVectorGetX(projectedPos), XMVectorGetY(projectedPos));
    }

    if (!ok) return;

    // --- build 2D rectangle from projected AABB ---
    ImVec2 minP = screenPos[0];
    ImVec2 maxP = screenPos[0];
    for (int i = 1; i < 8; ++i)
    {
        minP.x = (std::min)(minP.x, screenPos[i].x);
        minP.y = (std::min)(minP.y, screenPos[i].y);
        maxP.x = (std::max)(maxP.x, screenPos[i].x);
        maxP.y = (std::max)(maxP.y, screenPos[i].y);
    }

    // sanity: reject degenerate rectangles
    if ((maxP.x - minP.x) < 2.0f || (maxP.y - minP.y) < 2.0f)
        return;

    

    // --- aspect ratio handling (optional) ---
    ImVec2 p0 = minP;
    ImVec2 p1 = maxP;

    

    // --- draw the PNG as a billboard ---
    dl->AddImage(
        (ImTextureID)tex.gpuHandle.ptr,
        p0,
        p1,
        ImVec2(0, 0),
        ImVec2(1, 1),
        IM_COL32(255, 255, 255, 255)
    );
}


bool IsPointInsideAABB(const Vector3& point, const Vector3& min, const Vector3& max) {
    return (point.x >= min.x && point.x <= max.x &&
        point.y >= min.y && point.y <= max.y &&
        point.z >= min.z && point.z <= max.z);
}


void Draw3DBoxAroundEntity(Entity* entity, const ImU32 baseColor, const char* EntryName,
    const XMMATRIX& cameraMatrix, float fov, float aspectRatio, float nearZ, float farZ,
    float viewportWidth, float viewportHeight)
{
    if (!entity) return;

    char* entityBase = reinterpret_cast<char*>(entity);

    int team = *(reinterpret_cast<int*>(entityBase + 0x4DC));
    ImU32 color = baseColor;
    switch (team) {
    case 1: color = IM_COL32(0, 0, 255, 255);   break;
    case 2: color = IM_COL32(255, 0, 0, 255);   break;
    case 3: color = IM_COL32(255, 255, 0, 255); break;
    case 4: color = IM_COL32(0, 0, 255, 255);   break;
    default: color = IM_COL32(255, 255, 255, 255); break;
    }

    ImDrawList* drawList = ImGui::GetBackgroundDrawList();

    // XZ from RBBOXMIN/MAX — world space, correct
    float rbMinX = *reinterpret_cast<float*>(entityBase + 0x898);
    float rbMinZ = *reinterpret_cast<float*>(entityBase + 0x8A0);
    float rbMaxX = *reinterpret_cast<float*>(entityBase + 0x8A4);
    float rbMaxZ = *reinterpret_cast<float*>(entityBase + 0x8AC);

    // Y from Motion.BoundingBox — local space, camera-stable, but stored negated
    // worldMinY = ty - motMaxY,  worldMaxY = ty - motMinY
    float motMinY = *reinterpret_cast<float*>(entityBase + 0x28C);
    float motMaxY = *reinterpret_cast<float*>(entityBase + 0x298);

    XMMATRIX entityWorld = *reinterpret_cast<XMMATRIX*>(entityBase + 0x40);
    float ty = entityWorld.r[3].m128_f32[1];

    float worldMinY = ty + motMaxY;
    float worldMaxY = ty + motMinY;

    if (std::isnan(rbMinX) || std::isnan(rbMinZ) || std::isnan(rbMaxX) || std::isnan(rbMaxZ) ||
        std::isnan(worldMinY) || std::isnan(worldMaxY) ||
        std::isinf(rbMinX) || std::isinf(rbMinZ) || std::isinf(rbMaxX) || std::isinf(rbMaxZ) ||
        std::isinf(worldMinY) || std::isinf(worldMaxY)) {
        return;
    }

    Vector3 corners[8] = {
        { rbMinX, worldMinY, rbMinZ },
        { rbMaxX, worldMinY, rbMinZ },
        { rbMinX, worldMaxY, rbMinZ },
        { rbMaxX, worldMaxY, rbMinZ },
        { rbMinX, worldMinY, rbMaxZ },
        { rbMaxX, worldMinY, rbMaxZ },
        { rbMinX, worldMaxY, rbMaxZ },
        { rbMaxX, worldMaxY, rbMaxZ }
    };

    ImVec2 screenPos[8];
    for (int i = 0; i < 8; ++i) {
        XMVECTOR worldPos = XMVectorSet(corners[i].x, corners[i].y, corners[i].z, 1.0f);
        XMVECTOR projected;
        if (!NewWorldToScreen(worldPos, projected, viewportWidth, viewportHeight))
            return;
        screenPos[i] = ImVec2(XMVectorGetX(projected), XMVectorGetY(projected));
    }

    drawList->AddLine(screenPos[0], screenPos[1], color, 2.0f);
    drawList->AddLine(screenPos[1], screenPos[3], color, 2.0f);
    drawList->AddLine(screenPos[3], screenPos[2], color, 2.0f);
    drawList->AddLine(screenPos[2], screenPos[0], color, 2.0f);
    drawList->AddLine(screenPos[4], screenPos[5], color, 2.0f);
    drawList->AddLine(screenPos[5], screenPos[7], color, 2.0f);
    drawList->AddLine(screenPos[7], screenPos[6], color, 2.0f);
    drawList->AddLine(screenPos[6], screenPos[4], color, 2.0f);
    drawList->AddLine(screenPos[0], screenPos[4], color, 2.0f);
    drawList->AddLine(screenPos[1], screenPos[5], color, 2.0f);
    drawList->AddLine(screenPos[2], screenPos[6], color, 2.0f);
    drawList->AddLine(screenPos[3], screenPos[7], color, 2.0f);

    XMVECTOR topCenter = XMVectorSet(
        (corners[4].x + corners[5].x + corners[6].x + corners[7].x) * 0.25f,
        (corners[4].y + corners[5].y + corners[6].y + corners[7].y) * 0.25f,
        (corners[4].z + corners[5].z + corners[6].z + corners[7].z) * 0.25f,
        1.0f);

    XMVECTOR topProjected;
    if (NewWorldToScreen(topCenter, topProjected, viewportWidth, viewportHeight)) {
        ImVec2 textPos = ImVec2(XMVectorGetX(topProjected), XMVectorGetY(topProjected) - 15.0f);
        drawList->AddText(textPos, IM_COL32(255, 255, 255, 255), EntryName);
    }
}

void Draw3DBoxAtPosition(const Vector3& worldPos, const Vector3& boxSize, const ImU32 baseColor, const char* EntryName, const XMMATRIX& viewMatrix, float fov, float aspectRatio, float nearZ, float farZ, float viewportWidth, float viewportHeight)
{
    if (std::isnan(worldPos.x) || std::isnan(worldPos.y) || std::isnan(worldPos.z) ||
        std::isinf(worldPos.x) || std::isinf(worldPos.y) || std::isinf(worldPos.z) ||
        std::isnan(boxSize.x) || std::isnan(boxSize.y) || std::isnan(boxSize.z) ||
        std::isinf(boxSize.x) || std::isinf(boxSize.y) || std::isinf(boxSize.z)) {
        return; // Skip drawing if position or box size values are invalid
    }

    ImDrawList* drawList = ImGui::GetBackgroundDrawList();

    // Define half extents based on the box size
    Vector3 halfSize = { boxSize.x * 0.5f, boxSize.y * 0.5f, boxSize.z * 0.5f };

    // Define AABB corners relative to the world position
    Vector3 corners[8] = {
        { worldPos.x - halfSize.x, worldPos.y - halfSize.y, worldPos.z - halfSize.z },
        { worldPos.x + halfSize.x, worldPos.y - halfSize.y, worldPos.z - halfSize.z },
        { worldPos.x - halfSize.x, worldPos.y + halfSize.y, worldPos.z - halfSize.z },
        { worldPos.x + halfSize.x, worldPos.y + halfSize.y, worldPos.z - halfSize.z },
        { worldPos.x - halfSize.x, worldPos.y - halfSize.y, worldPos.z + halfSize.z },
        { worldPos.x + halfSize.x, worldPos.y - halfSize.y, worldPos.z + halfSize.z },
        { worldPos.x - halfSize.x, worldPos.y + halfSize.y, worldPos.z + halfSize.z },
        { worldPos.x + halfSize.x, worldPos.y + halfSize.y, worldPos.z + halfSize.z }
    };

    ImVec2 screenPos[8];
    bool validProjection = true;

    // Project AABB corners into screen space
    for (int i = 0; i < 8; ++i) {
        XMVECTOR cornerWorldPos = XMVectorSet(corners[i].x, corners[i].y, corners[i].z, 1.0f);
        XMVECTOR projectedPos;

        if (!WorldToScreen(cornerWorldPos, viewMatrix, fov, aspectRatio, nearZ, farZ, viewportWidth, viewportHeight, projectedPos)) {
            validProjection = false;
            break;
        }
        screenPos[i] = ImVec2(XMVectorGetX(projectedPos), XMVectorGetY(projectedPos));
    }

    if (!validProjection) return;

    // Draw 3D box outline by connecting the corners
    drawList->AddLine(screenPos[0], screenPos[1], baseColor, 2.0f); // Bottom front
    drawList->AddLine(screenPos[1], screenPos[3], baseColor, 2.0f); // Bottom right front
    drawList->AddLine(screenPos[3], screenPos[2], baseColor, 2.0f); // Bottom right back
    drawList->AddLine(screenPos[2], screenPos[0], baseColor, 2.0f); // Bottom left back

    drawList->AddLine(screenPos[4], screenPos[5], baseColor, 2.0f); // Top front
    drawList->AddLine(screenPos[5], screenPos[7], baseColor, 2.0f); // Top right front
    drawList->AddLine(screenPos[7], screenPos[6], baseColor, 2.0f); // Top right back
    drawList->AddLine(screenPos[6], screenPos[4], baseColor, 2.0f); // Top left back

    drawList->AddLine(screenPos[0], screenPos[4], baseColor, 2.0f); // Front left vertical
    drawList->AddLine(screenPos[1], screenPos[5], baseColor, 2.0f); // Front right vertical
    drawList->AddLine(screenPos[2], screenPos[6], baseColor, 2.0f); // Back left vertical
    drawList->AddLine(screenPos[3], screenPos[7], baseColor, 2.0f); // Back right vertical

    // Project the center of the top face (average of top 4 corners)
    XMVECTOR topCenterWorldPos = XMVectorSet(
        (corners[4].x + corners[5].x + corners[6].x + corners[7].x) * 0.25f,
        (corners[4].y + corners[5].y + corners[6].y + corners[7].y) * 0.25f,
        (corners[4].z + corners[5].z + corners[6].z + corners[7].z) * 0.25f,
        1.0f);

    // Project this center point into screen space
    XMVECTOR topCenterProjected;
    if (WorldToScreen(topCenterWorldPos, viewMatrix, fov, aspectRatio, nearZ, farZ, viewportWidth, viewportHeight, topCenterProjected)) {
        ImVec2 topCenterScreenPos(XMVectorGetX(topCenterProjected), XMVectorGetY(topCenterProjected));

        // Offset the text position slightly above the 3D box
        float textOffset = 15.0f; // Adjust this value to control how far above the box the text appears
        ImVec2 textPos = ImVec2(topCenterScreenPos.x, topCenterScreenPos.y - textOffset);

        // Draw the entity name above the 3D box
        drawList->AddText(textPos, IM_COL32(255, 255, 255, 255), EntryName);
    }
}








void Draw3DBoxAroundZone(const ImU32 baseColor, const XMMATRIX& viewMatrix, const Vector3& zoneMin, const Vector3& zoneMax,
    float fov, float aspectRatio, float nearZ, float farZ,
    float viewportWidth, float viewportHeight)
{
    ImDrawList* drawList = ImGui::GetBackgroundDrawList();

    // Define the corners of the zone box in world space
    Vector3 corners[8] = {
        { zoneMin.x, zoneMin.y, zoneMin.z },
        { zoneMax.x, zoneMin.y, zoneMin.z },
        { zoneMin.x, zoneMax.y, zoneMin.z },
        { zoneMax.x, zoneMax.y, zoneMin.z },
        { zoneMin.x, zoneMin.y, zoneMax.z },
        { zoneMax.x, zoneMin.y, zoneMax.z },
        { zoneMin.x, zoneMax.y, zoneMax.z },
        { zoneMax.x, zoneMax.y, zoneMax.z }
    };

    ImVec2 screenPos[8];
    bool validProjection = true;

    // Project zone corners into screen space
    for (int i = 0; i < 8; ++i) {
        XMVECTOR worldPos = XMVectorSet(corners[i].x, corners[i].y, corners[i].z, 1.0f);
        XMVECTOR projectedPos;

        // Assuming WorldToScreen is already defined to project world coordinates to screen space
        if (!WorldToScreen(worldPos, viewMatrix, fov, aspectRatio, nearZ, farZ, viewportWidth, viewportHeight, projectedPos)) {
            validProjection = false;
            break;
        }
        screenPos[i] = ImVec2(XMVectorGetX(projectedPos), XMVectorGetY(projectedPos));
    }

    if (!validProjection) return;

    // Draw 3D box outline by connecting the corners
    drawList->AddLine(screenPos[0], screenPos[1], baseColor, 2.0f); // Bottom front
    drawList->AddLine(screenPos[1], screenPos[3], baseColor, 2.0f); // Bottom right front
    drawList->AddLine(screenPos[3], screenPos[2], baseColor, 2.0f); // Bottom right back
    drawList->AddLine(screenPos[2], screenPos[0], baseColor, 2.0f); // Bottom left back

    drawList->AddLine(screenPos[4], screenPos[5], baseColor, 2.0f); // Top front
    drawList->AddLine(screenPos[5], screenPos[7], baseColor, 2.0f); // Top right front
    drawList->AddLine(screenPos[7], screenPos[6], baseColor, 2.0f); // Top right back
    drawList->AddLine(screenPos[6], screenPos[4], baseColor, 2.0f); // Top left back

    drawList->AddLine(screenPos[0], screenPos[4], baseColor, 2.0f); // Front left vertical
    drawList->AddLine(screenPos[1], screenPos[5], baseColor, 2.0f); // Front right vertical
    drawList->AddLine(screenPos[2], screenPos[6], baseColor, 2.0f); // Back left vertical
    drawList->AddLine(screenPos[3], screenPos[7], baseColor, 2.0f); // Back right vertical
}

void Draw3DCylinderAroundZone(const ImU32 baseColor, const XMMATRIX& viewMatrix, float fov, float aspectRatio, float nearZ, float farZ, float viewportWidth, float viewportHeight)
{
    const UNIT_DATA_ZONE* zoneData = g_unitGroup->ZoneData;

    if (zoneData->Shape != SHAPE_CYLINDER) {
        return; // Skip drawing if the shape is not a cylinder
    }

    ImDrawList* drawList = ImGui::GetBackgroundDrawList();

    // Cylinder center and size from zone data
    float zonePosX = zoneData->Xpos;
    float zonePosY = zoneData->Ypos;
    float zonePosZ = zoneData->Zpos;
    float radius = zoneData->Xsize / 2; // Assuming Xsize is the diameter
    float height = zoneData->Ysize;

    // Define points around the base of the cylinder
    const int segments = 32; // Number of segments to approximate the cylinder
    Vector3 basePoints[segments];
    for (int i = 0; i < segments; ++i) {
        float angle = (i * 2.0f * XM_PI) / segments;
        basePoints[i] = {
            radius * cos(angle),
            0, // Base at Y=0
            radius * sin(angle)
        };
    }

    // Apply rotation and translation to the base points
    XMMATRIX rotationMatrix = DirectX::XMMatrixRotationRollPitchYaw(XMConvertToRadians(zoneData->Yrot), XMConvertToRadians(zoneData->Xrot), XMConvertToRadians(zoneData->Zrot));
    for (int i = 0; i < segments; ++i) {
        XMVECTOR basePoint = XMVector3Transform(XMVectorSet(basePoints[i].x, basePoints[i].y, basePoints[i].z, 1.0f), rotationMatrix);
        basePoints[i] = { XMVectorGetX(basePoint) + zonePosX, XMVectorGetY(basePoint) + zonePosY, XMVectorGetZ(basePoint) + zonePosZ };
    }

    // Project base points into screen space
    ImVec2 screenBasePoints[segments];
    for (int i = 0; i < segments; ++i) {
        XMVECTOR worldPos = XMVectorSet(basePoints[i].x, basePoints[i].y, basePoints[i].z, 1.0f);
        XMVECTOR projectedPos;
        if (!WorldToScreen(worldPos, viewMatrix, fov, aspectRatio, nearZ, farZ, viewportWidth, viewportHeight, projectedPos)) {
            return; // Skip drawing if projection fails
        }
        screenBasePoints[i] = ImVec2(XMVectorGetX(projectedPos), XMVectorGetY(projectedPos));
    }

    // Draw cylinder base outline
    for (int i = 0; i < segments; ++i) {
        drawList->AddLine(screenBasePoints[i], screenBasePoints[(i + 1) % segments], baseColor, 2.0f); // Connect the base points
    }

    // Draw the vertical lines of the cylinder
    for (int i = 0; i < segments; ++i) {
        drawList->AddLine(screenBasePoints[i], ImVec2(screenBasePoints[i].x, screenBasePoints[i].y - height), baseColor, 2.0f);
    }

    // Draw the top outline
    for (int i = 0; i < segments; ++i) {
        drawList->AddLine(ImVec2(screenBasePoints[i].x, screenBasePoints[i].y - height), ImVec2(screenBasePoints[(i + 1) % segments].x, screenBasePoints[(i + 1) % segments].y - height), baseColor, 2.0f); // Connect the top points
    }

    // Optionally, you can project the center of the top face for labeling
    XMVECTOR topCenterWorldPos = XMVectorSet(zonePosX, zonePosY + height, zonePosZ, 1.0f);
    XMVECTOR topCenterProjected;
    if (WorldToScreen(topCenterWorldPos, viewMatrix, fov, aspectRatio, nearZ, farZ, viewportWidth, viewportHeight, topCenterProjected)) {
        ImVec2 topCenterScreenPos(XMVectorGetX(topCenterProjected), XMVectorGetY(topCenterProjected));
        float textOffset = 15.0f; // Adjust this value as needed
        ImVec2 textPos = ImVec2(topCenterScreenPos.x, topCenterScreenPos.y - textOffset);

        // Draw the entity name above the cylinder (replace with actual name if needed)
        // drawList->AddText(textPos, IM_COL32(255, 255, 255, 255), "Cylinder Zone");
    }
}


float GetAnimationDuration(Entity* entity) {
    // Calculate the animation duration using ent_current_anim_frames and ent_max_anim_frames
    float entityMotionMaxDuration = *(float*)(entity + 0x198);
    float entityMotionCurrentDuration = *(float*)(entity + 0x19C);

    // Ensure that maxFrames is not zero to avoid division by zero
    if (entityMotionMaxDuration > 0) {
        return (entityMotionCurrentDuration / entityMotionMaxDuration);
    }
    else {
        return 0.0f; // Handle the case where maxFrames is zero (optional)
    }
}


void DrawSpeechBubble(ImDrawList* drawList, ImVec2 center, const char* text, const ImVec4& bubbleColor, const ImVec4& textColor) {
    // Calculate text size
    ImVec2 textSize = ImGui::CalcTextSize(text);

    // Define bubble size and padding
    const float padding = 10.0f;
    ImVec2 bubbleSize = ImVec2(textSize.x + padding * 2, textSize.y + padding * 2);

    // Calculate bubble position
    ImVec2 bubblePos = ImVec2(center.x - bubbleSize.x / 2, center.y - bubbleSize.y / 2);

    // Convert colors
    ImU32 colBubble = ImGui::ColorConvertFloat4ToU32(bubbleColor);
    ImU32 colText = ImGui::ColorConvertFloat4ToU32(textColor);

    // Draw rounded rectangle for the bubble
    drawList->AddRectFilled(bubblePos, ImVec2(bubblePos.x + bubbleSize.x, bubblePos.y + bubbleSize.y), colBubble, 10.0f);

    // Calculate and draw the tail
    ImVec2 tailV1 = ImVec2(center.x, bubblePos.y + bubbleSize.y);
    ImVec2 tailV2 = ImVec2(center.x - 10.0f, bubblePos.y + bubbleSize.y + 20.0f);
    ImVec2 tailV3 = ImVec2(center.x + 10.0f, bubblePos.y + bubbleSize.y + 20.0f);
    drawList->AddTriangleFilled(tailV1, tailV2, tailV3, colBubble);

    // Draw the text
    ImVec2 textPos = ImVec2(bubblePos.x + padding, bubblePos.y + padding);
    drawList->AddText(textPos, colText, text);
}

void AddCaption(const std::string& entityName, const std::string& displayEntityName, const std::string& message, int animID, Entity* entity) {
    // The key is a pair of entity name and animation ID
    std::pair<std::string, int> key = std::make_pair(entityName, animID);

    // Determine the duration based on the animation ID and the entity
    float duration = GetAnimationDuration(entity);

    // Create the caption object
    Caption newCaption = { entityName, displayEntityName, message, duration, duration, animID };

    // Insert into the map
    captionMap[key].push_back(newCaption);
}

void DrawCaptions(ImDrawList* drawList) {
    InitializePointers();
    XMMATRIX viewMatrix = XMLoadFloat4x4(reinterpret_cast<XMFLOAT4X4*>(&Matrix));
    float fov = XMConvertToRadians(90.0f);
    float nearZ = 1.0f;
    float farZ = 4000000.0f;
    //uintptr_t maxEntitiesAddress = moduleBase + 0x2967CA8;

    //int maxEntities = *(int*)maxEntitiesAddress;
    //auto Viewport = Memory::Internal::read<DirectX::XMFLOAT2>(moduleBase + 0x22B7298);

    std::random_device rd;
    std::mt19937 gen(rd());

    for (auto& captionPair : captionMap) {
        const auto& key = captionPair.first;
        auto& captions = captionPair.second;

        // Initialize selectedCaption outside of the loop
        YS::OBJ* i;
        


        for (i = EachOBJFunction(nullptr); i; i = EachOBJFunction(i)) {

            Entity* entity = reinterpret_cast<Entity*>(i);
            DirectX::XMVECTOR entPosVec = DirectX::XMLoadFloat4(reinterpret_cast<const DirectX::XMFLOAT4*>(&entity->ent_pos));

            Caption* selectedCaption = nullptr;
            float entityMotionMaxDuration = *(float*)(i + 0x198);
            float entityMotionCurrentDuration = *(float*)(i + 0x19C);
            unsigned char entityMotionID = *(char*)(i + 0x180);

            Vector2* viewport = 0;

            if (DetectGameVersion() == "Steam") {
                viewport = reinterpret_cast<Vector2*>(SteamPointer - 0x26CC18);

            }
            else if (DetectGameVersion() == "Epic") {
                viewport = reinterpret_cast<Vector2*>(EpicPointer - 0x26CC18);
            }


            Vector2 myViewport = *viewport; // Dereference the pointer to read the value.

            float aspectX = myViewport.x;
            float aspectY = myViewport.y;
            float aspectRatio = aspectX / aspectY;


            const char* names = GetOBJEntryNameFunction(reinterpret_cast<YS::OBJ*>(i));
            if (names) {
                //char* EntName = names->EntityName;

                if (std::string(names) == key.first && entityMotionID == key.second) {
                    if (!captions.empty()) {
                        // Select the caption once for this animation ID
                        if (!selectedCaption) {
                            std::uniform_int_distribution<> dis(0, captions.size() - 1);
                            int randomIndex = dis(gen);
                            selectedCaption = &captions[randomIndex];
                        }

                        if (selectedCaption->timeRemaining > 0) {
                            DirectX::XMFLOAT3 feetPos(entity->ent_pos.x, entity->ent_pos.y, entity->ent_pos.z);
                            DirectX::XMVECTOR feetPosVec = DirectX::XMLoadFloat3(&feetPos);
                            XMVECTOR screenPos;

                            if (WorldToScreen(feetPosVec, viewMatrix, fov, aspectRatio, nearZ, farZ, myViewport.x, myViewport.y, screenPos)) {
                                ImGui::PushFont(g_ComicHeartsFont);

                                // Define the text content
                                std::string fullCaption = "[" + selectedCaption->displayEntityName + "]\n" + selectedCaption->message;

                                // Calculate text base position (as before)
                                ImVec2 textBasePosition = ImVec2(XMVectorGetX(screenPos), XMVectorGetY(screenPos));
                                const ImVec2 textOffset(10.0f, -20.0f);
                                textBasePosition.x += textOffset.x;
                                textBasePosition.y += textOffset.y;

                                // Define colors for the speech bubble and text
                                ImVec4 bubbleColor = ImVec4(1.0f, 1.0f, 1.0f, 1.0f); // e.g., Black
                                ImVec4 textColor = ImVec4(0.0f, 0.0f, 0.0f, 1.0f); // e.g., White

                                // Draw the speech bubble with text
                                DrawSpeechBubble(drawList, textBasePosition, fullCaption.c_str(), bubbleColor, textColor);

                                ImGui::PopFont();
                            }
                        }
                    }
                    break;
                }
            }
        }

        // Remove captions that have expired
        for (auto& captionPair : captionMap) {
            auto& captions = captionPair.second;
            captions.erase(std::remove_if(captions.begin(), captions.end(),
                [](const Caption& c) { return c.timeRemaining <= 0.0f; }),
                captions.end());
        }
    }
}

Entity* FindEntityByName(const std::string& name) {
    //uintptr_t maxEntitiesAddress = moduleBase + 0x2967CA8;
    //YS::OBJ* i;
    
    //DirectX::XMVECTOR entPosVec = DirectX::XMLoadFloat4(reinterpret_cast<const DirectX::XMFLOAT4*>(&entity->ent_pos));


    for (YS::OBJ* i = EachOBJFunction(nullptr); i; i = EachOBJFunction(i)) {
        Entity* entity = reinterpret_cast<Entity*>(i);
        const char* names = GetOBJEntryNameFunction(reinterpret_cast<YS::OBJ*>(i));

        if (std::string(names) == name) {
            return entity; // Found a matching entity, return it
        }
    }
    
    
        
    

    // If no matching entity is found, return nullptr
    return nullptr;
}

void DrawCircle(ImDrawList* draw_list, const ImVec2& center, float radius, ImU32 color) {
    const int num_segments = 16; // Number of segments for the circle
    for (int i = 0; i < num_segments; i++) {
        float theta1 = i * (2.0f * IM_PI / num_segments);
        float theta2 = (i + 1) * (2.0f * IM_PI / num_segments);
        ImVec2 p1(center.x + radius * cosf(theta1), center.y + radius * sinf(theta1));
        ImVec2 p2(center.x + radius * cosf(theta2), center.y + radius * sinf(theta2));
        draw_list->AddTriangleFilled(center, p1, p2, color);
    }
}

void DrawBillboardMooseHead(const XMVECTOR& cameraPos, const XMMATRIX& viewMatrix, float fov, float aspectRatio, float nearZ, float farZ, float viewportWidth, float viewportHeight) {
    // Position of the moose head in world space
    XMVECTOR mooseHeadPosition = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f); // Adjust as needed for your scene

    // Draw edges for the moose head
    for (const auto& edge : mooseHeadEdges) {
        const Vector3& startVertex = mooseHeadVertices[edge.first];
        const Vector3& endVertex = mooseHeadVertices[edge.second];

        // Convert the vertices to XMVECTOR
        XMVECTOR startWorldPos = XMVectorSet(startVertex.x, startVertex.y, startVertex.z, 1.0f) + mooseHeadPosition;
        XMVECTOR endWorldPos = XMVectorSet(endVertex.x, endVertex.y, endVertex.z, 1.0f) + mooseHeadPosition;

        // Get screen positions using your WorldToScreen function
        XMVECTOR startScreenPos, endScreenPos;
        if (WorldToScreen(startWorldPos, viewMatrix, fov, aspectRatio, nearZ, farZ, viewportWidth, viewportHeight, startScreenPos) &&
            WorldToScreen(endWorldPos, viewMatrix, fov, aspectRatio, nearZ, farZ, viewportWidth, viewportHeight, endScreenPos)) {

            // Convert XMVECTOR to ImVec2 for ImGui drawing
            ImVec2 startImGuiPos(XMVectorGetX(startScreenPos), XMVectorGetY(startScreenPos));
            ImVec2 endImGuiPos(XMVectorGetX(endScreenPos), XMVectorGetY(endScreenPos));
            ImDrawList* draw_list = ImGui::GetBackgroundDrawList();
            // Draw the line using ImGui
            draw_list->AddLine(startImGuiPos, endImGuiPos, IM_COL32(255, 255, 255, 255), 1.0f);
        }
    }

    // Draw the eyes as circles
    ImDrawList* draw_list = ImGui::GetBackgroundDrawList();
    float eyeRadius = 0.05f; // Adjust eye size as needed
    XMVECTOR leftEyePos = XMVectorSet(-0.25f, -0.3f, 0.0f, 1.0f) + mooseHeadPosition;
    XMVECTOR rightEyePos = XMVectorSet(0.25f, -0.3f, 0.0f, 1.0f) + mooseHeadPosition;

    // Get screen positions for eyes
    XMVECTOR leftEyeScreenPos, rightEyeScreenPos;
    WorldToScreen(leftEyePos, viewMatrix, fov, aspectRatio, nearZ, farZ, viewportWidth, viewportHeight, leftEyeScreenPos);
    WorldToScreen(rightEyePos, viewMatrix, fov, aspectRatio, nearZ, farZ, viewportWidth, viewportHeight, rightEyeScreenPos);

    // Draw circles for eyes
    DrawCircle(draw_list, ImVec2(XMVectorGetX(leftEyeScreenPos), XMVectorGetY(leftEyeScreenPos)), eyeRadius, IM_COL32(0, 0, 0, 255)); // Left eye
    DrawCircle(draw_list, ImVec2(XMVectorGetX(rightEyeScreenPos), XMVectorGetY(rightEyeScreenPos)), eyeRadius, IM_COL32(0, 0, 0, 255)); // Right eye
}

















// Data

// Define the input layout for vertex data (position only in this case)
D3D12_INPUT_ELEMENT_DESC debugInputLayout[] =
{
    {
        "POSITION",                      // SemanticName
        0,                                // SemanticIndex
        DXGI_FORMAT_R32G32B32_FLOAT,      // Format (3 floats for position)
        0,                                // InputSlot
        0,                                // AlignedByteOffset (0 means tightly packed)
        D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, // InputSlotClass
        0                                 // InstanceDataStepRate
    }
};


void RenderVoteOverlay(bool canVoteNow, bool votesPaused) {
    // If voting not active at all, do not render overlay (close it)
    if (!voteManager.IsVotingActive()) return;

    // Begin window setup (same as before)
    const auto& options = voteManager.GetCurrentOptions();
    const auto& votes = voteManager.GetVoteCounts();

    int timeRemaining = voteManager.GetSecondsRemaining();
    int totalTime = voteManager.GetVoteDuration();
    float timeProgress = totalTime > 0 ? 1.0f - ((float)timeRemaining / (float)totalTime) : 0.0f;

    int totalVotes = 0;
    for (int v : votes) totalVotes += v;

    ImVec2 screenSize = ImGui::GetIO().DisplaySize;
    float fullWidth = screenSize.x;

    float boxHeight = 140.0f;
    float spacingBelowProgressBar = 10.0f;
    float progressBarHeight = 30.0f;
    float overlayTotalHeight = progressBarHeight + spacingBelowProgressBar + boxHeight + 50.0f;

    float boxSpacing = 5.0f;
    float boxWidth = options.size() > 0 ? (fullWidth - ((options.size() - 1) * boxSpacing)) / options.size() : fullWidth;

    ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_Always);
    ImGui::SetNextWindowSize(ImVec2(fullWidth, overlayTotalHeight), ImGuiCond_Always);
    ImGui::SetNextWindowBgAlpha(0.0f);
    ImGui::Begin("VotingOverlay", nullptr,
        ImGuiWindowFlags_NoTitleBar |
        ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoSavedSettings |
        ImGuiWindowFlags_NoFocusOnAppearing |
        ImGuiWindowFlags_NoNav |
        ImGuiWindowFlags_NoBackground);

    if (canVoteNow) {
        // Draw progress bar + time text
        ImGui::PushStyleColor(ImGuiCol_PlotHistogram, ImVec4(0.8f, 0.2f, 0.2f, 1.0f));
        ImGui::ProgressBar(timeProgress, ImVec2(fullWidth, progressBarHeight), "");
        ImGui::PopStyleColor();

        ImVec2 barStart = ImGui::GetItemRectMin();
        ImVec2 barEnd = ImGui::GetItemRectMax();
        ImVec2 barSize = ImVec2(barEnd.x - barStart.x, barEnd.y - barStart.y);

        char timeText[64];
        snprintf(timeText, sizeof(timeText), "Voting ends in %d sec", timeRemaining);

        ImVec2 textSize = ImGui::CalcTextSize(timeText);
        ImVec2 textPos = ImVec2(
            barStart.x + (barSize.x - textSize.x) * 0.5f,
            barStart.y + (barSize.y - textSize.y) * 0.5f
        );
        ImGui::GetWindowDrawList()->AddText(textPos, ImGui::GetColorU32(ImGuiCol_Text), timeText);

        ImGui::Dummy(ImVec2(0.0f, spacingBelowProgressBar));

        // Render vote boxes side-by-side
        for (int i = 0; i < options.size(); ++i) {
            if (i > 0) ImGui::SameLine(0.0f, boxSpacing);

            ImGui::BeginChild(i + 1, ImVec2(boxWidth, boxHeight), true,
                ImGuiWindowFlags_NoScrollbar |
                ImGuiWindowFlags_NoScrollWithMouse);

            ImVec2 childMin = ImGui::GetWindowPos();
            ImVec2 childMax = ImVec2(childMin.x + boxWidth, childMin.y + boxHeight);
            ImGui::GetWindowDrawList()->AddRectFilled(
                childMin,
                childMax,
                IM_COL32(30, 30, 30, 220),
                5.0f
            );

            ImGui::PushTextWrapPos(0.0f);
            ImGui::Text("%d. %s", i + 1, options[i].c_str());
            ImGui::PopTextWrapPos();

            float percent = (totalVotes > 0) ? (float)votes[i] / totalVotes : 0.0f;

            ImGui::Spacing();
            ImGui::ProgressBar(percent, ImVec2(-1, 25), (std::to_string(votes[i]) + " votes").c_str());
            ImGui::Spacing();
            ImGui::Text("Send %d in chat", i + 1);

            ImGui::EndChild();
        }
    }
    else if (votesPaused) {
        // Voting active but paused: show message instead of boxes
        ImGui::SetCursorPos(ImVec2(20, overlayTotalHeight * 0.4f));
        ImGui::TextColored(ImVec4(1, 1, 0, 1), "Voting paused until player returns or cutscene ends.");
    }

    ImGui::End();
}


void DrawSaveramOverlay()
{
    if (!g_Saveram)
        return;

    ImGui::Begin("Saveram Debug");

    // --- Basic Info ---
    ImGui::Text("=== Player Info ===");
    const auto& sora = g_Saveram->Battle.PartRam[0]; // Index 0 is usually Sora
    ImGui::Text("Level: %d", sora.Level);
    ImGui::Text("HP: %d / %d", sora.Hp, sora.MaxHp);
    ImGui::Text("MP: %d / %d", sora.Mp, sora.MaxMp);
    ImGui::Text("AP: %d", sora.Ap);
    ImGui::Text("Attack: %d", sora.Attack);
    ImGui::Text("Defense: %d", sora.Defence);
    ImGui::Text("Munnies: %d", g_Saveram->System.Munnies);
    ImGui::Text("Current World: %d", g_Saveram->System.World);
    ImGui::Separator();

    // --- Form Info ---
    ImGui::Text("=== Drive Forms ===");
    ImGui::Text("Current Form ID: %d", g_Saveram->Battle.Form);
    ImGui::Text("Drive Gauge: %d / %d", g_Saveram->Battle.Drive, g_Saveram->Battle.DriveMax);
    ImGui::Text("Drive Time: %.2f / %.2f", g_Saveram->Battle.DriveTime, g_Saveram->Battle.DriveTimeMax);
    ImGui::Text("Drive Uses: %d", g_Saveram->Battle.DriveCount);

    static const char* formNames[] = {
        "Valor", "Wisdom", "Limit", "Master", "Final", "Anti", "None", "Other", "Unknown", "Last"
    };

    for (int i = 0; i < 10; ++i) {
        const auto& form = g_Saveram->Battle.FormRam[i];

        ImGui::Separator();
        ImGui::Text("Form ID %d (%s)", i, (i < 6 ? formNames[i] : "Custom/Unused"));
        ImGui::Text("  Level: %d", form.Level);
        ImGui::Text("  Ability Level: %d", form.AbilityLevel);
        ImGui::Text("  Exp: %d", form.Exp);

        // Ability readout (optional, if you want to show some of them)
        int equippedAbilities = 0;
        for (int j = 0; j < 24; ++j) {
            if (form.Ability[j].IsEquip)
                equippedAbilities++;
        }
        ImGui::Text("  Equipped Abilities: %d", equippedAbilities);
    }

    ImGui::End();
}






void MaintainEnemyAggro(YS::ENEMY* enemyA, YS::ENEMY* enemyB)
{
    if (!enemyA || !enemyB)
        return;

    static FunctionCalls::tTargetReplace ReplaceTargetFunction = nullptr;
    if (!ReplaceTargetFunction)
    {
        uintptr_t addr = ReadLEA32(Patterns::pattern_for_ReplaceTarget, L"Replace target", 0, 1, 5);
        ReplaceTargetFunction = reinterpret_cast<FunctionCalls::tTargetReplace>(addr);
    }

    LOG(L"MaintainEnemyAggro called - enemyA: %p, enemyB: %p\n", enemyA, enemyB);

    //ReplaceTargetFunction((YS::OBJ*)enemyA, (YS::OBJ*)enemyB);
   // ReplaceTargetFunction((YS::OBJ*)enemyB, (YS::OBJ*)enemyA);
    *(uintptr_t*)((char*)enemyA + 0xBF8) = (uintptr_t)enemyB;
    *(uintptr_t*)((char*)enemyB + 0xBF8) = (uintptr_t)enemyA;
}

void SetupEnemyFight(YS::ENEMY* enemyA, YS::ENEMY* enemyB)
{
    if (!enemyA || !enemyB)
        return;

    // Assign different teams
    *(int*)((char*)enemyA + 0x4DC) = 1;
    //*(int*)((char*)enemyB + 0x4DC) = 2;

    // Set each other's target directly
    //*(uintptr_t*)((char*)enemyA + 0xBF8) = (uintptr_t)enemyB;
    //*(uintptr_t*)((char*)enemyB + 0xBF8) = (uintptr_t)enemyA;

    // Optional: Create gauges (one-time)
    //CreateGauge(1, (OBJ_131*)enemyA);
    //CreateGauge(2, (OBJ_131*)enemyB);
}

bool IsValidEnemy(YS::ENEMY* enemy, FunctionCalls::tIsOBJValid IsOBJValidFunction)
{
    return enemy && IsOBJValidFunction(reinterpret_cast<YS::OBJ*>(enemy));
}


bool FindEnemiesByNames(const char* nameA, const char* nameB, YS::ENEMY** outEnemyA, YS::ENEMY** outEnemyB)
{
    uintptr_t GetEachEnemyFunctionAddress = ReadLEA32(Patterns::pattern_for_GetEachEnemy, L"Get Each Enemy", 0, 1, 5);
    FunctionCalls::tEachEnemy GetEachEnemyFunction = reinterpret_cast<FunctionCalls::tEachEnemy>(GetEachEnemyFunctionAddress);

    uintptr_t GetOBJEntrynameFunctionAddress = ReadLEA32(Patterns::pattern_for_GetObjEntryName, L"Get obj Entryname", 0, 1, 5);
    FunctionCalls::tGetObjEntryName GetOBJEntrynameFunction = reinterpret_cast<FunctionCalls::tGetObjEntryName>(GetOBJEntrynameFunctionAddress);

    if (!outEnemyA || !outEnemyB)
        return false;

    *outEnemyA = nullptr;
    *outEnemyB = nullptr;

    YS::ENEMY* objPtr = GetEachEnemyFunction(nullptr);
    while (objPtr)
    {
        const char* entryName = GetOBJEntrynameFunction((YS::OBJ*)objPtr);

        printf("Found enemy with entry name: %s\n", entryName);  // Debug log each enemy name found

        if (!*outEnemyA && strcmp(entryName, nameA) == 0)
        {
            printf("Matched enemyA: %s\n", nameA);
            *outEnemyA = objPtr;
        }
        else if (!*outEnemyB && strcmp(entryName, nameB) == 0)
        {
            printf("Matched enemyB: %s\n", nameB);
            *outEnemyB = objPtr;
        }

        if (*outEnemyA && *outEnemyB)
        {
            printf("Both enemies found.\n");
            return true;
        }

        objPtr = GetEachEnemyFunction(objPtr);
    }

    printf("Could not find both enemies yet.\n");
    return false; // Could not find both enemies
}






void OnFrameUpdate()
{
    /*if (g_CurrentFrameLabel)
    {
        char buffer[64];
        sprintf(buffer, "Current Frame: %d", g_event_m_frame);
        g_CurrentFrameLabel->SetText(buffer);
    }*/

     
    

    bool isPlayerInArea = (*(bool*)(moduleBase + 0x9BA8D0));
    int event_mode = *(int*)(moduleBase + 0xB65210);
    g_Saveram = (SAVERAM::SAVERAM*)(moduleBase + 0x9A98B0);
    Entity* playerSpawned = GetPlayer1();
    g_currentFormTime = *(float*)(moduleBase + 0x2A234D4);
    g_currentFormMaxTime = *(float*)(moduleBase + 0x2A234D8);
    bool isPauseMenuOpen = *(bool*)(moduleBase + 0x9006B0);



    bool chaosEnabled = chaosManager.IsChaosEnabled();
    bool votingActive = voteManager.IsVotingActive();
    bool canVoteNow = chaosEnabled && isPlayerInArea && event_mode != 3 && (playerSpawned != nullptr) && !isPauseMenuOpen;
    bool votesPaused = votingActive && !canVoteNow;
    
    if (ImGui::IsKeyPressed(ImGuiKey_9)) {
        showSaveramOverlay = !showSaveramOverlay;
    }

    if (showSaveramOverlay) {
        DrawSaveramOverlay();
    }
    

    if (canVoteNow) {
        chaosManager.Update();

        if (ImGui::IsKeyPressed(ImGuiKey_1)) voteManager.CastVote(0);
        if (ImGui::IsKeyPressed(ImGuiKey_2)) voteManager.CastVote(1);
        if (ImGui::IsKeyPressed(ImGuiKey_3)) voteManager.CastVote(2);
        if (ImGui::IsKeyPressed(ImGuiKey_4)) voteManager.CastVote(3);
        if (ImGui::IsKeyPressed(ImGuiKey_5)) voteManager.CastVote(4);
    }

    RenderVoteOverlay(canVoteNow, votesPaused);


    UpdateAudioParamsFromScrollbars();

    if (!gGlobalObjPtr1) return;

    float* scaleX = reinterpret_cast<float*>((char*)gGlobalObjPtr1 + 0x30);
    float* scaleY = reinterpret_cast<float*>((char*)gGlobalObjPtr1 + 0x34);
    float* scaleZ = reinterpret_cast<float*>((char*)gGlobalObjPtr1 + 0x38);
    float* scaleW = reinterpret_cast<float*>((char*)gGlobalObjPtr1 + 0x3C);

    if (g_ScaleSliderX) UpdateScrollbar(g_ScaleSliderX, scaleX);
    if (g_ScaleSliderY) UpdateScrollbar(g_ScaleSliderY, scaleY);
    if (g_ScaleSliderZ) UpdateScrollbar(g_ScaleSliderZ, scaleZ);
    if (g_ScaleSliderW) UpdateScrollbar(g_ScaleSliderW, scaleW);
}









static void CleanupDeviceD3D12();
static void CleanupRenderTarget();
//static void RenderImGui_DX12(IDXGISwapChain3* pSwapChain);

void CreateTriangleVertexBuffer()
{
    // Describe the vertex buffer
    D3D12_HEAP_PROPERTIES heapProps = {};
    heapProps.Type = D3D12_HEAP_TYPE_UPLOAD;
    heapProps.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
    heapProps.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;

    D3D12_RESOURCE_DESC bufferDesc = {};
    bufferDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
    bufferDesc.Width = sizeof(vertices); // Size of the vertex buffer
    bufferDesc.Height = 1;
    bufferDesc.DepthOrArraySize = 1;
    bufferDesc.MipLevels = 1;
    bufferDesc.Format = DXGI_FORMAT_UNKNOWN;
    bufferDesc.SampleDesc.Count = 1;
    bufferDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

    // Create the buffer resource
    if (FAILED(g_pd3dDevice->CreateCommittedResource(
        &heapProps,
        D3D12_HEAP_FLAG_NONE,
        &bufferDesc,
        D3D12_RESOURCE_STATE_GENERIC_READ,
        nullptr,
        IID_PPV_ARGS(&g_pd3dVertexBuffer))))
    {
        LOG("[!] Failed to create vertex buffer.\n");
        return;
    }

    // Map the buffer and copy the vertex data into it
    void* pData = nullptr;
    g_pd3dVertexBuffer->Map(0, nullptr, &pData);
    memcpy(pData, vertices, sizeof(vertices));
    g_pd3dVertexBuffer->Unmap(0, nullptr);
}
void DrawArrow(ImVec2 startPoint, ImVec2 endPoint, ImU32 arrowColor, float thickness) {
    // Access ImGui's drawing context
    ImDrawList* draw_list = ImGui::GetBackgroundDrawList();

    // Draw the arrow shaft
    draw_list->AddLine(startPoint, endPoint, arrowColor, thickness);

    // Calculate direction vector for the arrow shaft
    ImVec2 direction = ImVec2(endPoint.x - startPoint.x, endPoint.y - startPoint.y);
    float length = sqrtf(direction.x * direction.x + direction.y * direction.y);

    // Normalize the direction vector
    ImVec2 normDirection = ImVec2(direction.x / length, direction.y / length);

    // Ortho direction (perpendicular to the shaft)
    ImVec2 orthoDirection = ImVec2(-normDirection.y, normDirection.x);

    // Arrow head size
    float arrowHeadSize = 10.0f;

    // Calculate points for the arrow head
    ImVec2 arrowP1 = ImVec2(
        endPoint.x - arrowHeadSize * normDirection.x + arrowHeadSize * orthoDirection.x,
        endPoint.y - arrowHeadSize * normDirection.y + arrowHeadSize * orthoDirection.y
    );
    ImVec2 arrowP2 = ImVec2(
        endPoint.x - arrowHeadSize * normDirection.x - arrowHeadSize * orthoDirection.x,
        endPoint.y - arrowHeadSize * normDirection.y - arrowHeadSize * orthoDirection.y
    );

    // Draw the arrow head using a filled triangle
    draw_list->AddTriangleFilled(endPoint, arrowP1, arrowP2, arrowColor);
}

void RenderPlayerMovementArrow(Entity* player, const XMMATRIX& viewMatrix, float fov, float aspectRatio, float nearZ, float farZ, float viewportWidth, float viewportHeight) {
    // Load player position
    XMVECTOR position = XMVectorSet(player->ent_pos.x, player->ent_pos.y, player->ent_pos.z, player->ent_pos.w);

    // Inertia is a Vector4, so calculate its magnitude (speed-like value)
    XMVECTOR inertia = XMVectorSet(player->Inertia.x, player->Inertia.y, player->Inertia.z, player->Inertia.w);
    float inertiaMagnitude = XMVectorGetX(XMVector3Length(inertia)); // Use 3D length, ignoring w

    // Hardcoded values for movement
    const float acceleration = 0.95f;  // Acceleration constant (smaller means slower changes)
    const float walkSpeed = 2.0f;      // Walk speed constant
    const float runSpeed = 8.0f;       // Run speed constant

    // Calculate the speed based on inertia magnitude, scaled by the maximum run speed
    static float smoothedSpeed = 0.0f;  // Keep a smoothed version of the speed
    float targetSpeed = (inertiaMagnitude / runSpeed) * runSpeed;

    // Apply acceleration smoothing to the speed
    smoothedSpeed = smoothedSpeed * acceleration + targetSpeed * (1.0f - acceleration);

    // Early exit if there's no movement (i.e., inertia magnitude is zero)
    if (smoothedSpeed == 0.0f) {
        return; // No need to draw the arrow if there's no movement
    }

    // Normalize the player's direction (assuming player->Direction is a heading in radians)
    float playerDirection = player->Direction;
    XMVECTOR direction = XMVectorSet(cosf(playerDirection), 0.0f, sinf(playerDirection), 0.0f);  // 2D direction vector

    // Scale the arrow length based on the smoothed speed
    float arrowLength = (smoothedSpeed / runSpeed) * 100.0f;

    // Calculate the end point of the arrow based on the direction and scaled length
    XMVECTOR endPoint = XMVectorAdd(position, XMVectorScale(direction, arrowLength));

    // Screen positions for start and end points
    XMVECTOR startPointScreen, endPointScreen;

    // Convert world positions to screen positions
    if (WorldToScreen(position, viewMatrix, fov, aspectRatio, nearZ, farZ, viewportWidth, viewportHeight, startPointScreen) &&
        WorldToScreen(endPoint, viewMatrix, fov, aspectRatio, nearZ, farZ, viewportWidth, viewportHeight, endPointScreen)) {

        // Extract 2D screen coordinates
        ImVec2 startPoint = ImVec2(XMVectorGetX(startPointScreen), XMVectorGetY(startPointScreen));
        ImVec2 endPointVec = ImVec2(XMVectorGetX(endPointScreen), XMVectorGetY(endPointScreen));

        // Determine arrow color based on speed (optional)
        ImU32 arrowColor = IM_COL32(255, 0, 0, 255);  // Red color

        // Draw the arrow from start to end on the screen
        DrawArrow(startPoint, endPointVec, arrowColor, 3.0f);  // Draw a red arrow with thickness 3
    }
}

// Define max walking and running speeds (in meters per second)
const float maxSpeedWalking = 2.0f;  // Max walking speed in m/s
const float maxSpeedRunning = 8.0f;  // Max running speed in m/s

// Define the acceleration multiplier
const float acceleration = 0.94999999f;  // Acceleration multiplier
const float deceleration = 0.94999999f;

// Minimum inertia threshold for movement (to avoid sudden speed jumps)

const float movementThreshold = 0.1f; // Only consider movement if movement speed exceeds this threshold // Only consider movement if inertia exceeds this threshold

// Smooth speed change factor (to smooth out the needle movement)
const float smoothFactor = 0.1f;  // Lower values make it smoother

// Function to calculate and render the speedometer based on current speed
void RenderSpeedometer(float speedInKph);

// Function to get the player entity, assuming it's already defined elsewhere
//Entity* GetPlayer1();

void RenderSpeedometer(float speedInKph) {
    ImVec2 center(ImGui::GetIO().DisplaySize.x - 150, ImGui::GetIO().DisplaySize.y - 150);  // Position on screen
    float radius = 100.0f;  // Radius of the speedometer
    float maxSpeed = 100.0f;  // Maximum speed displayed on the speedometer (in km/h)

    // Background and tick marks
    ImDrawList* draw_list = ImGui::GetBackgroundDrawList();
    draw_list->AddCircle(center, radius, IM_COL32(255, 255, 255, 255), 64, 3.0f);  // Draw outer circle

    // Draw tick marks
    for (int i = 0; i <= 10; ++i) {
        float angle = (i / 10.0f) * IM_PI * 1.5f + IM_PI / 4;  // Position ticks between 45° and 315°
        ImVec2 tickStart = ImVec2(center.x + cos(angle) * (radius - 10), center.y + sin(angle) * (radius - 10));
        ImVec2 tickEnd = ImVec2(center.x + cos(angle) * radius, center.y + sin(angle) * radius);
        draw_list->AddLine(tickStart, tickEnd, IM_COL32(255, 255, 255, 255), 2.0f);
    }

    // Draw speed text
    char speedText[32];
    snprintf(speedText, sizeof(speedText), "%.1f km/h", speedInKph);
    draw_list->AddText(ImVec2(center.x - 20, center.y + 30), IM_COL32(255, 255, 255, 255), speedText);

    // Draw needle
    float needleAngle = ((speedInKph / maxSpeed) * 1.5f * IM_PI) + (IM_PI / 4);  // Map speed to needle angle
    ImVec2 needleEnd = ImVec2(center.x + cos(needleAngle) * (radius - 20), center.y + sin(needleAngle) * (radius - 20));
    draw_list->AddLine(center, needleEnd, IM_COL32(255, 0, 0, 255), 3.0f);  // Red needle
}

DirectX::XMFLOAT4 Vector4ToXMFLOAT4(const Vector4& v) {
    return DirectX::XMFLOAT4(v.x, v.y, v.z, v.w);
}

// Linear interpolation function (lerp)
float Lerp(float start, float end, float alpha) {
    return start + alpha * (end - start);
}


void RenderSpeedometer(float speedInKph);

// Function to get the player entity, assuming it's already defined elsewhere
//Entity* GetPlayer1();

void UpdatePlayerSpeed() {
    // Get the player entity
    Entity* player = GetPlayer1();
    if (!player) {
        return; // If no player is found, return
    }

    Vector4 movement = player->Movement;  // Get player inertia (movement speed vector)

    // Convert Vector4 to XMFLOAT4
    DirectX::XMFLOAT4 movementFloat4 = Vector4ToXMFLOAT4(movement);

    // Convert XMFLOAT4 to XMVECTOR
    XMVECTOR movementXM = XMLoadFloat4(&movementFloat4);

    // Extract the current speed directly from the 'w' component of the movement vector
    float currentSpeed = XMVectorGetW(movementXM);  // Speed is stored in the W component of movement

    // Check if the player is moving by looking at the speed value
    bool isMoving = currentSpeed > movementThreshold;  // Only consider movement if speed exceeds a threshold

    // Determine max speed based on whether the player is running or walking
    float targetSpeed = 0.0f;
    if (isMoving) {
        if (currentSpeed >= maxSpeedRunning) {
            targetSpeed = maxSpeedRunning;  // Cap at running speed
        }
        else {
            targetSpeed = maxSpeedWalking;  // Cap at walking speed
        }
    }

    // Smoothly adjust current speed based on acceleration and target speed
    if (isMoving) {
        // Use a lerp (linear interpolation) for smoother acceleration
        currentSpeed = Lerp(currentSpeed, targetSpeed, smoothFactor);
    }
    else {
        // Apply deceleration to smooth out the stop
        currentSpeed = Lerp(currentSpeed, 0.0f, smoothFactor);  // Gradually reduce speed
    }

    // Convert speed from m/s to km/h (1 m/s = 3.6 km/h)
    float speedInKph = currentSpeed * 3.6f;

    // Render the speedometer with the calculated speed in km/h
    RenderSpeedometer(speedInKph);
}


void DrawBdxMenuText() {

    auto* menu = reinterpret_cast<YW::WIDGET*>(g_MenuUI.bdxMenuPtr);

    if (!menu) {
        LOG("[DEBUG] Menu pointer is nullptr, skipping draw.\n");
        return;
    }

    // Simple sanity checks
    if (menu->Rect.W <= 0 || menu->Rect.H <= 0) {
        LOG("[DEBUG] Menu rect size invalid (W: %d, H: %d), skipping draw.\n", menu->Rect.W, menu->Rect.H);
        g_MenuUI.bdxMenuPtr = nullptr;
        g_MenuUI.isBdxMenuOpen = false;
        return;
    }

    LOG("[DEBUG] Drawing menu at X: %d Y: %d\n", menu->Rect.X, menu->Rect.Y);


    if (g_MenuUI.bdxMenuPtr != nullptr) {
        YW::WIDGET* menu = reinterpret_cast<YW::WIDGET*>(g_MenuUI.bdxMenuPtr);
        if (menu->Rect.W > 0 && menu->Rect.H > 0) {
            YS::MESSAGE msg;
            msg.text = "5š¥Ÿ¨Ÿ­¡ž­¢¦ž¡š¬©š¬¬ž";
            YSDrawMessageFunction(
                Message::FONT_TYPE::TYPE_DEBUG,
                msg,
                menu->Rect.X,
                menu->Rect.Y,
                0x80FFFFFF,
                16,
                Message::ALIGN::ALIGN_LEFT
            );
        }
        else {
            g_MenuUI.bdxMenuPtr = nullptr;
            g_MenuUI.isBdxMenuOpen = false;
        }
    }
}

void DrawGlowLine(ImDrawList* draw_list, ImVec2 p1, ImVec2 p2, ImU32 color, float glow_radius = 8.0f, int layers = 6)
{
    ImColor base_color = ImColor(color);

    for (int i = layers; i > 0; --i)
    {
        float alpha = base_color.Value.w * (0.02f * i); // Decrease alpha
        float thickness = glow_radius * ((float)i / layers);
        ImColor glow_col = ImColor(base_color.Value.x, base_color.Value.y, base_color.Value.z, alpha);
        draw_list->AddLine(p1, p2, glow_col, thickness);
    }

    // Draw the core sharp line
    draw_list->AddLine(p1, p2, color, 1.0f);
}

void DrawGlowTriangle(ImDrawList* draw_list, ImVec2 p1, ImVec2 p2, ImVec2 p3, ImU32 color, float glow_radius = 8.0f, int layers = 6)
{
    ImColor base_color = ImColor(color);

    for (int i = layers; i > 0; --i)
    {
        float offset = glow_radius * ((float)i / layers);
        float alpha = base_color.Value.w * (0.02f * i); // Fading alpha
        ImColor glow_col = ImColor(base_color.Value.x, base_color.Value.y, base_color.Value.z, alpha);

        ImVec2 o = ImVec2(offset, offset);

        draw_list->AddTriangleFilled(ImVec2(p1.x - o.x, p1.y - o.y),
            ImVec2(p2.x - o.x, p2.y - o.y),
            ImVec2(p3.x - o.x, p3.y - o.y),
            glow_col);

        draw_list->AddTriangleFilled(ImVec2(p1.x + o.x, p1.y + o.y),
            ImVec2(p2.x + o.x, p2.y + o.y),
            ImVec2(p3.x + o.x, p3.y + o.y),
            glow_col);

        draw_list->AddTriangleFilled(ImVec2(p1.x + offset, p1.y),
            ImVec2(p2.x + offset, p2.y),
            ImVec2(p3.x + offset, p3.y),
            glow_col);

        draw_list->AddTriangleFilled(ImVec2(p1.x, p1.y - offset),
            ImVec2(p2.x, p2.y - offset),
            ImVec2(p3.x, p3.y - offset),
            glow_col);
    }

    // Draw the solid triangle core
    draw_list->AddTriangleFilled(p1, p2, p3, color);
}


ImVec2 FlipY(ImVec2 point, float screenHeight) {
    return ImVec2(point.x, screenHeight - point.y);
}



YS::OBJ* GetClosestEntityToCursor(const Vector3& cursorPos, float maxDistance) {
    YS::OBJ* closest = nullptr;
    float closestDistSq = maxDistance * maxDistance;

    for (YS::OBJ* obj : g_spawnedObjects) {
        if (!obj) continue;

        Entity* entity = ToEntity(obj);
        if (!entity) continue;

        Vector3 min = entity->RBBOXMIN;
        Vector3 max = entity->RBBOXMAX;

        // AABB center
        Vector3 center = {
            (min.x + max.x) * 0.5f,
            (min.y + max.y) * 0.5f,
            (min.z + max.z) * 0.5f
        };

        float dx = center.x - cursorPos.x;
        float dy = center.y - cursorPos.y;
        float dz = center.z - cursorPos.z;

        float distSq = dx * dx + dy * dy + dz * dz;

        if (distSq < closestDistSq) {
            closest = obj;
            closestDistSq = distSq;
        }
    }

    return closest;
}




void DrawLevelEditorInfoBox() {
    ImGui::PushFont(g_ArialFont);

    ImVec2 viewportSize = ImGui::GetIO().DisplaySize;
    ImGui::SetNextWindowPos(ImVec2(viewportSize.x - 10, viewportSize.y - 10), ImGuiCond_Always, ImVec2(1.0f, 1.0f));
    ImGui::SetNextWindowSize(ImVec2(350, 0), ImGuiCond_Always);

    ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.6f, 0.6f, 0.6f, 1.f)); // Gray background
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0, 0, 0, 1)); // Black text

    ImGuiWindowFlags flags =
        ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoCollapse |
        ImGuiWindowFlags_NoTitleBar;

    ImGui::Begin("Info Box", nullptr, flags);

    // Draw custom title bar (black background + white text)
    ImDrawList* draw_list = ImGui::GetWindowDrawList();
    ImVec2 p = ImGui::GetWindowPos();
    ImVec2 size = ImGui::GetWindowSize();
    float titleBarHeight = ImGui::GetFrameHeight();

    // Draw black rect for title bar area
    draw_list->AddRectFilled(p, ImVec2(p.x + size.x, p.y + titleBarHeight), IM_COL32(0, 0, 0, 255));

    // Draw white title text, padded slightly
    ImVec2 textPos = ImVec2(p.x + 10, p.y + 2);
    draw_list->AddText(textPos, IM_COL32(255, 255, 255, 255), "Info Box");

    ImGui::Dummy(ImVec2(0.0f, titleBarHeight)); // Leave space for custom title bar

    OBJENTRY_0* entry = GetOBJENTRYFunction(g_SelectedEntityID);

    ImGui::Text("Selected Entity:");

    if (entry && entry->EntryName) {
        ImGui::Text("Name: %s", entry->EntryName);
    }
    else {
        ImGui::Text("Name: <invalid or none>");
    }

    ImGui::Text("Entity ID: %d", static_cast<int>(g_SelectedEntityID));

    ImGui::Text("Total Spawned Entities: %d", g_SpawnedEntityCount);

    ImGui::Text("Snap to Ground: %s", snapToGroundEnabled ? "Enabled" : "Disabled");

    ImGui::Separator();

    ImGui::Text("Cursor Position:");
    ImGui::Text("X: %.2f", AxisCursorPos.x);
    ImGui::Text("Y: %.2f", AxisCursorPos.y);
    ImGui::Text("Z: %.2f", AxisCursorPos.z);

    ImGui::Text("Cursor Speed:");
    ImGui::Text("%1.0f", FreecamSpeed);

    ImGui::Separator();

    //ImGui::Text("Preview Cooldown:");
   // ImGui::Text("%.3f seconds", std::max(0.0f, previewSpawnCooldown - timeSinceLastSpawn));

    ImGui::End();

    ImGui::PopStyleColor(2);
    ImGui::PopFont();
}





void UpdateLevelEditor(float viewportWidth, float viewportHeight) {

    if (!destroyObjInitialized) {
        DestroyObjFunctionAddress = ReadLEA32(Patterns::pattern_for_DestroyObj, L"Destroy Obj", 0, 1, 5);
        DestroyObjFunction = reinterpret_cast<FunctionCalls::tDestroyObj>(DestroyObjFunctionAddress);

        RealDestroyobjFunctionAddress = ReadLEA32(Patterns::pattern_for_RealDestroyObj, L"Real Destroy Obj", 0, 1, 5);
       
        RealDestroyobjFunction = reinterpret_cast<FunctionCalls::tDestroyObj>(RealDestroyobjFunctionAddress);

        destroyObjInitialized = true;
    }

   // UpdatePreviewSpawnTimer();
    //ImDrawList* drawList = ImGui::GetBackgroundDrawList();

    YS::PAD* leftStickPad = GetHardPadFunction(0);

    const int SHIFT_MASK = static_cast<int>(YS::HARDPAD::SHIFT_OFF) | static_cast<int>(YS::HARDPAD::SHIFT_ON);

    int buttons = leftStickPad->Button & ~SHIFT_MASK;

    int triggers = leftStickPad->Trigger & ~SHIFT_MASK;

    

    if ((triggers & static_cast<int>(YS::HARDPAD::CROSS)) != 0 && (DWORD)GetOBJENTRYFunction(g_SelectedEntityID)) {
        NewLevelEditorObj(g_SelectedEntityID, 1);
        
    }

    if ((triggers & static_cast<int>(YS::HARDPAD::TRIANGLE)) != 0) {
        if (selectedObject) {
            DestroyObjFunction(selectedObject); // Destroy the object
           
            RealDestroyobjFunction(selectedObject);

            // Optionally: remove it from your spawned list if you're tracking them
            auto it = std::find(g_spawnedObjects.begin(), g_spawnedObjects.end(), selectedObject);
            if (it != g_spawnedObjects.end()) {
                g_spawnedObjects.erase(it);
            }

            selectedObject = nullptr;
            selectedEntity = nullptr;
        }
    }

    if ((triggers & static_cast<int>(YS::HARDPAD::SELECT)) != 0) {
        Vector3 cursorPos3 = { AxisCursorPos.x, AxisCursorPos.y, AxisCursorPos.z };

        // Find closest object
        YS::OBJ* closest = GetClosestEntityToCursor(cursorPos3, 500.0f); // 500 units max range

        if (closest) {
            selectedObject = closest;
            selectedEntity = ToEntity(closest);

            // Snap cursor to center of AABB
            Vector3 min = selectedEntity->RBBOXMIN;
            Vector3 max = selectedEntity->RBBOXMAX;

            Vector3 center = {
                (min.x + max.x) * 0.5f,
                (min.y + max.y) * 0.5f,
                (min.z + max.z) * 0.5f
            };

            AxisCursorPos = { center.x, center.y, center.z, 1.0f };

            printf("Snapped cursor to entity center!\n");
        }
        else {
            printf("No nearby entity found to snap to.\n");
        }
    }

    if ((buttons & static_cast<int>(YS::HARDPAD::CIRCLE)) != 0 && selectedEntity) {
        // Move the entity's position to match the cursor position
        Vector4 newPosition = {
            AxisCursorPos.x,
            AxisCursorPos.y,
            AxisCursorPos.z,
            1.0f // w component usually 1.0 for position vectors
        };

        // Update the position vector
        selectedEntity->ent_pos = newPosition;

        

        printf("Moved entity with cursor to: (%.2f, %.2f, %.2f)\n",
            AxisCursorPos.x, AxisCursorPos.y, AxisCursorPos.z);
    }

}



void R(F& x, F& y, F r) { F s = sin(r), c = cos(r), t = c * x - s * y; y = s * x + c * y; x = t; }
F c, e, f, g, l, r, x, y, z;
int i, j, k, u, v, K = 24, J = 48;
F L(F a) { l *= l > 0; return sqrt(1 - exp(-a * (l + .1) - l * l * l)); }
void T(F u) { R(x, z, c + 6.28 * u); }
V P(F t) { t *= 3.14 / K; return V(sin(t), cos(t)); }
void FX(ImDrawList* d, V a, V b, V S, ImVec4 I, F t) {
    V o[4], m = (a + b) / 2; c = t / 2;
    k = sin(t * 13) * 24 + 24; j = -0xafefb0 + k * 0x101; //d->AddRectFilledMultiColor(a, b, j, j + 0x503010, j + 0x4050, j + 0x7040);
    for (k = K; --k;) {
        for (j = J; j--;) {
            for (i = 4; i--;) {
                u = j + (i % 3 > 0); v = -(i / 2);
                V p = P(k + v); x = p.x; y = p.y; z = 0;
                T((F)u / J); e = x; f = y; z = S.y / (g = z + 2.5);
                o[i] = m + V(x, y) * z;
            }
            V v1 = P(k), v2 = P(k - 1);
            x = v1.y - v2.y; y = v2.x - v1.x; z = 0;
            l = sqrt(x * x + y * y); x = x / l; y = y / l; T((j + .5) / J);
            r = 2 * (x * e + y * f + z * g);
            e -= r * x; f -= r * y; g -= r * z;
            l = sqrt(e * e + f * f + g * g);
            x = atan2(e, g); y = atan2(sqrt(e * e + g * g), f);
            l = sin(x * 5 + sin(y * 3 + t)) + sin(x * 9 + sin(y * 5 + t)) + cos(y * 4 + sin(x * 5 + t));
            l = l / 4 + .5;
            I.x = l; I.y = l * .9 * (.8 + e / 4); I.z = (.5 - f / 2) * l / 2; I.w = 1;
            if (r > 0)d->AddConvexPolyFilled(o, 4, ImColor(I)), d->AddPolyline(o, 4, 0xff << 24, true, 1);
        }
    }
}

void GenerateDiscoBallGeometry(std::vector<Tile>& tiles, V a, V b, V S, ImVec4 I, F t) {
    tiles.clear();

    // center point
    V m = (a + b) * 0.5f;

    int K = 24, J = 48;

    // reusable vars
    F c, e, f, g, l, r, x, y, z;

    // Rotation helper (same as your R)
    auto R = [](F& x, F& y, F r) {
        F s = sin(r), c = cos(r), t = c * x - s * y;
        y = s * x + c * y;
        x = t;
        };

    // T helper rotation with offset c
    c = t / 2;
    auto T = [&](F u) { R(x, z, c + 6.28f * u); };

    // P helper to get circle points
    auto P = [&](F t) -> V {
        t *= 3.14f / K;
        return V(sin(t), cos(t));
        };

    // Color base calc (similar to your FX)
    int k_color = static_cast<int>(sin(t * 13) * 24 + 24);
    int j_color = -0xafefb0 + k_color * 0x101;

    // Loop over rings and segments
    for (int k = K; --k;) {
        for (int j = J; j--;) {
            Tile tile;

            for (int i = 0; i < 4; ++i) {
                int u = j + (i % 3 > 0);
                int v = -(i / 2);

                V p = P(k + v);
                x = p.x; y = p.y; z = 0;
                T(static_cast<F>(u) / J);
                e = x; f = y;
                z = S.y / (g = z + 2.5f);

                tile.verts[i] = m + V(x, y) * z;
            }

            V v1 = P(k);
            V v2 = P(k - 1);
            x = v1.y - v2.y;
            y = v2.x - v1.x;
            z = 0;

            l = sqrt(x * x + y * y);
            x /= l;
            y /= l;
            T((j + .5f) / J);

            r = 2 * (x * e + y * f + z * g);
            e -= r * x;
            f -= r * y;
            g -= r * z;

            l = sqrt(e * e + f * f + g * g);

            x = atan2(e, g);
            y = atan2(sqrt(e * e + g * g), f);

            l = sin(x * 5 + sin(y * 3 + t)) + sin(x * 9 + sin(y * 5 + t)) + cos(y * 4 + sin(x * 5 + t));
            l = l / 4 + 0.5f;

            ImVec4 col;
            col.x = l;
            col.y = l * .9f * (.8f + e / 4);
            col.z = (.5f - f / 2) * l / 2;
            col.w = 1;

            tile.color = ImColor(col);

            if (r > 0) {
                tiles.push_back(tile);
            }
        }
    }
}


void UpdatePartyFog(Fog* fog, float amplitude)
{
    if (!fog) return;

    float t = ImGui::GetTime();

    // -----------------------------------------------------------------
    // Smooth amplitude (prevents harsh jumps)
    // -----------------------------------------------------------------
    static float smoothAmp = 0.0f;
    amplitude = std::clamp(amplitude, 0.0f, 1.0f);
    smoothAmp += (amplitude - smoothAmp) * 0.15f;

    // -----------------------------------------------------------------
    // HSV → RGB helper
    // -----------------------------------------------------------------
    auto HSVtoRGB = [](float h, float s, float v) -> ImVec4 {
        float r, g, b;
        int i = int(h * 6.0f);
        float f = h * 6.0f - i;
        float p = v * (1.0f - s);
        float q = v * (1.0f - f * s);
        float t = v * (1.0f - (1.0f - f) * s);

        switch (i % 6) {
        case 0: r = v; g = t; b = p; break;
        case 1: r = q; g = v; b = p; break;
        case 2: r = p; g = v; b = t; break;
        case 3: r = p; g = q; b = v; break;
        case 4: r = t; g = p; b = v; break;
        default: r = v; g = p; b = q; break;
        }

        return ImVec4(r, g, b, 1.0f);
        };

    // -----------------------------------------------------------------
    // Color (same motion, safer values)
    // -----------------------------------------------------------------
    float hue = fmod(t * 0.25f + smoothAmp * 0.5f, 1.0f);

    float saturation = 0.65f;                      // softer than 1.0
    float brightness = 0.55f + smoothAmp * 0.3f;   // no pure white
    brightness = std::clamp(brightness, 0.45f, 0.85f);

    ImVec4 rgb = HSVtoRGB(hue, saturation, brightness);

    fog->m_color.R = (u_char)(rgb.x * 255.0f);
    fog->m_color.G = (u_char)(rgb.y * 255.0f);
    fog->m_color.B = (u_char)(rgb.z * 255.0f);
    fog->m_color.A = 255;

    // -----------------------------------------------------------------
    // ORIGINAL fog values (unchanged)
    // -----------------------------------------------------------------
    float sinWave = sinf(t * 4.0f) * 0.5f + 0.5f;

    float pulseNear = 1000.0f + smoothAmp * 8000.0f + sinWave * 2000.0f;
    float pulseFar = 100000.0f - smoothAmp * 90000.0f + cosf(t * 3.0f) * 3000.0f;

    fog->m_near = std::clamp(pulseNear, 500.0f, 30000.0f);
    fog->m_far = std::clamp(pulseFar, 10000.0f, 120000.0f);

    fog->m_min = smoothAmp * 120.0f;
    fog->m_max = 255.0f - smoothAmp * 150.0f;

    // -----------------------------------------------------------------
    // Beat accent (NO FLASHING)
    // -----------------------------------------------------------------
    static float lastAmp = 0.0f;
    float delta = smoothAmp - lastAmp;
    lastAmp = smoothAmp;

    if (delta > 0.25f)
    {
        // Small brightness lift instead of white flash
        fog->m_color.R = std::min(255, fog->m_color.R + 25);
        fog->m_color.G = std::min(255, fog->m_color.G + 25);
        fog->m_color.B = std::min(255, fog->m_color.B + 25);
    }
}






void StartRotationModeForEntity(YS::OBJ* obj, Entity* entity) {
    selectedObject = obj;
    selectedEntity = entity;

    // Set initial positions
    selectedEntityPos = Vector4{ entity->RBBOXMIN.x + (entity->RBBOXMAX.x - entity->RBBOXMIN.x) * 0.5f,
                               entity->RBBOXMIN.y + (entity->RBBOXMAX.y - entity->RBBOXMIN.y) * 0.5f,
                               entity->RBBOXMIN.z + (entity->RBBOXMAX.z - entity->RBBOXMIN.z) * 0.5f,
                               1.0f };
    rotationHandlePos = AxisCursorPos; // start line to cursor pos

    rotationModeActive = true;
    arrowGrabbed = false;
}


void DrawLine3D(const Vector4& start, const Vector4& end,
    const XMMATRIX& viewMatrix, float fov, float aspectRatio,
    float nearZ, float farZ, float viewportX, float viewportY,
    ImDrawList* drawList, ImU32 color, float thickness = 2.0f)
{
    XMVECTOR vStart = XMVectorSet(start.x, start.y, start.z, 1.0f);
    XMVECTOR vEnd = XMVectorSet(end.x, end.y, end.z, 1.0f);

    XMVECTOR screenStart, screenEnd;

    bool startOnScreen = WorldToScreenNoClip(vStart, viewMatrix, fov, aspectRatio, nearZ, farZ, viewportX, viewportY, screenStart);
    bool endOnScreen = WorldToScreenNoClip(vEnd, viewMatrix, fov, aspectRatio, nearZ, farZ, viewportX, viewportY, screenEnd);

    if (startOnScreen && endOnScreen) {
        drawList->AddLine(
            ImVec2(XMVectorGetX(screenStart), XMVectorGetY(screenStart)),
            ImVec2(XMVectorGetX(screenEnd), XMVectorGetY(screenEnd)),
            color, thickness);
    }
}

void UpdatePreviewSpawnTimer() {
    timeSinceLastSpawn += ImGui::GetIO().DeltaTime;
}

//static void DrawImGuiFrame();

__int64 __fastcall hkWriteVM(
    __int64 a1,
    __m128* a2,
    __m128* a3,
    int a4
)
{
    if (FreeCameraToggle)
    {
        // overwrite with your own matrix
        __m128 vm[4];

        memcpy(vm, (void*)viewMatrixAddress, sizeof(vm));

        return oWriteVM(a1, a2, vm, a4);
    }

    return oWriteVM(a1, a2, a3, a4);
}


static void RenderImGui_DX12(IDXGISwapChain3* pSwapChain);

static bool CreateDeviceD3D12(HWND hWnd) {
    // Setup swap chain
    DXGI_SWAP_CHAIN_DESC1 sd = {};
    sd.BufferCount = NUM_BACK_BUFFERS;
    sd.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    sd.Flags = DXGI_SWAP_CHAIN_FLAG_FRAME_LATENCY_WAITABLE_OBJECT;
    sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    sd.SampleDesc.Count = 1;
    sd.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;

    // Create device
    D3D_FEATURE_LEVEL featureLevel = D3D_FEATURE_LEVEL_11_0;
    if (D3D12CreateDevice(NULL, featureLevel, IID_PPV_ARGS(&g_pd3dDevice)) != S_OK)
        return false;

    D3D12_COMMAND_QUEUE_DESC desc = {};
    if (g_pd3dDevice->CreateCommandQueue(&desc, IID_PPV_ARGS(&g_pd3dCommandQueue)) != S_OK)
        return false;

    IDXGISwapChain1* swapChain1 = NULL;
    if (CreateDXGIFactory1(IID_PPV_ARGS(&g_dxgiFactory)) != S_OK)
        return false;
    if (g_dxgiFactory->CreateSwapChainForHwnd(g_pd3dCommandQueue, hWnd, &sd, NULL, NULL, &swapChain1) != S_OK)
        return false;
    if (swapChain1->QueryInterface(IID_PPV_ARGS(&g_pSwapChain)) != S_OK)
        return false;
    swapChain1->Release();

    return true;
}




static void CreateRenderTarget(IDXGISwapChain* pSwapChain) {
    for (UINT i = 0; i < NUM_BACK_BUFFERS; ++i) {
        ID3D12Resource* pBackBuffer = NULL;
        pSwapChain->GetBuffer(i, IID_PPV_ARGS(&pBackBuffer));
        if (pBackBuffer) {
            DXGI_SWAP_CHAIN_DESC sd;
            pSwapChain->GetDesc(&sd);

            D3D12_RENDER_TARGET_VIEW_DESC desc = {};
            desc.Format = static_cast<DXGI_FORMAT>(Utils::GetCorrectDXGIFormat(sd.BufferDesc.Format));
            desc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;

            g_pd3dDevice->CreateRenderTargetView(pBackBuffer, &desc, g_mainRenderTargetDescriptor[i]);
            g_mainRenderTargetResource[i] = pBackBuffer;
        }
    }
}

static std::add_pointer_t<HRESULT WINAPI(IDXGISwapChain3*, UINT, UINT)> oPresent;
static HRESULT WINAPI hkPresent(IDXGISwapChain3* pSwapChain,
    UINT SyncInterval,
    UINT Flags) {
    RenderImGui_DX12(pSwapChain);

    return oPresent(pSwapChain, SyncInterval, Flags);
}

static std::add_pointer_t<HRESULT WINAPI(IDXGISwapChain3*, UINT, UINT, const DXGI_PRESENT_PARAMETERS*)> oPresent1;
static HRESULT WINAPI hkPresent1(IDXGISwapChain3* pSwapChain,
    UINT SyncInterval,
    UINT PresentFlags,
    const DXGI_PRESENT_PARAMETERS* pPresentParameters) {
    RenderImGui_DX12(pSwapChain);

    return oPresent1(pSwapChain, SyncInterval, PresentFlags, pPresentParameters);
}

static std::add_pointer_t<HRESULT WINAPI(IDXGISwapChain*, UINT, UINT, UINT, DXGI_FORMAT, UINT)> oResizeBuffers;
static HRESULT WINAPI hkResizeBuffers(IDXGISwapChain* pSwapChain,
    UINT        BufferCount,
    UINT        Width,
    UINT        Height,
    DXGI_FORMAT NewFormat,
    UINT        SwapChainFlags) {
    CleanupRenderTarget();

    return oResizeBuffers(pSwapChain, BufferCount, Width, Height, NewFormat, SwapChainFlags);
}

static std::add_pointer_t<HRESULT WINAPI(IDXGISwapChain3*, UINT, UINT, UINT, DXGI_FORMAT, UINT, const UINT*, IUnknown* const*)> oResizeBuffers1;
static HRESULT WINAPI hkResizeBuffers1(IDXGISwapChain3* pSwapChain,
    UINT        BufferCount,
    UINT        Width,
    UINT        Height,
    DXGI_FORMAT NewFormat,
    UINT        SwapChainFlags,
    const UINT* pCreationNodeMask,
    IUnknown* const* ppPresentQueue) {
    CleanupRenderTarget();

    return oResizeBuffers1(pSwapChain, BufferCount, Width, Height, NewFormat, SwapChainFlags, pCreationNodeMask, ppPresentQueue);
}

static std::add_pointer_t<void WINAPI(ID3D12CommandQueue*, UINT, ID3D12CommandList*)> oExecuteCommandLists;
static void WINAPI hkExecuteCommandLists(ID3D12CommandQueue* pCommandQueue,
    UINT NumCommandLists,
    ID3D12CommandList* ppCommandLists) {
    if (!g_pd3dCommandQueue) {
        g_pd3dCommandQueue = pCommandQueue;
    }

    return oExecuteCommandLists(pCommandQueue, NumCommandLists, ppCommandLists);
}

static std::add_pointer_t<HRESULT WINAPI(IDXGIFactory*, IUnknown*, DXGI_SWAP_CHAIN_DESC*, IDXGISwapChain**)> oCreateSwapChain;
static HRESULT WINAPI hkCreateSwapChain(IDXGIFactory* pFactory,
    IUnknown* pDevice,
    DXGI_SWAP_CHAIN_DESC* pDesc,
    IDXGISwapChain** ppSwapChain) {
    CleanupRenderTarget();

    return oCreateSwapChain(pFactory, pDevice, pDesc, ppSwapChain);
}

static std::add_pointer_t<HRESULT WINAPI(IDXGIFactory*, IUnknown*, HWND, const DXGI_SWAP_CHAIN_DESC1*, const DXGI_SWAP_CHAIN_FULLSCREEN_DESC*, IDXGIOutput*, IDXGISwapChain1**)> oCreateSwapChainForHwnd;
static HRESULT WINAPI hkCreateSwapChainForHwnd(IDXGIFactory* pFactory,
    IUnknown* pDevice,
    HWND hWnd,
    const DXGI_SWAP_CHAIN_DESC1* pDesc,
    const DXGI_SWAP_CHAIN_FULLSCREEN_DESC* pFullscreenDesc,
    IDXGIOutput* pRestrictToOutput,
    IDXGISwapChain1** ppSwapChain) {
    CleanupRenderTarget();

    return oCreateSwapChainForHwnd(pFactory, pDevice, hWnd, pDesc, pFullscreenDesc, pRestrictToOutput, ppSwapChain);
}

static std::add_pointer_t<HRESULT WINAPI(IDXGIFactory*, IUnknown*, IUnknown*, const DXGI_SWAP_CHAIN_DESC1*, IDXGIOutput*, IDXGISwapChain1**)> oCreateSwapChainForCoreWindow;
static HRESULT WINAPI hkCreateSwapChainForCoreWindow(IDXGIFactory* pFactory,
    IUnknown* pDevice,
    IUnknown* pWindow,
    const DXGI_SWAP_CHAIN_DESC1* pDesc,
    IDXGIOutput* pRestrictToOutput,
    IDXGISwapChain1** ppSwapChain) {
    CleanupRenderTarget();

    return oCreateSwapChainForCoreWindow(pFactory, pDevice, pWindow, pDesc, pRestrictToOutput, ppSwapChain);
}

static std::add_pointer_t<HRESULT WINAPI(IDXGIFactory*, IUnknown*, const DXGI_SWAP_CHAIN_DESC1*, IDXGIOutput*, IDXGISwapChain1**)> oCreateSwapChainForComposition;
static HRESULT WINAPI hkCreateSwapChainForComposition(IDXGIFactory* pFactory,
    IUnknown* pDevice,
    const DXGI_SWAP_CHAIN_DESC1* pDesc,
    IDXGIOutput* pRestrictToOutput,
    IDXGISwapChain1** ppSwapChain) {
    CleanupRenderTarget();

    return oCreateSwapChainForComposition(pFactory, pDevice, pDesc, pRestrictToOutput, ppSwapChain);
}



namespace DX12 {
    void Hook(HWND hwnd) {

        if (!CreateDeviceD3D12(GetConsoleWindow())) {
            LOG("[!] CreateDeviceD3D12() failed.\n");
            return;
        }

        LOG("[+] DirectX12: g_pd3dDevice: 0x%p\n", g_pd3dDevice);
        LOG("[+] DirectX12: g_dxgiFactory: 0x%p\n", g_dxgiFactory);
        LOG("[+] DirectX12: g_pd3dCommandQueue: 0x%p\n", g_pd3dCommandQueue);
        LOG("[+] DirectX12: g_pSwapChain: 0x%p\n", g_pSwapChain);

        if (g_pd3dDevice) {
            // Init ImGui
            ImGui::CreateContext();
            ImGui_ImplWin32_Init(hwnd);

            ImGuiIO& io = ImGui::GetIO();

            io.IniFilename = nullptr;
            io.LogFilename = nullptr;

            // Hook
            void** pVTable = *reinterpret_cast<void***>(g_pSwapChain);
            void** pCommandQueueVTable = *reinterpret_cast<void***>(g_pd3dCommandQueue);
            void** pFactoryVTable = *reinterpret_cast<void***>(g_dxgiFactory);

            void* fnCreateSwapChain = pFactoryVTable[10];
            void* fnCreateSwapChainForHwndChain = pFactoryVTable[15];
            void* fnCreateSwapChainForCWindowChain = pFactoryVTable[16];
            void* fnCreateSwapChainForCompChain = pFactoryVTable[24];

            void* fnPresent = pVTable[8];
            void* fnPresent1 = pVTable[22];

            void* fnResizeBuffers = pVTable[13];
            void* fnResizeBuffers1 = pVTable[39];

            void* fnExecuteCommandLists = pCommandQueueVTable[10];

            if (g_pd3dCommandQueue) { g_pd3dCommandQueue->Release(); g_pd3dCommandQueue = NULL; }
            CleanupDeviceD3D12();

            static MH_STATUS cscStatus = MH_CreateHook(reinterpret_cast<void**>(fnCreateSwapChain), &hkCreateSwapChain, reinterpret_cast<void**>(&oCreateSwapChain));
            static MH_STATUS cschStatus = MH_CreateHook(reinterpret_cast<void**>(fnCreateSwapChainForHwndChain), &hkCreateSwapChainForHwnd, reinterpret_cast<void**>(&oCreateSwapChainForHwnd));
            static MH_STATUS csccwStatus = MH_CreateHook(reinterpret_cast<void**>(fnCreateSwapChainForCWindowChain), &hkCreateSwapChainForCoreWindow, reinterpret_cast<void**>(&oCreateSwapChainForCoreWindow));
            static MH_STATUS csccStatus = MH_CreateHook(reinterpret_cast<void**>(fnCreateSwapChainForCompChain), &hkCreateSwapChainForComposition, reinterpret_cast<void**>(&oCreateSwapChainForComposition));

            static MH_STATUS presentStatus = MH_CreateHook(reinterpret_cast<void**>(fnPresent), &hkPresent, reinterpret_cast<void**>(&oPresent));
            static MH_STATUS present1Status = MH_CreateHook(reinterpret_cast<void**>(fnPresent1), &hkPresent1, reinterpret_cast<void**>(&oPresent1));

            static MH_STATUS resizeStatus = MH_CreateHook(reinterpret_cast<void**>(fnResizeBuffers), &hkResizeBuffers, reinterpret_cast<void**>(&oResizeBuffers));
            static MH_STATUS resize1Status = MH_CreateHook(reinterpret_cast<void**>(fnResizeBuffers1), &hkResizeBuffers1, reinterpret_cast<void**>(&oResizeBuffers1));

            static MH_STATUS eclStatus = MH_CreateHook(reinterpret_cast<void**>(fnExecuteCommandLists), &hkExecuteCommandLists, reinterpret_cast<void**>(&oExecuteCommandLists));

            MH_EnableHook(fnCreateSwapChain);
            MH_EnableHook(fnCreateSwapChainForHwndChain);
            MH_EnableHook(fnCreateSwapChainForCWindowChain);
            MH_EnableHook(fnCreateSwapChainForCompChain);

            MH_EnableHook(fnPresent);
            MH_EnableHook(fnPresent1);

            MH_EnableHook(fnResizeBuffers);
            MH_EnableHook(fnResizeBuffers1);

            MH_EnableHook(fnExecuteCommandLists);

            MH_CreateHook(
                (LPVOID)(moduleBase + 0x125D30),
                &hkWriteVM,
                reinterpret_cast<LPVOID*>(&oWriteVM)
            );

            MH_EnableHook((LPVOID)(moduleBase + 0x125D30));



        }
    }


    void Unhook() {
        if (ImGui::GetCurrentContext()) {
            if (ImGui::GetIO().BackendRendererUserData)
                ImGui_ImplDX12_Shutdown();

            ImGui_ImplWin32_Shutdown();
            ImGui::DestroyContext();
        }

        CleanupDeviceD3D12();
    }
}


static void CleanupRenderTarget() {
    for (UINT i = 0; i < NUM_BACK_BUFFERS; ++i)
        if (g_mainRenderTargetResource[i]) { g_mainRenderTargetResource[i]->Release(); g_mainRenderTargetResource[i] = NULL; }
}

static void CleanupDeviceD3D12() {
    CleanupRenderTarget();

    if (g_pSwapChain) { g_pSwapChain->Release(); g_pSwapChain = NULL; }
    for (UINT i = 0; i < NUM_BACK_BUFFERS; ++i)
        if (g_commandAllocators[i]) { g_commandAllocators[i]->Release(); g_commandAllocators[i] = NULL; }
    if (g_pd3dCommandList) { g_pd3dCommandList->Release(); g_pd3dCommandList = NULL; }
    if (g_pd3dRtvDescHeap) { g_pd3dRtvDescHeap->Release(); g_pd3dRtvDescHeap = NULL; }
    if (g_pd3dSrvDescHeap) { g_pd3dSrvDescHeap->Release(); g_pd3dSrvDescHeap = NULL; }
    if (g_pd3dDevice) { g_pd3dDevice->Release(); g_pd3dDevice = NULL; }
    if (g_dxgiFactory) { g_dxgiFactory->Release(); g_dxgiFactory = NULL; }
}

    static D3D12_CPU_DESCRIPTOR_HANDLE CpuSrvHandle(UINT idx)
    {
        D3D12_CPU_DESCRIPTOR_HANDLE h = g_pd3dSrvDescHeap->GetCPUDescriptorHandleForHeapStart();
        h.ptr += (SIZE_T)idx * g_srvDescriptorSize;
        return h;
    }

    static D3D12_GPU_DESCRIPTOR_HANDLE GpuSrvHandle(UINT idx)
    {
        D3D12_GPU_DESCRIPTOR_HANDLE h = g_pd3dSrvDescHeap->GetGPUDescriptorHandleForHeapStart();
        h.ptr += (UINT64)idx * g_srvDescriptorSize;
        return h;
    }

    static inline UINT64 AlignUp(UINT64 v, UINT64 align)
    {
        return (v + (align - 1)) & ~(align - 1);
    }

    static bool CreateImguiTextureFromRGBA(
        const void* rgbaPixels, int width, int height,
        Dx12ImguiTexture& outTex,
        IDXGISwapChain3* swapChainForAllocatorReset // pass your pSwapChain from RenderImGui_DX12
    )
    {

        if (!g_pd3dDevice) { LOG("[ImGuiTex] no device\n"); return false; }
        if (!g_pd3dSrvDescHeap) { LOG("[ImGuiTex] no srv heap\n"); return false; }
        if (!g_pd3dCommandQueue) { LOG("[ImGuiTex] no queue\n"); return false; }
        if (!g_pd3dCommandList) { LOG("[ImGuiTex] no cmdlist\n"); return false; }

        if (!g_pd3dDevice || !g_pd3dSrvDescHeap || !g_pd3dCommandQueue || !g_pd3dCommandList)
            return false;
        if (!rgbaPixels || width <= 0 || height <= 0)
            return false;

        // If already created, do nothing
        if (outTex.texture)
            return true;

        // --- Create DEFAULT heap texture ---
        D3D12_RESOURCE_DESC texDesc = {};
        texDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
        texDesc.Alignment = 0;
        texDesc.Width = (UINT)width;
        texDesc.Height = (UINT)height;
        texDesc.DepthOrArraySize = 1;
        texDesc.MipLevels = 1;
        texDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        texDesc.SampleDesc.Count = 1;
        texDesc.SampleDesc.Quality = 0;
        texDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
        texDesc.Flags = D3D12_RESOURCE_FLAG_NONE;

        D3D12_HEAP_PROPERTIES heapDefault = {};
        heapDefault.Type = D3D12_HEAP_TYPE_DEFAULT;

        HRESULT hr = g_pd3dDevice->CreateCommittedResource(
            &heapDefault,
            D3D12_HEAP_FLAG_NONE,
            &texDesc,
            D3D12_RESOURCE_STATE_COPY_DEST,
            nullptr,
            IID_PPV_ARGS(&outTex.texture));

        if (FAILED(hr) || !outTex.texture)
            return false;

        // --- Create UPLOAD heap buffer for pixels ---
        const UINT bytesPerPixel = 4;
        const UINT rowPitch = (UINT)width * bytesPerPixel;
        // D3D12 requires row pitch aligned to 256 for CopyTextureRegion using placed footprint
        const UINT alignedRowPitch = (UINT)AlignUp(rowPitch, D3D12_TEXTURE_DATA_PITCH_ALIGNMENT);
        const UINT64 uploadSize = (UINT64)alignedRowPitch * (UINT64)height;

        D3D12_HEAP_PROPERTIES heapUpload = {};
        heapUpload.Type = D3D12_HEAP_TYPE_UPLOAD;

        D3D12_RESOURCE_DESC bufDesc = {};
        bufDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
        bufDesc.Alignment = 0;
        bufDesc.Width = uploadSize;
        bufDesc.Height = 1;
        bufDesc.DepthOrArraySize = 1;
        bufDesc.MipLevels = 1;
        bufDesc.Format = DXGI_FORMAT_UNKNOWN;
        bufDesc.SampleDesc.Count = 1;
        bufDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
        bufDesc.Flags = D3D12_RESOURCE_FLAG_NONE;

        hr = g_pd3dDevice->CreateCommittedResource(
            &heapUpload,
            D3D12_HEAP_FLAG_NONE,
            &bufDesc,
            D3D12_RESOURCE_STATE_GENERIC_READ,
            nullptr,
            IID_PPV_ARGS(&outTex.upload));

        if (FAILED(hr) || !outTex.upload)
            return false;

        // Map upload + copy pixels with row pitch
        uint8_t* mapped = nullptr;
        D3D12_RANGE range = { 0, 0 }; // we won't read
        hr = outTex.upload->Map(0, &range, (void**)&mapped);
        if (FAILED(hr) || !mapped)
            return false;

        const uint8_t* src = (const uint8_t*)rgbaPixels;
        for (int y = 0; y < height; ++y)
        {
            memcpy(mapped + (size_t)alignedRowPitch * y, src + (size_t)rowPitch * y, rowPitch);
        }
        outTex.upload->Unmap(0, nullptr);

        // --- Record copy commands on YOUR existing ImGui command list ---
        // Use current backbuffer allocator, since you already reset it each present
        UINT bb = swapChainForAllocatorReset ? swapChainForAllocatorReset->GetCurrentBackBufferIndex() : 0;
        ID3D12CommandAllocator* alloc = g_commandAllocators[bb];
        if (!alloc) return false;

        alloc->Reset();
        g_pd3dCommandList->Reset(alloc, nullptr);

        // Source footprint
        D3D12_PLACED_SUBRESOURCE_FOOTPRINT footprint = {};
        footprint.Offset = 0;
        footprint.Footprint.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        footprint.Footprint.Width = (UINT)width;
        footprint.Footprint.Height = (UINT)height;
        footprint.Footprint.Depth = 1;
        footprint.Footprint.RowPitch = alignedRowPitch;

        D3D12_TEXTURE_COPY_LOCATION srcLoc = {};
        srcLoc.pResource = outTex.upload;
        srcLoc.Type = D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT;
        srcLoc.PlacedFootprint = footprint;

        D3D12_TEXTURE_COPY_LOCATION dstLoc = {};
        dstLoc.pResource = outTex.texture;
        dstLoc.Type = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;
        dstLoc.SubresourceIndex = 0;

        g_pd3dCommandList->CopyTextureRegion(&dstLoc, 0, 0, 0, &srcLoc, nullptr);

        // Transition to PIXEL_SHADER_RESOURCE so ImGui can sample it
        D3D12_RESOURCE_BARRIER barrier = {};
        barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
        barrier.Transition.pResource = outTex.texture;
        barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
        barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;
        barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;

        g_pd3dCommandList->ResourceBarrier(1, &barrier);

        g_pd3dCommandList->Close();

        ID3D12CommandList* lists[] = { g_pd3dCommandList };
        g_pd3dCommandQueue->ExecuteCommandLists(1, lists);

        // NOTE: We should ideally fence here to know upload is complete before freeing upload.
        // To keep it simple, we KEEP outTex.upload alive (stored in the struct).
        // You can later release it after a few frames or when you add a fence.

        // --- Create SRV in the shared ImGui heap ---
        outTex.srvIndex = g_nextSrvIndex++;
        outTex.gpuHandle = GpuSrvHandle(outTex.srvIndex);
        outTex.width = width;
        outTex.height = height;

        D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
        srvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
        srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
        srvDesc.Texture2D.MipLevels = 1;

        g_pd3dDevice->CreateShaderResourceView(outTex.texture, &srvDesc, CpuSrvHandle(outTex.srvIndex));

        return true;
    }

    static bool LoadImageRGBA_WIC(const wchar_t* filename, std::vector<uint8_t>& outRGBA, int& outW, int& outH)
    {
        outRGBA.clear();
        outW = outH = 0;

        IWICImagingFactory* factory = nullptr;
        HRESULT hr = CoCreateInstance(
            CLSID_WICImagingFactory,
            nullptr,
            CLSCTX_INPROC_SERVER,
            IID_PPV_ARGS(&factory));

        if (FAILED(hr) || !factory)
            return false;

        IWICBitmapDecoder* decoder = nullptr;
        hr = factory->CreateDecoderFromFilename(filename, nullptr, GENERIC_READ, WICDecodeMetadataCacheOnLoad, &decoder);
        if (FAILED(hr) || !decoder) { factory->Release(); return false; }

        IWICBitmapFrameDecode* frame = nullptr;
        hr = decoder->GetFrame(0, &frame);
        if (FAILED(hr) || !frame) { decoder->Release(); factory->Release(); return false; }

        UINT w = 0, h = 0;
        frame->GetSize(&w, &h);
        outW = (int)w;
        outH = (int)h;

        // Convert to 32bpp RGBA
        IWICFormatConverter* conv = nullptr;
        hr = factory->CreateFormatConverter(&conv);
        if (FAILED(hr) || !conv) { frame->Release(); decoder->Release(); factory->Release(); return false; }

        hr = conv->Initialize(
            frame,
            GUID_WICPixelFormat32bppRGBA,
            WICBitmapDitherTypeNone,
            nullptr,
            0.0,
            WICBitmapPaletteTypeCustom);

        if (FAILED(hr)) {
            conv->Release(); frame->Release(); decoder->Release(); factory->Release();
            return false;
        }

        const size_t stride = (size_t)outW * 4;
        outRGBA.resize(stride * (size_t)outH);

        hr = conv->CopyPixels(nullptr, (UINT)stride, (UINT)outRGBA.size(), outRGBA.data());

        conv->Release();
        frame->Release();
        decoder->Release();
        factory->Release();

        return SUCCEEDED(hr);
    }

    static bool CreateImguiTextureFromFilePNG(
        const wchar_t* path,
        Dx12ImguiTexture& outTex,
        IDXGISwapChain3* swapChain)
    {
        std::vector<uint8_t> rgba;
        int w = 0, h = 0;
        if (!LoadImageRGBA_WIC(path, rgba, w, h)) {
            LOG("[ImGuiTex] WIC load failed: %ls\n", path);
            return false;
        }

        // reuse your existing uploader:
        if (!CreateImguiTextureFromRGBA(rgba.data(), w, h, outTex, swapChain)) {
            LOG("[ImGuiTex] CreateImguiTextureFromRGBA failed for %ls\n", path);
            return false;
        }

        LOG("[ImGuiTex] Loaded %ls (%dx%d) srv=%u gpu=%llx\n",
            path, w, h, outTex.srvIndex, (unsigned long long)outTex.gpuHandle.ptr);

        return true;
    }

    void NopBytes(uintptr_t address, size_t length)
    {
        DWORD oldProtect;

        VirtualProtect((void*)address, length, PAGE_EXECUTE_READWRITE, &oldProtect);

        memset((void*)address, 0x90, length);

        VirtualProtect((void*)address, length, oldProtect, &oldProtect);
    }

    BYTE original1[5];
    BYTE original2[5];
    bool originalsSaved = false;

    void DisableGameCameraControl()
    {
        uintptr_t addr1 = moduleBase + 0x1657FB;
        uintptr_t addr2 = moduleBase + 0x165837;

        DWORD oldProtect;

        VirtualProtect((void*)addr1, 5, PAGE_EXECUTE_READWRITE, &oldProtect);

        if (!originalsSaved)
        {
            memcpy(original1, (void*)addr1, 5);
            memcpy(original2, (void*)addr2, 5);
            originalsSaved = true;
        }

        memset((void*)addr1, 0x90, 5);
        VirtualProtect((void*)addr1, 5, oldProtect, &oldProtect);

        VirtualProtect((void*)addr2, 5, PAGE_EXECUTE_READWRITE, &oldProtect);
        memset((void*)addr2, 0x90, 5);
        VirtualProtect((void*)addr2, 5, oldProtect, &oldProtect);
    }

    void RestoreGameCameraControl()
    {
        DWORD oldProtect;

        uintptr_t addr1 = moduleBase + 0x1657FB;
        uintptr_t addr2 = moduleBase + 0x165837;

        VirtualProtect((void*)addr1, 5, PAGE_EXECUTE_READWRITE, &oldProtect);
        memcpy((void*)addr1, original1, 5);
        VirtualProtect((void*)addr1, 5, oldProtect, &oldProtect);

        VirtualProtect((void*)addr2, 5, PAGE_EXECUTE_READWRITE, &oldProtect);
        memcpy((void*)addr2, original2, 5);
        VirtualProtect((void*)addr2, 5, oldProtect, &oldProtect);
    }

    BYTE originalVMWrite[44];

    void DisableViewMatrixWrites()
    {
        uintptr_t addr = moduleBase + 0x125E14;

        memcpy(originalVMWrite, (void*)addr, 44);

        BYTE patch[44] =
        {
            0x90,0x90,0x90,0x90,0x90,0x90,0x90,0x90,
            0x0F,0x10,0x4E,0x10,
            0x90,0x90,0x90,0x90,0x90,0x90,0x90,0x90,
            0x0F,0x10,0x46,0x20,
            0x90,0x90,0x90,0x90,0x90,0x90,0x90,0x90,
            0x0F,0x10,0x4E,0x30,
            0x90,0x90,0x90,0x90,0x90,0x90,0x90,0x90
        };

        DWORD oldProtect;
        VirtualProtect((void*)addr, 44, PAGE_EXECUTE_READWRITE, &oldProtect);
        memcpy((void*)addr, patch, 44);
        VirtualProtect((void*)addr, 44, oldProtect, &oldProtect);
    }

    void RestoreViewMatrixWrites()
    {
        uintptr_t addr = moduleBase + 0x125E14;

        DWORD oldProtect;
        VirtualProtect((void*)addr, 44, PAGE_EXECUTE_READWRITE, &oldProtect);
        memcpy((void*)addr, originalVMWrite, 44);
        VirtualProtect((void*)addr, 44, oldProtect, &oldProtect);
    }





static void RenderImGui_DX12(IDXGISwapChain3* pSwapChain) {
    if (!ImGui::GetIO().BackendRendererUserData) {
        if (SUCCEEDED(pSwapChain->GetDevice(IID_PPV_ARGS(&g_pd3dDevice)))) {
            {
                D3D12_DESCRIPTOR_HEAP_DESC desc = {};
                desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
                desc.NumDescriptors = NUM_BACK_BUFFERS;
                desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
                desc.NodeMask = 1;
                if (g_pd3dDevice->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&g_pd3dRtvDescHeap)) != S_OK)
                    return;

                SIZE_T rtvDescriptorSize = g_pd3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
                D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = g_pd3dRtvDescHeap->GetCPUDescriptorHandleForHeapStart();
                for (UINT i = 0; i < NUM_BACK_BUFFERS; ++i) {
                    g_mainRenderTargetDescriptor[i] = rtvHandle;
                    rtvHandle.ptr += rtvDescriptorSize;
                }
            }

            {
                D3D12_DESCRIPTOR_HEAP_DESC desc = {};
                desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
                desc.NumDescriptors = 1;
                desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
                if (g_pd3dDevice->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&g_pd3dSrvDescHeap)) != S_OK)
                    return;
            }

            for (UINT i = 0; i < NUM_BACK_BUFFERS; ++i)
                if (g_pd3dDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&g_commandAllocators[i])) != S_OK)
                    return;

            if (g_pd3dDevice->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, g_commandAllocators[0], NULL, IID_PPV_ARGS(&g_pd3dCommandList)) != S_OK ||
                g_pd3dCommandList->Close() != S_OK)
                return;

            ImGui_ImplDX12_Init(g_pd3dDevice, NUM_BACK_BUFFERS,
                DXGI_FORMAT_R8G8B8A8_UNORM, g_pd3dSrvDescHeap,
                g_pd3dSrvDescHeap->GetCPUDescriptorHandleForHeapStart(),
                g_pd3dSrvDescHeap->GetGPUDescriptorHandleForHeapStart());
        }
    }


    //printf("DrawImGuiFrame called\n");


    InitializePointers();

    int currentHp = 0;
    int maxHp = 0;
    int currentKarma = 0;
    int MaxKarma = 0;

    if (!H::bShuttingDown) {
        if (!g_mainRenderTargetResource[0]) {
            CreateRenderTarget(pSwapChain);
        }

    if (ImGui::GetCurrentContext() && g_pd3dCommandQueue && g_mainRenderTargetResource[0]) {
    ImGui_ImplDX12_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();
    
            //ImguiTexView tex = ImguiTextureManager::GetOrLoadPNG(s_pngPath, pSwapChain);

           // if (H::bShowDemoWindow) {
             //   ImGui::ShowDemoWindow();
            //}
            //uintptr_t allocator = moduleBase + 0x9BA920;
            //debug_task_pc(allocator);
          
            //PollVMs();

            //DrawBdxMenuText();

            /*if (g_testTex.gpuHandle.ptr)
            {
                ImGui::Begin("Texture test");
                ImGui::Text("SRV idx=%u", g_testTex.srvIndex);
                ImGui::Image((ImTextureID)g_testTex.gpuHandle.ptr, ImVec2(256, 256));
                ImGui::End();
            }
            else
            {
                ImGui::Begin("Texture test");
                ImGui::Text("g_testTex not created yet");
                ImGui::End();
            }*/

            /*if (g_pngTex.gpuHandle.ptr)
            {
                ImGui::Begin("PNG test");
                ImGui::Text("srv=%u", g_pngTex.srvIndex);

                // Draw at native size, or scale
                ImGui::Image((ImTextureID)g_pngTex.gpuHandle.ptr, ImVec2((float)g_pngTex.width, (float)g_pngTex.height));
                ImGui::End();
            }*/

            if (PartyModeToggle)
            {
                if (!gCurrentFog)
                {
                    //printf("[FOG] PartyMode ON but no fog\n");
                    return;
                }

                float amplitude = GetCurrentAmplitude();

                if (!std::isfinite(amplitude))
                {
                   /// printf("[FOG] Invalid amp: %f\n", amplitude);
                    return;
                }

               // printf("[FOG] ptr=%p amp=%f\n", gCurrentFog, amplitude);
                UpdatePartyFog(gCurrentFog, amplitude);
            }



            OnFrameUpdate();
           

            UpdateViewMatrix();

            UpdateFunctionAddresses();

           // UpdateP2MovementOncePerFrame();


           // if (!g_cam2TaskCreated) {

           //     Cam2_TickInit();
           // }

            XMMATRIX viewMatrix = XMLoadFloat4x4(reinterpret_cast<XMFLOAT4X4*>(&Matrix));

            //float fov = XMConvertToRadians(85.94367f);
            float fov = 1.5f;
            //float aspectX = 0.8000000119f;
            //float aspectY = 0.8666666746f;

            //float nearZ = 0.1f;// 0.1f
            float nearZ = 1.0f;
            float farZ = 4000000.0f; // 1000.0f
            //float farZ = 10000.0f;
            Vector2* viewport = 0;

            if (DetectGameVersion() == "Steam") {
                viewport = reinterpret_cast<Vector2*>(SteamPointer - 0x26CC18);

            }
            else if (DetectGameVersion() == "Epic") {
                viewport = reinterpret_cast<Vector2*>(EpicPointer - 0x26CC18);
            }


            Vector2 myViewport = *viewport; // Dereference the pointer to read the value.

            float aspectX = myViewport.x;
            float aspectY = myViewport.y;
            float aspectRatio = aspectX / aspectY;

            /*if (FreeCameraToggle == true) {


                CreateFreeCamera();
                UpdateFreeCamera();
            }

            if (EntityMovementToggle == true) {
                //CreateEntityMovement();
                FreeCameraToggle = true;
                UpdateEntityMovement();

            }*/

            bool cameraOverride =
                FreeCameraToggle ||
                LevelEditorToggle ||
                (CinematicCameraToggle && !g_cineGameplayEnabled);

            bool anyTool =
                cameraOverride ||
                EntityMovementToggle;

            if (anyTool)
            {
                if (!freeCameraCreated)
                {
                    CreateFreeCamera();
                   // DisableViewMatrixWrites();
                    DisableGameCameraControl();
                    freeCameraCreated = true;
                }

                //-----------------------------------------
                // NON-CAMERA SYSTEMS
                //-----------------------------------------

                if (EntityMovementToggle)
                    UpdateEntityMovement();

                if (LevelEditorToggle)
                {
                    DrawLevelEditorInfoBox();

                    float screenWidth = ImGui::GetIO().DisplaySize.x;
                    float screenHeight = ImGui::GetIO().DisplaySize.y;

                    UpdateLevelEditor(screenWidth, screenHeight);
                }

                //-----------------------------------------
                // CAMERA (ONLY if overriding)
                //-----------------------------------------

                if (cameraOverride)
                {
                    UpdateFreeCamera_ViewMatrix(viewMatrixAddress);
                }

                //-----------------------------------------
                // CINEMATIC UI (ONLY when enabled)
                //-----------------------------------------

                if (CinematicCameraToggle)
                {
                    UpdateCinematicSlidersFromUI();
                    DrawCinematicCameraInfoBox();

                    obj_camera* objcamera = GetOBJCameraInstanceFunction();
                    objcamera = reinterpret_cast<obj_camera*>(
                        reinterpret_cast<uintptr_t>(objcamera) + 0x08);

                    DrawCinematicPathAndKeyframes(
                        viewMatrix,
                        objcamera->m_fov,
                        aspectRatio,
                        nearZ,
                        farZ
                    );
                }
            }
            else
            {
                if (freeCameraCreated)
                {
                    RestoreGameCameraControl();
                   // RestoreViewMatrixWrites();

                    freecamInitialized = false;
                    DestroyFreeCamera();
                    freeCameraCreated = false;
                }
            }



            bool isInArea = (*(bool*)(moduleBase + 0x9BA8D0)); // 0x9B80D0 = old updated to steam

            bool hasEnteredArea = false;

            bool isPauseMenuOpen = *(bool*)(moduleBase + 0x9006B0);

            uintptr_t YS_STATUS = *(uintptr_t*)(moduleBase + 0x2AF7940);

            int event_mode = *(int*)(moduleBase + 0xB65210);

            uint8_t event_ctrl_end = *(uint8_t*)(moduleBase + 0xB64FA9);
            int event_buffer_size = *(int*)(moduleBase + 0xB65570);

            int event_voice_play_count = *(int*)(moduleBase + 0xB64FB8);
            int event_m_frame = *(int*)(moduleBase + 0xB64F98);
            int event_read_wait_frame = *(int*)(moduleBase + 0xB655FC);
            //int event_mode = *(int*)(moduleBase + 0xB65210); 
            int ground_type_value = 0;

            // Define the address of LOCKON_ENTITY
            uintptr_t LOCKON_ENTITY_ADDR = moduleBase + 0x2A1120C;

            uint32_t LOCKON_ENTITY = *reinterpret_cast<uint32_t*>(LOCKON_ENTITY_ADDR);

            // Read the 32-bit value from the address
            //uint32_t LOCKON_ENTITY = *(uint32_t*)LOCKON_ENTITY_ADDR;

            uintptr_t LockedOnEntity = 0;

            //StatusPrintfFunction("LOCKON_ENTITY Address: 0x%p\n", (void*)LOCKON_ENTITY_ADDR);

            // Print the value stored at that address
            //StatusPrintfFunction("LOCKON_ENTITY Value: 0x%X\n", LOCKON_ENTITY);

            //StatusPrintfFunction("LOCKON_ENTITY before decode: 0x%X\n", LOCKON_ENTITY);


            if (LOCKON_ENTITY != 0) {
                //uint32_t LOCKON_ENTITY = *LOCKON_ENTITY_ADDR;
                
                LockedOnEntity = CustomDecodePointer(LOCKON_ENTITY, moduleBase);
                //StatusPrintfFunction("LockedOnEntity after decode: 0x%p\n", (void*)LockedOnEntity);
            }

            //StatusPrintfFunction("");

            /*if (event_mode == 3) {
                if (!g_EventPlayerOpened) {
                    EventPlayerWidget();
                    g_EventPlayerOpened = true;
                }

                UpdateCutscenePlayer();
                DebugSliderState();
                UpdateCutsceneSliderManual();
            }
            else {
                g_EventPlayerOpened = false;
            }*/



            if (YS_STATUS) {
                if (isPauseMenuOpen == 0) {
                    if (GetPlayerFunction()) {

                        Entity* player = GetPlayer1();
                        uintptr_t playerGroundTypeAddress = *reinterpret_cast<uintptr_t*>(player + 0x780);
                        if (playerGroundTypeAddress != 0) {
                            ground_type_value = *reinterpret_cast<int*>(playerGroundTypeAddress);

                        }

                        StatusPrintfFunction("player(%.1f, %.1f, %.1f, %.1f)\n", player->ent_pos.x, player->ent_pos.y, player->ent_pos.z, player->ent_pos.w);


                        /*if (groundTypeNames.find(ground_type_value) != groundTypeNames.end()) {
                            // Print the corresponding enum name
                            std::string groundTypeName = groundTypeNames[ground_type_value];
                            StatusPrintfFunction(" @066ground @666%s \n", groundTypeName.c_str());
                        }
                        else {
                            // Handle the case where the enum value is not found in the map
                            StatusPrintfFunction("@066ground @666UnknownGroundType \n");
                        }*/


                   /*     StatusPrintfFunction("%d %d %d %d @066F@666 %d @066r@666%d @066%s@666 @066w@666%d\n",
                    event_ctrl_end,
                    event_buffer_size + 1023 / 1024,
                    SoundGetLinkNumFunction(3),
                    event_voice_play_count,
                    event_m_frame - 1,
                    GetEventRestFrameFunction(),
                    GetEventProjectNameFunction(),
                    event_read_wait_frame);*/

                        if (LockedOnEntity != 0) {
                            //StatusPrintfFunction("Inside the if block! LockedOnEntity: 0x%p\n", (void*)LockedOnEntity);

                            const char* EntryName = "";
                            EntryName = GetOBJEntryNameFunction((YS::OBJ*)LockedOnEntity);

                            uintptr_t entityStartAddress1 = (uintptr_t)LockedOnEntity; // Get the starting address of the entity
                            uintptr_t entityStatsAddress = entityStartAddress1 + 0x5C0; // Offset to the stats section
                            uintptr_t* obj_sheet = reinterpret_cast<uintptr_t*>(reinterpret_cast<char*>(LockedOnEntity) + 0x5C0);

                            // Print the entityStatsAddress
                           // StatusPrintfFunction("Entity Stats Address: 0x%p\n", (void*)entityStatsAddress);

                            // Read and print the value at entityStatsAddress
                            uintptr_t statsValue = *(uintptr_t*)entityStatsAddress; // Assuming it's a 32-bit value
                            //StatusPrintfFunction("Value at Entity Stats Address: 0x%llx\n", statsValue);

                            // Check if stats address is valid
                            if (obj_sheet != 0 && statsValue != 0) {
                                // Read current HP from the stats address
                                int currentHp = *reinterpret_cast<int*>(statsValue);
                                // Read max HP from the next memory address
                                int maxHp = *reinterpret_cast<int*>(statsValue + sizeof(int));
                                uintptr_t defAddress = *obj_sheet + 0x18C; // Defense pointer address
                                uintptr_t atkAddress = *obj_sheet + 0x188; // Attack pointer address

                                currentKarma = (int)*(float*)(LockedOnEntity + 0xD48);
                                MaxKarma = (int)*(float*)(LockedOnEntity + 0xD4C);


                                // Read the values at defAddress and atkAddress
                                int defValue = *reinterpret_cast<int*>(defAddress);
                                int atkValue = *reinterpret_cast<int*>(atkAddress);

                                uintptr_t ActObjectptr = (uintptr_t)LockedOnEntity + 0x4A8;

                                uintptr_t ActValue64 = *reinterpret_cast<uintptr_t*>(ActObjectptr);

                                uintptr_t ResultAct = 0;


                                //StatusPrintfFunction("KarmaOffset Address: 0x%llx\n", karmaOffset);
                                //StatusPrintfFunction("KarmaMaxOffset Address: 0x%llx\n", karmaMaxOffset);

                                if (ActValue64 != 0) {
                                    // ActValue64 contains the 64-bit pointer, now extract the 32-bit pointer



                                    uint32_t ActValue32 = *reinterpret_cast<uint32_t*>(ActValue64);

                                    //uintptr_t moduleBase = reinterpret_cast<uintptr_t>(baseModule);

                                    // Decode the pointer using CustomDecodePointer
                                    //ResultAct = DecodePointerFunction(ActValue32);
                                    ResultAct = CustomDecodePointer(ActValue32, moduleBase);

                                    //uintptr_t ResultAct = *reinterpret_cast<uintptr_t*>(&ActValue32);

                                    //char ActValueStr[200];
                                    //sprintf_s(ActValueStr, sizeof(ActValueStr), "Act Value Str: 0x%08X\n", ResultAct);

                                    const char* actString = reinterpret_cast<const char*>(ResultAct);







                                    StatusPrintfFunction("@060%s hp:%d/%d atk:%d def:%d karma %d/%d\n", EntryName, currentHp, maxHp, atkValue, defValue, currentKarma, MaxKarma);
                                    StatusPrintfFunction("@060Current Act: '%s'\n", actString);
                                }


                            }
                        }
                    }

                        

                        entityCount = 0;
                        YS::OBJ* objPtr = EachOBJFunction(nullptr); // Start with nullptr to get the first object
                        // Initialize the entity count
                        while (objPtr) {



                            entityCount++;



                            //  const char* objEntryName = GetObjEntryName((OBJ_125*)objPtr);
                             // char outputString[1024]; // Adjust the size as needed
                             // sprintf_s(outputString, "objPtr: 0x%llx, Name: %s\n", (unsigned long long)objPtr, objEntryName);
                             // OutputDebugStringA(outputString);


                            objPtr = EachOBJFunction(objPtr);


                        }
                        StatusPrintfFunction("@660ObjNum@666 %d\n", entityCount);



                    }
                }
            


            if (isInArea && !hasEnteredArea) {

                


                hasEnteredArea = true;
                //LOG(" START! ***\n");

            }
            else if (!isInArea && hasEnteredArea) {
                hasEnteredArea = false; // Reset the "entered area" flag
                // TwilightTownMusicExecutedOnce = false;
                //LOG("AREA::Exit\n");
                //SetConsoleTextAttribute(consoleOutput, 0);
                //StopAudio();
            }

            

            if (ShowBuild) {

                ImFont* font = g_KHMenuFont;

                ImGui::PushFont(g_KHMenuFont);

                float scale = 0.5f; // 1.0 = normal, 2.0 = double size
                float fontSize = font->FontSize * scale;


                // Get the window draw list
                ImDrawList* draw_list = ImGui::GetBackgroundDrawList();

                // Define the position where the text will start
                ImVec2 text_pos = ImVec2(10.0f, 10.0f); // You can adjust this as needed

                std::stringstream ss;
                ss << PROJECT_NAME << " (" << BUILD_TYPE << ")";

                if ((uintptr_t)baseModule != 0x140000000ULL) {
                    ss << " (ASLR)";
                }
                else {
                    ss << " (NO ASLR)\n";
                }
                ss << BUILD_STAMP;


                // Draw the text
                draw_list->AddText(text_pos, IM_COL32(255, 255, 255, 255), ss.str().c_str());

                ImGui::PopFont();
            }

            

            /*for (const auto& name : entityNames) {
                //Entity* entityPtr = FindEntityByName(name.c_str()); // Adjust FindEntityByName if necessary to accept std::string
                //if (entityPtr) {
                    //AddCaption("ANSEM1", "Ansem", "Darkness Within Darkness!", 0xD7, entityPtr);
                    //ansem bosses
                for (YS::OBJ* i = EachOBJFunction(nullptr); i; i = EachOBJFunction(i)) {
                    Entity* entity = reinterpret_cast<Entity*>(i);
                    const char* names = GetOBJEntryNameFunction(reinterpret_cast<YS::OBJ*>(i));
                    if (std::string(names) == name) {
                        
                    if (name == "P_EX100_XM") {
                        //AddCaption(name.c_str(), "Ansem", "Take This!", 0xD7, entityPtr);
                       // AddCaption(name.c_str(), "Ansem", "My strength returns!", 0xD8, entityPtr);
                       // AddCaption(name.c_str(), "Ansem", "Come Here Boy!", 0xCD, entityPtr);
                        //AddCaption(name.c_str(), "Ansem", "HAARGH!!", 0x48, entityPtr); // ansem is dead
                       // AddCaption(name.c_str(), "Ansem", "Cum Guardian!", 0xCF, entityPtr);
                       // AddCaption(name.c_str(), "Ansem", "You insolent little!", 0xC9, entityPtr);
                       // AddCaption(name.c_str(), "Ansem", "SUBMIT BITCH!!", 0xD3, entityPtr);
                       // AddCaption(name.c_str(), "Ansem", "Taste Lightning!", 0xD6, entityPtr);
                       // AddCaption(name.c_str(), "Ansem", "Come back here Bitch!", 0xd4, entityPtr);
                       // AddCaption(name.c_str(), "Ansem", "Cum! open your heart", 0xd5, entityPtr);
                       // AddCaption(name.c_str(), "Ansem", "I'm avoiding you", 0xc8, entityPtr);
                        AddCaption(name.c_str(), "Sora", "Hello World", 0x0, entity);
                        AddCaption(name.c_str(), "Sora", "How Are You?", 0x0, entity);

                    }







                    }
                    else {

                    }
                }
            }*/


            
        
            
            //if (H::bShowDemoWindow) {
           //     ImGui::ShowDemoWindow();
           // }
            //InitializePointers();
            
            //obj_camera* camera = 0;//GetOBJCameraInstanceFunction();
            //obj_camera* camera1 = 0;

            

            /*if (DetectGameVersion() == "Steam") {
                camera = reinterpret_cast<obj_camera*>(SteamPointer - 0x3F4930);

            }
            else if (DetectGameVersion() == "Epic") {
                camera = reinterpret_cast<obj_camera*>(EpicPointer - 0x3F4930);
            }*/
            

            
           // if (FreeCameraToggle == false) {
                
                
            //    DestroyFreeCamera();
          //  }

            /*if (LevelEditorToggle == true) {


                //CreateLevelEditorCamera(camera);
                CreateLevelEditorCamera(camera1);
                UpdateLevelEditorCamera(camera1);



                //GetOBJControlOFFFunction(0);
            }
            if (LevelEditorToggle == false) {
                DestroyLevelEditorCamera(camera1);
            }*/
            
            
            

            auto bdrawlist = ImGui::GetBackgroundDrawList();

            //DrawCaptions(bdrawlist);

            uintptr_t debugDrawFlagAddress = 0;//updated to steam
            

                if (DetectGameVersion() == "Steam") {
                    debugDrawFlagAddress = (SteamPointer - 0x3C3D80);

                }
                else if (DetectGameVersion() == "Epic") {
                    debugDrawFlagAddress = (EpicPointer - 0x3C3D80);
                }

            // Cast this address to a pointer to BITFLAG
            BITFLAG* debugDrawFlagPtr = reinterpret_cast<BITFLAG*>(debugDrawFlagAddress);

            // Dereference the pointer to get the BITFLAG struct
            BITFLAG debugDrawFlag = *debugDrawFlagPtr;

            // Convert the flag array to an integer for easy bitwise operations
            uint32_t flags = *(reinterpret_cast<uint32_t*>(debugDrawFlag.Flag));

            //const int DEBUGDRAW_OPTION_TARGET = 0x01;
            const int DEBUGDRAW_OPTION_STATUS = 0x80;
            const int DEBUGDRAW_OPTION_ZONE = 0x40;
            const int DEBUGDRAW_OPTION_BBOX = 0x20;
            const int DEBUGDRAW_OPTION_RC = 0x10;
            const int DEBUGDRAW_OPTION_ATTACK = 0x08;
            const int DEBUGDRAW_OPTION_HIT = 0x04;
            const int DEBUGDRAW_OPTION_OBJ = 0x02; // Binary 10
            const int DEBUGDRAW_OPTION_BG = 0x01; // Binary 01

            DWORD* debugAddress = reinterpret_cast<DWORD*>(moduleBase + 0x749804);//updated to steam
            uint8_t* debugAddress2 = reinterpret_cast<uint8_t*>(moduleBase + 0x749838);//updated to steam

            int* StatusFlag = (int*)(moduleBase + 0x2A11400);//updated to steam




            auto fdrawlist = ImGui::GetForegroundDrawList();

            
            //int event_mode = 0;

            if (DetectGameVersion() == "Steam") {
                event_mode = *(int*)(SteamPointer + 0x57C78);

            }
            else if (DetectGameVersion() == "Epic") {
                event_mode = *(int*)(EpicPointer + 0x57C78);
            }


            if (!GetPlayerFunction() && event_mode != 3) {
                //printf("PLAYER NOT FOUND!!\n");
            }
            else {
                memcpy(Matrix, reinterpret_cast<const float*>(viewMatrixAddress), sizeof(Matrix));
                //MMATRIX* mat = MatrixOperator((XMMATRIX*)Matrix, (XMMATRIX*)GetProjection(1), (XMMATRIX*)((char*)knGetCamera(1) + 8));

               

                if (isPlaying == true) {
                    uintptr_t SoundBGMFadeFunctionAddress = ReadLEA32(Patterns::pattern_for_SoundBGMFade, L"SoundBGMFade", 0, 1, 5);
                    FunctionCalls::tSoundBGMFadeOut SoundBGMFadeFunction = reinterpret_cast<FunctionCalls::tSoundBGMFadeOut>(SoundBGMFadeFunctionAddress);

                    

                    

                    SoundBGMFadeFunction(0, 0);
                    SoundBGMFadeFunction(0, 1);
                }


                //float* ScreenOffsetX = (float*)(moduleBase + 0x5AFD80);
               // float* ScreenOffsetY = (float*)(moduleBase + 0x5AFD84);
               // *ScreenOffsetX = -1420.0f;
               // *ScreenOffsetY = -1750.0f;

                //int objectIndex = 0;
                //ImVec2 screenPos1 = Memory::Internal::read<ImVec2>(moduleBase + 0x716B58);
               /* StatusPrintf("View Matrix:\n");
                for (int i = 0; i < 16; ++i) {
                    StatusPrintf("%f ", Matrix[i]);
                    if ((i + 1) % 4 == 0) {
                        StatusPrintf("\n"); // New line after every 4 elements for a 4x4 matrix format
                    }
                }*/

                // StatusPrintf("Address of View Matrix: %p\n", reinterpret_cast<void*>(viewMatrix));


                 // Calculate and store screen positions using entityPositions

                if ((*debugAddress & 0x400) != 0) { // 0x400 bitflag = infinite item
                    //debugprotectionInfItemsOn();
                }
                else {
                    //debugprotectionInfItemsOff();
                }

                if ((*debugAddress & 0x10000) != 0) { // 0x10000 bitflag = FREE ABILITY

                    *StatusFlag = 0x2000;

                }

                if ((*debugAddress & 0x100) != 0) { // 0x100 bitflag = STOP ENEMY
                    debugprotectionLobotomiseAIOn();
                }
                else {
                    debugprotectionLobotomiseAIOff();
                }





                YS::OBJ* i;

                Entity* player = GetPlayer1();

                std::vector<Entity*> partyMembers = GetParty();

                for (YS::OBJ* i = EachOBJFunction(nullptr); i; i = EachOBJFunction(i)) {
                    // Cast OBJ_132* to Entity*
                    Entity* entity = reinterpret_cast<Entity*>(i);

                    currentHp = 0;
                    maxHp = 0;

                    MaxKarma = 0;
                    currentKarma = 0;

                    //const CollisionElem* Celem = EachOBJcollision(i, 1, 0);




                    DirectX::XMVECTOR entPosVec = DirectX::XMLoadFloat4(reinterpret_cast<const DirectX::XMFLOAT4*>(&entity->ent_pos));

                    Vector4 EntityPos = entity->ent_pos;



                    //Vector4 result = { 0.0f, 0.0f, 0.0f, 1.0f };
                    //GetHeadPos(&result, i);

                    //XMVECTOR headPosXM = ConvertVector4ToXMVECTOR(result);



                   /* Vector3 entityWorldPos = Vector3(entity->ent_pos.x, entity->ent_pos.y, entity->ent_pos.z);

                    Vector3 worldSpaceCorners[8];
                    worldSpaceCorners[0] = entityWorldPos + Vector3(entity->RBBOXMIN.x, entity->RBBOXMIN.y, entity->RBBOXMIN.z); // Bottom-front-left
                    worldSpaceCorners[1] = entityWorldPos + Vector3(entity->RBBOXMAX.x, entity->RBBOXMIN.y, entity->RBBOXMIN.z); // Bottom-front-right
                    worldSpaceCorners[2] = entityWorldPos + Vector3(entity->RBBOXMIN.x, entity->RBBOXMAX.y, entity->RBBOXMIN.z); // Top-front-left
                    worldSpaceCorners[3] = entityWorldPos + Vector3(entity->RBBOXMAX.x, entity->RBBOXMAX.y, entity->RBBOXMIN.z); // Top-front-right
                    worldSpaceCorners[4] = entityWorldPos + Vector3(entity->RBBOXMIN.x, entity->RBBOXMIN.y, entity->RBBOXMAX.z); // Bottom-back-left
                    worldSpaceCorners[5] = entityWorldPos + Vector3(entity->RBBOXMAX.x, entity->RBBOXMIN.y, entity->RBBOXMAX.z); // Bottom-back-right
                    worldSpaceCorners[6] = entityWorldPos + Vector3(entity->RBBOXMIN.x, entity->RBBOXMAX.y, entity->RBBOXMAX.z); // Top-back-left
                    worldSpaceCorners[7] = entityWorldPos + Vector3(entity->RBBOXMAX.x, entity->RBBOXMAX.y, entity->RBBOXMAX.z); // Top-back-right*/


                    entityCount++;






                    

                    

                    

                    //float distance = 0.0f; // Declare distance variable outside the if statement
                    Vector4* CameraEye = 0;



                    if (DetectGameVersion() == "Steam") {
                        CameraEye = reinterpret_cast<Vector4*>(SteamPointer - 0x3F4920);

                    }
                    else if (DetectGameVersion() == "Epic") {
                        CameraEye = reinterpret_cast<Vector4*>(EpicPointer - 0x3F4920);
                    }

                    // Assuming Vector4 has x, y, z, and w members
                    // Read the values directly from CameraEye
                    float cameraEyeX = CameraEye->x;
                    float cameraEyeY = CameraEye->y;
                    float cameraEyeZ = CameraEye->z;
                    float cameraEyeW = CameraEye->w;
                    float playerHeight = cameraEyeY;
                    float entityHeight = entity->ent_pos.y; // Y position of the entity

                    //XMVECTOR mooseHeadPosition = XMVectorSet(LevelEditorcamPos.x, LevelEditorcamPos.y, LevelEditorcamPos.z, LevelEditorcamPos.w);

                    //DrawBillboardMooseHead(mooseHeadPosition, viewMatrix, fov, aspectRatio, nearZ, farZ, myViewport.x, myViewport.y);

                   // Vector3 b_00boxPosition = { -22597.0f, -540.0f, 3418.0f };
                   // Vector3 b_00boxSize = { 1386.0f, 571.0f, 1589.0f };

                   // Vector3 b_00boxPosition1 = { -22588.0f, 0.0f, 7527.0f };
                   // Vector3 b_00boxSize1 = { 1409.0f, 908.0f, 1501.0f };

                    //Draw3DBoxAtPosition(b_00boxPosition, b_00boxSize, RedColor, "b_00", viewMatrix, fov, aspectRatio, nearZ, farZ, myViewport.x, myViewport.y);
                    //Draw3DBoxAtPosition(b_00boxPosition1, b_00boxSize1, RedColor, "b_00", viewMatrix, fov, aspectRatio, nearZ, farZ, myViewport.x, myViewport.y);
                    //Draw3DBoxAtPosition(b_00boxPosition, b_00boxSize, RedColor, "b_00");
                    //Draw3DBoxAtPosition(b_00boxPosition1, b_00boxSize1, RedColor, "b_00");


                    //DrawTextAtWorldPos(bdrawlist, "MOOSE", myXMVECTOR, viewMatrix, fov, aspectRatio, nearZ, farZ, myViewport.x, myViewport.y, g_KHMenuFont, 56.0f);

                   // LOG("Checking area status...\n");

                    std::string gameVersion = DetectGameVersion();
                    // LOG("Detected Game Version: %s\n", gameVersion.c_str());

                    if (gameVersion == "Steam") {
                        isInArea = (*(bool*)(SteamPointer - 0x152CC8));
                        //LOG("SteamPointer Address: 0x%llx\n", SteamPointer);
                    }
                    else if (gameVersion == "Epic") {
                        isInArea = (*(bool*)(EpicPointer - 0x152CC8));
                    }

                    //LOG("isInArea after check: %d\n", isInArea);

                   // Log the current state of isInArea
                    //LOG("isInArea after check: %d\n", isInArea);

                    if (isInArea) {
                        if (!hasEnteredArea) { // Only log and act if we are entering the area
                            hasEnteredArea = true;
                            // LOG("Entered area\n");




                             //if (GetOBJEntryNameFunction(i) == "P_NM000_SANTA") {
                              //   uintptr_t entityAddress1 = reinterpret_cast<uintptr_t>(entity);

                            // }
                             //ImGui::PopFont();
                        }
                    }
                    else {
                        if (hasEnteredArea) { // Only log and act if we are exiting the area
                            hasEnteredArea = false; // Reset the "entered area" flag
                            // LOG("AREA::Exit\n");
                        }
                    }

                    // Log the current state of hasEnteredArea
                    //LOG("Current hasEnteredArea: %d\n", hasEnteredArea);


                    /*ReadUnitPointers();

                    const UNIT_DATA_ZONE* zoneData = g_unitGroup->ZoneData;

                    // Extract position and size
                    float Xpos = zoneData->Xpos;
                    float Ypos = zoneData->Ypos;
                    float Zpos = zoneData->Zpos;
                    float Xsize = zoneData->Xsize;
                    float Ysize = zoneData->Ysize;
                    float Zsize = zoneData->Zsize;

                    // Calculate min and max coordinates
                    Vector3 zoneMin = {
                        Xpos - (Xsize / 2.0f), // Min X
                        Ypos - (Ysize / 2.0f), // Min Y
                        Zpos - (Zsize / 2.0f)  // Min Z
                    };

                    Vector3 zoneMax = {
                        Xpos + (Xsize / 2.0f), // Max X
                        Ypos + (Ysize / 2.0f), // Max Y
                        Zpos + (Zsize / 2.0f)  // Max Z
                    };

                    Draw3DBoxAroundZone(RedColor, viewMatrix, zoneMin, zoneMax,fov, aspectRatio, nearZ, farZ, myViewport.x, myViewport.y);
                    Draw3DCylinderAroundZone(RedColor, viewMatrix, fov, aspectRatio, nearZ, farZ, myViewport.x, myViewport.y);*/

                    


                    /*if (LevelEditorToggle) {
                        ImGui::PushFont(g_ArialFont);

                        // Time update
                        float deltaTime = ImGui::GetIO().DeltaTime;
                        timeSinceLastUpdate += deltaTime;

                        // Periodically update selected object under cursor
                        if (timeSinceLastUpdate >= updateInterval) {
                            timeSinceLastUpdate = 0.0f;

                            cachedSelectedObject = nullptr;
                            cachedSelectedEntity = nullptr;

                            Vector3 cursorPos3 = { AxisCursorPos.x, AxisCursorPos.y, AxisCursorPos.z };

                            for (YS::OBJ* obj : g_spawnedObjects) {
                                if (!obj) continue;

                                Entity* entity = ToEntity(obj);
                                if (!entity) continue;

                                Vector3 min = entity->RBBOXMIN;
                                Vector3 max = entity->RBBOXMAX;

                                if (IsPointInsideAABB(cursorPos3, min, max)) {
                                    cachedSelectedObject = obj;
                                    cachedSelectedEntity = entity;
                                    break;
                                }
                            }
                        }

                        // Invalidate cache if view matrix, camera pos, or cursor pos changed
                        if (AxisCursorPos.x != lastCursorPos.x || AxisCursorPos.y != lastCursorPos.y || AxisCursorPos.z != lastCursorPos.z ||
                            FreecamPos.x != lastCameraPos.x || FreecamPos.y != lastCameraPos.y || FreecamPos.z != lastCameraPos.z ||
                            memcmp(&viewMatrix, &lastViewMatrix, sizeof(XMMATRIX)) != 0)
                        {
                            lastCursorPos = AxisCursorPos;
                            lastCameraPos = FreecamPos;
                            lastViewMatrix = viewMatrix;
                            cacheValid = false;
                        }

                        // Recalculate screen points if needed
                        if (!cacheValid) {
                            XMVECTOR origin = XMVectorSet(AxisCursorPos.x, AxisCursorPos.y, AxisCursorPos.z, 1.0f);
                            XMVECTOR cameraPos = XMVectorSet(FreecamPos.x, FreecamPos.y, FreecamPos.z, 1.0f);

                            float rawDistance = XMVectorGetX(XMVector3Length(XMVectorSubtract(origin, cameraPos)));
                            float clampedDistance = std::clamp(rawDistance, 1.0f, 500.0f);
                            float axisScale = clampedDistance * 0.05f;

                            XMVECTOR axisPoints[6] = {
                                XMVectorAdd(origin, XMVectorSet(axisScale, 0, 0, 0)),    // x1
                                XMVectorSubtract(origin, XMVectorSet(axisScale, 0, 0, 0)), // x2
                                XMVectorAdd(origin, XMVectorSet(0, axisScale, 0, 0)),    // y1
                                XMVectorSubtract(origin, XMVectorSet(0, axisScale, 0, 0)), // y2
                                XMVectorAdd(origin, XMVectorSet(0, 0, axisScale, 0)),    // z1
                                XMVectorSubtract(origin, XMVectorSet(0, 0, axisScale, 0))  // z2
                            };

                            for (int i = 0; i < 6; ++i) {
                                XMVECTOR screenPoint;
                                if (WorldToScreenNoClip(axisPoints[i], viewMatrix, fov, aspectRatio, nearZ, farZ, myViewport.x, myViewport.y, screenPoint)) {
                                    cachedScreenPoints[i] = screenPoint;
                                }
                                else {
                                    cachedScreenPoints[i] = XMVectorSet(-1000, -1000, 0, 0); // Off-screen marker
                                }
                            }

                            cacheValid = true;
                        }

                        // Draw selected entity outline
                        if (cachedSelectedEntity) {
                            Draw3DBoxAroundEntity(
                                cachedSelectedEntity,
                                IM_COL32(255, 255, 255, 255),
                                "Selected",
                                viewMatrix, fov, aspectRatio,
                                nearZ, farZ,
                                myViewport.x, myViewport.y
                            );
                        }

                        // Draw axis gizmo using cached screen points
                        auto DrawAxis = [&](int i1, int i2, ImU32 color, const char* label) {
                            XMVECTOR s1 = cachedScreenPoints[i1];
                            XMVECTOR s2 = cachedScreenPoints[i2];

                            if (XMVectorGetX(s1) >= 0 && XMVectorGetY(s1) >= 0) {
                                bdrawlist->AddLine(
                                    ImVec2(XMVectorGetX(s1), XMVectorGetY(s1)),
                                    ImVec2(XMVectorGetX(s2), XMVectorGetY(s2)),
                                    color, 2.0f);

                                ImVec2 textSize = ImGui::CalcTextSize(label);
                                float textX = XMVectorGetX(s1) - textSize.x * 0.5f;
                                float textY = XMVectorGetY(s1) - textSize.y - 4;

                                bdrawlist->AddText(ImVec2(textX, textY), color, label);
                            }
                            };

                        DrawAxis(0, 1, IM_COL32(255, 0, 0, 255), "X");
                        DrawAxis(2, 3, IM_COL32(0, 255, 0, 255), "Y");
                        DrawAxis(4, 5, IM_COL32(0, 128, 255, 255), "Z");



                        ImGui::PopFont();
                    }*/


                

                               

                            if (LevelEditorToggle) {
                                ImGui::PushFont(g_ArialFont);

                                // Time update
                                /*float deltaTime = ImGui::GetIO().DeltaTime;
                                timeSinceLastUpdate += deltaTime;

                                // --- PREVIEW ENTITY LOGIC ---
                                static float previewRotation = 0.0f;

                                // Blinking state variables
                                enum BlinkPhase {
                                    Visible,
                                    Invisible
                                };
                                static BlinkPhase blinkPhase = Visible;
                                static float blinkTimer = 0.0f;

                                const float visibleDuration = 5.0f;   // Visible for 1.5 seconds
                                const float invisibleDuration = 5.0f; // Invisible for 1.0 seconds

                                if (isPreviewSpawned && previewEntity && previewObject) {
                                    // 1. Update position to match cursor
                                    previewEntity->ent_pos = AxisCursorPos;

                                    // 2. Rotate around yaw (0x7A8)
                                    //previewRotation += deltaTime * XM_2PI / 6.0f;  // One full spin every 6 seconds
                                    //if (previewRotation > XM_2PI)
                                    //    previewRotation -= XM_2PI;
                                    //
                                    //*(float*)((char*)previewObject + 0x7A8) = previewRotation;

                                    // 3. Discrete blinking with longer visible/invisible pause
                                    blinkTimer += deltaTime;

                                    switch (blinkPhase) {
                                    case Visible:
                                        if (blinkTimer >= visibleDuration) {
                                            blinkTimer = 0.0f;
                                            blinkPhase = Invisible;
                                        }
                                        break;
                                    case Invisible:
                                        if (blinkTimer >= invisibleDuration) {
                                            blinkTimer = 0.0f;
                                            blinkPhase = Visible;
                                        }
                                        break;
                                    }

                                    // Write the integer alpha (0 or 128)
                                    uint8_t* alpha = (uint8_t*)((char*)previewEntity + 0x4DB);
                                    *alpha = (blinkPhase == Visible) ? 128 : 0;
                                }*/

                                // Periodically update selected object under cursor
                                if (timeSinceLastUpdate >= updateInterval) {
                                    timeSinceLastUpdate = 0.0f;

                                    cachedSelectedObject = nullptr;
                                    cachedSelectedEntity = nullptr;

                                    Vector3 cursorPos3 = { AxisCursorPos.x, AxisCursorPos.y, AxisCursorPos.z };

                                    for (YS::OBJ* obj : g_spawnedObjects) {
                                        if (!obj) continue;

                                        Entity* entity = ToEntity(obj);
                                        if (!entity) continue;

                                        Vector3 min = entity->RBBOXMIN;
                                        Vector3 max = entity->RBBOXMAX;

                                        if (IsPointInsideAABB(cursorPos3, min, max)) {
                                            cachedSelectedObject = obj;
                                            cachedSelectedEntity = entity;
                                            break;
                                        }
                                    }
                                }

                                if (AxisCursorPos.x != lastCursorPos.x ||
                                    AxisCursorPos.y != lastCursorPos.y ||
                                    AxisCursorPos.z != lastCursorPos.z ||

                                    vm_eye.x != lastCameraPos.x ||
                                    vm_eye.y != lastCameraPos.y ||
                                    vm_eye.z != lastCameraPos.z ||

                                    memcmp(&viewMatrix, &lastViewMatrix, sizeof(XMMATRIX)) != 0)
                                {
                                    lastCursorPos = AxisCursorPos;
                                    lastCameraPos = vm_eye;
                                    lastViewMatrix = viewMatrix;
                                    cacheValid = false;
                                }

                                // Recalculate screen points if needed
                                if (!cacheValid) {
                                    XMVECTOR origin = XMVectorSet(AxisCursorPos.x, AxisCursorPos.y, AxisCursorPos.z, 1.0f);
                                    XMVECTOR cameraPos = XMVectorSet(vm_eye.x, vm_eye.y, vm_eye.z, 1.0f);

                                    float rawDistance = XMVectorGetX(XMVector3Length(XMVectorSubtract(origin, cameraPos)));
                                    float clampedDistance = std::clamp(rawDistance, 1.0f, 500.0f);
                                    float axisScale = clampedDistance * 0.05f;

                                    XMVECTOR axisPoints[6] = {
                                        XMVectorAdd(origin, XMVectorSet(axisScale, 0, 0, 0)),    // x1
                                        XMVectorSubtract(origin, XMVectorSet(axisScale, 0, 0, 0)), // x2
                                        XMVectorAdd(origin, XMVectorSet(0, axisScale, 0, 0)),    // y1
                                        XMVectorSubtract(origin, XMVectorSet(0, axisScale, 0, 0)), // y2
                                        XMVectorAdd(origin, XMVectorSet(0, 0, axisScale, 0)),    // z1
                                        XMVectorSubtract(origin, XMVectorSet(0, 0, axisScale, 0))  // z2
                                    };

                                    for (int i = 0; i < 6; ++i) {
                                        XMVECTOR screenPoint;
                                        if (WorldToScreenNoClip(axisPoints[i], viewMatrix, fov, aspectRatio, nearZ, farZ, myViewport.x, myViewport.y, screenPoint)) {
                                            cachedScreenPoints[i] = screenPoint;
                                        }
                                        else {
                                            cachedScreenPoints[i] = XMVectorSet(-1000, -1000, 0, 0); // Off-screen marker
                                        }
                                    }

                                    cacheValid = true;
                                }

                                // Draw selected entity outline
                                if (cachedSelectedEntity) {
                                    Draw3DBoxAroundEntity(
                                        cachedSelectedEntity,
                                        IM_COL32(255, 255, 255, 255),
                                        "Selected",
                                        viewMatrix, fov, aspectRatio,
                                        nearZ, farZ,
                                        myViewport.x, myViewport.y);
                                    
                                }

                                // Draw axis gizmo using cached screen points
                                auto DrawAxis = [&](int i1, int i2, ImU32 color, const char* label) {
                                    XMVECTOR s1 = cachedScreenPoints[i1];
                                    XMVECTOR s2 = cachedScreenPoints[i2];

                                    if (XMVectorGetX(s1) >= 0 && XMVectorGetY(s1) >= 0) {
                                        bdrawlist->AddLine(
                                            ImVec2(XMVectorGetX(s1), XMVectorGetY(s1)),
                                            ImVec2(XMVectorGetX(s2), XMVectorGetY(s2)),
                                            color, 2.0f);

                                        ImVec2 textSize = ImGui::CalcTextSize(label);
                                        float textX = XMVectorGetX(s1) - textSize.x * 0.5f;
                                        float textY = XMVectorGetY(s1) - textSize.y - 4;

                                        bdrawlist->AddText(ImVec2(textX, textY), color, label);
                                    }
                                    };

                                DrawAxis(0, 1, IM_COL32(255, 0, 0, 255), "X");
                                DrawAxis(2, 3, IM_COL32(0, 255, 0, 255), "Y");
                                DrawAxis(4, 5, IM_COL32(0, 128, 255, 255), "Z");

                                ImGui::PopFont();
                            }
                    

                    // Calculate the height difference
                    //float heightDifference = entityHeight - playerHeight;
                    
                    


                    //XMMATRIX viewNoRot = RemoveRotationKeepPosition(viewMatrix);
                    XMVECTOR screenPos;
                    if (WorldToScreen(entPosVec, viewMatrix, fov, aspectRatio, nearZ, farZ, myViewport.x, myViewport.y, screenPos)) {
                    

                        // ImVec2 imguiScreenPos = DX12::ConvertWorldToImGuiScreenPos(screenPos, (FMATRIX44*)Matrix);
                         //screenPositions.push_back(imguiScreenPos);
                         //debugprintf("Object %d - Screen Position Address: %p\n", i, &screenPosPtr);
                         // Access x and y coordinates from the Vector4
                         //int ScreenX = static_cast<int>(screenPos.x);
                        // int ScreenY = static_cast<int>(screenPos.y);

                         //ImVec2 imguiScreenPos;
                        // imguiScreenPos.x = screenPos.x;
                         //imguiScreenPos.y = screenPos.y;

                        //if (GetPlayer1) {
                       //     RenderPlayerMovementArrow(GetPlayer1());

                       // }

                        //if (Entity* player = GetPlayer1()) {
                        //    UpdatePlayerSpeed();
                        //}


                        static bool bgWasPressed = false;

                        bool bgPressed = (flags & DEBUGDRAW_OPTION_BG) != 0;

                        if (bgPressed && !bgWasPressed)
                        {
                            ShowBuild = !ShowBuild;
                        }

                        bgWasPressed = bgPressed;



                        // Output debugging information
                        //printf("ImGui Screen Position: (%f, %f)\n", imguiScreenPos.x, imguiScreenPos.y);

                        // Use imguiScreenPos for drawing with ImGui
                        //ImGui::SetCursorScreenPos(imguiScreenPos);
                        //ImGui::Text("Hello, ImGui!");
                        //printf("Intermediate values - worldPos: (%f, %f, %f)\n", worldPos.x, worldPos.y, worldPos.z);
                        //printf("Intermediate values - screenPos: (%f, %f)\n", screenPos.x, screenPos.y);
                        uintptr_t entityAddress = reinterpret_cast<uintptr_t>(entity);

                        const char* EntryName = GetOBJEntryNameFunction(reinterpret_cast<YS::OBJ*>(entity));


                        //bdrawlist->AddText(ImVec2(ScreenX, ScreenY), Cyan, EntryName);
                       // Get the player object
                        YS::OBJ* playerObj = GetPlayerFunction();

                        // Define your scale factor
                        float scaleFactor = 0.1f; // Start with 1.0, adjust as necessary

                        // Calculate height difference
                        float heightDifference = entityHeight - playerHeight;

                        


                        float x = XMVectorGetX(screenPos);
                        float y = XMVectorGetY(screenPos);

                        // Adjust the arrow's Y endpoint using the scale factor
                        float adjustedY = y + heightDifference * scaleFactor; // y is the original screen Y position from WorldToScreen

                        ImVec2 screenSize = ImGui::GetIO().DisplaySize;
                        ImVec2 startPoint(screenSize.x / 2, screenSize.y);
                        ImVec2 endPoint(x, adjustedY);
                        
                        ImU32 color = IM_COL32(255, 0, 0, 255); // Red color
                        float thickness = 2.0f; // Line thickness


                        // Cast the player object to Entity*
                        Entity* playerEntity = reinterpret_cast<Entity*>(playerObj);


                        float horizontalDistance = sqrt(
                            pow(cameraEyeX - entity->ent_pos.x, 2) +
                            pow(cameraEyeZ - entity->ent_pos.z, 2)
                        );



                        ImU32 arrowColor = ColorBasedOnDistance(horizontalDistance);



                        char distanceStr[64];
                        snprintf(distanceStr, sizeof(distanceStr), "%.2f m", horizontalDistance);

                        //ImVec2 endPoint = ImVec2(XMVectorGetX(screenPos), XMVectorGetY(screenPos));

                        //float thickness = 2.0f;
                        // Fixed arrow length


                        // Calculate the arrow head points
                        float arrowHeadSize = 10.0f; // Size of the arrow head
                        ImVec2 direction = ImVec2(endPoint.x - startPoint.x, endPoint.y - startPoint.y);
                        float len = sqrtf(direction.x * direction.x + direction.y * direction.y);
                        
                            ImVec2 normDirection = ImVec2(direction.x / len, direction.y / len);

                           


                            ImVec2 orthoDirection = ImVec2(-normDirection.y, normDirection.x);

                            ImVec2 arrowP1 = ImVec2(endPoint.x - arrowHeadSize * normDirection.x + arrowHeadSize * orthoDirection.x,
                                endPoint.y - arrowHeadSize * normDirection.y + arrowHeadSize * orthoDirection.y);
                            ImVec2 arrowP2 = ImVec2(endPoint.x - arrowHeadSize * normDirection.x - arrowHeadSize * orthoDirection.x,
                               endPoint.y - arrowHeadSize * normDirection.y - arrowHeadSize * orthoDirection.y);

                            

                            
                            

                            if (flags & DEBUGDRAW_OPTION_OBJ) {

                                //DrawBoundingBox(entity, White, viewMatrix, fov, aspectRatio, nearZ, farZ, myViewport.x, myViewport.y);

                                //float radius = 200.0f;

                                //DrawWireframeSphere(entity, White, viewMatrix, fov, aspectRatio, nearZ, farZ, myViewport.x, myViewport.y);
                                
                                

                                

                               // bdrawlist->AddLine(startPoint, endPoint, arrowColor, thickness);
                                DrawGlowLine(bdrawlist, startPoint, endPoint, IM_COL32(0, 255, 255, 255));

                                // Draw the arrow head
                                //bdrawlist->AddTriangle(endPoint, arrowP1, arrowP2, arrowColor, thickness);

                                DrawGlowTriangle(bdrawlist, endPoint, arrowP1, arrowP2, arrowColor, thickness);

                                //ImVec2 displaySize = ImGui::GetIO().DisplaySize;
                                //float width = 1400.0f;      // total width of bars
                               // float maxHeight = 100.0f;  // max bar height

                                // bottom center X = half screen width minus half the width of bars
                               // float startX = (displaySize.x * 0.5f) - (width * 0.5f);
                                // Y = bottom of screen minus some padding (e.g. 20 pixels)
                               // float startY = displaySize.y - 20.0f;

                              //  ImVec2 startPos(startX, startY);

                              //  ImDrawList* drawList = ImGui::GetBackgroundDrawList();

                               // float amplitude = GetCurrentAmplitude();

                                //DrawHorizontalAmplitudeBars(drawList, startPoint, width, maxHeight, ImGui::GetTime(), amplitude);

                                

                                

                                //FX(draw_list, a, b, scale, base_color, t);


                            }

                            if (flags & DEBUGDRAW_OPTION_ATTACK) {

                                
                                uintptr_t entityStartAddress1 = (uintptr_t)entity; // Get the starting address of the entity
                                uintptr_t entityStatsAddress1 = entityStartAddress1 + 0x5C0; // Offset to the stats section

                                if (entityStatsAddress1 != 0) {
                                    uintptr_t* statsPtrAddress1 = reinterpret_cast<uintptr_t*>(entityStatsAddress1); // Interpret it as a pointer
                                    uintptr_t statsAddress1 = *statsPtrAddress1; // Dereference to get the actual stats address

                                    // Check if stats address is valid
                                    if (statsAddress1 != 0) {
                                        // Read current HP from the stats address
                                        int currentHp = *reinterpret_cast<int*>(statsAddress1);
                                        // Read max HP from the next memory address
                                        int maxHp = *reinterpret_cast<int*>(statsAddress1 + sizeof(int));

                                        uintptr_t enemyParamPtr = (uintptr_t)statsAddress1 + 0x270; // Base pointer + offset
                                        uint32_t EnemyParam32 = *reinterpret_cast<uint32_t*>(enemyParamPtr);
                                        //uint32_t EnemyParam32 = enemyParamPtr;
                                        //LOG(L"EnemyParam32: 0x%llx\n", EnemyParam32);
                                        

                                        uintptr_t ResultAct = 0;

                                        if (EnemyParam32 != 0) {
                                            

                                            //uintptr_t moduleBase = reinterpret_cast<uintptr_t>(baseModule);

                                            // Decode the pointer using CustomDecodePointer
                                            ResultAct = CustomDecodePointer(EnemyParam32,moduleBase);

                                            // Ensure ResultAct is valid before accessing further data
                                            if (ResultAct != 0) {
                                                // Safely dereference the 1-byte level at offset 0x02
                                                uint8_t* EnemyLevel = reinterpret_cast<uint8_t*>(ResultAct + 0x02);
                                                if (EnemyLevel) {
                                                   // printf("Enemy Level: %d\n", *EnemyLevel); // Print enemy level (0-255)
                                                    
                                                }
                                                else {
                                                   // printf("Invalid Enemy Level pointer.\n");
                                                }

                                                // Safely dereference the 4-byte EXP value at offset 0x56
                                                int* EnemyEXP = reinterpret_cast<int*>(ResultAct + 0x56);
                                                if (EnemyEXP) {

                                                    
                                                   // printf("Enemy EXP: %d\n", *EnemyEXP); // Print enemy EXP
                                                    //InitializeEntityHealth(entity);
                                                    DrawBillboard(entity, RedColor, currentHp, maxHp, EnemyLevel, EntryName, EnemyEXP, viewMatrix, fov, aspectRatio, nearZ, farZ, myViewport.x, myViewport.y);
                                                    
                                                    //DrawBillboardUsingMatrix(entity, RedColor, currentHp, maxHp, EnemyLevel, EntryName, EnemyEXP, viewMatrix, fov, aspectRatio, nearZ, farZ, myViewport.x, myViewport.y);
                                                    //UpdateAndCalculateDamage(entity, viewMatrix, fov, aspectRatio, nearZ, farZ, myViewport.x, myViewport.y);
                                                    

                                                }
                                                else {
                                                  //  printf("Invalid Enemy EXP pointer.\n");
                                                }
                                            }
                                            else {
                                               // printf("Invalid ResultAct pointer.\n");
                                            }
                                        }
                                        else {
                                            //printf("EnemyParam64 is invalid (null).\n");
                                        }
                                            //const char* EntryName = GetOBJEntryNameFunction(reinterpret_cast<YS::OBJ*>(entity));
                                       // DAMAGE damageStruct = GetEntityDamageStruct();
                                        
                                       // float currentTime = GetNowCurrentTime();

                                        //UpdateAndDrawDamage(damageStruct, currentTime, viewMatrix, fov, aspectRatio,
                                       //     nearZ, farZ, myViewport.x, myViewport.y, bdrawlist);

                                       // RenderDamageTexts(currentTime, viewMatrix, fov, aspectRatio,
                                       //     nearZ, farZ, myViewport.x, myViewport.y, bdrawlist);

                                            //DrawBillboardForNonEnemy(entity, RedColor, currentHp, maxHp, EntryName, viewMatrix, fov, aspectRatio, nearZ, farZ, myViewport.x, myViewport.y);

                                            //DrawBillboard(entity, RedColor, currentHp, maxHp, EnemyLevel, EntryName, EnemyEXP, viewMatrix, fov, aspectRatio, nearZ, farZ, myViewport.x, myViewport.y);

                                            

                                        }
                                    }
                                //DrawBillboardForNpcs(entity, RedColor, EntryName, viewMatrix, fov, aspectRatio, nearZ, farZ, myViewport.x, myViewport.y);
                                //uintptr_t viewMatrixAddress = reinterpret_cast<uintptr_t>(&viewMatrix); // Get the address
                                //float deltaTime = GetDeltaTime();
                                
                                //UpdateFreeCamera(deltaTime, viewMatrix, viewMatrixAddress); // Dereference viewMatrix to get its contents

                                    
                                    //DebugOBJCameraValues(camera);
                                
                                }
                            
                                
                            

                        



                            if (flags & DEBUGDRAW_OPTION_RC) {

                                short uniqueID = GetUniqueIDFromEntity(entity);

                                //ReadMDLXbonesAmount();
                                ReadMDLXbonesAmount(entity, uniqueID);

                                float yOffset = 50.0f; // Adjust this value as needed.

                                // Check if the entity's bone IDs are available in the map
                                if (entityBoneIDs.find(uniqueID) != entityBoneIDs.end()) {
                                    const std::vector<int>& boneIDs = entityBoneIDs[uniqueID];

                                    for (int boneID : boneIDs) {
                                        Matrix4x4 boneMatrix;
                                        if (GetOBJBoneMatrixFunction((YS::OBJ*)entity, boneID, &boneMatrix)) {
                                            // Convert the bone position to XMVECTOR
                                            XMVECTOR bonePosition = XMVectorSet(boneMatrix.v3.x, boneMatrix.v3.y, boneMatrix.v3.z, 1.0f);

                                            // Prepare the variables for the screen position
                                            XMVECTOR bonePositionScreen;

                                            // Convert world position to screen position
                                            if (WorldToScreen(bonePosition, viewMatrix, fov, aspectRatio, nearZ, farZ, myViewport.x, myViewport.y, bonePositionScreen)) {

                                                // Convert XMVECTOR to ImVec2 for Dear ImGui
                                                ImVec2 bonePos2D = ImVec2(XMVectorGetX(bonePositionScreen), XMVectorGetY(bonePositionScreen) + yOffset);

                                                // Draw the circle for the bone
                                                bdrawlist->AddCircleFilled(bonePos2D, 3.0f, IM_COL32(255, 255, 255, 255)); // White circle

                                                // Draw text for the bone ID
                                               // std::string boneIDText = std::to_string(boneID);
                                              //  bdrawlist->AddText(bonePos2D, IM_COL32(0, 0, 255, 255), boneIDText.c_str());

                                               // Draw the line to the parent bone (if not the root bone)
                                                Matrix4x4 parentBoneMatrix;
                                                if (GetOBJBoneMatrixFunction((YS::OBJ*)entity, boneID - 1, &parentBoneMatrix)) {
                                                    XMVECTOR parentBonePosition = XMVectorSet(parentBoneMatrix.v3.x, parentBoneMatrix.v3.y, parentBoneMatrix.v3.z, 1.0f);
                                                    XMVECTOR parentPositionScreen;

                                                    if (WorldToScreen(parentBonePosition, viewMatrix, fov, aspectRatio, nearZ, farZ, myViewport.x, myViewport.y, parentPositionScreen)) {

                                                        ImVec2 parentPos2D = ImVec2(XMVectorGetX(parentPositionScreen), XMVectorGetY(parentPositionScreen) + yOffset);

                                                        // Draw the line between the bone and its parent
                                                        bdrawlist->AddLine(bonePos2D, parentPos2D, IM_COL32(255, 255, 255, 255)); // White line
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                                else {
                                    // Handle the case where bone IDs for this entity are not available
                                    // This might include logging an error or a similar action
                                }
                            }

                            /*if (LevelEditorToggle) {
                                obj_camera* objcamera = GetOBJCameraInstanceFunction();
                                objcamera = reinterpret_cast<obj_camera*>(reinterpret_cast<uintptr_t>(objcamera) + 0x08);

                                // Get freecam yaw and pitch
                                float yaw = objcamera->m_Yaw;
                                float pitch = objcamera->m_Pitch;

                                // Freecam world position
                                XMVECTOR cameraPos = XMVectorSet(FreecamPos.x, FreecamPos.y, FreecamPos.z, 1.0f);

                                //  Correct forward direction (Z-forward in right-handed system)
                                XMVECTOR forwardDir = XMVectorSet(
                                    cosf(pitch) * sinf(yaw),
                                    sinf(pitch),
                                    cosf(pitch) * cosf(yaw),
                                    0.0f
                                );
                                forwardDir = XMVector3Normalize(forwardDir);

                                // Position the cursor a bit in front of the camera
                                float cursorOffset = 10.0f;  // Reduced for visibility
                                XMVECTOR cursorPos = XMVectorAdd(cameraPos, XMVectorScale(forwardDir, cursorOffset));

                                // Project to screen space
                                XMVECTOR screenPos;
                                if (WorldToScreen(cursorPos, viewMatrix, fov, aspectRatio, nearZ, farZ, myViewport.x, myViewport.y, screenPos)) {
                                    ImVec2 cursor2D = ImVec2(XMVectorGetX(screenPos), XMVectorGetY(screenPos));

                                    ImDrawList* drawList = ImGui::GetBackgroundDrawList();

                                    float crossSize = 6.0f;
                                    ImU32 color = IM_COL32(255, 255, 0, 255);  // Yellow

                                    // Draw cross
                                    drawList->AddLine(ImVec2(cursor2D.x - crossSize, cursor2D.y), ImVec2(cursor2D.x + crossSize, cursor2D.y), color, 2.0f);
                                    drawList->AddLine(ImVec2(cursor2D.x, cursor2D.y - crossSize), ImVec2(cursor2D.x, cursor2D.y + crossSize), color, 2.0f);

                                    // Optional label
                                    drawList->AddText(ImVec2(cursor2D.x + 8, cursor2D.y + 8), IM_COL32(255, 255, 255, 255), "Spawn");
                                }
                            }*/


                            

                            











                            if (flags & DEBUGDRAW_OPTION_BBOX) {
                                Matrix4x4 entityMatrix = entity->Matrix;

                                XMVECTOR position = XMVectorSet(entityMatrix.v3.x, entityMatrix.v3.y, entityMatrix.v3.z, 1.0f);

                                float axisLength = 50.0f;

                                // Get local axes from matrix (v0 = X, v1 = Y, v2 = Z)
                                XMVECTOR rightDir = XMVectorSet(entityMatrix.v0.x, entityMatrix.v0.y, entityMatrix.v0.z, 0.0f);

                                // **Flip the Y component of the upDir vector**
                                XMVECTOR upDir = XMVectorSet(entityMatrix.v1.x, -entityMatrix.v1.y, entityMatrix.v1.z, 0.0f);

                                XMVECTOR forwardDir = XMVectorSet(entityMatrix.v2.x, entityMatrix.v2.y, entityMatrix.v2.z, 0.0f);

                                // Axis end positions in world space
                                XMVECTOR xAxisEnd = XMVectorAdd(position, XMVectorScale(rightDir, axisLength));
                                XMVECTOR yAxisEnd = XMVectorAdd(position, XMVectorScale(upDir, axisLength));
                                XMVECTOR zAxisEnd = XMVectorAdd(position, XMVectorScale(forwardDir, axisLength));

                                // Project to screen space
                                XMVECTOR position2D, xAxisEnd2D, yAxisEnd2D, zAxisEnd2D;

                                if (WorldToScreen(position, viewMatrix, fov, aspectRatio, nearZ, farZ, myViewport.x, myViewport.y, position2D) &&
                                    WorldToScreen(xAxisEnd, viewMatrix, fov, aspectRatio, nearZ, farZ, myViewport.x, myViewport.y, xAxisEnd2D) &&
                                    WorldToScreen(yAxisEnd, viewMatrix, fov, aspectRatio, nearZ, farZ, myViewport.x, myViewport.y, yAxisEnd2D) &&
                                    WorldToScreen(zAxisEnd, viewMatrix, fov, aspectRatio, nearZ, farZ, myViewport.x, myViewport.y, zAxisEnd2D))
                                {
                                    ImVec2 pos2D = ImVec2(XMVectorGetX(position2D), XMVectorGetY(position2D));
                                    ImVec2 xAxis2D = ImVec2(XMVectorGetX(xAxisEnd2D), XMVectorGetY(xAxisEnd2D));
                                    ImVec2 yAxis2D = ImVec2(XMVectorGetX(yAxisEnd2D), XMVectorGetY(yAxisEnd2D));
                                    ImVec2 zAxis2D = ImVec2(XMVectorGetX(zAxisEnd2D), XMVectorGetY(zAxisEnd2D));

                                    // Draw axis lines
                                    bdrawlist->AddLine(pos2D, xAxis2D, IM_COL32(255, 0, 0, 255));   // X = red
                                    bdrawlist->AddLine(pos2D, yAxis2D, IM_COL32(0, 255, 0, 255));   // Y = green (now flipped)
                                    bdrawlist->AddLine(pos2D, zAxis2D, IM_COL32(0, 0, 255, 255));   // Z = blue

                                    // Optional: offset labels slightly in the direction of each axis
                                    auto OffsetLabel = [](ImVec2 from, ImVec2 to, float offsetPixels) -> ImVec2 {
                                        ImVec2 dir = ImVec2(to.x - from.x, to.y - from.y);
                                        float len = sqrtf(dir.x * dir.x + dir.y * dir.y);
                                        if (len > 0.0f) {
                                            dir.x = (dir.x / len) * offsetPixels;
                                            dir.y = (dir.y / len) * offsetPixels;
                                            return ImVec2(to.x + dir.x, to.y + dir.y);
                                        }
                                        return to;
                                        };

                                    float labelOffset = 8.0f;

                                    ImVec2 labelX = OffsetLabel(pos2D, xAxis2D, labelOffset);
                                    ImVec2 labelY = OffsetLabel(pos2D, yAxis2D, labelOffset);
                                    ImVec2 labelZ = OffsetLabel(pos2D, zAxis2D, labelOffset);

                                    bdrawlist->AddText(labelX, IM_COL32(255, 0, 0, 255), "X");
                                    bdrawlist->AddText(labelY, IM_COL32(0, 255, 0, 255), "Y");
                                    bdrawlist->AddText(labelZ, IM_COL32(0, 0, 255, 255), "Z");

                                    Draw3DBoxAroundEntity(entity, RedColor, GetOBJEntryNameFunction((YS::OBJ*)entity), viewMatrix, fov, aspectRatio, nearZ, farZ, myViewport.x, myViewport.y);
                                }
                            }



                        gGlobalObjPtr = GetMenuOBJFunction();
                        if (gGlobalObjPtr != nullptr) {
                            Entity* entity = reinterpret_cast<Entity*>(gGlobalObjPtr);
                            Draw3DBoxAroundEntity(entity, RedColor, GetOBJEntryNameFunction((YS::OBJ*)entity), viewMatrix, fov, aspectRatio, nearZ, farZ, myViewport.x, myViewport.y);
                            //DrawPngBillboardForEntity(entity, g_pngTex,
                           //     viewMatrix,
                           //     fov, aspectRatio, nearZ, farZ, myViewport.x, myViewport.y);
                        }




                        if (flags & DEBUGDRAW_OPTION_STATUS) {

                            //Collision collision;
                           // collision.Shape = 0; // Initialize Shape
                            //collision.Radius = 1.0f; // Initialize Radius
                            //collision.Height = 2.0f; // Initialize Height
                            //collision.Elem = nullptr; // Initialize Elem (assuming nullptr for now)
                            //collision.Pos = Vector4(0.0f, 0.0f, 0.0f, 1.0f); // Initialize Pos with appropriate values

                            //SetCollision(&collision, i, Celem);

                            //Celem = EachOBJcollision(i, 1, Celem);


                            /*if (Celem) {
                                switch (Celem->Shape) {
                                case 0:
                                    DrawEllipsoid(x, y, Celem->Radius, Celem->Radius, Celem->Height);
                                    break;
                                case 1:
                                    DrawCylinder(x, y, Celem->Radius, Celem->Height, 32);
                                    break;
                                case 2:
                                    DrawCube(x, y, Celem->Height);
                                    break;
                                case 3:
                                    DrawSphere(x, y, Celem->Hofs, Celem->Radius);
                                    break;
                                default:
                                    // Handle unsupported shapes or do nothing
                                    break;
                                }
                            }*/


                            //float boxWidth = Celem->Radius * 2.0f;
                            //float boxHeight = Celem->Height;

                            //float entityBoxX = x - (boxWidth / 2); // Adjust as needed
                            //float entityBoxY = y - (boxHeight / 2); // Adjust as needed
                            //float entityBoxWidth = 200.0f; // Adjust as needed
                            //float entityBoxHeight = 400.0f; // Adjust as needed


                       // uintptr_t* EntityStats = reinterpret_cast<uintptr_t*>(entity + 0x5C0);
                            uintptr_t entityStartAddress = (uintptr_t)entity;
                            uintptr_t entityStatsAddress = entityStartAddress + 0x5C0; // Calculate the address of EntityStats.
                            unsigned char entityMotionID = *(char*)(entityStartAddress + 0x180);
                            float entityMotionMaxDuration = *(float*)(entityStartAddress + 0x198);
                            float entityMotionCurrentDuration = *(float*)(entityStartAddress + 0x19C);

                            //printf("Entity Name: %s\n", EntryName);
                          //  printf("Entity Start Address: 0x%llX\n", static_cast<unsigned long long>(entityStartAddress)); // Format as hexadecimal.
                           // printf("EntityStats Address: 0x%llX\n", static_cast<unsigned long long>(entityStatsAddress)); // Format as hexadecimal.

                            uintptr_t ActObjectptr = (uintptr_t)entityStartAddress + 0x4A8;
                            uintptr_t ActObjectCurrentTableptr = (uintptr_t)entityStartAddress + 0x4A0;

                            uintptr_t ActValue64 = *reinterpret_cast<uintptr_t*>(ActObjectptr);
                            uintptr_t ActObjectCurrentTableValue64 = *reinterpret_cast<uintptr_t*>(ActObjectCurrentTableptr);

                            uintptr_t ResultAct = 0;

                            uintptr_t ActObjectCurrentTableResultAct = 0;


                            currentKarma = (int)*(float*)(entityStartAddress + 0xD48);
                            MaxKarma = (int)*(float*)(entityStartAddress + 0xD4C);

                            char KarmaStr[68];
                            //StatusPrintf(" karma %d/%d\n", currentKarma, maxKarma);
                            

                            if (entityStatsAddress != 0) {

                                char ActStr[255];

                                uintptr_t* statsPtrAddress = reinterpret_cast<uintptr_t*>(entityStatsAddress);
                                uintptr_t statsAddress = *statsPtrAddress; // Dereference to get the actual stats address



                                // Now statsAddress points to the actual stats
                                if (statsAddress != 0) {
                                    currentHp = *reinterpret_cast<int*>(statsAddress);
                                    maxHp = *reinterpret_cast<int*>(statsAddress + sizeof(int));



                                    // Printing values and addresses
                                    //printf("Current HP: %d, Address: %p\n", currentHp, reinterpret_cast<void*>(statsAddress));
                                   // printf("Max HP: %d, Address: %p\n", maxHp, reinterpret_cast<void*>(statsAddress + sizeof(int)));
                                }
                                else {
                                    // printf("Stats address is null.\n");
                                }




                                if (ActValue64 != 0) {
                                    // ActValue64 contains the 64-bit pointer, now extract the 32-bit pointer



                                    uint32_t ActValue32 = *reinterpret_cast<uint32_t*>(ActValue64);

                                    //uintptr_t moduleBase = reinterpret_cast<uintptr_t>(baseModule);

                                    // Decode the pointer using CustomDecodePointer
                                    //ResultAct = DecodePointerFunction(ActValue32);
                                    ResultAct = CustomDecodePointer(ActValue32, moduleBase);

                                    //uintptr_t ResultAct = *reinterpret_cast<uintptr_t*>(&ActValue32);

                                    //char ActValueStr[200];
                                    //sprintf_s(ActValueStr, sizeof(ActValueStr), "Act Value Str: 0x%08X\n", ResultAct);

                                    char* actString = reinterpret_cast<char*>(ResultAct);
                                    // Now, ActValue32 contains the corrected 32-bit pointer
                                    if (actString != nullptr) {

                                        snprintf(ActStr, sizeof(ActStr), "Current Act: '%s'", actString);
                                    }
                                }
                                else {

                                    snprintf(ActStr, sizeof(ActStr), "No Current Act Available!");
                                }

                                ImGui::PushFont(g_ArialFont);
                                // Convert distance to string
                                //char distanceStr[64];
                                //snprintf(distanceStr, sizeof(distanceStr), "%.2f m", horizontalDistance);


                                char addressStr[64];
                                snprintf(addressStr, sizeof(addressStr), "0x%llX", static_cast<unsigned long long>(entityAddress));

                                // Create HP string
                                char HPstr[32];
                                snprintf(HPstr, sizeof(HPstr), "hp:%d/%d", currentHp, maxHp);

                                snprintf(KarmaStr, sizeof(KarmaStr), "karma %d/%d", currentKarma, MaxKarma);

                                char MotionIDStr[100];

                                snprintf(MotionIDStr, sizeof(MotionIDStr), "Motion ID: %d", entityMotionID);

                                char MotionMaxFramesStr[100];

                                snprintf(MotionMaxFramesStr, sizeof(MotionMaxFramesStr), "Motion Max Frames Amount: %1.0f", entityMotionMaxDuration);

                                char MotionCurrentFramesStr[100];

                                snprintf(MotionCurrentFramesStr, sizeof(MotionCurrentFramesStr), "Motion Current Frames Amount: %1.0f", entityMotionCurrentDuration);

                                // Define a fixed offset for text relative to the arrowhead
                                const ImVec2 textOffset(10.0f, -20.0f);  // X and Y offset from the arrowhead

                                // Calculate the base position for the text
                                ImVec2 textBasePosition = ImVec2(endPoint.x + textOffset.x, endPoint.y + textOffset.y);

                                // Now, draw the text at this position
                                //bdrawlist->AddText(textBasePosition, arrowColor, distanceStr);

                                // If you have multiple lines of text, increment the Y position for each line
                                float lineSpacing = 30.0f;
                                ImVec2 secondTextPosition = ImVec2(textBasePosition.x, textBasePosition.y + lineSpacing);

                                //bdrawlist->AddText(secondTextPosition, arrowColor, distanceStr);

                                ImVec2 thirdTextPosition = ImVec2(textBasePosition.x, secondTextPosition.y + lineSpacing);

                                //bdrawlist->AddText(thirdTextPosition, Green, EntryName);



                                ImVec2 fourthTextPosition = ImVec2(textBasePosition.x, thirdTextPosition.y + lineSpacing);



                                //bdrawlist->AddText(fourthTextPosition, Cyan, HPstr);

                                ImU32 backgroundColor = IM_COL32(255, 255, 255, 192); // White background with some transparency

                                ImU32 backgroundColor2 = IM_COL32(0, 0, 0, 192); // White background with some transparency

                                ImVec2 Acttext_size = ImGui::CalcTextSize(ActStr);

                                ImVec2 ActbackgroundSize(Acttext_size.x + 8.0f, Acttext_size.y + 4.0f); // Adjust these values as needed

                                ImVec2 HPtext_size = ImGui::CalcTextSize(HPstr);

                                ImVec2 HPbackgroundSize(HPtext_size.x + 8.0f, HPtext_size.y + 4.0f); // Adjust these values as needed

                                ImVec2 EntryNametext_size = ImGui::CalcTextSize(EntryName);

                                ImVec2 EntryNamebackgroundSize(EntryNametext_size.x + 8.0f, EntryNametext_size.y + 4.0f); // Adjust these values as needed

                                ImVec2 Addresstext_size = ImGui::CalcTextSize(addressStr);

                                ImVec2 AddressbackgroundSize(Addresstext_size.x + 8.0f, Addresstext_size.y + 4.0f); // Adjust these values as needed

                                ImVec2 Karmatext_size = ImGui::CalcTextSize(KarmaStr);

                                ImVec2 KarmabackgroundSize(Karmatext_size.x + 8.0f, Karmatext_size.y + 4.0f); // Adjust these values as needed

                                ImVec2 MotionIDtext_size = ImGui::CalcTextSize(MotionIDStr);

                                ImVec2 MotionIDbackgroundSize(MotionIDtext_size.x + 8.0f, MotionIDtext_size.y + 4.0f); // Adjust these values as needed

                                ImVec2 MotionMaxFramestext_size = ImGui::CalcTextSize(MotionMaxFramesStr);

                                ImVec2 MotionMaxFramesbackgroundSize(MotionMaxFramestext_size.x + 8.0f, MotionMaxFramestext_size.y + 4.0f); // Adjust these values as needed

                                ImVec2 MotionCurrentFramestext_size = ImGui::CalcTextSize(MotionCurrentFramesStr);

                                ImVec2 MotionCurrentFramesbackgroundSize(MotionCurrentFramestext_size.x + 8.0f, MotionCurrentFramestext_size.y + 4.0f); // Adjust these values as needed

                                ImVec2 fifthTextPosition = ImVec2(textBasePosition.x, fourthTextPosition.y + lineSpacing);
                                ImVec2 sixthTextPosition = ImVec2(textBasePosition.x, fifthTextPosition.y + lineSpacing);
                                ImVec2 seventhTextPosition = ImVec2(textBasePosition.x, sixthTextPosition.y + lineSpacing);
                                ImVec2 eighthTextPosition = ImVec2(textBasePosition.x, seventhTextPosition.y + lineSpacing);
                                ImVec2 ninethTextPosition = ImVec2(textBasePosition.x, eighthTextPosition.y + lineSpacing);

                                ImVec2 ActbackgroundPos(sixthTextPosition.x - 4.0f, sixthTextPosition.y - 2.0f); // Adjust these values as needed

                                ImVec2 HPbackgroundPos(fourthTextPosition.x - 4.0f, fourthTextPosition.y - 2.0f); // Adjust these values as needed

                                ImVec2 EntryNamebackgroundPos(secondTextPosition.x - 4.0f, secondTextPosition.y - 2.0f); // Adjust these values as needed

                                ImVec2 AddressbackgroundPos(thirdTextPosition.x - 4.0f, thirdTextPosition.y - 2.0f); // Adjust these values as needed

                                ImVec2 KarmabackgroundPos(fifthTextPosition.x - 4.0f, fifthTextPosition.y - 2.0f); // Adjust these values as needed

                                ImVec2 MotionIDbackgroundPos(seventhTextPosition.x - 4.0f, seventhTextPosition.y - 2.0f); // Adjust these values as needed

                                ImVec2 MotionMaxFramesbackgroundPos(eighthTextPosition.x - 4.0f, eighthTextPosition.y - 2.0f); // Adjust these values as needed

                                ImVec2 MotionCurrentFramesbackgroundPos(ninethTextPosition.x - 4.0f, ninethTextPosition.y - 2.0f); // Adjust these values as needed

                                bdrawlist->AddRectFilled(ActbackgroundPos, ImVec2(ActbackgroundPos.x + ActbackgroundSize.x, ActbackgroundPos.y + ActbackgroundSize.y), backgroundColor);

                                bdrawlist->AddRectFilled(HPbackgroundPos, ImVec2(HPbackgroundPos.x + HPbackgroundSize.x, HPbackgroundPos.y + HPbackgroundSize.y), backgroundColor);

                                bdrawlist->AddRectFilled(EntryNamebackgroundPos, ImVec2(EntryNamebackgroundPos.x + EntryNamebackgroundSize.x, EntryNamebackgroundPos.y + EntryNamebackgroundSize.y), backgroundColor);

                                bdrawlist->AddRectFilled(AddressbackgroundPos, ImVec2(AddressbackgroundPos.x + AddressbackgroundSize.x, AddressbackgroundPos.y + AddressbackgroundSize.y), backgroundColor);

                                bdrawlist->AddRectFilled(KarmabackgroundPos, ImVec2(KarmabackgroundPos.x + KarmabackgroundSize.x, KarmabackgroundPos.y + KarmabackgroundSize.y), backgroundColor);

                                bdrawlist->AddRectFilled(MotionIDbackgroundPos, ImVec2(MotionIDbackgroundPos.x + MotionIDbackgroundSize.x, MotionIDbackgroundPos.y + MotionIDbackgroundSize.y), backgroundColor);
                                 
                                bdrawlist->AddRectFilled(MotionMaxFramesbackgroundPos, ImVec2(MotionMaxFramesbackgroundPos.x + MotionMaxFramesbackgroundSize.x, MotionMaxFramesbackgroundPos.y + MotionMaxFramesbackgroundSize.y), backgroundColor);

                                bdrawlist->AddRectFilled(MotionCurrentFramesbackgroundPos, ImVec2(MotionCurrentFramesbackgroundPos.x + MotionCurrentFramesbackgroundSize.x, MotionCurrentFramesbackgroundPos.y + MotionCurrentFramesbackgroundSize.y), backgroundColor);
                                //bdrawlist->AddText(fifthTextPosition, Yellow, addressStr);
                               // bdrawlist->AddText(sixthTextPosition, Cyan, KarmaStr);
                                //bdrawlist->AddText(seventhTextPosition, RedColor, ActStr);

                                //DrawEntityBox(entityBoxX, entityBoxY, entityBoxWidth, entityBoxHeight, IM_COL32(255, 0, 0, 255));
                                //ImU32 boxColor = IM_COL32(255, 0, 0, 255); // Red color
                                //bdrawlist->AddRect(ImVec2(entityBoxX, entityBoxY), ImVec2(entityBoxX + boxWidth, entityBoxY + boxHeight), boxColor);

                                //ImVec2 entityNamePos(secondTextPosition);
                                //ImVec2 addressPos(thirdTextPosition); // Below the entity name
                                //ImVec2 objIdPos(boxX, addressPos.y + text_size.y + 10.0f); // Below the address
                                //ImVec2 hpPos(fourthTextPosition);

                                // Draw the background rectangles for specific strings
                                //bdrawlist->AddRectFilled(entityNamePos, ImVec2(entityNamePos.x + text_size.x * 1.5f + 8.0f, entityNamePos.y + text_size.y + 8.0f), backgroundColor2); // Background for entity name
                                //bdrawlist->AddRectFilled(hpPos, ImVec2(hpPos.x + text_size.x + 8.0f, hpPos.y + text_size.y + 8.0f), backgroundColor); // Background for HP

                                // Draw the text elements on top of the background rectangles
                                bdrawlist->AddText(secondTextPosition, Black, EntryName);
                                bdrawlist->AddText(thirdTextPosition, Black, addressStr);
                                // bdrawlist->AddText(objIdPos, Cyan, OBJ_IDStr);
                                bdrawlist->AddText(fourthTextPosition, Black, HPstr);
                                bdrawlist->AddText(fifthTextPosition, Black, KarmaStr);
                                bdrawlist->AddText(sixthTextPosition, Black, ActStr);
                                bdrawlist->AddText(seventhTextPosition, Black, MotionIDStr);
                                bdrawlist->AddText(eighthTextPosition, Black, MotionMaxFramesStr);
                                bdrawlist->AddText(ninethTextPosition, Black, MotionCurrentFramesStr);

                                ImGui::PopFont();

                            }




                        }
                    }

                }

            }

            ImGui::Render();

            UINT backBufferIdx = pSwapChain->GetCurrentBackBufferIndex();
            ID3D12CommandAllocator* commandAllocator = g_commandAllocators[backBufferIdx];
            commandAllocator->Reset();

            D3D12_RESOURCE_BARRIER barrier = {};
            barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
            barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
            barrier.Transition.pResource = g_mainRenderTargetResource[backBufferIdx];
            barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
            barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
            barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
            g_pd3dCommandList->Reset(commandAllocator, NULL);
            g_pd3dCommandList->ResourceBarrier(1, &barrier);

            g_pd3dCommandList->OMSetRenderTargets(1, &g_mainRenderTargetDescriptor[backBufferIdx], FALSE, NULL);
            g_pd3dCommandList->SetDescriptorHeaps(1, &g_pd3dSrvDescHeap);
            ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), g_pd3dCommandList);
            barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
            barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
            g_pd3dCommandList->ResourceBarrier(1, &barrier);
            g_pd3dCommandList->Close();

            g_pd3dCommandQueue->ExecuteCommandLists(1, reinterpret_cast<ID3D12CommandList* const*>(&g_pd3dCommandList));
        }
        }
        }


