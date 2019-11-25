#ifndef SOCKET_HPP
#define SOCKET_HPP
#include "basesocket.hpp"
#include "event.hpp" // EventDispatcher

extern std::atomic<Uuid> g_context;

class Socket : public BaseSocket {
public:
    static const int IP_PORT;
    Socket(EventDispatcher* const dispatcher);
    virtual void init();
    virtual void run();
    virtual ~Socket();
};

#endif