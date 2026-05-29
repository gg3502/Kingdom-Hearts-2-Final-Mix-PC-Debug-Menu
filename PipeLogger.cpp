#include "PipeLogger.hpp"
#include <windows.h>
#include <thread>
#include <mutex>
#include <queue>
#include <atomic>
#include <chrono>
#include <sstream>
#include <CommandRegistry.hpp>

namespace {
    const char* PIPE_NAME = R"(\\.\pipe\MyUniversalConsolePipe)";

    HANDLE g_pipe = INVALID_HANDLE_VALUE;
    std::mutex g_mutex;
    std::queue<std::string> g_queue;
    std::atomic<bool> g_running{ true };
    std::thread g_worker;

    void WorkerThread()
    {
        while (g_running) {
            if (g_pipe == INVALID_HANDLE_VALUE) {
                g_pipe = CreateFileA(
                    PIPE_NAME,
                    GENERIC_WRITE,
                    0,
                    nullptr,
                    OPEN_EXISTING,
                    0,
                    nullptr
                );

                if (g_pipe == INVALID_HANDLE_VALUE) {
                    std::this_thread::sleep_for(std::chrono::milliseconds(500));
                    continue;
                }
            }

            {
                std::lock_guard<std::mutex> lock(g_mutex);
                while (!g_queue.empty()) {
                    const std::string& msg = g_queue.front();
                    DWORD written = 0;
                    BOOL success = WriteFile(g_pipe, msg.c_str(),
                        (DWORD)msg.size(), &written, nullptr);
                    if (!success) {
                        CloseHandle(g_pipe);
                        g_pipe = INVALID_HANDLE_VALUE;
                        break;
                    }
                    g_queue.pop();
                }
            }

            std::this_thread::sleep_for(std::chrono::milliseconds(50));
        }
    }

    struct WorkerInit {
        WorkerInit() { g_worker = std::thread(WorkerThread); }
        ~WorkerInit() {
            g_running = false;
            if (g_worker.joinable()) g_worker.join();
            if (g_pipe != INVALID_HANDLE_VALUE) CloseHandle(g_pipe);
        }
    } g_init;
}

void SendLogToPipe(const std::string& channel, const std::string& message)
{
    std::ostringstream oss;
    oss << "#TAB#" << channel << "\n";
    oss << "[" << channel << "] " << message << "\n";

    std::lock_guard<std::mutex> lock(g_mutex);
    g_queue.push(oss.str());
}

void SendCommandListToPipe() {
    for (auto& [name, desc] : CommandRegistry::instance().listCommands()) {
        // Format: #COMMAND#<name>:<desc>
        SendLogToPipe("COMMANDS", "#COMMAND#" + name + ":" + desc);
    }
}



