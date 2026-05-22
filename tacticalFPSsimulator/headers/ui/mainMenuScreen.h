#pragma once
#include "ui/screen.h"
#include "ui/widgets.h"
#include "ui/assetManager.h"
#include "repository.h"
#include "gameMap.h"
#include "agentFactory.h"
#include <memory>
#include <vector>
#include <string>

class MainMenuScreen : public Screen {
    sf::Vector2u              winSize_;
    BackgroundLayer           background_;
    std::shared_ptr<sf::Font> font_;

    sf::Text titleText_;
    sf::Text subtitleText_;
    sf::RectangleShape titleBar_;

    sf::Text             mapLabel_;
    std::vector<Button>  mapButtons_;
    int                  selectedMap_ = 0;

    sf::Text             agentLabel_;
    std::vector<Button>  agentButtons_;
    int                  selectedAgent_ = 0;

    sf::Text             stratLabel_;
    std::vector<Button>  stratButtons_;
    int                  selectedStrat_ = 0;

    std::unique_ptr<Button> startButton_;

    std::vector<std::string> mapIds_;
    std::vector<std::string> agentIds_;
    std::vector<std::string> stratIds_;

    ScreenResult result_;

    void buildLayout();
    void updateSelectionVisuals();

public:
    MainMenuScreen(sf::Vector2u windowSize,
        const std::vector<std::string>& availableMaps,
        const std::vector<std::string>& availableAgents);

    void onEnter(const std::string& payload = "") override;
    void handleEvent(const sf::Event& event) override;
    void update(sf::Time dt) override;
    void draw(sf::RenderWindow& window) override;
    [[nodiscard]] ScreenResult result() const override { return result_; }

    [[nodiscard]] const std::string& selectedMapId()   const { return mapIds_[selectedMap_]; }
    [[nodiscard]] const std::string& selectedAgentId() const { return agentIds_[selectedAgent_]; }
    [[nodiscard]] const std::string& selectedStratId() const { return stratIds_[selectedStrat_]; }
};
