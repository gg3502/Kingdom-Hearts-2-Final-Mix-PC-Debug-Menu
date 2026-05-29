#pragma once

#include <cstdint>
#include "OBJENRTY_0.hpp"

namespace YW {

    struct FLAG {
        unsigned int Flag;
    };

    struct WINDOW_OPTION : FLAG {
    };

    struct EVENT : FLAG {
    };

    struct RECT {
        int X;
        int Y;
        int W;
        int H;
    };

    struct CONTAINER;

    struct YW_RECT {
        int X;
        int Y;
        int W;
        int H;
    };

    struct YW_FLAG {
        uint32_t Flag;
    };

    struct YW_POSITION {
        int X;
        int Y;
    };

    typedef __int8 YW_CONTAINER_46[100];
    struct CONTAINER_0;
    struct YW_WIDGET_0;

    struct WIDGET_FLAG {
        unsigned int Flag;
    };

    struct COLOR_0 {
        int Red;
        int Green;
        int Blue;
        int Alpha;
    };

    struct YW_EVENT : YW_FLAG {
    };

    struct CONTAINER_0 {
        YW_WIDGET_0* Child;
        CONTAINER_0* Delegate;
        YW_POSITION IconPos;
    };

    struct YW_WIDGET_0 {
        const int* _vptr_;
        YW_RECT Rect;
        YW_RECT Frame;
        YW_POSITION Offset;
        CONTAINER_0* Parent;
        YW_WIDGET_0* Next;
        WIDGET_FLAG Flag;
        YW_RECT DrawRect;
        const YW::COLOR_0* Color[2];
        void (*Finalizer)(YW_WIDGET_0*);
    };

    struct YW_LIST_0 : YW_WIDGET_0 {
        const char** Elem;
        int ElemNum;
        int Current;
        void (*SelectCallback)(YW_EVENT, int, unsigned int*);
        unsigned int* SelectCallbackArg;
    };

    

    typedef unsigned int u_int;

    struct YW_CONTAINER_0 : YW_WIDGET_0 {
        unsigned char gap54[16];
    };

    struct YW_COLOR_0 {
        int R;
        int G;
        int B;
        int A;
    };

    enum YW_WINDOW_STATE : __int32 {
        WINDOW_STATE_NORMAL = 0x0,
        WINDOW_STATE_MOVE = 0x1,
        WINDOW_STATE_RESIZE = 0x2,
        WINDOW_STATE_ICON = 0x3,
    };

    struct YW_WINDOW_OPTION : YW_FLAG {
    };

    typedef YW_WINDOW_STATE STATE_0;

    

    namespace SLIDEBAR {
        enum STATE : __int32 {
            STATE_NORMAL_0 = 0x0,
            STATE_TOUCH = 0x1,
            STATE_DRAG = 0x2,
        };
    }

    

    struct COLOR {
        char data[16]; // Assuming the actual content is not necessary for now
    };

    

    struct WIDGET {
        const int* _vptr_;
        RECT Rect;
        RECT Frame;
        YW_POSITION Offset;
        CONTAINER* Parent;
        WIDGET* Next;
        FLAG Flag;
        RECT DrawRect;
        YW_COLOR_0* Color[2];
        void (*Finalizer)(WIDGET*);
    };

    struct CONTAINER : WIDGET {
        WIDGET* Child;
        CONTAINER* Delegate;
        YW_POSITION IconPos;
    };

    enum SCROLLBAR_STATE : int32_t {
        STATE_NORMAL = 0,
        STATE_TOUCH = 1,
        STATE_DRAG = 2
    };

    struct SCROLLBAR : WIDGET {
        float Min;
        float Max;
        float Value;
        float Width;
        float Step;
        RECT Slide;
        SCROLLBAR_STATE State;
        YW_POSITION DragPos;
    };

    struct H_SCROLLBAR : SCROLLBAR {
        // Additional H_SCROLLBAR-specific members (if any)
    };

    struct V_SCROLLBAR : SCROLLBAR {
        // Additional V_SCROLLBAR-specific members (if any)
    };

    struct FRAME : CONTAINER {
        H_SCROLLBAR* HBar;
        V_SCROLLBAR* VBar;
        float Step;
    };

   

    struct WINDOW : CONTAINER {
        const WINDOW_OPTION Option;
        STATE_0 State;
        YW_POSITION DragPos;
        int MinW;
        int MinH;
        int MaxW;
        int MaxH;
        char Title[32];
        int TitleWidth;
        RECT BeforeIcon;
    };


    struct YW_WINDOW_39 : CONTAINER {
        const WINDOW_OPTION Option;
        STATE_0 State;
        YW_POSITION DragPos;
        int MinW;
        int MinH;
        int MaxW;
        int MaxH;
        char Title[32];
        int TitleWidth;
        RECT BeforeIcon;
    };

    typedef __int8 WIDGET_5[84];

    typedef __int8 CONTAINER_5[100];

    typedef __int8 STATUS_0[96];

    typedef __int8 TOGGLE_BUTTON_0[108];

    typedef __int8 LABEL_3[96];

    typedef __int8 DIAL_3[144];

    typedef __int8 BUTTON_3[108];

    struct LABEL : YW::WIDGET
    {
        const char* Label;
        int LabelWidth;
        int LabelHeight;

        void SetText(const char* newText);
    };


    struct ELEMDATA_0
    {
        int Version;
        int Num;
        __int8 Elem[96];
    };

    struct MEMORY {
        char Buff[32768];
    };

    struct OBJENTRY_DATA : ELEMDATA_0
    {
    };

    struct YS_TABLE
    {
        const OBJENTRY_DATA* Data[3];
    };

    struct ENTRY_SELECTION : YW::CONTAINER
    {
        OBJENTRY_DATA* EntryData;
        u_int Type;
        int Page;
        FRAME* FrameWidget;
        const char* List[1024];
        const OBJENTRY_0* Entry[1024];
        void (*Callback)(OBJENTRY_ID);
        WINDOW* Window;
    };

    typedef __int8 YW_CONTAINER_44[100];


    struct CONFIG_WIDGET
    {
        YW_CONTAINER_44 baseclass_0;
    };

    struct CONFIG_WIDGET_2
    {
        YW_CONTAINER_44 baseclass_0;
    };

    struct TEST_BATTLEREPORT
    {
        YW_POSITION Pos;
    };

    typedef __int8 STATUS[96];

    struct TEXT : YW::YW_WIDGET_0
    {
        char* Text;
        char* Pt;
        size_t Len;
    };

    struct LIST_BUTTON {
        int Current;
        const char** List;
        int LabelWidth;
        int LabelHeight;
        WINDOW* ListWindow;
        int ScrollSize;
        void (*PressCallback)(int, unsigned int*);
        unsigned int* CallbackArg;
    };

    struct BUTTON : YW::YW_WIDGET_0
    {
        char* Label;
        int LabelWidth;
        int LabelHeight;
        int SinkTime;
        void (*PressCallback)(unsigned int*);
        unsigned int* PressCallbackArg;
    };


    struct  CHAT : WIDGET
    {
        char* Text;
        char* Pt;
        size_t Len;
    };


}
