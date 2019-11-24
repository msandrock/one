#ifndef SOCKET_HPP
#define SOCKET_HPP
#include "event.hpp" // EventDispatcher
#include <atomic>    // std::atomic

extern std::atomic<Uuid> g_context;
const int PORT = 9999;

class Socket {
    int socketFd;
    EventDispatcher* const dispatcher;
protected:
    bool handleConnection(int clientFd);
public:
    Socket(EventDispatcher* const dispatcher);
    void run();
    virtual ~Socket();
};

#endif