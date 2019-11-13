#include "../../include/one/resourcemanager.hpp"
#include "../../include/one/storageengine.hpp"        // AbstractStorageEngine

//
// Constructs the resource manager and injects the storage enginge
//
ResourceManager::ResourceManager(
    std::unique_ptr<AbstractStorageEngine>&& storage,
    const Diagnostic& diagnostic
) : resourceView(diagnostic), diag(diagnostic) {

    this->storage = std::move(storage);
}

//
// Initializes the master resource view and the storage engine
//
void ResourceManager::init() {
    this->resourceView.init();
    // Start the loader at the seed resource (zeroed uuid)
    Uuid seed;
    relation_tuple tuple;

    std::get<0>(tuple) = seed;
    std::get<1>(tuple) = "";
    std::get<2>(tuple) = seed;

    // Add the root resource to the loaderQueue, to seed the loader
    loaderQueue.push(tuple);
}

//
// The worker method that synchronizes the master resource view with the filesystem
//
bool ResourceManager::sync() {
    bool processedWrite = false;
    // Check the load factor - load more stuff, if below load factor
    // Check the distance from the curent scope for all resources
    // release unused stuff
    // prevent endless loops

    if (!loaderQueue.empty()) {
        // Load the resource and add it to the master resource view
        this->processLoaderQueue(loaderQueue.front());

        loaderQueue.pop();
    }

    if (!writerQueue.empty()) {
        // Find the resource in the master resource view and store it
        processedWrite = this->processWriterQueue(writerQueue.front());

        writerQueue.pop();
    }

    return processedWrite;
}

//
// Processes a single item from the loader queue and loads it
//
void ResourceManager::processLoaderQueue(const relation_tuple& tuple) {
    Uuid rootUuid;
    Uuid parentUuid = std::get<0>(tuple);
    std::string predicate = std::get<1>(tuple);
    Uuid resourceUuid = std::get<2>(tuple);

    this->diag.log("loading resource %s from disk", ((std::string)resourceUuid).c_str());
    auto resource = storage->loadResource(resourceUuid);

    // Only add the queued resource, if it's not the root
    if (resourceUuid != rootUuid) {
        // Get the parent view resource
        auto parent = this->findResource(parentUuid);

        if (parent != nullptr) {
            this->addRelation(parent, predicate, resource);
        }
    }

    // TODO: Update the relation of the parent resource
    // set nullptr to instance - consider returning the new resource in addRelation


    // Add all related resources to the queue
    for (auto relation : *resource) {
        // Add the current subjects relation to the object to the end of the queue
        relation_tuple t;
        std::get<0>(t) = resource->getUuid();
        std::get<1>(t) = relation.predicate;
        std::get<2>(t) = relation.object;
        
        loaderQueue.push(t);
    }
}

//
// Processes a single item from the writer queue and stores it
//
bool ResourceManager::processWriterQueue(const Uuid& uuid) {
    this->diag.log("Save data for %s", ((std::string)uuid).c_str());
    auto resource = this->findResource(uuid);

    // TODO: Check under which conditions the resource cannot be found
    // How do we handle this error?
    if (resource != nullptr) {
        storage->storeResource(resource);
        // Mark resource and relations as saved
        resource->setNeedsWrite(false);

        for (auto& relation : *resource) {
            relation.needsWrite = false;
        }
    }
    
    return true;
}

//
// Creates a new resource and connects it to the given resource
//
void ResourceManager::addRelation(sp_resource resource, std::string predicate, std::string object) {
    auto relatedResource = newResource(object);
    ResourceRelation rel {
        predicate,
        relatedResource->getUuid(),
        relatedResource,
        true    // needsWrite
    };

    this->resourceView.addRelation(resource->getUuid(), rel);

    // TODO: Consider pushing a portable structure with all data
    // --> Could be the base class for use outside of the resource view

    // Replace the parent resource, to add the new relation
    writerQueue.push(resource->getUuid());
    // Add the related resource
    writerQueue.push(relatedResource->getUuid());
}

//
// Connects an existing resource to the given resource
//
void ResourceManager::addRelation(sp_resource resource, std::string predicate, sp_resource object) {
    ResourceRelation rel {
        predicate,
        object->getUuid(),
        object,
        false    // needsWrite
    };

    this->resourceView.addRelation(resource->getUuid(), rel);
}

//
// Find resource with the given uuid
//
sp_resource ResourceManager::findResource(const Uuid& uuid) {
    return this->resourceView.findResource(uuid);
}

//
// Find first resource with the given subject
//
sp_resource ResourceManager::findResourceBySubject(const std::string& subject) {
    return this->resourceView.findResourceBySubject(subject);
}
