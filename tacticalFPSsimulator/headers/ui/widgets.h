#pragma once
#include <cstdint> 
#include <SFML/Graphics.hpp>
#include <string>
#include <functional>
#include <memory>
#include <vector>

namespace Theme {
    inline const sf::Color BgDark = sf::Color(15, 15, 20);
    inline const sf::Color PanelBg = sf::Color(22, 22, 32, 220);
    inline const sf::Color AccentRed = sf::Color(255, 70, 70);
    inline const sf::Color AccentGold = sf::Color(255, 200, 60);
    inline const sf::Color TextPrimary = sf::Color(240, 240, 240);
    inline const sf::Color TextSecondary = sf::Color(160, 160, 160);
    inline const sf::Color BtnNormal = sf::Color(40, 40, 60, 230);
    inline const sf::Color BtnHover = sf::Color(60, 60, 100, 255);
    inline const sf::Color BtnPressed = sf::Color(255, 70, 70, 255);
    inline const sf::Color BtnBorder = sf::Color(100, 100, 150);
    inline const sf::Color BtnBorderHov = sf::Color(255, 200, 60);
}

class Button {
    sf::RectangleShape shape_;
    sf::Text           label_;
    sf::Text           keyHint_;
    bool               hovered_ = false;
    bool               pressed_ = false;
    bool               disabled_ = false;
    std::string        actionKey_;

    void refreshColors();

public:
    Button(const sf::Font& font, const std::string& text,
        const std::string& key, sf::Vector2f pos, sf::Vector2f size);

    void setPosition(sf::Vector2f pos);
    void setSize(sf::Vector2f size);
    void setDisabled(bool d);

    bool handleEvent(const sf::Event& event);
    void draw(sf::RenderWindow& window) const;

    [[nodiscard]] bool      isHovered()  const { return hovered_; }
    [[nodiscard]] const std::string& key() const { return actionKey_; }
};

class TextPanel {
    sf::RectangleShape background_;
    sf::Text           text_;
    sf::Text           titleText_;
    bool               hasTitle_ = false;

public:
    TextPanel(const sf::Font& font, sf::Vector2f pos, sf::Vector2f size,
        unsigned int charSize = 18);

    void setText(const std::string& text);
    void setTitle(const std::string& title);
    void setPosition(sf::Vector2f pos);
    void draw(sf::RenderWindow& window) const;
};

class AgentPortrait {
    std::unique_ptr<sf::Sprite> portrait_; 
    sf::RectangleShape         bubble_;
    sf::Text                   speechText_;
    sf::Text                   nameText_;
    sf::RectangleShape         nameBar_;

    std::shared_ptr<sf::Texture> texture_;
    bool textureLoaded_ = false;

    float slideOffset_ = 300.f;
    bool  animating_ = true;

public:
    AgentPortrait(const sf::Font& font, sf::Vector2f pos, sf::Vector2f portraitSize);

    void loadTexture(std::shared_ptr<sf::Texture> tex);
    void setSpeech(const std::string& text);
    void setAgentName(const std::string& name);
    void update(sf::Time dt);
    void draw(sf::RenderWindow& window) const;
    void resetAnimation();
};

class BackgroundLayer {
    std::unique_ptr<sf::Sprite>   sprite_;
    sf::RectangleShape            vignette_;
    sf::RectangleShape            overlay_;
    std::shared_ptr<sf::Texture>  texture_;

public:
    BackgroundLayer();
    void loadTexture(std::shared_ptr<sf::Texture> tex, sf::Vector2u windowSize);
    void setOverlayAlpha(std::uint8_t alpha); 
    void draw(sf::RenderWindow& window) const;
};