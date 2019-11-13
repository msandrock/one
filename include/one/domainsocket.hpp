#pragma once
#include "event.hpp" // EventDispatcher
#include <atomic>           // std::atomic

extern std::atomic<Uuid> g_context;
const char* const SOCKET_NAME = "daimon.sock";

class DomainSocket {
    int socketFd;
    EventDispatcher* const dispatcher;
protected:
    bool handleConnection(int clientFd);
public:
    DomainSocket(EventDispatcher* const dispatcher);
    void run();
    virtual ~DomainSocket();
};