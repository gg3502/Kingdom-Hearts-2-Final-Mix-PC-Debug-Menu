#pragma once
#include "YW.hpp"

struct MenuUI {
    YW::RECT mainMenu;
    YW::RECT optionsMenu;
    YW::RECT musicPlayerMenu;
    YW::RECT bdxMenu;

    bool isMainMenuOpen = false;
    bool isOptionsMenuOpen = false;
    bool isMusicPlayerOpen = false;
    bool isBdxMenuOpen = false;

    YW::WIDGET* bdxMenuPtr = nullptr;
};

extern MenuUI g_MenuUI;
