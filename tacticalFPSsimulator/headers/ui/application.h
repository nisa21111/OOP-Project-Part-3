#pragma once
#include "ui/screen.h"
#include "ui/mainMenuScreen.h"
#include "ui/gameScreen.h"
#include "ui/statsScreen.h"
#include "ui/assetManager.h"
#include "gameMap.h"
#include "agent.h"
#include "agentFactory.h"
#include "repository.h"
#include "observer.h"
#include "outcomeStrategy.h"
#include <SFML/Graphics.hpp>
#include <memory>
#include <string>

class Application {
    sf::RenderWindow window_;
    sf::Clock        clock_;

    std::unique_ptr<MainMenuScreen> menuScreen_;
    std::unique_ptr<GameScreen>     gameScreen_;
    std::unique_ptr<StatsScreen>    statsScreen_;

    Screen* activeScreen_ = nullptr;

    Repository<GameMap>       mapRepo_;
    std::shared_ptr<EventBus> eventBus_;
    std::unique_ptr<Agent>    currentAgent_;

    std::string lastMapId_;
    std::string lastAgentId_;
    std::string lastStratId_;

    int sessionRounds_ = 0;
    int sessionWins_ = 0;

    void loadMaps();
    void initMenuScreen();
    void startGame(const std::string& mapId,
        const std::string& agentId,
        const std::string& stratId);
    void handleTransition(const ScreenResult& res);
    std::unique_ptr<IOutcomeStrategy> buildStrategy(const std::string& stratId);

public:
    Application();
    ~Application() = default;
    void run();
};