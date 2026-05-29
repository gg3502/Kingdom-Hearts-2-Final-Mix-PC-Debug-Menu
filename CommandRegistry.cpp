#include "CommandRegistry.hpp"
#include <sstream>

CommandRegistry& CommandRegistry::instance() {
    static CommandRegistry inst;
    return inst;
}

void CommandRegistry::registerCommand(const std::string& name, const std::string& description, CommandHandler handler) {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_commands[name] = { description, handler };
}

bool CommandRegistry::execute(const std::string& input) {
    std::lock_guard<std::mutex> lock(m_mutex);

    std::istringstream iss(input);
    std::string cmd;
    iss >> cmd;

    auto it = m_commands.find(cmd);
    if (it != m_commands.end()) {
        std::string args;
        std::getline(iss, args);
        if (!args.empty() && args[0] == ' ')
            args.erase(0, 1);
        it->second.second(args); // call handler
        return true;
    }
    return false;
}

bool CommandRegistry::hasCommands() const {
    std::lock_guard<std::mutex> lock(m_mutex);
    return !m_commands.empty();
}

std::map<std::string, std::string> CommandRegistry::listCommands() const {
    std::lock_guard<std::mutex> lock(m_mutex);
    std::map<std::string, std::string> list;
    for (const auto& [name, pair] : m_commands) {
        list[name] = pair.first; // description
    }
    return list;
}
