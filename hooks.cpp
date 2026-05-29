#include <cstdio>
#include <thread>

#include "hooks.hpp"

#include "hook_directx12.hpp"

#include "utils.hpp"

#include "MinHook.h"

#include "imgui.h"
#include "helper.hpp"


static HWND g_hWindow = NULL;
static HHOOK g_hMouseHook = NULL;
static HHOOK g_hKeyboardHook = NULL;


static DWORD WINAPI ReinitializeGraphicalHooks(LPVOID lpParam) {
	LOG("[!] Hooks will reinitialize!\n");

	HWND hNewWindow = U::GetProcessWindow();
	while (hNewWindow == reinterpret_cast<HWND>(lpParam)) {
		hNewWindow = U::GetProcessWindow();
	}

	H::bShuttingDown = true;

	H::Free();
	H::Init();

	H::bShuttingDown = false;
	H::bShowDemoWindow = true;

	return 0;
}



typedef BOOL(WINAPI* hk_SetCursorPos)(int, int);
hk_SetCursorPos origSetCursorPos = NULL;

BOOL WINAPI HOOK_SetCursorPos(int X, int Y)
{
	if (H::bShowDemoWindow)
		return FALSE;

	return origSetCursorPos(X, Y);
}

void InitCursorHook()
{
	if (MH_CreateHook(&SetCursorPos, &HOOK_SetCursorPos, reinterpret_cast<LPVOID*>(&origSetCursorPos)) != MH_OK)
	{
		printf("Couldn't create hook for SetCursorPos.");
		return;
	}

	if (MH_EnableHook(&SetCursorPos) != MH_OK)
	{
		printf("Couldn't enable SetCursorPos hook.");
	}
}


LRESULT CALLBACK LowLevelMouseProc(int nCode, WPARAM wParam, LPARAM lParam)
{
	if (nCode >= 0 && ImGui::GetCurrentContext())
	{
		MSLLHOOKSTRUCT* pMouseStruct = (MSLLHOOKSTRUCT*)lParam;
		ImGuiIO& io = ImGui::GetIO();

		switch (wParam)
		{
		case WM_LBUTTONDOWN: io.MouseDown[0] = true; break;
		case WM_LBUTTONUP:   io.MouseDown[0] = false; break;
		case WM_RBUTTONDOWN: io.MouseDown[1] = true; break;
		case WM_RBUTTONUP:   io.MouseDown[1] = false; break;
		case WM_MOUSEMOVE:
		{
			POINT pt = pMouseStruct->pt;
			ScreenToClient(g_hWindow, &pt);
			io.MousePos = ImVec2((float)pt.x, (float)pt.y);
			break;
		}
		case WM_MOUSEWHEEL:
			io.MouseWheel += GET_WHEEL_DELTA_WPARAM(pMouseStruct->mouseData) > 0 ? +1.0f : -1.0f;
			break;
		}
	}
	return CallNextHookEx(g_hMouseHook, nCode, wParam, lParam);
}

LRESULT CALLBACK LowLevelKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam)
{
	if (nCode >= 0 && ImGui::GetCurrentContext())
	{
		ImGuiIO& io = ImGui::GetIO();
		KBDLLHOOKSTRUCT* pKeyboardStruct = (KBDLLHOOKSTRUCT*)lParam;
		bool keyDown = (wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN);
		bool keyUp = (wParam == WM_KEYUP || wParam == WM_SYSKEYUP);

		if (keyDown || keyUp)
		{
			io.KeysDown[pKeyboardStruct->vkCode] = keyDown;

			io.KeyCtrl = (GetKeyState(VK_CONTROL) & 0x8000) != 0;
			io.KeyShift = (GetKeyState(VK_SHIFT) & 0x8000) != 0;
			io.KeyAlt = (GetKeyState(VK_MENU) & 0x8000) != 0;
			io.KeySuper = false;
		}
	}
	return CallNextHookEx(g_hKeyboardHook, nCode, wParam, lParam);
}


static WNDPROC oWndProc;
extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_KEYDOWN:
		if (wParam == VK_INSERT)
		{
			H::bShowDemoWindow = !H::bShowDemoWindow;
			return 0;
		}
		break;

	case WM_INPUT:
		if (ImGui_ImplWin32_WndProcHandler(hWnd, uMsg, wParam, lParam))
			return true;  // ImGui handled raw input, block game processing
		break;

		// You can add other input messages if needed, e.g. WM_MOUSEMOVE, WM_LBUTTONDOWN, etc.

	default:
		break;
	}

	if (H::bShowDemoWindow)
	{
		if (ImGui_ImplWin32_WndProcHandler(hWnd, uMsg, wParam, lParam))
			return true;  // ImGui handled this message, so block game from processing it
	}

	return CallWindowProc(oWndProc, hWnd, uMsg, wParam, lParam);
}





namespace Hooks {

	void InstallLowLevelHooks()
	{
		if (!g_hMouseHook)
			g_hMouseHook = SetWindowsHookEx(WH_MOUSE_LL, LowLevelMouseProc, GetModuleHandle(NULL), 0);

		if (!g_hKeyboardHook)
			g_hKeyboardHook = SetWindowsHookEx(WH_KEYBOARD_LL, LowLevelKeyboardProc, GetModuleHandle(NULL), 0);
	}

	void UninstallLowLevelHooks()
	{
		if (g_hMouseHook)
		{
			UnhookWindowsHookEx(g_hMouseHook);
			g_hMouseHook = NULL;
		}

		if (g_hKeyboardHook)
		{
			UnhookWindowsHookEx(g_hKeyboardHook);
			g_hKeyboardHook = NULL;
		}
	}



	void Init() {
		g_hWindow = U::GetProcessWindow(); //U::GetProcessWindow( )

#ifdef DISABLE_LOGGING_CONSOLE
		bool bNoConsole = GetConsoleWindow() == NULL;
		if (bNoConsole) {
			AllocConsole();
		}
#endif

		RenderingBackend_t eRenderingBackend = U::GetRenderingBackend();
		switch (eRenderingBackend) {
		case DIRECTX12: DX12::Hook(g_hWindow); break;
		}

#ifdef DISABLE_LOGGING_CONSOLE
		if (bNoConsole) {
			FreeConsole();
		}
#endif

		oWndProc = reinterpret_cast<WNDPROC>(SetWindowLongPtr(g_hWindow, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(WndProc)));

		//InstallLowLevelHooks();
	}

	void Free() {
		MH_DisableHook(MH_ALL_HOOKS);
		std::this_thread::sleep_for(std::chrono::milliseconds(100));

		RenderingBackend_t eRenderingBackend = U::GetRenderingBackend();
		switch (eRenderingBackend) {
		case DIRECTX12: DX12::Unhook(); break;
		}

		UninstallLowLevelHooks();

		if (oWndProc) {
			SetWindowLongPtr(g_hWindow, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(oWndProc));
		}
	}
}
