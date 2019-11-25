#ifndef DOMAINSOCKET_HPP
#define DOMAINSOCKET_HPP
#include "basesocket.hpp"
#include "event.hpp" // EventDispatcher

class DomainSocket : public BaseSocket {
public:
    static const char* SOCKET_NAME;

    DomainSocket(EventDispatcher* const dispatcher);
    virtual void init();
    virtual void run();
    virtual ~DomainSocket();
};

#endif