#pragma once

#include "YW.hpp"


namespace pa {

    enum GameModeType {
        MODE_FIELD = 0,
        MODE_GUMI_BATTLE = 1,
        MODE_GUMI_MENU = 2
    };

    struct PA_EDITER_WINDOW : YW::WINDOW {
    };

    // Global pointer like the original codebase likely has
    extern PA_EDITER_WINDOW* PaEditerWindow = nullptr;

    //extern void* GameMode;
    extern int EditerMode;
    

    
    PA_EDITER_WINDOW* CreatePAEditerWindow(int x, int y, int mode);

}