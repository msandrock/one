#ifndef BASESOCKET_HPP
#define BASESOCKET_HPP
#include "event.hpp" // EventDispatcher
#include <atomic>    // std::atomic

extern std::atomic<Uuid> g_context;

class BaseSocket {
protected:
    int socketFd;
    EventDispatcher* const dispatcher;
    virtual bool handleConnection(int clientFd);
public:
    BaseSocket(EventDispatcher* const dispatcher);
    virtual void init() = 0;
    virtual void run() = 0;
    virtual ~BaseSocket() {}
};

#endif