#ifndef EVENTHANDLER_HPP
#define EVENTHANDLER_HPP
#include "task.hpp"
#include "event.hpp" // Event

class EventHandlerTask final : public Task {
    bool run() {
        Event e;
        // TODO: Poll for events - delegate event handling

        return true;
    }
};

#endif