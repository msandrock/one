#include <atomic>
#include <thread>
#include "../include/one/stdafx.hpp"
#include "../include/one/worker.hpp"
#include "../include/one/diagnostic.hpp"
#include "../include/one/io.hpp"
#include "../include/one/resource.hpp"

EventDispatcher g_dispatcher;
std::atomic<Uuid> g_context;

int main(int argc, const char** argv) {
    if (argc < 2) {
        std::cout << "Please specify a storage folder" << std::endl;
        return EXIT_FAILURE;
    }

    std::string storageFolder(argv[1]);
    g_dispatcher.registerChannel(CHANNEL_MAIN);
    Diagnostic diag;

    // Start the worker thread
    std::thread w(worker, std::ref(storageFolder), std::ref(diag));
    // Start the io thread
    std::thread i(io, std::ref(diag));

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
