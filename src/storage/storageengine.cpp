#include <string>               // std::string
#include <sys/stat.h>           // stat
#include "../../include/one/stdafx.hpp"        // Exception
#include "../../include/one/resource.hpp"
#include "../../include/one/serialization.hpp"
#include "../../include/one/storageengine.hpp"

//
// Takes a path string and transforms it into an uniform format
//
std::string FileStorageEngine::normalizePath(const std::string path) {
    std::string normalizedPath = path; 

    // Ensure that the storage folder path ends with a /
    if (normalizedPath[normalizedPath.size() - 1] != '/') {
        normalizedPath += "/";
    }

    return normalizedPath;
}

//
// Converts the uuid to a physical storage path in the storage folder
//
std::string FileStorageEngine::uuidToStoragePath(const Uuid& uuid) {
    // Check if the storage folder is set
    if (this->storageFolder == "") {
        throw Exception("Storage folder is not set");
    }
    // Concat uri to root folder
    return this->storageFolder + static_cast<std::string>(uuid) + ".json";
}

//
// Checks if the given folder path exists in the filesystem
//
const bool FileStorageEngine::directoryExists(const std::string& path) {
    struct stat info;
    bool exists = false;

    if (stat(path.c_str(), &info) != 0) {
        //this->diag.log("cannot access %s", path.c_str());

        /*
        int mkdir(const char *path, mode_t mode);

        int status;
        ...
        status = mkdir("/home/cnd/mod1", S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);

        if (status == 0) // ok, else -1 and errno() will be set

        */ 

    } else if (info.st_mode & S_IFDIR) {
        // path is a directory
        exists = true;

    } else {
        // path is not a directory
    }

    return exists;
}

//
// Checks if a corresponding resource file exists for the given uuid
//
const bool FileStorageEngine::resourceFileExists(const Uuid& uuid) {
    struct stat info;
    bool exists = false;
    std::string storagePath = this->uuidToStoragePath(uuid);

    if (stat(storagePath.c_str(), &info) != 0) {
        this->diag.log("cannot access %s", storagePath.c_str());
    } else if (info.st_mode & S_IFREG) {
        exists = true;
    } else {
        this->diag.log("%s is not a file", storagePath.c_str());
    }

    return exists;
}

//
// Creates the file storage engine and sets the storage folder
//
FileStorageEngine::FileStorageEngine(
        std::string storageFolder,
        std::unique_ptr<AbstractFileResourceSerializer>&& serializer,
        const Diagnostic& diag
    ) : diag(diag) {

    this->storageFolder = this->normalizePath(storageFolder);
    this->serializer = std::move(serializer);

    if (!this->directoryExists(this->storageFolder)) {
        throw Exception("The storage folder does not exist");
    }
}

//
// Initialize the file storage engine
//
void FileStorageEngine::init() {
    // Create an empty root resource file, if it does not exist yet
    Uuid root;

    if (!this->resourceFileExists(root)) {
        this->storeResource(makeRootResource());
    }
}

//
// Loads the given resource from disk and returns it
//
sp_resource FileStorageEngine::loadResource(const Uuid& uuid) {
    std::string path = this->uuidToStoragePath(uuid);

    return this->serializer->deserialize(path);
}

//
// Stores the given resource on disk
//
void FileStorageEngine::storeResource(sp_resource resource) {
    std::string path = this->uuidToStoragePath(resource->getUuid());
    this->serializer->serialize(path, resource); 
}