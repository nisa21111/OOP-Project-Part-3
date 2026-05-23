#pragma once
#include <SFML/Graphics.hpp>
#include <unordered_map>
#include <string>
#include <memory>
#include <optional>

class AssetManager {
    std::unordered_map<std::string, std::shared_ptr<sf::Texture>> textures_;
    std::unordered_map<std::string, std::shared_ptr<sf::Font>>    fonts_;

    AssetManager() = default;

public:
    static AssetManager& instance();

    AssetManager(const AssetManager&) = delete;
    AssetManager& operator=(const AssetManager&) = delete;
    AssetManager(AssetManager&&) = delete;
    AssetManager& operator=(AssetManager&&) = delete;

    std::shared_ptr<sf::Texture> getTexture(const std::string& path);

    std::shared_ptr<sf::Font> getFont(const std::string& path);

    std::shared_ptr<sf::Texture> getPlaceholder(const std::string& label,
        unsigned int w, unsigned int h,
        sf::Color color);
    void clearCache();
};
