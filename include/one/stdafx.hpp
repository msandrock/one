#ifndef STDAFX_HPP
#define STDAFX_HPP
#include <exception>    // std::exception
#include <string>       // std::string

//
// Base class of all exceptions
//
class Exception : public std::exception {
    std::string message;

    public:
    Exception(const std::string& message)
        : message(message) {
    }

    std::string getMessage() const {
        return this->message;
    }
};

#endif