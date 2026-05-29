#pragma once

#include "imgui.h"

#ifndef FONTS_H
#define FONTS_H

extern ImFont* g_ArialFont;
extern ImFont* g_CmFont;
extern ImFont* g_KHMenuFont;
extern ImFont* g_ComicHeartsFont;

// Function to load fonts
extern void LoadFonts(ImGuiIO& IO);

#endif // FONTS_H