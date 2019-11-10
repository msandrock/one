#include <cstdio>
#include <execinfo.h>
#include <iostream>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include "stdafx.hpp"

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
    exit(EXIT_FAILURE);
}

int main(int argc, const char** argv) {
    signal(SIGSEGV, shutdown_handler);
    // Check for command line parameters
    if (argc > 1 && std::string(argv[1]) == "-v") {
        std::cout << VERSION << std::endl;
        return EXIT_SUCCESS;
    }

    return EXIT_SUCCESS;
} 
