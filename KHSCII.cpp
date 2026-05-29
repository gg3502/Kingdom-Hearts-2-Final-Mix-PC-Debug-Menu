#include "KHSCII.hpp"
#include <unordered_map>
#include <regex>

std::vector<uint8_t> KHSCII::ToKHSCII(const std::string& inText) {
    static const std::unordered_map<char, uint8_t> specialDict = {
        { ' ', 0x01 }, { '\n', 0x02 }, { '-', 0x54 }, { '!', 0x48 },
        { '?', 0x49 }, { '%', 0x4A }, { '/', 0x4B }, { '.', 0x4F },
        { ',', 0x50 }, { ';', 0x51 }, { ':', 0x52 }, { '\'', 0x57 },
        { '(', 0x5A }, { ')', 0x5B }, { '[', 0x62 }, { ']', 0x63 },
        { 'à', 0xB7 }, { 'á', 0xB8 }, { 'â', 0xB9 }, { 'ä', 0xBA },
        { 'è', 0xBB }, { 'é', 0xBC }, { 'ê', 0xBD }, { 'ë', 0xBE },
        { 'ì', 0xBF }, { 'í', 0xC0 }, { 'î', 0xC1 }, { 'ï', 0xC2 },
        { 'ñ', 0xC3 }, { 'ò', 0xC4 }, { 'ó', 0xC5 }, { 'ô', 0xC6 },
        { 'ö', 0xC7 }, { 'ù', 0xC8 }, { 'ú', 0xC9 }, { 'û', 0xCA },
        { 'ü', 0xCB }, { 'ç', 0xE8 }, { 'À', 0xD0 }, { 'Á', 0xD1 },
        { 'Â', 0xD2 }, { 'Ä', 0xD3 }, { 'È', 0xD4 }, { 'É', 0xD5 },
        { 'Ê', 0xD6 }, { 'Ë', 0xD7 }, { 'Ì', 0xD8 }, { 'Í', 0xD9 },
        { 'Î', 0xDA }, { 'Ï', 0xDB }, { 'Ñ', 0xDC }, { 'Ò', 0xDD },
        { 'Ó', 0xDE }, { 'Ô', 0xDF }, { 'Ö', 0xE0 }, { 'Ù', 0xE1 },
        { 'Ú', 0xE2 }, { 'Û', 0xE3 }, { 'Ü', 0xE4 }, { '¡', 0xE5 },
        { '¿', 0xE6 }, { 'Ç', 0xE7 }
    };

    std::vector<uint8_t> out;
    size_t i = 0;

    while (i < inText.length()) {
        char c = inText[i];

        if (c >= 'a' && c <= 'z') {
            out.push_back(static_cast<uint8_t>(c + 0x39));
            i++;
        }
        else if (c >= 'A' && c <= 'Z') {
            out.push_back(static_cast<uint8_t>(c - 0x13));
            i++;
        }
        else if (c >= '0' && c <= '9') {
            out.push_back(static_cast<uint8_t>(c + 0x60));
            i++;
        }
        else if (c == '{' && i + 5 < inText.length()) {
            std::string sub = inText.substr(i, 6);
            if (std::regex_match(sub, std::regex("\\{0x[0-9a-fA-F]{2}\\}"))) {
                std::string hexByte = sub.substr(3, 2);
                out.push_back(static_cast<uint8_t>(std::stoi(hexByte, nullptr, 16)));
                i += 6;
                continue;
            }
        }

        else {
            auto it = specialDict.find(c);
            if (it != specialDict.end())
                out.push_back(it->second);
            else
                out.push_back(0x01); // fallback
            i++;
        }
    }

    out.push_back(0x00); // null terminator
    return out;
}
