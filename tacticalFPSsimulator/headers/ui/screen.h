#pragma once
#include <SFML/Graphics.hpp>
#include <string>

enum class ScreenStatus {
    Running,   
    PushScreen, 
    PopScreen,      
    ReplaceScreen, 
    Quit    
};

struct ScreenResult {
    ScreenStatus              status = ScreenStatus::Running;
    std::string               nextScreen; 
    std::string               payload;     
};

class Screen {
public:
    virtual ~Screen() = default;

    virtual void onEnter(const std::string& payload = "") {}

    virtual void onExit() {}

    virtual void handleEvent(const sf::Event& event) = 0;
    virtual void update(sf::Time dt) = 0;
    virtual void draw(sf::RenderWindow& window) = 0;

    [[nodiscard]] virtual ScreenResult result() const = 0;
};
