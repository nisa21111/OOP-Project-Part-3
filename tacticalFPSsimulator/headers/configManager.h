#pragma once
#include <string>
#include <unordered_map>
#include <optional>

class ConfigManager {
    std::unordered_map<std::string, std::string> config_;
    std::string configPath_;

    ConfigManager() = default;
    void load(const std::string& path);

public:
    static ConfigManager& instance();

    ConfigManager(const ConfigManager&) = delete;
    ConfigManager& operator=(const ConfigManager&) = delete;
    ConfigManager(ConfigManager&&) = delete;
    ConfigManager& operator=(ConfigManager&&) = delete;

    void loadFromFile(const std::string& path);

    [[nodiscard]] std::optional<std::string> get(const std::string& key) const;
    [[nodiscard]] std::string get(const std::string& key, const std::string& def) const;
    [[nodiscard]] int  getInt(const std::string& key, int  def = 0)     const;
    [[nodiscard]] bool getBool(const std::string& key, bool def = false)  const;

};