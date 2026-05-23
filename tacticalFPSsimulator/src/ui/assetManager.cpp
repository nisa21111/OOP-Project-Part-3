#include "ui/assetManager.h"
#include "logger.h"
#include <SFML/Graphics.hpp>
#include <algorithm>

AssetManager& AssetManager::instance() {
    static AssetManager inst;
    return inst;
}

std::shared_ptr<sf::Texture> AssetManager::getTexture(const std::string& path) {
    auto it = textures_.find(path);
    if (it != textures_.end()) return it->second;

    auto tex = std::make_shared<sf::Texture>();


    if (!tex->loadFromFile(path)) {
        LOG_WARN("Textura negasita: " + path + " — folosesc placeholder");
        return getPlaceholder(path, 256, 256, sf::Color(80, 80, 80));
    }

    textures_[path] = tex;
    LOG_INFO("Loaded texture: " + path);
    return tex;
}

std::shared_ptr<sf::Font> AssetManager::getFont(const std::string& path) {
    auto it = fonts_.find(path);
    if (it != fonts_.end()) return it->second;

    auto font = std::make_shared<sf::Font>();

    if (!font->openFromFile(path)) {
        LOG_ERROR("Font negasit: " + path);
        return nullptr;
    }

    fonts_[path] = font;
    return font;
}

std::shared_ptr<sf::Texture> AssetManager::getPlaceholder(const std::string& label,
    unsigned int w, unsigned int h,
    sf::Color color) {
    std::string key = "__placeholder_" + label;
    auto it = textures_.find(key);
    if (it != textures_.end()) return it->second;

    sf::Image img({ w, h }, color);

    for (unsigned int x = 0; x < w; ++x) {
        img.setPixel({ x, 0 }, sf::Color::White);
        img.setPixel({ x, h - 1 }, sf::Color::White);
    }
    for (unsigned int y = 0; y < h; ++y) {
        img.setPixel({ 0,     y }, sf::Color::White);
        img.setPixel({ w - 1, y }, sf::Color::White);
    }
    for (unsigned int i = 0; i < std::min(w, h); ++i) {
        img.setPixel({ i,         i }, sf::Color(200, 200, 200, 120));
        img.setPixel({ w - 1 - i, i }, sf::Color(200, 200, 200, 120));
    }

    auto tex = std::make_shared<sf::Texture>();
    if (!tex->loadFromImage(img)) {
        LOG_ERROR("Nu pot crea placeholder texture pentru: " + label);

        auto tinyTex = std::make_shared<sf::Texture>();
        if (tinyTex->loadFromImage(sf::Image({ 1, 1 }, color))) {
            textures_[key] = tinyTex;
            return tinyTex;
        }
        return nullptr;
    }

    textures_[key] = tex;
    return tex;
}

void AssetManager::clearCache() {
    textures_.clear();
    fonts_.clear();
}