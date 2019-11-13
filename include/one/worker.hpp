#include "diagnostic.hpp"
#include "command.hpp"
#include "config.hpp"
#include "event.hpp"

// This instance is also used in main.cpp 
extern EventDispatcher g_dispatcher;
extern std::atomic<Uuid> g_context;

void worker(const Config& config, const Diagnostic& diag);
std::string createRepresentation(sp_resource resource);
