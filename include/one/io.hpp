#ifndef IO_HPP
#define IO_HPP
#include "event.hpp"     // EventDispatcher
#include "diagnostic.hpp"

// This instance is also used in main.cpp 
extern EventDispatcher g_dispatcher;

void io(const Diagnostic& diag);

#endif