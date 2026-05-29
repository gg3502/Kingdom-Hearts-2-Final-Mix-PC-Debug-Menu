#pragma once

#include <string>

void SendLogToPipe(const std::string& channel, const std::string& message);
void SendCommandListToPipe();

#define LOG_CHANNEL(channel, fmt, ...) \
    do { \
        char buffer[1024]; \
        snprintf(buffer, sizeof(buffer), fmt, __VA_ARGS__); \
        SendLogToPipe(channel, buffer); \
    } while(0)

