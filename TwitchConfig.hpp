#pragma once

#include "inipp.h"
#include <string>
#include <iostream>

struct TwitchConfig {
    std::wstring oauth_token;
    std::wstring username;
    std::wstring channel;

    bool LoadFromIni(const inipp::Ini<wchar_t>& ini);
};

extern TwitchConfig twitchConfig;
