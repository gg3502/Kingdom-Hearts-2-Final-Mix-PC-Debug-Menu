#pragma once

#define PROJECT_NAME "KH2FM.ModClient"

#ifdef _DEBUG
#define BUILD_TYPE "Debug"
#else
#define BUILD_TYPE "Release"
#endif

#define BUILD_STAMP "Built: " __DATE__ " @ " __TIME__

#ifdef _WIN64
#define ARCHITECTURE "x64"
#else
#define ARCHITECTURE "x86"
#endif

#ifdef _MSC_VER
#ifdef _DLL
#define BUILD_CONFIGURATION BUILD_TYPE " DLL"
#else
#define BUILD_CONFIGURATION BUILD_TYPE " Static"
#endif
#else
#define BUILD_CONFIGURATION BUILD_TYPE
#endif
