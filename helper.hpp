#pragma once

#include "stdafx.h"
#include <Windows.h>
#include <stdio.h>
#include <vector>


extern bool bLoggingEnabled;
extern FILE* fp_log;
extern HMODULE baseModule;

//#define str(s) #s
#define xstr(s) str(s)

enum LogLevel {
    LOG_ERROR,
    LOG_WARNING,
    LOG_INFO,
    LOG_VERBOSE
};


extern void log_message(LogLevel level, const wchar_t* fmt, ...);
void file_log(const wchar_t* fmt, ...);
const wchar_t* GetBoolStr(bool input_bool);
void LoggingInit(const wchar_t* Project_Name, const wchar_t* Project_Log_Name);
void ShowPatchInfo(size_t Patch_Size, uint64_t Patch_Addr, const wchar_t* Patch_Name, uint64_t Patch_Function_Target);
void LogPatchFailed(const wchar_t* Patch_Name, const wchar_t* Patch_Pattern);
uintptr_t WritePatchPattern(const wchar_t* Patch_Pattern, const unsigned char* Patch_Bytes, size_t Patch_Size, const wchar_t* Patch_Name, uint64_t Patch_Offset);
uintptr_t WritePatchPatternWithOriginalBytes(const wchar_t* Patch_Pattern, const unsigned char* Patch_Bytes, size_t Patch_Size, const wchar_t* Patch_Name, uint64_t Patch_Offset, std::vector<unsigned char>& OriginalBytes);
void RestorePatch(uintptr_t Patch_Address, const std::vector<unsigned char>& OriginalBytes);
void WritePatchPattern_Hook(const wchar_t* Patch_Pattern, size_t Patch_Size, const wchar_t* Patch_Name, uint64_t Patch_Offset, void* Function_Target, uint64_t* Return_Address);
void WritePatchPattern_Int(size_t Patch_Size, const wchar_t* Patch_Pattern, void* Patch_Value, const wchar_t* Patch_Name, uint64_t Patch_Offset);
void WritePatchAddress(uint64_t Patch_Address, const unsigned char* Patch_Bytes, size_t Patch_Size, const wchar_t* Patch_Name, uint64_t Patch_Offset);
wchar_t* GetRunningPath(wchar_t* output);
uintptr_t FindAndPrintPatternW(const wchar_t* Patch_Pattern, const wchar_t* Pattern_Name, size_t offset = 0);
void Make32to64Hook(void* source_target, void* second_jmp, void* target_jmp, uint32_t source_size, const wchar_t* source_name, const wchar_t* second_jmp_name, const wchar_t* target_jmp_name);
uintptr_t ReadLEA32(const wchar_t* Patch_Pattern, const wchar_t* Pattern_Name, size_t offset, size_t lea_size, size_t lea_opcode_size);

#define LOG(fmt, ...) file_log(L"%-24s:%u " fmt, __FUNCTIONW__, __LINE__, __VA_ARGS__)
#define LOG_ERROR(fmt, ...)     log_message(LOG_ERROR, L"%-24ls:%u " fmt, __FUNCTIONW__, __LINE__, __VA_ARGS__)
#define LOG_WARNING(fmt, ...)   log_message(LOG_WARNING, L"%-24ls:%u " fmt, __FUNCTIONW__, __LINE__, __VA_ARGS__)
#define LOG_INFO(fmt, ...)      log_message(LOG_INFO, L"%-24ls:%u " fmt, __FUNCTIONW__, __LINE__, __VA_ARGS__)
#define LOG_VERBOSE(fmt, ...)   log_message(LOG_VERBOSE, L"%-24ls:%u " fmt, __FUNCTIONW__, __LINE__, __VA_ARGS__)
