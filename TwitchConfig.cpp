#include "TwitchConfig.hpp"


bool TwitchConfig::LoadFromIni(const inipp::Ini<wchar_t>& ini) {
    if (!ini.sections.count(L"Twitch")) {
        std::wcerr << L"Config missing [Twitch] section\n";
        return false;
    }

    const auto& section = ini.sections.at(L"Twitch");
    inipp::get_value(section, L"oauth_token", oauth_token);
    inipp::get_value(section, L"username", username);
    inipp::get_value(section, L"channel", channel);

    if (oauth_token.empty() || username.empty() || channel.empty()) {
        std::wcerr << L"Missing Twitch values in config\n";
        return false;
    }

    return true;
}