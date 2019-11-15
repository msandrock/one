#ifndef DIAGNOSTIC_HPP
#define DIAGNOSTIC_HPP
#include <cstddef>      // std::size_t
#include <cstdio>       // snprintf
#include <iostream>     // std::cout, std::endl
#include <memory>       // std::unique_ptr
#include <mutex>        // std::mutex
#include <string>       // std::string

class Diagnostic final {
    mutable std::mutex mutex;
public:
    void log(std::string message) const {
        std::lock_guard<std::mutex> lock(this->mutex);

        std::cout << message << std::endl;
    }

    // Cannot define the variadic function in a cpp file
    // @see https://stackoverflow.com/questions/3008541/template-class-symbols-not-found
    template<typename ...T>
    void log(std::string message, T ...args) const {
        std::lock_guard<std::mutex> lock(this->mutex);

        std::size_t size = snprintf(nullptr, 0, message.c_str(), args ...) + 1; // Extra space for '\0'
        std::unique_ptr<char[]> buf(new char[size]); 
        snprintf(buf.get(), size, message.c_str(), args ...);
        std::cout << std::string(buf.get(), buf.get() + size) << std::endl;
    }

    // timing
};

#endif