#include "configManager.h"
#include <fstream>
#include <iostream>
#include <algorithm>

ConfigManager& ConfigManager::instance() {
    static ConfigManager inst;
    return inst;
}

static void trim(std::string& s) {
    s.erase(0, s.find_first_not_of(" \t\r\n"));
    auto last = s.find_last_not_of(" \t\r\n");
    if (last != std::string::npos) s.erase(last + 1);
}

void ConfigManager::load(const std::string& path) {
    configPath_ = path;
    std::ifstream f(path);
    if (!f.is_open()) {
        std::cerr << "[Config] Cannot open " << path << ", using defaults.\n";
        return;
    }
    std::string line;
    while (std::getline(f, line)) {
        if (line.empty() || line[0] == '#') continue;
        auto eq = line.find('=');
        if (eq == std::string::npos) continue;
        std::string key = line.substr(0, eq);
        std::string value = line.substr(eq + 1);
        trim(key);
        trim(value);
        config_[key] = value;
    }
}

void ConfigManager::loadFromFile(const std::string& path) { load(path); }

std::optional<std::string> ConfigManager::get(const std::string& key) const {
    auto it = config_.find(key);
    if (it == config_.end()) return std::nullopt;
    return it->second;
}

std::string ConfigManager::get(const std::string& key, const std::string& def) const {
    auto v = get(key);
    return v ? *v : def;
}

int ConfigManager::getInt(const std::string& key, int def) const {
    auto v = get(key);
    if (!v) return def;
    try { return std::stoi(*v); }
    catch (...) { return def; }
}

bool ConfigManager::getBool(const std::string& key, bool def) const {
    auto v = get(key);
    if (!v) return def;
    return *v == "true" || *v == "1" || *v == "yes";
}

void ConfigManager::printAll() const {
    std::cout << "-- Config (" << configPath_ << ") --\n";
    for (auto& [k, v] : config_) std::cout << "  " << k << " = " << v << "\n";
}