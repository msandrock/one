#include <atomic>
#include <thread>
#include <execinfo.h>
#include <filesystem>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include "../include/one/stdafx.hpp"
#include "../include/one/worker.hpp"
#include "../include/one/diagnostic.hpp"
#include "../include/one/io.hpp"
#include "../include/one/resource.hpp"

EventDispatcher g_dispatcher;
std::atomic<Uuid> g_context;

/**
 * Handle segfaults and print a backtrace to stderr before exiting
 */
void shutdown_handler(int sig) {
    const size_t MAX_BACKTRACE_SIZE = 10;
    void *array[MAX_BACKTRACE_SIZE];
    size_t size;

    // get void*'s for all entries on the stack
    size = backtrace(array, MAX_BACKTRACE_SIZE);

    // print out all the frames to stderr
    fprintf(stderr, "Error: signal %d:\n", sig);
    backtrace_symbols_fd(array, size, STDERR_FILENO);
}

void signal_handler(int sig) {
    std::cout << "Caught signal " << sig << std::endl;

    if (sig == SIGSEGV) {
        shutdown_handler(sig);
    } else if (sig == SIGINT) {
        // Ctrl+c
        // Sync pending changes to disk
        std::remove("one.sock");
    }

    exit(EXIT_FAILURE);
}

int main(int argc, const char** argv) {
    // Register signal handler
    struct sigaction sigIntHandler;
    sigIntHandler.sa_handler = signal_handler;
    sigemptyset(&sigIntHandler.sa_mask);
    sigIntHandler.sa_flags = 0;
    sigaction(SIGINT, &sigIntHandler, NULL);

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
