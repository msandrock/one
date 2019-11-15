#ifndef STORAGEENGINE_HPP
#define STORAGEENGINE_HPP
#include "diagnostic.hpp"
#include "serialization.hpp"

// The storage engine is running in a separare thread and read and writes
// resources from the current resource view

//
// Defines the interface for every storage engine implementation. The storage engine is responsible for reading and writing resources to the storage medium
// Basic functionality includes initializing the engine and getting a resource based on the uuid
//
class AbstractStorageEngine {
public:
    virtual void init() = 0;
    virtual sp_resource loadResource(const Uuid& uuid) = 0;
    virtual void storeResource(sp_resource resource) = 0;
    // Make sure the derived destructor is invoked when working with
    // references to the base class
    virtual ~AbstractStorageEngine() {}
};

//
// Implements a file-based storage mechanism for the resources. This implementation stores the resources as files on the local disk.
// The serializer is responsible for serializing and deserializing resources to the storage format.
//
class FileStorageEngine : public AbstractStorageEngine {
public:
    FileStorageEngine(
        std::string storageFolder,
        std::unique_ptr<AbstractFileResourceSerializer>&& serializer,
        const Diagnostic& diag
    );
    virtual void init();
    virtual sp_resource loadResource(const Uuid& uuid);
    virtual void storeResource(sp_resource resource);

protected:
    std::string normalizePath(const std::string path);
    std::string uuidToStoragePath(const Uuid& uuid);
    const bool directoryExists(const std::string& path);
    const bool resourceFileExists(const Uuid& uuid);

private:
    std::string ROOT_FILENAME = "root";
    std::string storageFolder;
    std::unique_ptr<AbstractFileResourceSerializer> serializer;
    const Diagnostic& diag;
};

//
// Gets the implementation of storage engine that should be used
//
class StorageEngineFactory {
public:
    static std::unique_ptr<AbstractStorageEngine> create(const std::string& storageFolder, const Diagnostic& diag) {
        auto serializer = std::unique_ptr<AbstractFileResourceSerializer>(new JsonFileResourceSerializer);

        return std::unique_ptr<AbstractStorageEngine>(
            new FileStorageEngine(storageFolder, std::move(serializer), diag)
        );
    }
};

#endif