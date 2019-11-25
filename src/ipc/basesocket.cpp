#include "../../include/one/basesocket.hpp"
#include "../../include/one/command.hpp"
#include "../../include/one/event.hpp"     // EventDispatcher
#include <unistd.h>         // read, write

BaseSocket::BaseSocket(EventDispatcher* const dispatcher)
    : dispatcher(dispatcher) {}

bool BaseSocket::handleConnection(int clientFd) {
    while (true) {

        //
        // Handle received event
        //

        Event e;
        while (this->dispatcher->pop(CHANNEL_IO, e)) {
            if (e.type == EventType::IoResponse) {
                // Send back data through the socket
                const char* const output = e.arguments[0].stringArgument.c_str();
                write(clientFd, output, strlen(output));
            }
        }

        //
        // Handle input
        //

        char text[1024];   // TODO: Fix potential buffer overflow
        int length = read(clientFd, text, 1024);

        if (length <= 0) {
            continue;
        }

        // Terminate the string on \n
        text[length - 1] = '\0';

        std::string rawCommand(text);
        Command command;
        if (!parseCommand(rawCommand, command)) {
            const char* error = "Could not parse command\n";
            write(clientFd, error, strlen(error));
            continue;
        }
        
        // Dispatch events to worker thread
        if (command.type == CommandType::Get) {
            // Dispatch command to worker
            Event getRequest(EventType::GetRequest);
            getRequest.arguments.push_back(EventArgument(g_context));
            this->dispatcher->push(CHANNEL_WORKER, getRequest);
        } else if (command.type == CommandType::Add) {
            // Dispatch command to worker with subject, predicate, object
            Event addRequest(EventType::AddRequest);
            addRequest.arguments.push_back(EventArgument(g_context));
            addRequest.arguments.push_back(EventArgument(command.arguments[0]));
            addRequest.arguments.push_back(EventArgument(command.arguments[1]));
            this->dispatcher->push(CHANNEL_WORKER, addRequest);
        } else if (command.type == CommandType::Set) {
            // Dispatch command to worker with new context
            Event setRequest(EventType::SetRequest);
            setRequest.arguments.push_back(EventArgument(command.arguments[0]));
            this->dispatcher->push(CHANNEL_WORKER, setRequest);
        } else if (command.type == CommandType::Quit) {
            // Post a quit message to the worker thread
            Event quitRequest(EventType::QuitRequest);
            this->dispatcher->broadcast(quitRequest);
            // Terminate server
            return false;
        }
    }

    return true;
}
