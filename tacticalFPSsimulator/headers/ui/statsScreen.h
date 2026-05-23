#pragma once
#include "ui/screen.h"
#include "ui/widgets.h"
#include "ui/assetManager.h"
#include "observer.h"
#include <memory>
#include <string>
#include <vector>

class StatsScreen : public Screen {
    sf::Vector2u              winSize_;
    std::shared_ptr<sf::Font> font_;

    bool        won_ = false;
    std::string agentId_;
    std::string mapId_;
    int         teamCoord_ = 5;
    int         mapControl_ = 5;
    int         totalRounds_ = 0;
    int         totalWins_ = 0;

    BackgroundLayer           background_;
    sf::RectangleShape        resultBanner_;
    sf::Text                  resultText_;
    sf::Text                  subtitleText_;

    sf::RectangleShape        statPanel_;
    std::vector<sf::Text> statLines_;

    std::unique_ptr<Button>   playAgainBtn_;
    std::unique_ptr<Button>   mainMenuBtn_;
    std::unique_ptr<Button>   quitBtn_;

    float                     bannerScale_ = 0.f;
    bool                      animDone_ = false;

    ScreenResult              result_;

    void buildLayout();
    void parsePayload(const std::string& payload);

public:
    StatsScreen(sf::Vector2u windowSize, int& totalRounds, int& totalWins);

    void onEnter(const std::string& payload = "") override;
    void handleEvent(const sf::Event& event) override;
    void update(sf::Time dt) override;
    void draw(sf::RenderWindow& window) override;
    [[nodiscard]] ScreenResult result() const override { return result_; }
};
