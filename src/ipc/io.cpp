#include "../../include/one/io.hpp"
#include "../../include/one/domainsocket.hpp"
#include "../../include/one/socket.hpp"

// Talk to me with
// nc -U one.sock
// or
// nc 127.0.0.1 9999

//
// Io process for asynchronous command processing
//
void io(const Diagnostic& diag) {
    g_dispatcher.registerChannel(CHANNEL_IO);

    //DomainSocket domainSocket(&g_dispatcher);
    //domainSocket.run();
    Socket socket(&g_dispatcher);
    socket.run();

    diag.log("Exiting io thread");
}
