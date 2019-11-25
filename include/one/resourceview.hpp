#ifndef RESOURCEVIEW_HPP
#define RESOURCEVIEW_HPP
#include <functional>       // std::function
#include "diagnostic.hpp"
#include "resource.hpp"

typedef bool (*resourceCallback)(sp_resource res);

//
// The resource view contains a subset of resources and is managed by the resource manager
//
class ResourceView {
public:
    ResourceView(const Diagnostic& diag);
    void init();
    void addRelation(const Uuid& parentUuid, const ResourceRelation& relation);
    sp_resource findResource(const Uuid& uuid);
    sp_resource findResourceBySubject(const std::string& subject);
    void updateResource(const Uuid& uuid, const std::string& property, const std::string& value);

private:
    const Diagnostic& diag;
    sp_resource root;
    sp_resource walkResources(sp_resource resource, std::function<bool (sp_resource)> callback);
};

#endif