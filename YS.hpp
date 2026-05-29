#pragma once

#include "common_types.hpp"
#include "YW.hpp"

namespace YS {
    // Define a structure to represent a menu
    struct ELEM
    {
        const char* Label;
        const ELEM* SubMenu;
        void (*Callback)(int);
    };

    struct MENU {
        ELEM* items;
        int itemCount;
    };

    


    typedef __int8 OBJ[2496];

    typedef __int8 OBJ_132[912];

    typedef __int8 STDOBJ[2560];

    typedef __int8 FRIEND[3344];

    typedef __int8 OBJ_ATTACH[24];

    typedef __int8 TARGET[20];

    typedef char MOTION[448];

    typedef char MOTION_QUEUE[20];

    typedef __int8 PARTY[2880];

    typedef __int8 ACT[172];

    typedef __int8 FRIEND[3344];

    typedef __int8 PLAYER[3696];

    typedef __int8 GAMEOVER[80];

    typedef __int8 MISSION2D[44];

    typedef __int8 CHANGEFORM[0x38]; //32

    typedef __int8 SORA[2944];

    typedef __int8 SA_EFFECT_WK_0[52];

    typedef __int8 RYJ_EFFECT[92];

    typedef __int8 RYJ_PAX[28];

    typedef __int8 WINDOW_BASE[2924];

    typedef __int8 OBJ2D_WINDOW[28];

    typedef __int8 WINDOW[132];

    typedef __int8 GAME_MODE[44];

    //typedef __int8 TEST_FLAG[92];

    struct TEST_FLAG : YW::WIDGET
    {
        struct BUFF // Define BUFF inside TEST_FLAG
        {
            int Num;

            struct LabelType // Replace the long hashed name with a meaningful name
            {
                // Define the structure fields here
            } Label[512]; // Array of 512 LabelType elements
        };

        int Current;
        BUFF* Buff; // Pointer to BUFF structure
    };


    typedef __int8 VM[64];

    typedef __int8 TEST_OBJMENU[216];

    typedef void (*FILE_READ_CALLBACK)(unsigned int*, size_t, unsigned int*);

    typedef __int8 PLAYER_COMMAND[2640];

    typedef __int8 FIELD_COMMAND[2752];

    typedef __int8 ATTACK_13[208];

    typedef __int8 SLIST_ELEM_3736[4];



    struct ATTACKPARAM
    {
        __int8 gap0[48];
    };



    struct OBJ_WINDOW : YW::WINDOW
    {
        OBJ* Obj;
    };


    

    typedef unsigned __int64 ULONGLONG;
    typedef float FLOAT;

    struct PAD
    {
        ULONGLONG Button;
        ULONGLONG Trigger;
        ULONGLONG Release;
        ULONGLONG Repeat;
        Vector4 Stick[2];
        float RepeatCount;
    };

    enum class HARDPAD : __int32 {
        SELECT = 0x1,
        L3 = 0x2,
        R3 = 0x4,
        START = 0x8,
        UP = 0x10,
        RIGHT_0 = 0x20,
        DOWN = 0x40,
        LEFT_0 = 0x80,
        L2 = 0x100,
        R2 = 0x200,
        L1 = 0x400,
        R1 = 0x800,
        TRIANGLE = 0x1000,
        CIRCLE = 0x2000,
        CROSS = 0x4000,
        SQUARE = 0x8000,
        RIGHT_0_ALT = 0x10000,
        DOWN_0 = 0x20000,
        LEFT_0_ALT = 0x40000,
        UP_0_ALT = 0x80000,
        RIGHT_1 = 0x100000,
        DOWN_1 = 0x200000,
        LEFT_1 = 0x400000,
        UP_1 = 0x800000,
        DECIDE = 0x1000000,
        CANCEL = 0x2000000,
        SHIFT_OFF = 0x4000000,
        SHIFT_ON = 0x8000000
    };

    struct BGMSET
    {
        u_short Number[2];
    };

    struct member
    {
        u_char Member[4];
    };

    struct BTLOBJ
    {
        int ReflectRC;
        int TouchZone;
        int CaMedal;
        int NmPresent;
    };

    union COMMAND_ELEM_$F1D9B565482570AAD24B2ABFCC437FFB
    {
        __int16 Arg;
        __int16 Form;
        __int16 Magic;
    };

    struct COMMAND_ELEM
    {
        u_short Id;
        u_short Exec;
        COMMAND_ELEM_$F1D9B565482570AAD24B2ABFCC437FFB _anon_0;
        char SubMenu;
        u_char Icon;
        int MessageId;
        u_int Flag;
        float Range;
        float Dir;
        float DirRange;
        u_char Cost;
        u_char Camera;
        u_char Priority;
        u_char Receiver;
        u_short Time;
        u_short Require;
        u_char Mark;
        u_char Action;
        u_short RcCount;
        u_short DistRange;
        u_short Score;
        u_short DisableForm;
        u_char Group;
        u_char Reserve[1];
    };

    

    /* 57582 */
    struct MISSION_TIMER
    {
        MISSION2D baseclass_0;
    };

    

    /* 68024 */
    struct WINDOW_BALLOON
    {
        WINDOW_BASE baseclass_0;
    };

    struct ENEMYBASE {
        
    };

    struct ENEMY : ENEMYBASE {
        float Karma;
        float KarmaLimit;
    };

    struct MOTIONSET
    {
        __int8 gap0[1];
    };
    
    enum TARGET_TYPE
    {
        TYPE_ENEMY = 0x0,
        TYPE_REVENGE = 0x1,
        TYPE_PLAYER_0 = 0x2,
        TYPE_BTLNPC = 0x3,
        TYPE_LOCKON = 0x4,
        TYPE_BOSS = 0x5,
        TYPE_AUTO_LOCKON = 0x6,
        TYPE_NULL_2 = 0xFFFFFFFF,
    };

    union BD_VALUE
    {
        char gap0[4];
    };

    enum VM_STATUS : __int32
    {
        STATUS_EXEC_9 = 0x0,
        STATUS_END_9 = 0x1,
    };

    struct MESSAGE {
        const char* text;  // guessed from v9 usage
        // Possibly more fields after this
    };



    
}