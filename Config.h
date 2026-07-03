#pragma once
#include <string>
#include <fstream>
#include <unordered_map>

class Config {
public:
    std::unordered_map<std::string, double> values;
    bool load(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) return false;

    std::string line;

    while (std::getline(file, line)) {
        auto pos = line.find('=');

        if (pos == std::string::npos)
            continue;

        std::string key = line.substr(0, pos);
        double value = std::stod(line.substr(pos + 1));

        values[key] = value;
    }

    return true;
}

    double get(const std::string& key, double defaultValue) {
        return values.count(key) ? values[key] : defaultValue;
    }
};