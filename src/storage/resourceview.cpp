#include <chrono>       // std::chrono::milliseconds
#include <thread>       // std::this_thread::sleep_for
#include "../../include/one/stdafx.hpp"
#include "../../include/one/resourceview.hpp"

ResourceView::ResourceView(const Diagnostic& diag) : diag(diag), root(nullptr) {
}

//
// Initializes the resource view with a root node
//
void ResourceView::init() {
    // Initialize with a fresh root resource
    Uuid rootUuid;
    std::string rootSubject = "root";
    this->root = newResource(rootSubject, rootUuid);
}

//
// Adds a new resource to the master view collection
//
void ResourceView::addRelation(const Uuid& parentUuid, const ResourceRelation& relation) {
    auto parent = this->findResource(parentUuid);

    if (parent != nullptr) {
        parent->addRelation(relation);
    }
}

//
// Iterates over the map and finds the resource with the given uuid
//
sp_resource ResourceView::findResource(const Uuid& uuid) {
    this->diag.log("findResource");

    auto cb = [this, uuid](sp_resource resource) {
        this->diag.log("Checking %s", resource->getSubject().c_str());
        bool hit = uuid == resource->getUuid();

        if (hit) {
            this->diag.log("found match: %s", resource->getSubject().c_str());
        }

        return hit;
    };

    // Start at the root node
    return this->walkResources(this->root, cb);
}

//
// Iterates over the map and finds the first resource with the given subject
//
sp_resource ResourceView::findResourceBySubject(const std::string& subject) {
    this->diag.log("findResourceBySubject");

    auto cb = [/*this,*/ subject](sp_resource resource) {
        bool hit = subject == resource->getSubject();

        if (hit) {
            //this->diag.log("found match: %s", resource->getSubject().c_str());
        }

        return hit;
    };

    // Start at the root node
    return this->walkResources(this->root, cb);
}

//
// Iterate over all relations in the given seed node
//
sp_resource ResourceView::walkResources(
    sp_resource resource,
    std::function<bool (sp_resource)> callback) {

    auto currentResource = resource;
    this->diag.log("Inspecting node \"%s\"", resource->getSubject().c_str());

    //
    // Invoke the callback and return, if the condition is met
    //

    if (callback != nullptr && callback(resource)) {
        return currentResource;
    }

    //
    // Iterate over all related child nodes
    //

    for (const auto relation : *resource) {
        if (relation.resource == nullptr) {
            this->diag.log("Related resource is not loaded - skipping");
            continue;
        }

        this->diag.log("Proceeding to related node \"%s->%s->%s\"",
                        resource->getSubject().c_str(),
                        relation.predicate.c_str(),
                        relation.resource->getSubject().c_str());

        currentResource = this->walkResources(relation.resource, callback);

        if (currentResource != nullptr) {
            return currentResource;
        }
    }

    return nullptr;
}
