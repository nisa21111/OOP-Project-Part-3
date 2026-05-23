#pragma once
#include "ui/screen.h"
#include "ui/widgets.h"
#include "ui/assetManager.h"
#include "gameMap.h"
#include "agent.h"
#include "repository.h"
#include "observer.h"
#include "outcomeStrategy.h"
#include <memory>
#include <vector>
#include <string>
#include <functional>

struct RoundResult {
    bool        won = false;
    int         teamCoord = 5;
    int         mapControl = 5;
    std::string agentId;
    std::string mapId;
};

class GameScreen : public Screen {
    sf::Vector2u              winSize_;
    std::shared_ptr<sf::Font> font_;

    std::shared_ptr<const GameMap> map_; 
    const Agent* agent_;
    std::unique_ptr<IOutcomeStrategy>   strategy_;
    std::shared_ptr<EventBus>           eventBus_;

    int teamCoordination_ = 5;
    int mapControl_ = 5;

    std::shared_ptr<ScenarioNode> currentNode_;

    BackgroundLayer           background_;
    AgentPortrait             portrait_;
    TextPanel                 narrativePanel_;
    std::vector<Button>       choiceButtons_;

    sf::RectangleShape        hudBar_;
    sf::Text                  mapNameText_;
    sf::Text                  coordText_;
    sf::Text                  controlText_;
    sf::RectangleShape        coordBarBg_;
    sf::RectangleShape        coordBarFill_;
    sf::RectangleShape        controlBarBg_;
    sf::RectangleShape        controlBarFill_;

    ScreenResult result_;
    RoundResult  roundResult_;

    sf::RectangleShape fadeOverlay_;
    float              fadeAlpha_ = 0.f;
    bool               fadingOut_ = false;
    std::string        pendingNodeId_;
    bool               pendingExit_ = false;

    void loadNode(const std::shared_ptr<ScenarioNode>& node);
    void buildChoiceButtons();
    void updateHUD();
    void applyEventEffect(const std::string& eventType);
    void startFadeToNode(const std::string& nodeId);
    void startFadeToExit();
    void refreshBackground(const std::string& bgPath);

public:
    GameScreen(sf::Vector2u windowSize,
        std::shared_ptr<const GameMap> map_,
        const Agent& agent,
        std::unique_ptr<IOutcomeStrategy> strategy,
        std::shared_ptr<EventBus> eventBus);

    void onEnter(const std::string& payload = "") override;
    void handleEvent(const sf::Event& event) override;
    void update(sf::Time dt) override;
    void draw(sf::RenderWindow& window) override;
    [[nodiscard]] ScreenResult result() const override { return result_; }

    [[nodiscard]] const RoundResult& roundResult() const { return roundResult_; }
};
