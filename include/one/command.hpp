#ifndef COMMAND_HPP
#define COMMAND_HPP
#include <queue>        // std::queue
#include <string>       // std::string

enum class CommandType : uint8_t {
    Unknown,    // Default for uninitialized command
    Quit,       // Terminates the application
    Get,        // Show relations for the current subject
    Add,        // Add a new relation to the current subject
    Set,        // Updates the current subject
    Jmp         // Set the context to a new subject
};

struct Command {
    Command() : type(CommandType::Unknown) {}

    Command(Command const&) = delete;
    Command& operator=(Command const&) = delete;

    Command(Command&&) = delete;
    Command& operator=(Command&&) = delete;

    ~Command() {}

    CommandType type;
    std::vector<std::string> arguments;
};

void tokenizeCommand(const std::string& rawCommand, std::queue<std::string>& tokens);
bool parseCommand(const std::string& rawCommand, Command& command);

#endif