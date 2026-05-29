#pragma once

#include <stdlib.h>
#include <string>


extern bool bShowDebugConsole;
//extern std::wstring publicIP;   // Public IP address
//extern std::wstring username;   // Username
extern wchar_t exePath[_MAX_PATH];
//extern HMODULE baseModule;

#define wstr(s) L#s
#define wxstr(s) wstr(s)
//#define PROJECT_NAME "KH2FM.ModClient"
#define PROJECT_LOG_PATH PROJECT_NAME ".log"
#define BUILD_STAMP "Built: " __DATE__ " AT " __TIME__
#define BUILD_TIME PROJECT_NAME " " BUILD_STAMP