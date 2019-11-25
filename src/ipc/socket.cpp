#include "../../include/one/socket.hpp"
#include "../../include/one/event.hpp"     // EventDispatcher
#include "../../include/one/stdafx.hpp"    // Exception
#include <chrono>           // std::chrono::milliseconds
#include <cstdio>           // std::remove
#include <fcntl.h>          // fcntl, O_NONBLOCK
#include <sys/socket.h>     // socket, bind, listen, accept, sockaddr, socklen_t
#include <unistd.h>         // read, write, close
#include <netinet/in.h>

const int Socket::IP_PORT = 9999;

Socket::Socket(EventDispatcher* const dispatcher)
    : BaseSocket(dispatcher) {}

void Socket::init() {
    struct sockaddr_in serv_addr;

    socketFd = socket(AF_INET, SOCK_STREAM, 0);

    if (socketFd < 0) {
        throw Exception("Error opening socket");
    } 
        
    bzero((char*)&serv_addr, sizeof(serv_addr));

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(Socket::IP_PORT);

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

Socket::~Socket() {
    close(socketFd);
}