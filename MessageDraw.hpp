#pragma once
#include <winsock.h>

namespace Message {
    struct MESSAGE;

    enum class FONT_TYPE {
        TYPE_SYSTEM = 0x0,
        TYPE_SHADOW = 0x1,
        TYPE_OUTLINE = 0x2,
        TYPE_BALLOON = 0x3,
        TYPE_CREDIT = 0x4,
        TYPE_DEBUG = 0x5,
        TYPE_MAX_2 = 0x6,
        TYPE_PLACE = 0x2,
        TYPE_SUBMENU = 0x1,
        TYPE_COMMAND = 0x1,
    };
    enum class ALIGN {
        ALIGN_LEFT = 0x0,
        ALIGN_RIGHT = 0x1,
        ALIGN_CENTER = 0x2,
        ALIGN_HV_CENTER = 0x3,
        ALIGN_BOTTOM = 0x4,
        ALIGN_LEFT_BOTTOM = 0x5,
    };

    struct BITFLAG {

        __int8 Flag[4];
    };

    class MESSAGEDRAW_158 {
    public:
        // MESSAGEDRAW_158();
        // ~MESSAGEDRAW_158();
         //void DrawMessage(YS::FONT_TYPE type, const char* message, int size, YS::ALIGN align);

        struct FONT
        {
            FONT_TYPE Type;
            int TexBase;
            int TexW;
            int TexH;
            u_char Width;
            u_char Height;
            u_char TexWidth;
            u_char TexHeight;
            int LineSpace;
            int PacketSize;
            u_char* Info;
        };

        struct MESSAGE_SIZE
        {
            size_t Width;
            size_t Height;
        };

        struct MESSAGE
        {
            __int8 gap0[1];
        };

        struct FONTSTYLE
        {
            __int8 gap0[4];
        };

        enum WINDOW_PHASE
        {
            PHASE_OPEN = 0x0,
            PHASE_IDLE_0 = 0x1,
            PHASE_WAIT = 0x2,
            PHASE_END = 0x3,
            PHASE_CLOSE = 0x4,
            PHASE_CLOSE_2 = 0x5,
            PHASE_CLOSE_3 = 0x6,
            PHASE_LEAVE = 0x7,
        };


        struct SLIST_ELEM_112
        {
            int* Next[1];
        };

        struct MESSAGE_STYLE
        {
            u_int Color;
            float Scale;
            float HScale;
            int LineSpace;
            u_int OrigColor;
            float OrigScale;
        };

        struct MESSAGEDRAW
        {
            BITFLAG Flag;
            FONT* Font;
            const char* Message;
            const char* Current;
            MESSAGE_SIZE Size;
            //WINDOW* Window;
            float Time;
            int SelectNum;
            MESSAGE_STYLE Style;
            u_int Color;
            u_int ShadowColor;
            int XPos;
            int YPos;
            float FadeTime;
            MESSAGE_SIZE MaxSize;
            ALIGN Align;
            float FontTime;
        };

        struct WINDOW
        {
            const int* _vptr_;
            int XPos;
            int YPos;
            MESSAGEDRAW MessageDraw;
            const FONTSTYLE* FontStyle;
            WINDOW_PHASE Phase;
            float Time;
            int Priority;
            SLIST_ELEM_112 SList;
            int MessageId;
        };

        

    };
    struct gm_message
    {
        u_int Color;
        u_int FontSize;
        MESSAGEDRAW_158::MESSAGE_SIZE MessageSize;
        const char* Message;
        ALIGN MessageMode;
        char MessageBuff[32];
    };
}