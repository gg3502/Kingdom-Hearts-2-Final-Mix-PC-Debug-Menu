// OutputDebugStringA
#include <windows.h>
// _snprintf_s, printf_s
#include <stdio.h>
// PROJECT_NAME
#include "dllmain.hpp"
#include "stdafx.h"

namespace Assert
{
    void always_assert_(const char* reason, const char* file_, const char* func, const char* built, size_t line_num, const char* msg = nullptr)
    {
#ifndef _DEBUG
        char message[2048]{};
        _snprintf_s(message, sizeof(message), ""
            "ASSERTION: %s%s%s\n"
            "\n"
            "Function: %s\n"
            "File: %s\n"
            "Line: %d\n"
            "Built: %s",
            reason,
            msg ? "\nMessage: " : "",
            msg ? msg : "",
            func ? func : "(null)",
            file_ ? file_ : "(null)",
            line_num ? line_num : 0,
            built ? built : "(null)");
        MessageBoxA(nullptr, message, PROJECT_NAME " Assertion Failure", MB_ICONERROR | MB_OK);
#else
        char message[2048]{};
        _snprintf_s(message, sizeof(message),
            "******************************************************************************\n"
            "*** %s ASSERTION: %s%s%s\n"
            "***\n"
            "***  Function: %s\n"
            "***      File: %s\n"
            "***      Line: %d\n"
            "***     Built: %s\n"
            "******************************************************************************\n",
            PROJECT_NAME,  // Macro used directly
            reason,
            msg ? "\n*** Message: " : "",
            msg ? msg : "",
            func ? func : "(null)",
            file_ ? file_ : "(null)",
            line_num ? line_num : 0,
            built ? built : "(null)");
        OutputDebugStringA(message);
        printf_s("%s", message);
#endif
    }
}
