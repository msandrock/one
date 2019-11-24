#include "../../include/one/socket.hpp"
#include "../../include/one/command.hpp"
#include "../../include/one/event.hpp"     // EventDispatcher
#include "../../include/one/stdafx.hpp"    // Exception
#include <chrono>           // std::chrono::milliseconds
#include <cstdio>           // std::remove
#include <fcntl.h>          // fcntl, O_NONBLOCK
#include <sys/socket.h>     // socket, bind, listen, accept, sockaddr, socklen_t
#include <sys/stat.h>       // stat
//#include <sys/un.h>         // sockaddr_un
#include <thread>           // std::this_thread
#include <unistd.h>         // read, write, close
#include <netinet/in.h>

//
// Store the dispatcher and spin up a socket for lisening
//
Socket::Socket(EventDispatcher* const dispatcher) : dispatcher(dispatcher) {
    struct sockaddr_in serv_addr;

    socketFd = socket(AF_INET, SOCK_STREAM, 0);

    if (socketFd < 0) {
        throw Exception("Error opening socket");
    } 
        
    bzero((char*)&serv_addr, sizeof(serv_addr));

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(PORT);

    if (bind(socketFd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
        // error("ERROR on binding");
        throw Exception("Error binding socket");
    }

    // Listen for connections
    // Backlog (1) defines the maximum number of pending connection requests,
    // until ECONNREFUSED is sent to clients trying to connect
    listen(socketFd, 1);
}

//
// Accept incomming connections on the socket and handle i/o
//
void Socket::run() {
    bool running = true;
    do {
        sockaddr addr;
        socklen_t addrlen;
        int clientFd = accept(socketFd, (sockaddr*)&addr, &addrlen);

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
bool Socket::handleConnection(int clientFd) {
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
Socket::~Socket() {
    close(socketFd);
}