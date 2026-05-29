#include "TwitchChat.hpp"
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iostream>
#include <sstream>
#include <vector>
#include <algorithm>
#include "VoteManager.hpp"  // Assuming you have this
#include <helper.hpp>
#include <patterns.hpp>
#include <function_typedefs.hpp>
#include <KHSCII.hpp>

#pragma comment(lib, "ws2_32.lib")

//extern VoteManager voteManager;
//extern std::vector<VoteOption> currentVoteOptions;



std::atomic<bool> twitchThreadRunning = false;
std::thread twitchThread;


int ParseVoteStringToIndex(const std::string& voteStr) {
    try {
        int idx = std::stoi(voteStr);
        return idx - 1; // 1-based input, 0-based index
    }
    catch (...) {}

    const auto& options = voteManager.GetCurrentOptions();

    for (int i = 0; i < static_cast<int>(options.size()); i++) {
        std::string optLower = options[i];
        std::transform(optLower.begin(), optLower.end(), optLower.begin(), ::tolower);

        std::string voteLower = voteStr;
        std::transform(voteLower.begin(), voteLower.end(), voteLower.begin(), ::tolower);

        if (optLower.find(voteLower) != std::string::npos)
            return i;
    }

    return -1;
}


void TwitchChatThread(const std::string& oauthToken, const std::string& username, const std::string& channel) {
    // Helper: Convert std::string to std::wstring (UTF-8 aware)
    auto ToWide = [](const std::string& str) -> std::wstring {
        int size_needed = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, nullptr, 0);
        std::wstring wstr(size_needed, 0);
        MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, &wstr[0], size_needed);
        return wstr;
        };

    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        LOG(L"[Twitch] WSAStartup failed\n");
        return;
    }
    LOG(L"[Twitch] Winsock initialized\n");

    SOCKET sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sock == INVALID_SOCKET) {
        LOG(L"[Twitch] Socket creation failed\n");
        WSACleanup();
        return;
    }
    LOG(L"[Twitch] Socket created\n");

    addrinfo hints = {};
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;

    addrinfo* result = nullptr;
    int ret = getaddrinfo("irc.chat.twitch.tv", "6667", &hints, &result);
    if (ret != 0 || !result) {
        LOG(L"[Twitch] DNS resolution failed: %S\n", gai_strerrorA(ret));
        closesocket(sock);
        WSACleanup();
        return;
    }
    LOG(L"[Twitch] DNS resolved successfully\n");

    if (connect(sock, result->ai_addr, static_cast<int>(result->ai_addrlen)) == SOCKET_ERROR) {
        LOG(L"[Twitch] Connection failed\n");
        freeaddrinfo(result);
        closesocket(sock);
        WSACleanup();
        twitchConnected = false;
        return;
    }
    LOG(L"[Twitch] Connected to Twitch IRC server\n");
    twitchConnected = true;
    freeaddrinfo(result);

    auto SendCmd = [&](const std::string& cmd) {
        send(sock, cmd.c_str(), static_cast<int>(cmd.size()), 0);
        LOG(L"[Twitch] Sent command: %ls\n", ToWide(cmd).c_str());
        };

    SendCmd("PASS oauth:" + oauthToken + "\r\n");
    SendCmd("NICK " + username + "\r\n");
    SendCmd("JOIN #" + channel + "\r\n");
    LOG(L"[Twitch] Sent login and join commands\n");

    std::string partial;
    char recvbuf[512];

    while (twitchThreadRunning) {
        int recvlen = recv(sock, recvbuf, sizeof(recvbuf) - 1, 0);
        if (recvlen <= 0) {
            LOG(L"[Twitch] Connection closed or error on recv\n");
            break;
        }

        recvbuf[recvlen] = '\0';
        partial += std::string(recvbuf);

        size_t pos;
        while ((pos = partial.find("\r\n")) != std::string::npos) {
            std::string line = partial.substr(0, pos);
            partial.erase(0, pos + 2);

            LOG(L"[Twitch] Received: %ls\n", ToWide(line).c_str());

            // Respond to server PINGs
            if (line.rfind("PING", 0) == 0) {
                SendCmd("PONG :tmi.twitch.tv\r\n");
                LOG(L"[Twitch] Responded to PING with PONG\n");
                continue;
            }

            // Extract message
            size_t msgPos = line.find("PRIVMSG");
            if (msgPos != std::string::npos) {
                size_t msgStart = line.find(" :", msgPos);
                if (msgStart != std::string::npos) {
                    std::string msg = line.substr(msgStart + 2);

                    // Extract username
                    size_t excl = line.find('!');
                    std::string user = (excl != std::string::npos) ? line.substr(1, excl - 1) : "unknown";

                    // Check for !vote command
                    if (msg.size() > 6 && msg.substr(0, 6) == "!vote ") {
                        std::string voteStr = msg.substr(6);
                        int idx = ParseVoteStringToIndex(voteStr);

                        if (voteManager.IsVotingActive()) {
                            if (idx >= 0 && idx < static_cast<int>(voteManager.GetCurrentOptions().size())) {
                                voteManager.CastVote(idx);
                                LOG(L"[TwitchVote] %ls voted for option %d (%ls)\n",
                                    ToWide(user).c_str(), idx + 1, ToWide(voteManager.GetCurrentOptions()[idx]).c_str());
                            }
                            else {
                                LOG(L"[TwitchVote] Invalid vote by %ls: %ls\n",
                                    ToWide(user).c_str(), ToWide(voteStr).c_str());
                            }
                        }
                    }
                }
            }
        }
    }

    closesocket(sock);
    WSACleanup();
    LOG(L"[Twitch] TwitchChatThread exiting, cleaned up sockets and Winsock\n");
}




void StartTwitchChatThread(const std::string& oauthToken, const std::string& username, const std::string& channel) {
    if (twitchThreadRunning) return;
    twitchThreadRunning = true;
    twitchThread = std::thread(TwitchChatThread, oauthToken, username, channel);
}

void StopTwitchChatThread() {
    twitchThreadRunning = false;
    if (twitchThread.joinable())
        twitchThread.join();
}
