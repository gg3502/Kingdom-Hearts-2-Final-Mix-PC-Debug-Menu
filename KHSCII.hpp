#pragma once
#include <vector>
#include <string>

class KHSCII {
public:
    static std::vector<uint8_t> ToKHSCII(const std::string& inText);
};
