//#include "stdafx.h"
#include "helper.hpp"
#include "dllmain.hpp"
#include "ansi.hpp"
#include "patterns.hpp"
#include <vector>
#include "function_ptr.hpp"
#include "function_typedefs.hpp"
//#include "kh2-dmenu.hpp"
#include "patches.hpp"
//#include "helper.hpp"
#include "kh2-dmenu.hpp"

namespace Patches {

    struct PatternInfo {
        const wchar_t* pattern;
        const wchar_t* name;
    };

    // Populate the patterns and names into a vector
    std::vector<PatternInfo> patterns = {
    { Patterns::pattern_for_DbgWinCursorMovement, L"DbgWinCursorMovement" },
    { Patterns::pattern_for_InitDBGwin, L"InitDBGwin" },
    { Patterns::pattern_for_YWButton, L"YWButton" },
    { Patterns::pattern_for_OpenSingletonWindow, L"OpenSingletonWindow" },
    { Patterns::pattern_for_ContainerLink, L"ContainerLink" },
    { Patterns::pattern_for_YWLabel, L"YWLabel" },
    { Patterns::pattern_for_GetOBJENTRY, L"GetOBJENTRY" },
    { Patterns::pattern_for_ReadRequest, L"ReadRequest" },
    { Patterns::pattern_for_FlushCacheBuff, L"FlushCacheBuff" },
    { Patterns::pattern_for_CreateFieldThread, L"CreateFieldThread" },
    { Patterns::pattern_for_TaskSleep, L"TaskSleep" },
    { Patterns::pattern_for_AllocArea, L"AllocArea" },
    { Patterns::pattern_for_Alloc_YW_Memory, L"Alloc_YW_Memory" },
    { Patterns::pattern_for_ShowDebugWindow, L"ShowDebugWindow" },
    { Patterns::pattern_for_DebugWindowAddSubMenu, L"DebugWindowAddSubMenu" },
    { Patterns::pattern_for_Widget, L"Widget" },
    { Patterns::pattern_for_StatusPrintf, L"StatusPrintf" },
    { Patterns::pattern_for_GetPlayer, L"GetPlayer" },
    { Patterns::pattern_for_YWFrame, L"YWFrame" },
    { Patterns::pattern_for_YWListButton, L"YWListButton" },
    { Patterns::pattern_for_YWToggleButton, L"YWToggleButton" },
    { Patterns::pattern_for_PlaySystemSEsound, L"PlaySystemSEsound" },
    { Patterns::pattern_for_OBJENTRY_APPEAR, L"OBJENTRY_APPEAR" },
    { Patterns::pattern_for_DialWidget, L"DialWidget" },
    { Patterns::pattern_for_GetOBJMenuObj, L"GetOBJMenuObj" },
    { Patterns::pattern_for_GetHardPad, L"GetHardPad" },
    { Patterns::pattern_for_ClearStatusWindow, L"ClearStatusWindow" },
    { Patterns::pattern_for_SetCommandItem, L"SetCommandItem" },
    { Patterns::pattern_for_GetPartyChara, L"GetPartyChara" },
    { Patterns::pattern_for_DetatchObject, L"DetatchObject" },
    { Patterns::pattern_for_GetAreaName, L"GetAreaName" },
    { Patterns::pattern_for_SetObjPosition, L"SetObjPosition" },
    { Patterns::pattern_for_OpenInformationWindow, L"OpenInformationWindow" },
    { Patterns::pattern_for_CampMenuStart, L"CampMenuStart" },
    { Patterns::pattern_for_soundCreate, L"soundCreate" },
    { Patterns::pattern_for_STDOBJ_Call_se, L"STDOBJ_Call_se" },
    { Patterns::pattern_for_ControlEventOff, L"ControlEventOff" },
    { Patterns::pattern_for_ControlEventOn, L"ControlEventOn" },
    { Patterns::pattern_for_CommonLayerOff, L"CommonLayerOff" },
    { Patterns::pattern_for_CommonLayerOn, L"CommonLayerOn" },
    { Patterns::pattern_for_TargetSetObj, L"TargetSetObj" },
    { Patterns::pattern_for_SetCurrentBGM, L"SetCurrentBGM" },
    { Patterns::pattern_for_MapJump, L"MapJump" },
    { Patterns::pattern_for_gbVoice, L"gbVoice" },
    { Patterns::pattern_for_ObjClick, L"ObjClick" },
    { Patterns::pattern_for_GetObjEntryName, L"GetObjEntryName" },
    { Patterns::pattern_for_SetMember, L"SetMember" },
    { Patterns::pattern_for_GetMember, L"GetMember" },
    { Patterns::pattern_for_CreateObjCamera, L"CreateObjCamera" },
    { Patterns::pattern_for_AttackStrike, L"AttackStrike" },
    { Patterns::pattern_for_CanControlObj, L"CanControlObj" },
    { Patterns::pattern_for_FriendTarget, L"FriendTarget" },
    { Patterns::pattern_for_ObjActStart, L"ObjActStart" },
    { Patterns::pattern_for_SetObjSpecialEffect, L"SetObjSpecialEffect" },
    { Patterns::pattern_for_AttachOBj, L"AttachOBj" },
    { Patterns::pattern_for_OBJAttachCamera, L"OBJAttachCamera" },
    { Patterns::pattern_for_OBJscatterPrize, L"OBJscatterPrize" },
    { Patterns::pattern_for_SetWindowTitle, L"SetWindowTitle" },
    { Patterns::pattern_for_YWList, L"YWList" },
    { Patterns::pattern_for_SpawnPlayer, L"SpawnPlayer" },
    { Patterns::pattern_for_GetSoftPad, L"GetSoftPad" },
    { Patterns::pattern_for_CommandElem, L"CommandElem" },
    { Patterns::pattern_for_GetDialValue, L"GetDialValue" },
    { Patterns::pattern_for_SetDialValue, L"SetDialValue" },
    { Patterns::pattern_for_YWFontPrint, L"YWFontPrint" },
    { Patterns::pattern_for_ConfigWidget, L"ConfigWidget" },
    { Patterns::pattern_for_ConfigWidget2, L"ConfigWidget2" },
    { Patterns::pattern_for_GameOver, L"GameOver" },
    { Patterns::pattern_for_UnitGroupRandomPosPlayer, L"UnitGroupRandomPosPlayer" },
    { Patterns::pattern_for_GetObjPos, L"GetObjPos" },
    { Patterns::pattern_for_OpenTreasureEventInfo, L"OpenTreasureEventInfo" },
    { Patterns::pattern_for_GetTreasureInfoNum, L"GetTreasureInfoNum" },
    { Patterns::pattern_for_TreasureTableGetEvent, L"TreasreTableGetEvent" },
    { Patterns::pattern_for_GetTreasureTableTreasureBox, L"GetTreasureTableTreasureBox" },
    { Patterns::pattern_for_InformationTimer, L"InformationTimer" },
    { Patterns::pattern_for_MissionStart, L"MissionStart" },
    { Patterns::pattern_for_ActivateMission, L"ActivateMission" },
    { Patterns::pattern_for_RestartTimer, L"RestartTimer" },
    { Patterns::pattern_for_ActivateMissionTimer, L"ActivateMissionTimer" },
    { Patterns::pattern_for_Mission2DActivate, L"Mission2DActivate" },
    { Patterns::pattern_for_ChangeForm, L"ChangeForm" },
    { Patterns::pattern_for_ChangeFormCapture, L"ChangeFormCapture" },
    { Patterns::pattern_for_GetSora, L"GetSora" },
    { Patterns::pattern_for_File_Read, L"File_Read" },
    { Patterns::pattern_for_ApplyPlayerParam, L"ApplyPlayerParam" },
    { Patterns::pattern_for_SwitchStatus, L"SwitchStatus" },
    { Patterns::pattern_for_GetEventProjectName, L"GetEventProjectName" },
    { Patterns::pattern_for_ShowVersion, L"ShowVersion" },
    { Patterns::pattern_for_IsFreeAbility, L"IsFreeAbility" },
    { Patterns::pattern_for_CutsceneZoom, L"CutsceneZoom" },
    { Patterns::pattern_for_YWwindow_minSize, L"YWwindow_minSize" },
    { Patterns::pattern_for_YWwindow_maxSize, L"YWwindow_maxSize" }
    };



    void ApplyDebugPatches(HMODULE ExeModule, HMODULE dllModule)
    {


        




        if (bShowDebugConsole)
        {
            AllocConsole();
            SetConsoleTitleA("KH2FM - Debug Console");
            freopen_s(reinterpret_cast<FILE**>(stdin), "conin$", "r", stdin);
            freopen_s(reinterpret_cast<FILE**>(stdout), "conout$", "w", stdout);
            freopen_s(reinterpret_cast<FILE**>(stderr), "conout$", "w", stderr);
            ::ShowWindow(GetConsoleWindow(), SW_SHOW);
            HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
            DWORD dwMode = 0;
            GetConsoleMode(hConsole, &dwMode);
            printf("MADE BY gg3502\n");
            printf_s("dwMode: 0x08%x\n", dwMode);
            // enable ansi color formatting
            dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
            printf_s("dwMode | ENABLE_VIRTUAL_TERMINAL_PROCESSING: 0x08%x\n", dwMode);
            SetConsoleMode(hConsole, dwMode);
            SetConsoleOutputCP(CP_UTF8);

            printf_s(BUILD_TIME "\n");

            //puts("test ansi colors");
           // puts(ANSI_BLK "ANSI Black");
           // puts(ANSI_RED "ANSI Red");
           // puts(ANSI_GRN "ANSI Green");
           // puts(ANSI_YEL "ANSI Yellow");
          //  puts(ANSI_BLU "ANSI Blue");
           // puts(ANSI_MAG "ANSI Maganta");
          //  puts(ANSI_CYN "ANSI Cyan");
           // puts(ANSI_RESET "ANSI Reset");
          //  puts(ANSI_WHT "ANSI White");
          //  LOG_ERROR(L"This is error LOG\n");
          //  LOG_WARNING(L"This is warning LOG\n");
           // LOG_INFO(L"This is info LOG\n");
           // LOG_VERBOSE(L"This is verbose LOG\n");

            printf("Start pattern scan for patches\n");



           // for (const auto& patternInfo : patterns) {
            //    FindAndPrintPatternW(patternInfo.pattern, patternInfo.name);
           // }
            //uintptr_t EvalScriptWarns = FindAndPrintPatternW(Patterns::, wstr(Patterns::GameWarnScriptPrint2));
           // uintptr_t PrintAddr = FindAndPrintPatternW(Patterns::GameWarnScriptPrint, wstr(Patterns::GameWarnScriptPrint));
            const unsigned char jnzTojmp[] = { 0xEB, 0x08 };
            unsigned char nopInstruction[8] = { 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90 };
            unsigned char nopInstruction2[5] = { 0x90, 0x90, 0x90, 0x90, 0x90 };

            unsigned char nopInstructionMouseMovement1[3] = { 0x90, 0x90, 0x90 };
            unsigned char nopInstructionMouseMovement2[2] = { 0x90, 0x90 };

            uintptr_t InitDebugMenuFunctionAddress = FindAndPrintPatternW(Patterns::pattern_for_InitDBGwin, L"DebugMenuInit", 0);




            // Create a function pointer using the typedef from the namespace
            FunctionCalls::InitDebugMenu InitDebugMenuFunction = reinterpret_cast<FunctionCalls::InitDebugMenu>(InitDebugMenuFunctionAddress);

            if (InitDebugMenuFunction) {
                // Call the function
                InitDebugMenuFunction();
                LOG(L"InitDebugMenu function called successfully.\n");

            }
            else {
                LOG(L"Function pointer is null.\n");
            }


            uintptr_t PatchDbgWinCursor = WritePatchPattern(Patterns::pattern_for_DbgWinCursorMovement, jnzTojmp, sizeof(jnzTojmp), L"DbgWinCursorMovement", 8);

            uintptr_t DbgWinCursorPattern = FindAndPrintPatternW(Patterns::pattern_for_DbgWinCursorMovement, L"", 8);

            uintptr_t NopCutsceneZoom = WritePatchPattern(Patterns::pattern_for_CutsceneZoom, nopInstruction, sizeof(nopInstruction), L"DisableCutsceneZoom", 0);

            uintptr_t NopMouseCamera1 = WritePatchPattern(Patterns::pattern_for_MouseMovementInput1, nopInstructionMouseMovement1, sizeof(nopInstructionMouseMovement1), L"Disable mouse camera movement1", 0);

            uintptr_t NopMouseCamera2 = WritePatchPattern(Patterns::pattern_for_MouseMovementInput2, nopInstructionMouseMovement2, sizeof(nopInstructionMouseMovement2), L"Disable mouse camera movement2", 0);
            //uintptr_t NopStatusClear = WritePatchPattern(Patterns::pattern_for_StatusClearNOP, nopInstruction2, sizeof(nopInstruction2), L"DisableStatusClear", 0);


            printf("ExeModule Base: 0x%p\n", ExeModule);
            //printf("dllModule Base: 0x%p\n", dllModule);

            uintptr_t NopMouseButtonsAddress = (uintptr_t)ExeModule + 0x1030ED;




            //LOG("NopMouseButton Address: 0x%llx\n", NopMouseButtonsAddress);

           // memcpy((void*)NopMouseButtonsAddress, nopInstruction2, sizeof(nopInstruction2));
            WritePatchAddress(NopMouseButtonsAddress, nopInstruction2, sizeof(nopInstruction2), L"Disable mouse buttons", 0);
            

            DebugLoop();
            //CheckRoomLoop();



        }

    }
}