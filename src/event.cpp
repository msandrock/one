#include "../include/one/event.hpp"

void EventDispatcher::registerChannel(std::string channel) {
    this->queues.insert(std::make_pair(channel, std::make_shared<SynchronizedQueue<Event>>()));
}

void EventDispatcher::broadcast(Event event) {
    // Dispatch event to all channels
    for (auto const &iterator : queues) {
        iterator.second->push(event);
    }
}

void EventDispatcher::push(const std::string& channel, Event event) {
    auto iterator = queues.find(channel);

    if (iterator != queues.end()) {
        iterator->second->push(event);
    }
}

bool EventDispatcher::pop(const std::string& channel, Event& event) {
    auto iterator = queues.find(channel);
    
    if (iterator == queues.end()) {
        return false;
    }

    return iterator->second->pop(event, std::chrono::milliseconds(POP_WAIT_TIMEOUT));
}
