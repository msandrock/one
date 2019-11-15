#ifndef SERIALIZATION_HPP
#define SERIALIZATION_HPP
#include "resource.hpp"

//
// Abstraction provider for file based resource serializations. This class converts a resource from/to the storage format
//
class AbstractFileResourceSerializer {
public:
    virtual void serialize(const std::string& path, sp_resource resource) = 0;
    virtual sp_resource deserialize(const std::string& path) = 0;
    // Make sure the derived destructor is invoked when working with
    // references to the base class
    virtual ~AbstractFileResourceSerializer() {}
};

//
// Provides file based json resource serialization 
//
class JsonFileResourceSerializer final : public AbstractFileResourceSerializer {
public:
    void serialize(const std::string& path, sp_resource resource);
    sp_resource deserialize(const std::string& path);
};

#endif