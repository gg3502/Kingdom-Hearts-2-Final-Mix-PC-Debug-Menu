#pragma once
#include <string>
#include <map>
#include <functional>
#include <mutex>

class CommandRegistry {
public:
    using CommandHandler = std::function<void(const std::string& args)>;

    static CommandRegistry& instance();

    void registerCommand(const std::string& name, const std::string& description, CommandHandler handler);
    bool execute(const std::string& input);
    bool hasCommands() const;
    std::map<std::string, std::string> listCommands() const;

private:
    CommandRegistry() = default;
    CommandRegistry(const CommandRegistry&) = delete;
    CommandRegistry& operator=(const CommandRegistry&) = delete;

    mutable std::mutex m_mutex;
    std::map<std::string, std::pair<std::string, CommandHandler>> m_commands;
};
