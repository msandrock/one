#include <fstream>      // std::ifstream
#include <sstream>      // std::stringstream
#include <string>       // std::string
#include "../include/one/config.hpp"

Config::Config() {
    std::string line, key, value;
    std::ifstream configFile("config.cfg");

    while (std::getline(configFile, line)) {
        std::stringstream ss;
        ss.str(line);

        std::getline(ss, key, '=');
        std::getline(ss, value, '=');

        config.insert(std::make_pair(key, value));
    }

    configFile.close();
}

std::string Config::get(const std::string& key) const {
    return config.find(key)->second;
}