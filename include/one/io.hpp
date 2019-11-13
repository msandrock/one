#pragma once
#include "config.hpp"    // Config
#include "event.hpp"     // EventDispatcher
#include "diagnostic.hpp"

// This instance is also used in main.cpp 
extern EventDispatcher g_dispatcher;

void io(const Config& config, const Diagnostic& diag);
