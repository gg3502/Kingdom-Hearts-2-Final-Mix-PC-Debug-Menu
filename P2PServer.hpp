#pragma once

namespace P2P {

extern void StartServer();
extern void StartClient(const char* serverIp);
extern std::string GetLocalIPAddress();
extern void StartUDPDiscoveryServer(const char* publicIp, int port);
extern void StartUDPClient(const char* serverIp, int serverPort);
}