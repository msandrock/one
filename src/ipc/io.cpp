#include "../../include/one/io.hpp"
#include "../../include/one/domainsocket.hpp"

// Talk to me with
// nc -U daimon.sock 

//
// Io process for asynchronous command processing
//
void io(const Config& config, const Diagnostic& diag) {
    g_dispatcher.registerChannel(CHANNEL_IO);

    DomainSocket domainSocket(&g_dispatcher);
    domainSocket.run();

    diag.log("Exiting io thread");
}
