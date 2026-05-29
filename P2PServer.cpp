#include <winsock2.h>
#include <windows.h>
#include <iostream>
#include <ws2tcpip.h> // For InetPton
#include "helper.hpp"
#include <codecvt>
#include "AREA.hpp"
#include "function_typedefs.hpp"
#include "patterns.hpp"
#include <condition_variable>
#include <mutex>
#include <string>
#include "kh2-dmenu.hpp"
#ifdef str
#undef str
#endif

//#include "kh2-dmenu.cpp"

#pragma comment(lib, "ws2_32.lib")



namespace P2P {


    static int messageCount = 0;
    std::mutex mtx;
    std::condition_variable cv;
    bool serverReady = false;

    void CleanupSocket(SOCKET socket) {
        if (socket != INVALID_SOCKET) {
            closesocket(socket);
        }
        WSACleanup();
    }

    std::wstring StringToWstring(const std::string& str) {
        int size_needed = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), static_cast<int>(str.size()), NULL, 0);
        std::wstring wstr(size_needed, 0);
        MultiByteToWideChar(CP_UTF8, 0, str.c_str(), static_cast<int>(str.size()), &wstr[0], size_needed);
        return wstr;
    }

    std::string WStringToString(const std::wstring& wstr) {
        return std::string(wstr.begin(), wstr.end());

    }

    char GetCharacterFromKeyPress() {
        // Iterate through all possible keys
        for (int key = 32; key < 127; ++key) { // Printable ASCII range
            if (GetAsyncKeyState(key) & 0x8000) { // Check if the key is currently pressed
                // Return the character corresponding to the key
                return static_cast<char>(key);
            }
        }
        return '\0'; // No key pressed
    }

    bool IsKeyPressed(int key) {
        return (GetAsyncKeyState(key) & 0x8000) != 0;
    }

    void StartServer() {
        WSADATA wsaData;
        SOCKET listenSocket = INVALID_SOCKET;
        SOCKET clientSocket = INVALID_SOCKET;
        struct sockaddr_in serverAddress;
        int iResult;

        // Initialize Winsock
        if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
            LOG(L"WSAStartup failed: %u\n", WSAGetLastError());
            return;
        }

        // Create a socket
        listenSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        if (listenSocket == INVALID_SOCKET) {
            LOG(L"Error creating socket: %u\n", WSAGetLastError());
            WSACleanup();
            return;
        }

        serverAddress.sin_family = AF_INET;
        serverAddress.sin_addr.s_addr = INADDR_ANY;
        serverAddress.sin_port = htons(139);  // Example port

        // Bind the socket
        if (bind(listenSocket, (SOCKADDR*)&serverAddress, sizeof(serverAddress)) == SOCKET_ERROR) {
            int error = WSAGetLastError();
            LOG(L"Bind failed with error %d. Port might be in use or blocked.\n", error);
            if (error == WSAEADDRINUSE) {
                // Suggest trying a different port or address
                LOG(L"Try using a different port.\n");
            }
            CleanupSocket(listenSocket);
            WSACleanup();
            return;
        }

        // Start listening
        iResult = listen(listenSocket, SOMAXCONN);
        if (iResult == SOCKET_ERROR) {
            LOG(L"Listen failed: %u\n", WSAGetLastError());
            CleanupSocket(listenSocket);
            WSACleanup();
            return;
        }

        LOG(L"Listening on port: %u\n", ntohs(serverAddress.sin_port));

        // Accept a client socket
        clientSocket = accept(listenSocket, NULL, NULL);
        if (clientSocket == INVALID_SOCKET) {
            LOG(L"Accept failed: %u", WSAGetLastError());
            CleanupSocket(listenSocket);
            WSACleanup();
            return;
        }

        LOG(L"Client connected!\n");

        // Here you would read from and write to the client socket.

        CleanupSocket(clientSocket);
        CleanupSocket(listenSocket);
        WSACleanup();
    }

    /*void StartUDPDiscoveryServer(const char* publicIp, int port) {
        WSADATA wsaData;
        SOCKET udpSocket = INVALID_SOCKET;
        sockaddr_in serverAddress;
        sockaddr_in clientAddress;
        char buffer[1024];
        int bufferLen = sizeof(buffer);
        int clientAddrSize = sizeof(clientAddress);

        // Initialize Winsock
        if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
            LOG(L"WSAStartup failed: %u\n", WSAGetLastError());
            return;
        }

        // Create UDP socket
        udpSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
        if (udpSocket == INVALID_SOCKET) {
            LOG(L"Error creating socket: %u\n", WSAGetLastError());
            WSACleanup();
            return;
        }

        // Server address setup
        serverAddress.sin_family = AF_INET;
        serverAddress.sin_port = htons(port);
        serverAddress.sin_addr.s_addr = INADDR_ANY; // Listen on all interfaces (not the public IP)

        // Bind the socket to the server address
        if (bind(udpSocket, (SOCKADDR*)&serverAddress, sizeof(serverAddress)) == SOCKET_ERROR) {
            LOG(L"Bind failed: %u\n", WSAGetLastError());
            closesocket(udpSocket);
            WSACleanup();
            return;
        }

        // Log the public IP for reference (if it needs to be advertised or discovered by clients)
        LOG(L"UDP Server listening on public IP: %s and port: %d\n", StringToWstring(std::string(publicIp)).c_str(), port);

        // Main loop to receive messages
        while (true) {
            int recvLen = recvfrom(udpSocket, buffer, bufferLen - 1, 0, (SOCKADDR*)&clientAddress, &clientAddrSize);
            if (recvLen == SOCKET_ERROR) {
                LOG(L"Receive failed: %u\n", WSAGetLastError());
                break;
            }

            // Null-terminate the received data
            buffer[recvLen] = '\0';

            // Convert buffer to std::string for logging
            std::string receivedData(buffer);

            // Convert client IP address to string
            char clientIp[INET_ADDRSTRLEN];
            if (inet_ntop(AF_INET, &clientAddress.sin_addr, clientIp, sizeof(clientIp)) == NULL) {
                LOG(L"inet_ntop failed: %u\n", WSAGetLastError());
                continue;
            }

            // Log received data from client
            LOG(L"Received message from %s:%d - %s\n",
                StringToWstring(clientIp).c_str(),
                ntohs(clientAddress.sin_port),
                StringToWstring(receivedData).c_str());

            // Process the request and prepare a response (you can modify this part as needed)
            if (receivedData == "Client1: Send my public info") {
                // Send back the client's public info
                std::string clientInfo = std::string(clientIp) + ":" + std::to_string(ntohs(clientAddress.sin_port));
                if (sendto(udpSocket, clientInfo.c_str(), clientInfo.size(), 0, (SOCKADDR*)&clientAddress, clientAddrSize) == SOCKET_ERROR) {
                    LOG(L"Send failed: %u\n", WSAGetLastError());
                    break;
                }
            }
            else if (receivedData == "RequestAreaInfo") {
                // Example function to get area information
                AREA areaPointer;
                ReadSteamAreaPointer(areaPointer);

                // Send area info back to the client
                std::string areaInfo = "World: " + std::to_string(areaPointer.World) +
                    ", Area: " + std::to_string(areaPointer.Area) +
                    ", Entrance: " + std::to_string(areaPointer.Entrance) +
                    ", Map: " + std::to_string(areaPointer.Map) +
                    ", Battle: " + std::to_string(areaPointer.Battle) +
                    ", Event: " + std::to_string(areaPointer.Event);
                if (sendto(udpSocket, areaInfo.c_str(), areaInfo.size(), 0, (SOCKADDR*)&clientAddress, clientAddrSize) == SOCKET_ERROR) {
                    LOG(L"Send failed: %u\n", WSAGetLastError());
                    break;
                }

                // Map jump logic (if necessary)
               //uintptr_t MapJumpFunctionAddress = ReadLEA32(Patterns::pattern_for_MapJump, L"Map Jump", 0, 1, 5);
                //FunctionCalls::tMapJump MapJumpFunction = reinterpret_cast<FunctionCalls::tMapJump>(MapJumpFunctionAddress);
                //MapJumpFunction(areaPointer, 1, 0, 0, 0);
            }
        }

        // Clean up
        closesocket(udpSocket);
        WSACleanup();
    }*/
void StartUDPDiscoveryServer(const char* publicIpParam, int port) {
    InitializePointers();
    WSADATA wsaData;
    SOCKET udpSocket = INVALID_SOCKET;
    sockaddr_in serverAddress;
    sockaddr_in clientAddress;
    char buffer[1024];
    int bufferLen = sizeof(buffer);
    std::string inputMessage;
    std::string publicIpStr = GetPublicIP();  // Assume GetPublicIP returns a std::string
    const char* publicIp = publicIpStr.c_str();  // Convert to const char*
    int clientAddrSize = sizeof(clientAddress);

    LOG(L"Initializing Winsock...\n");

    // Initialize Winsock
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        LOG(L"WSAStartup failed: %u\n", WSAGetLastError());
        return;
    }

    LOG(L"Winsock initialized.\n");

    // Create UDP socket
    LOG(L"Creating UDP socket...\n");
    udpSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (udpSocket == INVALID_SOCKET) {
        LOG(L"Error creating socket: %u\n", WSAGetLastError());
        WSACleanup();
        return;
    }

    LOG(L"UDP socket created successfully.\n");

    // Server address setup
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(port);
    serverAddress.sin_addr.s_addr = INADDR_ANY;

    // Bind the socket to the server address
    LOG(L"Binding socket to port %d...\n", port);
    if (bind(udpSocket, (SOCKADDR*)&serverAddress, sizeof(serverAddress)) == SOCKET_ERROR) {
        LOG(L"Bind failed: %u\n", WSAGetLastError());
        closesocket(udpSocket);
        WSACleanup();
        return;
    }

    LOG(L"Socket successfully bound to port %d.\n", port);
    LOG(L"UDP Server listening on public IP: %s and port: %d\n", StringToWstring(std::string(publicIp)).c_str(), port);

    // Fetch function addresses for StatusPrintf and ClearStatusWindow
    LOG(L"Fetching StatusPrintf and ClearStatusWindow function addresses...\n");
    uintptr_t StatusPrintfFunctionAddress = FindAndPrintPatternW(Patterns::pattern_for_StatusPrintf, L"Add Debug Submenu", 0);
    FunctionCalls::tStatusPrintf StatusPrintfFunction = reinterpret_cast<FunctionCalls::tStatusPrintf>(StatusPrintfFunctionAddress);

    uintptr_t ClearStatusPrintfFunctionAddress = FindAndPrintPatternW(Patterns::pattern_for_ClearStatusWindow, L"Clear Status Window", 0);
    FunctionCalls::tClearStatusWindow ClearStatusPrintfFunction = reinterpret_cast<FunctionCalls::tClearStatusWindow>(ClearStatusPrintfFunctionAddress);

    //LOG(L"StatusPrintfFunctionAddress: %llx", StatusPrintfFunctionAddress);
   // LOG(L"ClearStatusPrintfFunctionAddress: %llx", ClearStatusPrintfFunctionAddress);

    uintptr_t YS_STATUS = 0;
    int* statusAddress = nullptr;

    // Determine game version
    LOG(L"Detecting game version...\n");
    if (DetectGameVersion() == "Steam") {
        LOG(L"Steam version detected.\n");
        statusAddress = reinterpret_cast<int*>(SteamPointer - 0x3C3D70);
        YS_STATUS = *(uintptr_t*)(SteamPointer + 0x1FEA3A8);
    }
    else if (DetectGameVersion() == "Epic") {
        LOG(L"Epic version detected.\n");
        statusAddress = reinterpret_cast<int*>(EpicPointer - 0x3C3D70);
        YS_STATUS = *(uintptr_t*)(EpicPointer + 0x1FEA3A8);
    }
    else {
        LOG(L"Unknown game version.\n");
        return;
    }

   // LOG(L"YS_STATUS address: %llx", YS_STATUS);
    //LOG(L"Status address: %llx", statusAddress);

    while (true) {
        // Listen for incoming messages from clients
        int recvLen = recvfrom(udpSocket, buffer, bufferLen - 1, 0, (SOCKADDR*)&clientAddress, &clientAddrSize);
        if (recvLen == SOCKET_ERROR) {
            LOG(L"recvfrom failed: %u\n", WSAGetLastError());
            continue;
        }

        buffer[recvLen] = '\0';  // Null-terminate the received message
        std::string receivedData(buffer);
        LOG(L"Received message from client: %s\n", StringToWstring(receivedData).c_str());

        // Check if the client is requesting the public IP
        if (receivedData == "REQUEST_IP") {
            LOG(L"Client requested public IP\n");

            // Send the public IP to the client
            int sentBytes = sendto(udpSocket, publicIp, strlen(publicIp), 0, (SOCKADDR*)&clientAddress, clientAddrSize);
            if (sentBytes == SOCKET_ERROR) {
                LOG(L"Send public IP failed: %u\n", WSAGetLastError());
            }
            else {
                LOG(L"Public IP sent to client: %s\n", StringToWstring(publicIpStr).c_str());
            }
        }

        // Check if the status window is visible
        bool statusWindowVisible = (*statusAddress == 1);  // 1 means visible

        if (statusWindowVisible) {
            LOG(L"Status window is visible.\n");

            // Get input from the console if the status window is visible
            std::cout << "Host Enter message: ";
            std::getline(std::cin, inputMessage);

            // Check for /clear command
            if (inputMessage == "/clear") {
                LOG(L"Received /clear command. Clearing status window.\n");
                ClearStatusPrintfFunction((YW::STATUS_0*)YS_STATUS);
                messageCount = 0;
                continue;
            }

            // Check if the input is valid
            if (!inputMessage.empty()) {
                std::string messageToSend = "Host: " + inputMessage;

                // Send the message to the client
                LOG(L"Sending message to client: %s\n", StringToWstring(inputMessage).c_str());
                int sentBytes = sendto(udpSocket, messageToSend.c_str(), messageToSend.size(), 0,
                    (SOCKADDR*)&clientAddress, sizeof(clientAddress));

                if (sentBytes == SOCKET_ERROR) {
                    LOG(L"Send failed: %u\n", WSAGetLastError());
                }
                else {
                    LOG(L"Message sent to client: %s\n", StringToWstring(inputMessage).c_str());

                    // Display the message in the status window
                    StatusPrintfFunction((messageToSend + "\n").c_str());
                }
            }
        }
        else {
            LOG(L"Status window is not visible.\n");
        }

        // Increment the message count and check if we need to clear the window
        messageCount++;
        if (messageCount >= 44) {
            LOG(L"Message count reached 44, clearing status window.\n");
            ClearStatusPrintfFunction((YW::STATUS_0*)YS_STATUS);
            messageCount = 0;
        }
    }

    // Clean up
    LOG(L"Cleaning up and closing socket.\n");
    closesocket(udpSocket);
    WSACleanup();
}

    void StartClient(const char* serverIp) {
        WSADATA wsaData;
        SOCKET connectSocket = INVALID_SOCKET;
        struct sockaddr_in serverAddress;
        int iResult;

        // Initialize Winsock
        if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
            LOG(L"WSAStartup failed: %u", WSAGetLastError());
            return;
        }

        // Create a socket
        connectSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        if (connectSocket == INVALID_SOCKET) {
            LOG(L"Error creating socket: %u", WSAGetLastError());
            WSACleanup();
            return;
        }

        serverAddress.sin_family = AF_INET;
        serverAddress.sin_port = htons(12345);

        // Convert the IP address from string to wide string
        std::wstring wideServerIp(serverIp, serverIp + strlen(serverIp)); // Convert to wide string

        // Convert the IP address from string to binary form
        if (InetPtonW(AF_INET, wideServerIp.c_str(), &serverAddress.sin_addr) <= 0) {
            LOG(L"Invalid address/ Address not supported");
            closesocket(connectSocket);
            WSACleanup();
            return;
        }

        // Connect to the server
        iResult = connect(connectSocket, (SOCKADDR*)&serverAddress, sizeof(serverAddress));
        if (iResult == SOCKET_ERROR) {
            LOG(L"Connect failed: %u", WSAGetLastError());
            closesocket(connectSocket);
            WSACleanup();
            return;
        }

        LOG(L"Connected to server at %s", serverIp);

        // Here you would communicate with the server.
        // For example, you might send data or read from the server.

        uintptr_t LocalsetGetAreaFunctionAddress = ReadLEA32(Patterns::pattern_for_LocalSetGetArea, L"Local Set Get AREA", 0, 1, 5);
        FunctionCalls::tLocalSetGetArea LocalsetGetAreaFunction = reinterpret_cast<FunctionCalls::tLocalSetGetArea>(LocalsetGetAreaFunctionAddress);

        AREA CurrentArea;
        CurrentArea.Area = -1;
        CurrentArea.Battle = -1;
        CurrentArea.Entrance = -1;
        CurrentArea.Event = -1;
        CurrentArea.Map = -1;
        CurrentArea.World = -1;

        LocalsetGetAreaFunction(CurrentArea, -1, -1);

        LOG("Client's CurrentArea: World = %d, Area = %d, Battle = %d, Entrance = %d, Event = %d, Map = %d",
            CurrentArea.World, CurrentArea.Area, CurrentArea.Battle, CurrentArea.Entrance, CurrentArea.Event, CurrentArea.Map);

        // Send area data to the server
        send(connectSocket, (char*)&CurrentArea, sizeof(CurrentArea), 0);

        LOG(L"Sent CurrentArea data to the server.\n");
        // Cleanup
        closesocket(connectSocket);
        WSACleanup();
    }

    void SendPublicInfoToServer(SOCKET udpSocket, sockaddr_in& serverAddress) {
        const char* initMessage = "Client1: Send my public info\n";
        int result = sendto(udpSocket, initMessage, strlen(initMessage), 0, (sockaddr*)&serverAddress, sizeof(serverAddress));
        if (result == SOCKET_ERROR) {
            LOG(L"Error sending public info to server: %u\n", WSAGetLastError());
        }
    }

    std::pair<std::string, int> ReceivePeerInfoFromServer(SOCKET udpSocket, sockaddr_in& serverAddress) {
        char buffer[1024];
        sockaddr_in fromAddress;
        int fromAddrSize = sizeof(fromAddress);

        int result = recvfrom(udpSocket, buffer, sizeof(buffer), 0, (sockaddr*)&fromAddress, &fromAddrSize);
        if (result == SOCKET_ERROR) {
            LOG(L"Error receiving peer info from server: %u\n", WSAGetLastError());
            return { "", 0 }; // Return empty info on error
        }

        // Assume the server sends peer info in the format "peerIp:peerPort"
        std::string peerInfo(buffer, result);
        size_t colonPos = peerInfo.find(':');
        if (colonPos == std::string::npos) {
            LOG(L"Invalid peer info format received from server\n");
            return { "", 0 };
        }

        std::string peerIp = peerInfo.substr(0, colonPos);
        int peerPort = std::stoi(peerInfo.substr(colonPos + 1));
        return { peerIp, peerPort };
    }

    void StartHolePunching(SOCKET udpSocket, sockaddr_in& peerAddress) {
        const char* punchMessage = "Punch through\n";
        for (int i = 0; i < 5; ++i) {
            int result = sendto(udpSocket, punchMessage, strlen(punchMessage), 0, (sockaddr*)&peerAddress, sizeof(peerAddress));
            if (result == SOCKET_ERROR) {
                LOG(L"Error sending punch message: %u\n", WSAGetLastError());
            }
            Sleep(500);
        }
    }

    void SendAreaInfoToServer(SOCKET connectSocket, const AREA& area) {
        // Convert area info to a char array
        char buffer[sizeof(AREA)];
        memcpy(buffer, &area, sizeof(AREA));

        // Send area info to the server
        int result = send(connectSocket, buffer, sizeof(AREA), 0);
        if (result == SOCKET_ERROR) {
            LOG(L"Failed to send area info to server. Error: %d", WSAGetLastError());
        }
        else {
            LOG(L"Sent area info to server successfully.");
        }
    }

    /*void StartUDPClient(const char* serverIp, int serverPort) {
        WSADATA wsaData;
        SOCKET udpSocket = INVALID_SOCKET;
        sockaddr_in serverAddress;
        sockaddr_in serverResponseAddress;
        char buffer[1024];
        int bufferLen = sizeof(buffer);

        // Initialize Winsock
        if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
            std::cerr << "WSAStartup failed: " << WSAGetLastError() << std::endl;
            return;
        }

        // Create UDP socket
        udpSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
        if (udpSocket == INVALID_SOCKET) {
            std::cerr << "Error creating socket: " << WSAGetLastError() << std::endl;
            WSACleanup();
            return;
        }

        // Server address setup
        serverAddress.sin_family = AF_INET;
        serverAddress.sin_port = htons(serverPort);
        if (inet_pton(AF_INET, serverIp, &serverAddress.sin_addr) <= 0) {
            std::cerr << "Invalid server IP address." << std::endl;
            closesocket(udpSocket);
            WSACleanup();
            return;
        }

        // Form the request message
        std::string requestMessage = "RequestAreaInfo";
        if (sendto(udpSocket, requestMessage.c_str(), requestMessage.length(), 0, (SOCKADDR*)&serverAddress, sizeof(serverAddress)) == SOCKET_ERROR) {
            std::cerr << "Send failed: " << WSAGetLastError() << std::endl;
            closesocket(udpSocket);
            WSACleanup();
            return;
        }

        // Receive area information from the server
        int serverAddrSize = sizeof(serverResponseAddress);
        int recvLen = recvfrom(udpSocket, buffer, bufferLen - 1, 0, (SOCKADDR*)&serverResponseAddress, &serverAddrSize);
        if (recvLen == SOCKET_ERROR) {
            std::cerr << "Receive failed: " << WSAGetLastError() << std::endl;
            closesocket(udpSocket);
            WSACleanup();
            return;
        }

        // Null-terminate the received data
        buffer[recvLen] = '\0';

        // Convert buffer to std::string for logging
        std::string receivedData(buffer);
        std::cout << "Received area info from server: " << receivedData << std::endl;

        // Parse received data into AREA structure
        AREA area;
        std::istringstream iss(receivedData);
        std::string token;

        auto parseToken = [&]() {
            std::getline(iss, token, ',');
            return std::stoi(token.substr(token.find(':') + 1));
            };

        try {
            area.World = parseToken();
            area.Area = parseToken();
            area.Entrance = parseToken();
            area.Map = parseToken();
            area.Battle = parseToken();
            area.Event = parseToken();
        }
        catch (const std::exception& e) {
            std::cerr << "Error parsing area info: " << e.what() << std::endl;
            closesocket(udpSocket);
            WSACleanup();
            return;
        }

        // Now you can use the area object
        std::cout << "Parsed area info: World: " << area.World
            << ", Area: " << area.Area
            << ", Entrance: " << area.Entrance
            << ", Map: " << area.Map
            << ", Battle: " << area.Battle
            << ", Event: " << area.Event << std::endl;

        // Call the MapJump function
        //uintptr_t MapJumpFunctionAddress = ReadLEA32(Patterns::pattern_for_MapJump, L"Map Jump", 0, 1, 5);
        //FunctionCalls::tMapJump MapJumpFunction = reinterpret_cast<FunctionCalls::tMapJump>(MapJumpFunctionAddress);
        //MapJumpFunction(area, 1, 0, 0, 0);

        // Clean up
        closesocket(udpSocket);
        WSACleanup();
    }*/
    


void StartUDPClient(const char* serverIp, int serverPort) {
    LOG(L"Initializing UDP client...\n");

    InitializePointers();
    WSADATA wsaData;
    SOCKET udpSocket = INVALID_SOCKET;
    sockaddr_in serverAddress;
    sockaddr_in hostAddress;
    char buffer[1024];
    int bufferLen = sizeof(buffer);
    std::string inputMessage;
    int serverAddrSize = sizeof(serverAddress);

    // Initialize Winsock
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        LOG(L"WSAStartup failed: %u\n", WSAGetLastError());
        return;
    }
    LOG(L"Winsock initialized successfully.\n");

    // Create UDP socket
    udpSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (udpSocket == INVALID_SOCKET) {
        LOG(L"Error creating socket: %u\n", WSAGetLastError());
        WSACleanup();
        return;
    }
    LOG(L"UDP socket created successfully.\n");

    // Server address setup
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(serverPort);
    if (inet_pton(AF_INET, serverIp, &serverAddress.sin_addr) <= 0) {
        LOG(L"Invalid server IP address.\n");
        closesocket(udpSocket);
        WSACleanup();
        return;
    }

    //LOG(L"Server IP and port set to: %s:%d\n", StringToWstring(std::string(serverIp)).c_str(), serverPort);

    // Send a request for the host's public IP
    const char* requestMessage = "REQUEST_IP";
    int sentBytes = sendto(udpSocket, requestMessage, strlen(requestMessage), 0, (SOCKADDR*)&serverAddress, sizeof(serverAddress));
    if (sentBytes == SOCKET_ERROR) {
        LOG(L"Failed to send IP request to host: %u\n", WSAGetLastError());
        closesocket(udpSocket);
        WSACleanup();
        return;
    }
    LOG(L"IP request sent to host.\n");

    // Wait for the response from the host
    int recvLen = recvfrom(udpSocket, buffer, bufferLen - 1, 0, (SOCKADDR*)&serverAddress, &serverAddrSize);
    if (recvLen == SOCKET_ERROR) {
        LOG(L"Failed to receive IP from host: %u\n", WSAGetLastError());
        closesocket(udpSocket);
        WSACleanup();
        return;
    }

    buffer[recvLen] = '\0';  // Null-terminate the received message
    std::string hostPublicIp(buffer);
    LOG(L"Received public IP from host\n");

    // Set the host's address for further communication
    hostAddress.sin_family = AF_INET;
    hostAddress.sin_port = htons(serverPort);
    if (inet_pton(AF_INET, hostPublicIp.c_str(), &hostAddress.sin_addr) <= 0) {
        LOG(L"Invalid public IP address received from host.\n");
        closesocket(udpSocket);
        WSACleanup();
        return;
    }

    //LOG(L"Host's public IP set to: %s\n", StringToWstring(hostPublicIp).c_str());

    // Fetch function addresses
    LOG(L"Fetching StatusPrintf and ClearStatusWindow function addresses...\n");
    uintptr_t StatusPrintfFunctionAddress = FindAndPrintPatternW(Patterns::pattern_for_StatusPrintf, L"Add Debug Submenu", 0);
    FunctionCalls::tStatusPrintf StatusPrintfFunction = reinterpret_cast<FunctionCalls::tStatusPrintf>(StatusPrintfFunctionAddress);

    uintptr_t ClearStatusPrintfFunctionAddress = FindAndPrintPatternW(Patterns::pattern_for_ClearStatusWindow, L"Clear Status Window", 0);
    FunctionCalls::tClearStatusWindow ClearStatusPrintfFunction = reinterpret_cast<FunctionCalls::tClearStatusWindow>(ClearStatusPrintfFunctionAddress);

    LOG(L"Function addresses fetched. StatusPrintf: 0x%llx, ClearStatusWindow: 0x%llx\n", StatusPrintfFunctionAddress, ClearStatusPrintfFunctionAddress);

    // Handle YS_STATUS based on game version
    uintptr_t YS_STATUS = 0;
    int* statusAddress = nullptr;

    LOG(L"Detecting game version...\n");
    if (DetectGameVersion() == "Steam") {
        statusAddress = reinterpret_cast<int*>(SteamPointer - 0x3C3D70);
        YS_STATUS = *(uintptr_t*)(SteamPointer + 0x1FEA3A8);
        LOG(L"Game version: Steam. YS_STATUS: 0x%llx\n", YS_STATUS);
    }
    else if (DetectGameVersion() == "Epic") {
        statusAddress = reinterpret_cast<int*>(EpicPointer - 0x3C3D70);
        YS_STATUS = *(uintptr_t*)(EpicPointer + 0x1FEA3A8);
        LOG(L"Game version: Epic. YS_STATUS: 0x%llx\n", YS_STATUS);
    }

    // Main client loop
    LOG(L"Starting main client loop...\n");
    while (true) {
        // Check if status window is visible
        bool statusWindowVisible = (*statusAddress == 1);

        if (statusWindowVisible) {
            std::cout << "Client Enter message: ";
            std::getline(std::cin, inputMessage);

            // Check for /clear command
            if (inputMessage == "/clear") {
                LOG(L"Clear command received, clearing status window.\n");
                ClearStatusPrintfFunction((YW::STATUS_0*)YS_STATUS);
                messageCount = 0;
                continue;
            }

            // Check if input is valid
            if (!inputMessage.empty()) {
                std::string messageToSend = "Client: " + inputMessage;

                // Send the message to the host
                int sentBytes = sendto(udpSocket, messageToSend.c_str(), messageToSend.size(), 0,
                    (SOCKADDR*)&hostAddress, sizeof(hostAddress));

                if (sentBytes == SOCKET_ERROR) {
                    LOG(L"Send failed: %u\n", WSAGetLastError());
                }
                else {
                    LOG(L"Message sent to host: %s\n", StringToWstring(inputMessage).c_str());
                }
            }
        }

        // Handling received messages
        int recvLen = recvfrom(udpSocket, buffer, bufferLen - 1, 0, (SOCKADDR*)&hostAddress, &serverAddrSize);
        if (recvLen != SOCKET_ERROR) {
            buffer[recvLen] = '\0';
            std::string receivedData(buffer);

            // Log the received message
            LOG(L"Received message from host: %s\n", StringToWstring(receivedData).c_str());

            // Display the message in the status window
            StatusPrintfFunction((receivedData + "\n").c_str());

            // Increment message count and clear if necessary
            messageCount++;
            if (messageCount >= 44) {
                LOG(L"Message count exceeded 44, clearing status window.\n");
                ClearStatusPrintfFunction((YW::STATUS_0*)YS_STATUS);
                messageCount = 0;
            }
        }
        else {
            LOG(L"Error receiving message: %u\n", WSAGetLastError());
        }
    }

    // Clean up
    LOG(L"Cleaning up UDP client...\n");
    closesocket(udpSocket);
    WSACleanup();
    LOG(L"UDP client closed.\n");
}
}