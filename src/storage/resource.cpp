#include <string>       // std::string
#include <memory>       // std::shared_ptr, std::make_shared
#include "../../include/one/resource.hpp"

using namespace std::chrono;

//
// Create a new resource and return a shared pointer to it
//

sp_resource makeRootResource() {
    Uuid root;
    int64_t timestamp = duration_cast<seconds>(system_clock::now().time_since_epoch()).count();
    auto resource = std::make_shared<Resource>(root, "root", timestamp);

    return resource;
}

sp_resource makeResource(const std::string& subject) {
    auto resource = std::make_shared<Resource>(subject);

    return resource;
}

sp_resource makeResource(const Uuid& uuid, const std::string& subject, int64_t timestamp) {
    auto resource = std::make_shared<Resource>(uuid, subject, timestamp);

    return resource;
}
