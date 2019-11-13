#pragma once
#include "stdafx.hpp" // Exception
#include <algorithm>     // std::equal
#include <iostream>      // std::ostream
#include <string>        // std::string
#include <tuple>         // std::tuple
#include <vector>        // std::vector
#include <uuid/uuid.h>   // uuid_generate, uuid_unparse

class Resource;
class Uuid;
typedef std::shared_ptr<Resource> sp_resource;
typedef std::tuple<Uuid, std::string, Uuid> relation_tuple;

//
// Simple wrapper with convenience methods for uuid handling
//
class Uuid {
    uuid_t uuid;

public:
    // Construct with root uuid
    Uuid() {
        memset(uuid, 0, sizeof(uuid_t));
    }
    // Construct from string of the form 1b4e28ba-2fa1-11d2-883f-b9a761bde3fb
    Uuid(std::string uuid) {
        uuid_t parsed;
        if (uuid_parse(uuid.c_str(), parsed) == -1) {
            throw Exception("Could not parse uuid");
        }

        this->set(parsed);
    }

    void generate() {
        uuid_generate(uuid);
    }

    void set(const uuid_t source) {
        memcpy(uuid, source, sizeof(uuid));
    }

    const uuid_t* data() const {
        //https://stackoverflow.com/questions/12328301/cleanest-way-to-copy-a-constant-size-array-in-c11
        //std::array<unsigned char, 16> dataArray;
        //memcpy(dataArray.data(), uuid, sizeof(uuid));
        //return dataArray;
        return &uuid;
    }

    bool operator==(const Uuid& other) const {
        // Compare the uuids element-wise
        return std::equal(std::begin(uuid), std::end(uuid), std::begin(other.uuid));
    }

    bool operator!=(const Uuid& other) const {
        return !(*this == other);
    }

    friend std::ostream& operator<<(std::ostream& stream, const Uuid& uuid) {
        stream << (std::string)uuid;
        return stream;
    }

    operator std::string() const { 
        char uuidBuffer[37];
        uuid_unparse(uuid, uuidBuffer);

        std::string uuidStr(uuidBuffer);
        return uuidStr;
    }
};

//
// Represents the relation between two resources
//
struct ResourceRelation {
    std::string predicate;
    Uuid object;
    sp_resource resource;
    bool needsWrite;
    // needsRead is implicit by checking the resource property for a nullptr
};

//
// Implementats a single resource representation
//
class Resource {
public:
    Resource() : needsWrite(true) /*not from disk*/ {
        uuid.generate();    // Initialize with a new uuid
    }
    Resource(Uuid uuid) : uuid(uuid), needsWrite(false) /*loaded*/ {
    }

    Uuid getUuid() const { return uuid; }

    std::string getSubject() const { return subject; }
    void setSubject(const std::string& subject) { this->subject = subject; }

    // Iterator support for relations
    std::__wrap_iter<ResourceRelation*> begin() { return this->relations.begin(); }
    std::__wrap_iter<ResourceRelation*> end() { return this->relations.end(); }
    std::__wrap_iter<const ResourceRelation*> cbegin() { return this->relations.cbegin(); }
    std::__wrap_iter<const ResourceRelation*> cend() { return this->relations.cend(); }

    std::size_t getRelationCount() const { return relations.size(); }

    void addRelation(const ResourceRelation& relation) {
        // Update the resource, if the relation is not from disk
        if (relation.needsWrite) {
            this->needsWrite = true;
        }

        // Check if the relation exists -> update
        for (ResourceRelation& rel : this->relations) {
            if (rel.object == relation.object) {
                // Update the existing relation
                rel.predicate = relation.predicate;
                rel.object = relation.object;
                rel.resource = relation.resource;
                return;
            }
        }

        this->relations.push_back(relation);
    }

    bool getNeedsWrite() const { return needsWrite; }
    void setNeedsWrite(bool needsWrite) { this->needsWrite = needsWrite; }

protected:
    Uuid uuid;
    std::string subject;
    std::vector<ResourceRelation> relations;
    bool needsWrite;
};

sp_resource newResource(const std::string& subject);
sp_resource newResource(const std::string& subject, const Uuid& uuid);
