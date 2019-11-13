#include "../../include/one/domainsocket.hpp"
#include "../../include/one/command.hpp"
#include "../../include/one/event.hpp"     // EventDispatcher
#include "../../include/one/stdafx.hpp"    // Exception
#include <chrono>           // std::chrono::milliseconds
#include <cstdio>           // std::remove
#include <fcntl.h>          // fcntl, O_NONBLOCK
#include <sys/socket.h>     // socket, bind, listen, accept, sockaddr, socklen_t
#include <sys/stat.h>       // stat
#include <sys/un.h>         // sockaddr_un
#include <thread>           // std::this_thread
#include <unistd.h>         // read, write, close

//
// Store the dispatcher and spin up an unix domain socket for lisening
//
DomainSocket::DomainSocket(EventDispatcher* const dispatcher) : dispatcher(dispatcher) {
    // Check if the socket file exists - an existing file will cause bind to fail
    struct stat info;

    if (stat(SOCKET_NAME, &info) != 0) {
        // Socket not found - proceed to set it up
    } else if (info.st_mode & S_IFSOCK) {
        // Socket was not closed - remove
        std::remove(SOCKET_NAME);
    } else {
        // File is not a socket - bail
        throw Exception("Socket is blocked");
    }

    sockaddr_un addr;
    // Create the socket
    socketFd = socket(PF_LOCAL, SOCK_STREAM, 0);
    // Configure as non-blocking socket
    int flags = fcntl(socketFd, F_GETFL, 0);
    fcntl(socketFd, F_SETFL, flags | O_NONBLOCK);
    // Indicate that this is a server
    addr.sun_family = AF_LOCAL;
    strcpy(addr.sun_path, SOCKET_NAME);
    // Bind to local file
    int bindResult = bind(socketFd, (const sockaddr*)&addr, SUN_LEN(&addr));

    if (bindResult == -1) {
        throw Exception("Socket bind failed");
    }

    // Listen for connections
    // Backlog (1) defines the maximum number of pending connection requests,
    // until ECONNREFUSED is sent to clients trying to connect
    listen(socketFd, 1);
}

//
// Accept incomming connections on the socket and handle i/o
//
void DomainSocket::run() {
    bool running = true;
    do {
        sockaddr_un addr;
        socklen_t addrlen;
        int clientFd;
        // Accept a connection
        clientFd = accept(socketFd, (sockaddr*)&addr, &addrlen);

        if (clientFd > 0) {
            // Handle the connection
            running = this->handleConnection(clientFd);
            // Close our end of the connection.
            close(clientFd);
        }
    } while (running);
}

//
// Parse incomming commands from the active connection and send back responses
//
bool DomainSocket::handleConnection(int clientFd) {
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

//
// Cleanup - tear down socket and delete the file
//
DomainSocket::~DomainSocket() {
    close(socketFd);
    std::remove(SOCKET_NAME);
}