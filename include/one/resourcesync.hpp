#pragma once
#include "task.hpp"
#include "resourcemanager.hpp"

class ResourceSyncTask final : public Task {
    ResourceManager* resourceManager;
public:
    ResourceSyncTask(ResourceManager* resourceManager) {
        this->resourceManager = resourceManager;
    }

    bool run() {
        this->resourceManager->sync();
        return true;
    }

    void shutdown() {
        // Flush all writes to disk before exiting
        while(this->resourceManager->sync());
    }
};