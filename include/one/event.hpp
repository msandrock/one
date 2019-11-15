#ifndef EVENT_HPP
#define EVENT_HPP
#include <map>          // std::map
#include <memory>       // std::shared_ptr
#include <string>       // std::string
#include <vector>       // std::vector
#include "queue.hpp"
#include "resource.hpp"

const char* const CHANNEL_MAIN = "main";
const char* const CHANNEL_WORKER = "worker";
const char* const CHANNEL_IO = "io";

enum class EventType : uint8_t {
    Nop,
    QuitRequest,
    GetRequest,
    AddRequest,
    SetRequest,
    IoResponse
};

enum class EventArgumentType : uint8_t {
    String,
    Uuid
};

struct EventArgument {
    EventArgument(const std::string& arg) : argumentType(EventArgumentType::String) {
        stringArgument = arg;
    }
    EventArgument(const Uuid& arg) : argumentType(EventArgumentType::Uuid) {
        uuidArgument = arg;
    }

    // Union support
    /*EventArgument(const EventArgument& other) {
        argumentType = other.argumentType;
    
        switch (other.argumentType) {
            case EventArgumentType::String:
                stringArgument = other.stringArgument;
            break;
            case EventArgumentType::Uuid:
                uuidArgument = other.uuidArgument;
            break;
        }
    }

    EventArgument& operator=(const EventArgument& other) {
        argumentType = other.argumentType;

        if (argumentType == EventArgumentType::Uuid) {
            uuidArgument = other.uuidArgument;
        } else {
            stringArgument = other.stringArgument;
        }

        return *this;
    }*/

    ~EventArgument() {}

    EventArgumentType argumentType;
    // Consider a std::variant in C++17
    //union {
        std::string stringArgument;
        Uuid uuidArgument;
    //};
};

struct Event {
    Event() : Event(EventType::Nop) {}
    Event(EventType type) : type(type) {}
    ~Event() {}

    EventType type;
    std::vector<EventArgument> arguments;
};

//
// Dispatch events to synchronized queues
//
class EventDispatcher {
    // Controls the time every call to pop waits, until it returns false
    const unsigned int POP_WAIT_TIMEOUT = 10;
    std::map<std::string, std::shared_ptr<SynchronizedQueue<Event>>> queues;
public:
    void registerChannel(std::string channel);
    void broadcast(Event event);
    void push(const std::string& channel, Event event);
    bool pop(const std::string& channel, Event& event);
};

#endif