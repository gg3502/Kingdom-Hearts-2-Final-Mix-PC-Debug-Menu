#include "stdafx.h"
#include "helper.hpp"
#include "proc-memory.hpp"
#include "ansi.hpp"
#include "assert.hpp"
#include <shlwapi.h>

#pragma comment(lib,"shlwapi.lib")

bool bLoggingEnabled{};
FILE* fp_log{};
time_t current_time{};

// Define log levels


// Set the current log level (Change this to control output)
LogLevel CURRENT_LOG_LEVEL = LOG_VERBOSE;



void log_message(LogLevel level, const wchar_t* fmt, ...) {
    if (level <= CURRENT_LOG_LEVEL) {
        // Set color based on log level
        const wchar_t* color = L"";
        switch (level) {
        case LOG_ERROR:   color = ANSI_RED; break;
        case LOG_WARNING: color = ANSI_YEL; break;
        case LOG_INFO:    color = ANSI_GRN; break;
        case LOG_VERBOSE: color = ANSI_CYN; break;
        }

        // Print the color to the console
        fwprintf_s(stdout,L"%s", color);
        
        

        // Handle the variadic arguments
        va_list args;
        va_start(args, fmt);
        file_log(fmt, args);  // Call to file_log that handles actual writing to the file
        va_end(args);

        // Reset color after the log
        fwprintf_s(stdout,L"%s", ANSI_RESET);
    }
}



void file_log(const wchar_t* fmt, ...)
{
    if (!fp_log)
    {
        return;
    }

    wchar_t buffer[1024]{};
    va_list args{};
    va_start(args, fmt);
    _vsnwprintf_s(buffer, _countof(buffer), fmt, args);
    va_end(args);

    wchar_t wtime[256]{};
    tm timeinfo{};

    current_time = std::time(nullptr);
    if (localtime_s(&timeinfo, &current_time) == 0)
    {
        // Use %c format specifier for a simple date and time string
        if (std::wcsftime(wtime, _countof(wtime), L"%c", &timeinfo))
        {
            fwprintf_s(stdout, L"%s: ", wtime);
            fwprintf_s(fp_log, L"%s: ", wtime);
        }
        else
        {
            fwprintf_s(stdout, L"Error formatting time\n");
            fwprintf_s(fp_log, L"Error formatting time\n");
        }
    }
    else
    {
        fwprintf_s(stdout, L"Error getting local time\n");
        fwprintf_s(fp_log, L"Error getting local time\n");
    }

    fwprintf_s(stdout, L"%s", buffer);
    fwprintf_s(fp_log, L"%s", buffer);
    fflush(fp_log);
}

const wchar_t* GetBoolStr(bool input_bool)
{
    return input_bool ? L"true" : L"false";
}

void LoggingInit(const wchar_t* Project_Name, const wchar_t* Project_Log_Name)
{
    errno_t file_stat = _wfopen_s(&fp_log, Project_Log_Name, L"w+, ccs=UTF-8");
    if (file_stat == 0)
    {
        bLoggingEnabled = true;
        std::locale::global(std::locale("fi_FI.utf8"));
        current_time = std::time(nullptr);
        LOG(L"Log file opened at %s\n", Project_Log_Name);
    }
    else
    {
        bLoggingEnabled = false;
        wchar_t errorText[256] = { 0 };
        wchar_t errorMsg[512] = { 0 };
        _wcserror_s(errorText, _countof(errorText), file_stat);
        _snwprintf_s(errorMsg, _countof(errorMsg), _TRUNCATE, L"Failed to open log file. (%s)\nError code: %i (0x%x) %s", Project_Log_Name, file_stat, file_stat, errorText);
        MessageBoxW(nullptr, errorMsg, Project_Name, MB_ICONWARNING);
    }
}

void ShowPatchInfo(size_t Patch_Size, uint64_t Patch_Addr, const wchar_t* Patch_Name, uint64_t Patch_Function_Target)
{
    //LOG(L"Patch Name: %s\n", Patch_Name);
    //LOG(L"Patch length: %llu bytes\n", Patch_Size);
   // LOG(L"Patch address: 0x%016llx\n", Patch_Addr);
    if (Patch_Function_Target)
        LOG(L"Patch Function Target: 0x%016llx\n", Patch_Function_Target);
}

void LogPatchFailed(const wchar_t* Patch_Name, const wchar_t* Patch_Pattern)
{
    wprintf_s(L"" ANSI_RED);
    LOG(L"%s Pattern Scan Failed. Please adjust your scan patterns and try again\n", Patch_Name);
    wprintf_s(L"" ANSI_RESET);
    LOG(L"Pattern %s\n", Patch_Pattern);
}

uintptr_t WritePatchPattern(const wchar_t* Patch_Pattern, const unsigned char* Patch_Bytes, size_t Patch_Size, const wchar_t* Patch_Name, uint64_t Patch_Offset)
{
    uint8_t* Address_Result = Memory::PatternScanW(baseModule, Patch_Pattern);
    uintptr_t Patch_Address = 0;
    if (Address_Result)
    {
        Patch_Address = (uintptr_t)Address_Result + Patch_Offset;
        Memory::PatchBytes(Patch_Address, Patch_Bytes, Patch_Size);
        ShowPatchInfo(Patch_Size, Patch_Address, Patch_Name, 0);
        return Patch_Address;
    }
    else
    {
        LogPatchFailed(Patch_Name, Patch_Pattern);
        return 0;
    }
    return 0;
}

uintptr_t WritePatchPatternWithOriginalBytes(const wchar_t* Patch_Pattern, const unsigned char* Patch_Bytes, size_t Patch_Size, const wchar_t* Patch_Name, uint64_t Patch_Offset, std::vector<unsigned char>& OriginalBytes)
{
    uint8_t* Address_Result = Memory::PatternScanW(baseModule, Patch_Pattern);
    uintptr_t Patch_Address = 0;

    if (Address_Result)
    {
        Patch_Address = (uintptr_t)Address_Result + Patch_Offset;

        // Save original bytes
        OriginalBytes.resize(Patch_Size);
        Memory::ReadBytes(Patch_Address, OriginalBytes.data(), Patch_Size);

        // Apply the patch
        Memory::PatchBytes(Patch_Address, Patch_Bytes, Patch_Size);

        ShowPatchInfo(Patch_Size, Patch_Address, Patch_Name, 0);
        return Patch_Address;
    }
    else
    {
        LogPatchFailed(Patch_Name, Patch_Pattern);
        return 0;
    }
}

void RestorePatch(uintptr_t Patch_Address, const std::vector<unsigned char>& OriginalBytes)
{
    if (Patch_Address != 0 && !OriginalBytes.empty())
    {
        Memory::PatchBytes(Patch_Address, OriginalBytes.data(), OriginalBytes.size());
        //LOG("Patch restored at address: 0x%llX", Patch_Address);
    }
    else
    {
        //LOG("Failed to restore patch. Invalid address or empty bytes.");
    }
}

void WritePatchPattern_Hook(const wchar_t* Patch_Pattern, size_t Patch_Size, const wchar_t* Patch_Name, uint64_t Patch_Offset, void* Function_Target, uint64_t* Return_Address)
{
    if (Patch_Size < 14) // less than minimum hookptr + jmp bytes
    {
        wprintf_s(L"" ANSI_RED);
        LOG(L"Patch '%s' with Pattern '%s' size is less than required of 14 bytes!\n", Patch_Name, Patch_Pattern);
        wprintf_s(L"" ANSI_RESET);
        return;
    }
    uint8_t* Address_Result = nullptr;
    Address_Result = Memory::PatternScanW(baseModule, Patch_Pattern);
    uint64_t Patch_Address = 0;
    if (Address_Result)
    {
        Patch_Address = (uintptr_t)Address_Result + Patch_Offset;
        if (Return_Address)
        {
            *Return_Address = Patch_Address + Patch_Size;
            LOG(L"%s Hook Info:\n", Patch_Name);
            LOG(L"Start address: 0x%016llx\n", Address_Result);
            LOG(L"Return address: 0x%016llx\n", *Return_Address);
            LOG(L"Function target address: 0x%016llx\n", uintptr_t(Function_Target));
        }
        else
        {
            wprintf_s(L"" ANSI_YEL);
            LOG(L"%s Hook does not contain a returning address\n", Patch_Name);
            LOG(L"Start address: 0x%016llx\n", Address_Result);
            LOG(L"Function target address: 0x%016llx\n", uintptr_t(Function_Target));
            LOG(L"Please make sure it is intentional\n");
            wprintf_s(L"" ANSI_RESET);
        }
        Memory::DetourFunction64((void*)(Patch_Address), Function_Target, Patch_Size);
    }
    else
    {
        LogPatchFailed(Patch_Name, Patch_Pattern);
    }
}

void WritePatchPattern_Int(size_t Patch_Size, const wchar_t* Patch_Pattern, void* Patch_Value, const wchar_t* Patch_Name, uint64_t Patch_Offset)
{
    WritePatchPattern(Patch_Pattern, (unsigned char*)Patch_Value, Patch_Size, Patch_Name, Patch_Offset);
}

void WritePatchAddress(uint64_t Patch_Address, const unsigned char* Patch_Bytes, size_t Patch_Size, const wchar_t* Patch_Name, uint64_t Patch_Offset)
{
    uint64_t Patch_Address_Offset = Patch_Address + Patch_Offset;
    Memory::PatchBytes(Patch_Address_Offset, Patch_Bytes, Patch_Size);
    ShowPatchInfo(Patch_Size, Patch_Address_Offset, Patch_Name, 0);
}

wchar_t* GetRunningPath(wchar_t* output)
{
    GetModuleFileNameW(nullptr, output, MAX_PATH);
    PathRemoveFileSpecW(output);
    return output;
}

wchar_t* ConvertToWideChar(const char* input)
{
    int length = MultiByteToWideChar(CP_UTF8, 0, input, -1, nullptr, 0);
    if (length == 0)
    {
        return nullptr;
    }
    wchar_t* output = new wchar_t[length];
    MultiByteToWideChar(CP_UTF8, 0, input, -1, output, length);
    return output;
}
//og code
uintptr_t FindAndPrintPatternW(const wchar_t* Patch_Pattern, const wchar_t* Pattern_Name, size_t offset)
{
    uintptr_t Address_Result = (uintptr_t)Memory::PatternScanW(baseModule, Patch_Pattern);
    uintptr_t Patch_Address = 0;

    if (Address_Result)
    {
        if (offset)
        {
            Patch_Address = offset + Address_Result;
            // Optional: If you want to log successful pattern matches, you can add the log here
             //LOG(L"%s: 0x%016llx - 0x%llx = 0x%016llx\n", Pattern_Name, Address_Result, offset, Patch_Address);
        }
        else
        {
            Patch_Address = Address_Result;
            // Optional: If you want to log successful pattern matches, you can add the log here
            // LOG(L"%s: 0x%016llx\n", Pattern_Name, Patch_Address);
        }
        return Patch_Address;
    }
    else
    {
        // Log only if the pattern is not found
        LogPatchFailed(Pattern_Name, Patch_Pattern);
    }

    return 0;
}


void Make32to64Hook(void* source_target, void* second_jmp, void* target_jmp, uint32_t source_size, const wchar_t* source_name, const wchar_t* second_jmp_name, const wchar_t* target_jmp_name)
{
    if (!source_target || !second_jmp || !target_jmp || source_size < 5)
    {
        always_assert(!source_target || !second_jmp || !target_jmp || source_size < 5, nullptr);
        wprintf_s(L"" ANSI_RED);
        LOG(L"Canoot create jump '%s' from '%s' to '%s'\n", source_name, second_jmp_name, target_jmp_name);
        LOG(L"source_target: 0x%p\n", source_target);
        LOG(L"second_jmp: 0x%p\n", second_jmp);
        LOG(L"target_jmp: 0x%p\n", target_jmp);
        wprintf_s(L"" ANSI_RESET);
        return;
    }
    Memory::DetourFunction32((void*)source_target, (void*)second_jmp, source_size);
    LOG(L"Created jump %s (0x%016llx) to %s (0x%016llx)\n", source_name, (uintptr_t)source_target, second_jmp_name, (uintptr_t)second_jmp);
    Memory::DetourFunction64((void*)second_jmp, (void*)target_jmp, 14);
    LOG(L"Created jump %s (0x%016llx) to %s (0x%016llx)\n", second_jmp_name, (uintptr_t)second_jmp, target_jmp_name, (uintptr_t)target_jmp);
}

/*uintptr_t ReadLEA32(const wchar_t* Patch_Pattern, const wchar_t* Pattern_Name, size_t offset, size_t lea_size, size_t lea_opcode_size)
{
    // Log the start of the function with provided parameters
    LOG(L"ReadLEA32: Pattern_Name: %s, Offset: %zu, LEA_Size: %zu, LEA_Opcode_Size: %zu\n",
        Pattern_Name, offset, lea_size, lea_opcode_size);

    // Attempt to scan for the pattern
    uintptr_t Address_Result = (uintptr_t)Memory::PatternScanW(baseModule, Patch_Pattern);
    uintptr_t Patch_Address = 0;
    int32_t lea_offset = 0;
    uintptr_t New_Offset = 0;

    // Log the result of the pattern scan
    if (Address_Result)
    {
        LOG(L"Pattern '%s' found at address: 0x%016llx\n", Pattern_Name, Address_Result);

        // Check if there's an additional offset to consider
        if (offset)
        {
            Patch_Address = offset + Address_Result;
            lea_offset = *(int32_t*)(lea_size + Patch_Address);
            New_Offset = Patch_Address + lea_offset + lea_opcode_size;

            // Log after calculating new offset
            LOG(L"Address after offset adjustment: 0x%016llx -> 0x%016llx\n", Patch_Address, New_Offset);
        }
        else
        {
            Patch_Address = Address_Result;
            lea_offset = *(int32_t*)(lea_size + Address_Result);
            New_Offset = Patch_Address + lea_offset + lea_opcode_size;

            // Log after calculating new offset without additional offset
            LOG(L"Address without additional offset: 0x%016llx -> 0x%016llx\n", Patch_Address, New_Offset);
        }

        // Log the final resolved address
        LOG(L"Resolved final address for '%s': 0x%016llx\n", Pattern_Name, New_Offset);
        return New_Offset;
    }
    else
    {
        // Log if pattern scan failed
        LOG(L"Pattern '%s' not found!\n", Pattern_Name);
        LogPatchFailed(Pattern_Name, Patch_Pattern); // Existing log for failure case
    }

    // Log the function returning 0
    LOG(L"ReadLEA32: Returning 0 for pattern '%s'\n", Pattern_Name);
    return 0;
}*/

uintptr_t ReadLEA32(const wchar_t* Patch_Pattern, const wchar_t* Pattern_Name, size_t offset, size_t lea_size, size_t lea_opcode_size)
{
    // Attempt to scan for the pattern
    uintptr_t Address_Result = (uintptr_t)Memory::PatternScanW(baseModule, Patch_Pattern);
    uintptr_t Patch_Address = 0;
    int32_t lea_offset = 0;
    uintptr_t New_Offset = 0;

    if (Address_Result)
    {
        // If pattern is found, continue as normal but don't log
        if (offset)
        {
            Patch_Address = offset + Address_Result;
            lea_offset = *(int32_t*)(lea_size + Patch_Address);
            New_Offset = Patch_Address + lea_offset + lea_opcode_size;
        }
        else
        {
            Patch_Address = Address_Result;
            lea_offset = *(int32_t*)(lea_size + Address_Result);
            New_Offset = Patch_Address + lea_offset + lea_opcode_size;
        }
        return New_Offset;
    }
    else
    {
        // Log only if the pattern is not found
        LOG(L"Pattern '%s' not found!\n", Pattern_Name);
        LogPatchFailed(Pattern_Name, Patch_Pattern);  // Optional: log failure in another way
    }

    return 0;
}

