#include <cstddef>      // std::size_t
#include <queue>        // std::queue
#include <sstream>      // std::stringstream
#include "../../include/one/command.hpp"

//
// Reads the raw command string and tries to interpret it as a command
//
bool parseCommand(const std::string& rawCommand, Command& command) {
    std::queue<std::string> commandTokens;
    tokenizeCommand(rawCommand, commandTokens);
    std::size_t tokenCount = commandTokens.size();

    if (tokenCount == 0) {
        return false;
    }

    if (tokenCount == 1) {
        std::string cmd = std::move(commandTokens.front());
        commandTokens.pop();

        if (cmd == "quit") {
            command.type = CommandType::Quit;
            return true;
        }

        if (cmd == "get") {
            command.type = CommandType::Get;
            return true;
        }
    }

    if (tokenCount == 2) {
        std::string cmd = std::move(commandTokens.front());
        commandTokens.pop();
        std::string arg1 = std::move(commandTokens.front());
        commandTokens.pop();

        if (cmd == "set") {
            // Set is of the format "set <subject>"
            command.type = CommandType::Set;
            command.arguments.push_back(arg1);
            return true;
        }
    }

    if (tokenCount == 3) {
        std::string cmd = std::move(commandTokens.front());
        commandTokens.pop();
        std::string arg1 = std::move(commandTokens.front());
        commandTokens.pop();
        std::string arg2 = std::move(commandTokens.front());
        commandTokens.pop();

        if (cmd == "add") {
            // Add is of the format "add <predicate> <object>"
            command.type = CommandType::Add;
            command.arguments.push_back(arg1);
            command.arguments.push_back(arg2);
            return true;
        }
    }

    return false;
}

//
// Takes the raw command string and splits it into tokens
//
void tokenizeCommand(const std::string& rawCommand, std::queue<std::string>& tokens) {
    std::stringstream stream(rawCommand);
    std::string token;

    while (std::getline(stream, token, ' ')) {
        tokens.push(std::move(token));
    }
}