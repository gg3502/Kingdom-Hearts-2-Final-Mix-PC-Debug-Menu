// dllmain.cpp : Defines the entry point for the DLL application.
#include "stdafx.h"
#include <stdlib.h>
#include "helper.hpp"
#include <Windows.h>
#include "dllmain.hpp"
#include "proc-memory.hpp"
#include "inipp.h"
#include "patches.hpp"
#include "utils.hpp"
#include "MinHook.h"
#include "hooks.hpp"
#include <thread>
#include "TwitchConfig.hpp"


HMODULE baseModule{};

wchar_t exePath[_MAX_PATH]{};

bool bShowDebugConsole{};

uintptr_t GetDefaultImageBase(HMODULE moduleHandle) {
    // Cast the base module address to a DOS header
    auto dosHeader = reinterpret_cast<PIMAGE_DOS_HEADER>(moduleHandle);

    // Get the NT headers using e_lfanew from the DOS header
    auto ntHeaders = reinterpret_cast<PIMAGE_NT_HEADERS>(
        reinterpret_cast<uint8_t*>(moduleHandle) + dosHeader->e_lfanew);

    // Check the NT signature to confirm it's valid
    if (ntHeaders->Signature != IMAGE_NT_SIGNATURE) {
        std::cerr << "Invalid PE header." << std::endl;
        return 0;
    }

    // Return the default ImageBase from the NT headers
    return static_cast<uintptr_t>(ntHeaders->OptionalHeader.ImageBase);
}

void ReadConfig()
{
    inipp::Ini<wchar_t> ini;
    LOG(L"" BUILD_TIME L"\n");
    LOG(L"Game Name: %s\n", Memory::GetVersionProductName().c_str());
    LOG(L"Game Path: %s\n", exePath);

    std::wstring config_path = L"" PROJECT_NAME ".ini";
    std::wifstream iniFile(config_path);

    if (!iniFile)
    {
        // No config file, generate a new one with default values.
        LOG(L"Failed to load config file.\n");

        std::wstring ini_defaults = L""
            L"[Settings]\n"
            L"bShowDebugConsole = true\n"
            L"\n"
            L"[Twitch]\n"
            L"oauth_token = oauth:your_token_here\n"
            L"username = your_username\n"
            L"channel = your_channel\n";

        std::wofstream iniFileOut(config_path);
        iniFileOut << ini_defaults;

        bShowDebugConsole = true;
        bLoggingEnabled = true;

        LOG(L"Created default config file.\n");
    }
    else
    {
        ini.parse(iniFile);
        iniFile.close();

        // Read values from [Settings] section
        inipp::get_value(ini.sections[L"Settings"], wstr(bShowDebugConsole), bShowDebugConsole);

        // *** Check if [Twitch] section exists, append if missing ***
        if (ini.sections.find(L"Twitch") == ini.sections.end())
        {
            LOG(L"[Twitch] section missing, adding default Twitch config.\n");

            std::wofstream iniFileAppend(config_path, std::ios::app);
            iniFileAppend << L"\n[Twitch]\n";
            iniFileAppend << L"oauth_token = oauth:your_token_here\n";
            iniFileAppend << L"username = your_username\n";
            iniFileAppend << L"channel = your_channel\n";
            iniFileAppend.close();

            // Re-parse the updated ini to load Twitch config
            std::wifstream iniFileReload(config_path);
            ini.parse(iniFileReload);
            iniFileReload.close();
        }

        // Read [Twitch] section now that it exists
        TwitchConfig twitchConfig; // Declare here or globally
        if (!twitchConfig.LoadFromIni(ini)) {
            LOG(L"Failed to load Twitch config.\n");
        }
        else {
            LOG(L"Twitch Username: %s\n", twitchConfig.username.c_str());
            LOG(L"Twitch Channel: %s\n", twitchConfig.channel.c_str());
        }
    }
}




int Main(HMODULE dllModule)
{
    


    bLoggingEnabled = false;
    baseModule = GetModuleHandle(NULL);
    uintptr_t defaultImageBase = GetDefaultImageBase(baseModule);
    //std::cout << "Default ImageBase: 0x" << std::hex << defaultImageBase << std::endl;
    wchar_t LogPath[_MAX_PATH]{};
    wcscpy_s(exePath, _countof(exePath), GetRunningPath(exePath));
    _snwprintf_s(LogPath, _countof(LogPath), _TRUNCATE, L"%s\\%s", exePath, L"" PROJECT_LOG_PATH);
    LoggingInit(L"" PROJECT_NAME, LogPath);
    ReadConfig();
    Patches::ApplyDebugPatches(baseModule, dllModule);
    LOG(L"Shutting down " wstr(fp_log) " file handle.\n");
    fclose(fp_log);
    return 1;

}

DWORD WINAPI OnProcessAttach(LPVOID lpParam) {

    Sleep(3000);

    U::SetRenderingBackend(DIRECTX12);


    MH_Initialize();
    LOG("[+] Rendering backend: %s\n", U::RenderingBackendToStr());

    if (U::GetRenderingBackend() == NONE) {
        LOG("[!] Will unload in 2 seconds...\nMake sure U::SetRenderingBackground( ) is called.\n");
        std::this_thread::sleep_for(std::chrono::seconds(2));
        FreeLibraryAndExitThread(reinterpret_cast<HMODULE>(lpParam), 0);
        return 0;
    }

    // Call Main, passing the module handle (dllModule)
    Main(static_cast<HMODULE>(lpParam));

    H::Init();

    return 0;
}

extern "C" BOOL WINAPI DllMain(
    HINSTANCE hinstDLL,
    DWORD fdwReason,
    LPVOID lpReserved
)
{
    if (fdwReason == DLL_PROCESS_ATTACH)
    {
        DisableThreadLibraryCalls(hinstDLL);

        HANDLE hHandle = CreateThread(
            NULL,
            0,
            OnProcessAttach,
            hinstDLL,
            0,
            NULL
        );

        if (hHandle)
            CloseHandle(hHandle);
    }

    return TRUE;
}





