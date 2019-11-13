#include <string>       // std::string
#include <memory>       // std::shared_ptr, std::make_shared
#include "../../include/one/resource.hpp"

//
// Creates a new resource entry and returns a shared pointer to it
//
sp_resource newResource(const std::string& subject) {
    auto resource = std::make_shared<Resource>(/* uuid as constructor argument? */);
    resource->setSubject(subject);

    return resource;
}

sp_resource newResource(const std::string& subject, const Uuid& uuid) {
    auto resource = std::make_shared<Resource>(uuid);
    resource->setSubject(subject);

    return resource;
}
