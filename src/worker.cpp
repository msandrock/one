#include <chrono>       // std::chrono::milliseconds
#include <memory>       // std::shared_ptr
#include <sstream>      // std::stringstream
#include <thread>       // std::this_thread
#include "../include/one/stdafx.hpp"
#include "../include/one/worker.hpp"
#include "../include/one/resourcesync.hpp"
#include "../include/one/eventhandler.hpp"
#include "../include/one/taskrunner.hpp"
#include "../include/one/resourcemanager.hpp"

//
// Main worker process for asynchronous resource synchronization
//
void worker(const std::string& storageFolder, const Diagnostic& diag) {
    g_dispatcher.registerChannel(CHANNEL_WORKER);

    //
    // Set up the resource manager
    //

    // Create the storage engine for the resource manager
    auto storage = StorageEngineFactory::create(storageFolder, diag);
    storage->init();
    // Create and initialize the resouce manager 
    ResourceManager resourceManager(std::move(storage), diag);
    resourceManager.init();
    
    //
    // Configure tasks
    //

    //TaskRunner taskRunner;

    // Load / write resources
    //std::unique_ptr<Task> syncTask(new ResourceSyncTask(&resourceManager));
    //taskRunner.addTask(std::move(syncTask));

    // Handle events
    //std::unique_ptr<Task> eventTask(new EventHandlerTask());
    //taskRunner.addTask(std::move(eventTask));

    //taskRunner.run();

    while (true) {
        resourceManager.sync();
        Event e;

        if (!g_dispatcher.pop(CHANNEL_WORKER, e)) {
            continue;
        }

        //
        // Handle received event
        //

        if (e.type == EventType::QuitRequest) {
            diag.log("Exiting worker thread");
            return;
        }

        if (e.type == EventType::GetRequest) {
            Uuid ctx = e.arguments[0].uuidArgument;
            // TODO: Spawn a new task
            auto resource = resourceManager.findResource(ctx);

            if (resource != nullptr) {
                // TODO: Create a copy of the resource and return it as an event argument
                // The data needs to be packaged, so it can be consumed by the ui layer
                // Needs a type to encapsulate the data for a single resource
                auto formattedResponse = createRepresentation(resource);

                Event ioResponse(EventType::IoResponse);
                ioResponse.arguments.push_back(EventArgument(formattedResponse));
                g_dispatcher.push(CHANNEL_IO, ioResponse);
            }
        }

        if (e.type == EventType::AddRequest) {
            Uuid ctx = e.arguments[0].uuidArgument;
            std::string predicate = e.arguments[1].stringArgument;
            std::string object = e.arguments[2].stringArgument;
            // TODO: Spawn a new task
            sp_resource resource = resourceManager.findResource(ctx);

            if (resource != nullptr) {
                diag.log("Add new node to view (this)->%s->%s", predicate.c_str(), ((std::string)object).c_str());
                resourceManager.addRelation(resource, predicate, object);
            }
        }

        /*if (e.type == EventType::UpdateRequest) {
            Uuid ctx = e.arguments[0].uuidArgument;
            std::string predicate = e.arguments[1].stringArgument;
            std::string object = e.arguments[2].stringArgument;
            // TODO: Spawn a new task
            sp_resource resource = resourceManager.findResource(ctx);

            if (resource != nullptr) {
                diag.log("Add new node to view (this)->%s->%s", predicate.c_str(), ((std::string)object).c_str());
                resourceManager.addRelation(resource, predicate, object);
            }
        }*/

        if (e.type == EventType::SetRequest) {
            //std::string subject = e.arguments[0].stringArgument;
            // TODO: Spawn a new task
            //sp_resource resource = resourceManager.findResourceBySubject(subject);
            Uuid uuid(e.arguments[0].stringArgument);
            sp_resource resource = resourceManager.findResource(uuid);

            if (resource != nullptr) {
                // Set the new uuid as the current context
                g_context = resource->getUuid();
                // Send some info on the new context
                auto formattedResponse = createRepresentation(resource);

                Event ioResponse(EventType::IoResponse);
                // This causes an exception --> Problem with move? 
                // --> we are using a shared pointer --> Copy by value
                ioResponse.arguments.push_back(EventArgument(formattedResponse));
                g_dispatcher.push(CHANNEL_IO, ioResponse);

            } else {
                diag.log("Could not find requested context");
            }
        }
    }
}

//
// Generates a formatted string for the given resource
//
std::string createRepresentation(sp_resource resource) {
    std::stringstream ss;
    ss << "*** [" << resource->getUuid() << "] " << resource->getSubject() << "***" << (resource->getNeedsWrite() ? " (needs write)" : "") << std::endl;

    for (const auto relation : *resource) {
        if (relation.resource == nullptr) {
            ss << "Resource is not loaded" << std::endl;
            continue;
        }

        ss << "->" << relation.predicate << "-> [" << relation.resource->getUuid() << "] " << relation.resource->getSubject() << (relation.needsWrite ? " (needs write)" : "") << std::endl;
    }

    return ss.str();
}