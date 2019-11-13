#include <atomic>           // std::atomic
#include <thread>           // std::thread
#include "../include/one/stdafx.hpp"
#include "../include/one/config.hpp"
#include "../include/one/worker.hpp"
#include "../include/one/diagnostic.hpp"
#include "../include/one/io.hpp"
#include "../include/one/resource.hpp"

EventDispatcher g_dispatcher;
std::atomic<Uuid> g_context;

int main() {
    g_dispatcher.registerChannel(CHANNEL_MAIN);
    Config config;
    Diagnostic diag;

    // Start the worker thread
    std::thread w(worker, std::ref(config), std::ref(diag));
    // Start the io thread
    std::thread i(io, std::ref(config), std::ref(diag));

    Event e;
    bool running = true;

    do {
        if (!g_dispatcher.pop(CHANNEL_MAIN, e)) {
            continue;
        }

        if (e.type == EventType::QuitRequest) {
            running = false;
        }
        
    } while (running);

    i.join();
    w.join();
    return EXIT_SUCCESS;
} 
