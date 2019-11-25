#include "../../include/one/domainsocket.hpp"
#include "../../include/one/event.hpp"     // EventDispatcher
#include "../../include/one/stdafx.hpp"    // Exception
#include <chrono>           // std::chrono::milliseconds
#include <cstdio>           // std::remove
#include <fcntl.h>          // fcntl, O_NONBLOCK
#include <sys/socket.h>     // socket, bind, listen, accept, sockaddr, socklen_t
#include <sys/stat.h>       // stat
#include <sys/un.h>         // sockaddr_un
#include <unistd.h>         // read, write, close

const char* DomainSocket::SOCKET_NAME = "one.sock";

DomainSocket::DomainSocket(EventDispatcher* const dispatcher)
    : BaseSocket(dispatcher) {}

void DomainSocket::init() {
    // Check if the socket file exists - an existing file will cause bind to fail
    struct stat info;

    if (stat(DomainSocket::SOCKET_NAME, &info) != 0) {
        // Socket not found - proceed to set it up
    } else if (info.st_mode & S_IFSOCK) {
        // Socket was not closed - remove
        std::remove(DomainSocket::SOCKET_NAME);
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
    strcpy(addr.sun_path, DomainSocket::SOCKET_NAME);
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

DomainSocket::~DomainSocket() {
    close(socketFd);
    std::remove(DomainSocket::SOCKET_NAME);
}