#pragma once
#include <queue>            // std::queue
#include "diagnostic.hpp"
#include "resource.hpp"
#include "storageengine.hpp"
#include "resourceview.hpp"

//
// The resource manager maintains the master resource view and controls any access to it
// It is also responsible for loading and storing resources via the storage engine
//
class ResourceManager {
public:
    ResourceManager(std::unique_ptr<AbstractStorageEngine>&& storage, const Diagnostic& diag);
    void init();
    bool sync();
    void addRelation(sp_resource resource, std::string predicate, std::string object);
    void addRelation(sp_resource resource, std::string predicate, sp_resource object);
    sp_resource findResource(const Uuid& uuid);
    sp_resource findResourceBySubject(const std::string& subject);
    // lockResource() --> ResourceView
    //uri getScope() const;

    //const int MAX_RESOURCE = 1000;  // Maximum number of loadable resources in the master view
    //const int MAX_DISTANCE = 3;     // Maximum distance from the current scope

private:
    void processLoaderQueue(const relation_tuple& tuple);
    bool processWriterQueue(const Uuid& uuid);

    std::queue<relation_tuple> loaderQueue;
    std::queue<Uuid> writerQueue;
    ResourceView resourceView;
    std::unique_ptr<AbstractStorageEngine> storage;
    const Diagnostic& diag;
};
