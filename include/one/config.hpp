#pragma once
#include <map>  // std::map

class Config {
    std::map<std::string, std::string> config;
public:
    Config();
    std::string get(const std::string& key) const;
};