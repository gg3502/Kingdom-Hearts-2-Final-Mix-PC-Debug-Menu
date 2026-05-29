#pragma once

#include <string>
#include <thread>
#include <atomic>

void StartTwitchChatThread(const std::string& oauthToken, const std::string& username, const std::string& channel);
void StopTwitchChatThread();

extern std::atomic<bool> twitchConnected;