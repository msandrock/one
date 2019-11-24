#include "../../include/one/serialization.hpp"
#include "../../include/one/resource.hpp"         // sp_resource
#include <fstream>              // std::ifstream, std::ofstream
#include "../../lib/json/src/json.hpp"

using json = nlohmann::json;

//
// Create json file from resource
//
void JsonFileResourceSerializer::serialize(const std::string& path, sp_resource resource) {
    std::ofstream file;
    file.open(path.c_str(), std::ios::out | std::ios::trunc);

    file << json(*resource).dump(4);    // Invokes to_json, use pretty printing
    file.close();
}

//
// Create resource from json file
//
sp_resource JsonFileResourceSerializer::deserialize(const std::string& path) {
    std::ifstream file;
    file.open(path.c_str(), std::ios::in);

    json j;
    file >> j;

    // Check if the resource has a timestamp
    // if no, set it to the current timestamp
    int64_t timestamp;

    try {
        timestamp = j["timestamp"];
    } catch(nlohmann::detail::type_error e) {
        // Use current time as timestamp
        timestamp = duration_cast<seconds>(system_clock::now().time_since_epoch()).count();
    }

    std::string uuid = j["uuid"];
    std::string subject = j["subject"];

    auto resource = makeResource(Uuid(uuid), subject, timestamp);

    // Add all relations to the resource
    if (j.find("relations") != j.end()) {
        for (auto relation : j["relations"].get<json::object_t>()) {
            ResourceRelation rr {
                relation.second,    // predicate
                relation.first,     // object
                nullptr,            // resource ptr
                false               // needsWrite
            };

            resource->addRelation(rr);
        }    
    }

    file.close();
    return resource;
}

//
// Will be used to serialize the given resource to json
//
void to_json(json& j, Resource& resource) {
    j = {
        {"uuid", resource.getUuid()},
        {"subject", resource.getSubject()},
        {"timestamp", resource.getTimestamp()}
    };

    for (const auto relation : resource) {
        j["relations"].emplace(relation.object, relation.predicate);
    }
}
