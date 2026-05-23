#include "ui/application.h"
#include "configManager.h"
#include "logger.h"
#include "exceptions.h"
#include <filesystem>
#include <algorithm>
#include <sstream>


Application::Application() {
    ConfigManager::instance().loadFromFile("data/config/settings.cfg");

    int W = ConfigManager::instance().getInt("window_width", 1280);
    int H = ConfigManager::instance().getInt("window_height", 720);
    std::string title = ConfigManager::instance().get("window_title", "Tactical FPS Simulator");

    window_.create(sf::VideoMode({ static_cast<unsigned>(W),
                                   static_cast<unsigned>(H) }),
        title, sf::Style::Close);
    window_.setFramerateLimit(60);

    eventBus_ = std::make_shared<EventBus>();

    loadMaps();
    initMenuScreen();

    LOG_INFO("Application initialized " + std::to_string(W) + "x" + std::to_string(H));
}


void Application::loadMaps() {
    namespace fs = std::filesystem;

    try {
        for (auto& entry : fs::directory_iterator("data/maps")) {
            if (entry.path().extension() == ".map") {
                try {
                    auto map = GameMap::loadFromFile(entry.path().string());
                    std::string id = map.id();
                    mapRepo_.add(id, std::make_shared<GameMap>(std::move(map)));
                    LOG_INFO("Loaded map: " + id);
                }
                catch (const GameException& e) {
                    LOG_ERROR("Failed to load map: " + std::string(e.what()));
                }
            }
        }
    }
    catch (...) {
        LOG_WARN("data/maps directory not found or empty");
    }

    if (mapRepo_.empty()) {
        LOG_WARN("No maps loaded — creating fallback map");

        auto fallback = std::make_shared<GameMap>("ascent");
        fallback->setDefaultBackground("data/images/ascent.png");

        auto n0 = std::make_shared<ScenarioNode>("start", "start",
            "Attacker spawn, Ascent. The round starts. Where does the team push?",
            NodeType::NARRATIVE);
        n0->addChoice({ "a", "Push A site through main",    "a_main",   std::nullopt });
        n0->addChoice({ "b", "Push B site via market",      "b_push",   std::nullopt });
        n0->addChoice({ "m", "Contest mid control first",   "mid_fight","ROTATE" });

        auto nA = std::make_shared<ScenarioNode>("a_main", "spike_dropped",
            "You push A main. The spike carrier gets caught and drops the spike!",
            NodeType::EVENT);
        nA->addChoice({ "p", "Pick up the spike and plant", "a_plant", "SPIKE_DROPPED" });
        nA->addChoice({ "r", "Rotate to B with teammates",  "b_rotate","TEAM_IGNORED" });

        auto nAPlant = std::make_shared<ScenarioNode>("a_plant", "win",
            "You plant under pressure. Defenders can't defuse in time.", NodeType::OUTCOME);
        nAPlant->setTerminal(true);

        auto nBPush = std::make_shared<ScenarioNode>("b_push", "start",
            "B market push. Your Jett entries and flashes the site.", NodeType::NARRATIVE);
        nBPush->addChoice({ "f", "Follow Jett in immediately",  "b_win",  std::nullopt });
        nBPush->addChoice({ "w", "Wait and let her solo entry", "b_lose", "TEAM_IGNORED" });

        auto nBWin = std::make_shared<ScenarioNode>("b_win", "win",
            "Full team execute. Spike planted, round won!", NodeType::OUTCOME);
        nBWin->setTerminal(true);

        auto nBLose = std::make_shared<ScenarioNode>("b_lose", "lose",
            "Jett gets traded alone. Defenders retake easily.", NodeType::OUTCOME);
        nBLose->setTerminal(false);

        auto nMid = std::make_shared<ScenarioNode>("mid_fight", "rotate",
            "Mid fight breaks out. Your team wins. B link is now open.", NodeType::EVENT);
        nMid->addChoice({ "b", "Rotate B through mid link", "b_win",   std::nullopt });
        nMid->addChoice({ "a", "Rotate A through B-link",   "a_plant", std::nullopt });

        auto nBRotate = std::make_shared<ScenarioNode>("b_rotate", "lose",
            "Rotation is too slow. Spike timer runs out. Round lost.", NodeType::OUTCOME);
        nBRotate->setTerminal(false);

        fallback->addNode(n0);
        fallback->addNode(nA);
        fallback->addNode(nAPlant);
        fallback->addNode(nBPush);
        fallback->addNode(nBWin);
        fallback->addNode(nBLose);
        fallback->addNode(nMid);
        fallback->addNode(nBRotate);
        fallback->setStartNode("start");

        mapRepo_.add("ascent", std::move(fallback));
    }
}


void Application::initMenuScreen() {
    auto winSize = window_.getSize();
    auto maps = mapRepo_.keys();
    auto agents = AgentFactory::instance().available();
    std::sort(agents.begin(), agents.end());

    menuScreen_ = std::make_unique<MainMenuScreen>(winSize, maps, agents);
    statsScreen_ = std::make_unique<StatsScreen>(winSize, sessionRounds_, sessionWins_);

    menuScreen_->onEnter();
    activeScreen_ = menuScreen_.get();
}


std::unique_ptr<IOutcomeStrategy> Application::buildStrategy(const std::string& stratId) {
    if (stratId == "aggressive")  return std::make_unique<AggressiveStrategy>();
    if (stratId == "coordinated") return std::make_unique<CoordinatedStrategy>();
    return std::make_unique<BalancedStrategy>();
}


void Application::startGame(const std::string& mapId,
    const std::string& agentId,
    const std::string& stratId) {
    lastMapId_ = mapId;
    lastAgentId_ = agentId;
    lastStratId_ = stratId;

    auto mapOpt = mapRepo_.find(mapId);
    if (!mapOpt) {
        LOG_ERROR("Map not found: " + mapId);
        return;
    }

    currentAgent_ = AgentFactory::instance().create(agentId);

    gameScreen_ = std::make_unique<GameScreen>(
        window_.getSize(),
        std::const_pointer_cast<const GameMap>(*mapOpt),
        *currentAgent_,
        buildStrategy(stratId),
        eventBus_);

    gameScreen_->onEnter();
    activeScreen_ = gameScreen_.get();
    LOG_INFO("Game started: map=" + mapId + " agent=" + agentId + " strat=" + stratId);
}


void Application::handleTransition(const ScreenResult& res) {
    if (res.status == ScreenStatus::Running) return;

    if (res.status == ScreenStatus::Quit) {
        window_.close();
        return;
    }

    if (res.nextScreen == "menu") {
        menuScreen_->onEnter();
        activeScreen_ = menuScreen_.get();
        return;
    }

    if (res.nextScreen == "game") {
        std::istringstream ss(res.payload);
        std::string mapId, agentId, stratId;
        std::getline(ss, mapId, '|');
        std::getline(ss, agentId, '|');
        std::getline(ss, stratId, '|');
        startGame(mapId, agentId, stratId);
        return;
    }

    if (res.nextScreen == "game_same") {
        startGame(lastMapId_, lastAgentId_, lastStratId_);
        return;
    }

    if (res.nextScreen == "stats") {
        statsScreen_->onEnter(res.payload);
        activeScreen_ = statsScreen_.get();
        return;
    }
}


void Application::run() {
    while (window_.isOpen()) {
        sf::Time dt = clock_.restart();

        while (auto event = window_.pollEvent()) {
            if (event->is<sf::Event::Closed>()) {
                window_.close();
                break;
            }
            if (activeScreen_) activeScreen_->handleEvent(*event);
        }

        if (!window_.isOpen()) break;

        if (activeScreen_) {
            activeScreen_->update(dt);
            ScreenResult res = activeScreen_->result();
            if (res.status != ScreenStatus::Running)
                handleTransition(res);
        }

        window_.clear(Theme::BgDark);
        if (activeScreen_) activeScreen_->draw(window_);
        window_.display();
    }

    LOG_INFO("Application closed. Rounds=" + std::to_string(sessionRounds_) +
        " Wins=" + std::to_string(sessionWins_));
}