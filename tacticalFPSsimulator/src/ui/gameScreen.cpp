#include "ui/gameScreen.h"
#include "logger.h"
#include "exceptions.h"
#include <algorithm>
#include <sstream>

GameScreen::GameScreen(sf::Vector2u windowSize,
    std::shared_ptr<const GameMap> map,
    const Agent& agent,
    std::unique_ptr<IOutcomeStrategy> strategy,
    std::shared_ptr<EventBus> eventBus)
    : winSize_(windowSize)
    , map_(std::move(map))
    , agent_(&agent)
    , strategy_(std::move(strategy))
    , eventBus_(std::move(eventBus))
    , portrait_(*AssetManager::instance().getFont("data/fonts/Rajdhani-Bold.ttf"),
        { 20.f, static_cast<float>(windowSize.y) - 280.f }, { 220.f, 260.f })
    , narrativePanel_(
        *AssetManager::instance().getFont("data/fonts/Rajdhani-Bold.ttf"),
        { 20.f, 60.f },
        { static_cast<float>(windowSize.x) - 40.f, 160.f }, 18)

    , mapNameText_(*AssetManager::instance().getFont("data/fonts/Rajdhani-Bold.ttf"), "", 22)
    , coordText_(*AssetManager::instance().getFont("data/fonts/Rajdhani-Bold.ttf"), "TEAM COORD", 12)
    , controlText_(*AssetManager::instance().getFont("data/fonts/Rajdhani-Bold.ttf"), "MAP CTRL", 12)
{
    font_ = AssetManager::instance().getFont("data/fonts/Rajdhani-Bold.ttf");
    if (!font_) font_ = AssetManager::instance().getFont("data/fonts/font.ttf");

    float W = static_cast<float>(winSize_.x);
    float H = static_cast<float>(winSize_.y);

    auto bgTex = AssetManager::instance().getTexture(
        map_->defaultBackground().empty() ? "data/images/ascent.png"
        : map_->defaultBackground());
    background_.loadTexture(bgTex, winSize_);
    background_.setOverlayAlpha(100);

    auto agentTex = AssetManager::instance().getTexture(agent_->portraitPath());
    portrait_.loadTexture(agentTex);
    portrait_.setAgentName(agent_->name());

    hudBar_.setSize({ W, 44.f });
    hudBar_.setFillColor(sf::Color(10, 10, 15, 220));
    hudBar_.setPosition({ 0.f, 0.f });

    mapNameText_.setString(map_->name());
    mapNameText_.setFillColor(Theme::AccentRed);
    mapNameText_.setStyle(sf::Text::Bold);
    mapNameText_.setPosition({ 16.f, 10.f });

    coordBarBg_.setSize({ 120.f, 12.f });
    coordBarBg_.setFillColor(sf::Color(40, 40, 40));
    coordBarBg_.setPosition({ W - 280.f, 10.f });

    coordBarFill_.setSize({ 60.f, 12.f });
    coordBarFill_.setFillColor(sf::Color(60, 180, 255));
    coordBarFill_.setPosition({ W - 280.f, 10.f });

    coordText_.setFillColor(Theme::TextSecondary);
    coordText_.setPosition({ W - 280.f, 24.f });

    controlBarBg_.setSize({ 120.f, 12.f });
    controlBarBg_.setFillColor(sf::Color(40, 40, 40));
    controlBarBg_.setPosition({ W - 140.f, 10.f });

    controlBarFill_.setSize({ 60.f, 12.f });
    controlBarFill_.setFillColor(sf::Color(255, 160, 40));
    controlBarFill_.setPosition({ W - 140.f, 10.f });

    controlText_.setFillColor(Theme::TextSecondary);
    controlText_.setPosition({ W - 140.f, 24.f });

    fadeOverlay_.setSize({ W, H });
    fadeOverlay_.setFillColor(sf::Color(0, 0, 0, 0));
}

void GameScreen::onEnter(const std::string&) {
    showingTerminal_ = false;
    terminalButtons_.clear();

    result_ = { ScreenStatus::Running, "", "" };
    roundResult_ = {};
    roundResult_.agentId = agent_->name();
    roundResult_.mapId = map_->name();

    try {
        auto startNode = map_->getNode(map_->startNodeId());
        loadNode(startNode);
    }
    catch (const GameException& e) {
        LOG_ERROR(e.what());
        result_ = { ScreenStatus::ReplaceScreen, "stats", "error" };
    }
}

void GameScreen::refreshBackground(const std::string& bgPath) {
    std::string path = bgPath.empty() ? map_->defaultBackground() : bgPath;
    if (path.empty()) path = "data/images/ascent.png";
    auto tex = AssetManager::instance().getTexture(path);
    background_.loadTexture(tex, winSize_);
}

void GameScreen::buildTerminalButtons() {
    terminalButtons_.clear();
    float W = static_cast<float>(winSize_.x);
    float H = static_cast<float>(winSize_.y);
    float btnW = 240.f;
    float btnH = 52.f;
    float gap = 20.f;
    float totalW = 3 * btnW + 2 * gap;
    float startX = W / 2.f - totalW / 2.f;
    float Y = H - 100.f;

    terminalButtons_.emplace_back(*font_, "SEE STATS", "S", sf::Vector2f(startX, Y), sf::Vector2f(btnW, btnH));
    terminalButtons_.emplace_back(*font_, "PLAY AGAIN", "R", sf::Vector2f(startX + btnW + gap, Y), sf::Vector2f(btnW, btnH));
    terminalButtons_.emplace_back(*font_, "MAIN MENU", "M", sf::Vector2f(startX + 2 * (btnW + gap), Y), sf::Vector2f(btnW, btnH));
}

void GameScreen::loadNode(const std::shared_ptr<ScenarioNode>& node) {
    currentNode_ = node;

    refreshBackground(node->backgroundImage());

    narrativePanel_.setTitle("SITUATION");
    narrativePanel_.setText(node->narrative());

    portrait_.setSpeech(agent_->callout(node->calloutKey()));
    portrait_.resetAnimation();

    buildChoiceButtons();
    updateHUD();

    if (node->isTerminal()) {
        roundResult_.won = node->isWin();
        roundResult_.teamCoord = teamCoordination_;
        roundResult_.mapControl = mapControl_;
        showingTerminal_ = true;
        buildTerminalButtons();
    }

    LOG_INFO("GameScreen loaded node: " + node->id());
}

void GameScreen::buildChoiceButtons() {
    choiceButtons_.clear();
    if (!currentNode_ || currentNode_->isTerminal()) return;

    float W = static_cast<float>(winSize_.x);
    float H = static_cast<float>(winSize_.y);
    float btnW = 320.f;
    float btnH = 52.f;
    float gap = 12.f;

    float totalH = currentNode_->choices().size() * (btnH + gap) - gap;

    float startY = H - totalH - 20.f;

    float startX = W - btnW - 30.f;

    for (size_t i = 0; i < currentNode_->choices().size(); ++i) {
        const auto& c = currentNode_->choices()[i];
        std::string label = "[" + c.key + "]  " + c.label;
        choiceButtons_.emplace_back(
            *font_, label, c.key,
            sf::Vector2f(startX, startY + i * (btnH + gap)),
            sf::Vector2f(btnW, btnH));
    }
}

void GameScreen::updateHUD() {
    float W = static_cast<float>(winSize_.x);
    float coordW = (teamCoordination_ / 10.f) * 120.f;
    float ctrlW = (mapControl_ / 10.f) * 120.f;
    coordBarFill_.setSize({ coordW, 12.f });
    controlBarFill_.setSize({ ctrlW, 12.f });
    (void)W;
}

void GameScreen::applyEventEffect(const std::string& eventType) {
    int penalty = 2;
    int bonus = 1;
    if (strategy_->name() == "Aggressive") {
        penalty = 1;
        bonus = 2;
    }
    else if (strategy_->name() == "Coordinated") {
        penalty = 3;
        bonus = 1;
    }

    if (eventType == "SPIKE_DROPPED") {
        mapControl_ = std::max(0, mapControl_ - penalty);
        eventBus_->notify({ "SPIKE_DROPPED", "Spike dropped mid-round" });
    }
    else if (eventType == "TEAM_IGNORED") {
        teamCoordination_ = std::max(0, teamCoordination_ - penalty);
        eventBus_->notify({ "TEAM_IGNORED", "Team didn't follow the call" });
    }
    else if (eventType == "ROTATE") {
        mapControl_ = std::min(10, mapControl_ + bonus);
        eventBus_->notify({ "ROTATE", "Mid-round rotation" });
    }
    updateHUD();
}

void GameScreen::startFadeToNode(const std::string& nodeId) {
    fadingOut_ = true;
    pendingNodeId_ = nodeId;
    pendingExit_ = false;
}

void GameScreen::startFadeToExit() {
    fadingOut_ = true;
    pendingExit_ = true;
}

void GameScreen::handleEvent(const sf::Event& event) {
    if (fadingOut_) return;
    if (!currentNode_) return;

    if (const auto* kp = event.getIf<sf::Event::KeyPressed>()) {
        if (kp->code == sf::Keyboard::Key::Escape) {
            result_ = { ScreenStatus::ReplaceScreen, "menu", "" };
            return;
        }
        if (!currentNode_->isTerminal()) {
            for (auto& c : currentNode_->choices()) {
                if (c.key.size() == 1) {
                    int keyIndex = static_cast<int>(kp->code)
                        - static_cast<int>(sf::Keyboard::Key::A);
                    char keyChar = 'a' + keyIndex;
                    if (keyIndex >= 0 && keyIndex < 26 && c.key[0] == keyChar) {
                        if (c.eventTrigger) applyEventEffect(*c.eventTrigger);
                        startFadeToNode(c.nextNodeId);
                        return;
                    }
                }
            }
        }
    }
    if (showingTerminal_) {

        if (terminalButtons_[0].handleEvent(event)) {
            startFadeToExit();
            return;
        }
        if (terminalButtons_[1].handleEvent(event)) {
            result_ = { ScreenStatus::ReplaceScreen, "game_same", "" };
            return;
        }
        if (terminalButtons_[2].handleEvent(event)) {
            result_ = { ScreenStatus::ReplaceScreen, "menu", "" };
            return;
        }
        if (const auto* kp = event.getIf<sf::Event::KeyPressed>()) {
            if (kp->code == sf::Keyboard::Key::S) { startFadeToExit(); return; }
            if (kp->code == sf::Keyboard::Key::R) {
                result_ = { ScreenStatus::ReplaceScreen, "game_same", "" };
                return;
            }
            if (kp->code == sf::Keyboard::Key::M) {
                result_ = { ScreenStatus::ReplaceScreen, "menu", "" };
                return;
            }
        }
        return;
    }

    if (!currentNode_->isTerminal()) {
        for (size_t i = 0; i < choiceButtons_.size(); ++i) {
            if (choiceButtons_[i].handleEvent(event)) {
                const auto& c = currentNode_->choices()[i];
                if (c.eventTrigger) applyEventEffect(*c.eventTrigger);
                startFadeToNode(c.nextNodeId);
                LOG_INFO("Click choice: " + c.key + " -> " + c.nextNodeId);
                return;
            }
        }
    }
}

void GameScreen::update(sf::Time dt) {
    portrait_.update(dt);

    constexpr float FADE_SPEED = 280.f;

    if (fadingOut_) {
        fadeAlpha_ = std::min(fadeAlpha_ + FADE_SPEED * dt.asSeconds(), 255.f);
        fadeOverlay_.setFillColor(sf::Color(0, 0, 0, static_cast<std::uint8_t>(fadeAlpha_)));

        if (fadeAlpha_ >= 255.f) {
            fadingOut_ = false;
            if (pendingExit_) {
                bool stratOk = true;
                if (strategy_) {
                    stratOk = strategy_->resolve(teamCoordination_, mapControl_);
                }
                else {
                    LOG_WARN("Strategy missing in GameScreen! Defaulting to true.");
                }

                roundResult_.won = roundResult_.won && stratOk;

                std::string payload = roundResult_.won ? "win" : "lose";
                payload += "|" + roundResult_.agentId + "|" + roundResult_.mapId;
                payload += "|" + std::to_string(roundResult_.teamCoord);
                payload += "|" + std::to_string(roundResult_.mapControl);

                result_ = { ScreenStatus::ReplaceScreen, "stats", payload };
            }

            else if (!pendingNodeId_.empty()) {
                try {
                    auto nextNode = map_->getNode(pendingNodeId_);

                    if (!nextNode) {
                        LOG_ERROR("CRITICAL: Node '" + pendingNodeId_ + "' was not found in the map registry!");
                        result_ = { ScreenStatus::ReplaceScreen, "menu", "" }; 
                        pendingNodeId_.clear();
                        return;
                    }

                    loadNode(nextNode);
                    pendingNodeId_.clear();
                    fadeAlpha_ = 255.f;
                }
                catch (const GameException& e) {
                    LOG_ERROR(e.what());
                    result_ = { ScreenStatus::ReplaceScreen, "stats", "error" };
                }
            }
        }
    }
    else if (fadeAlpha_ > 0.f) {
        fadeAlpha_ = std::max(fadeAlpha_ - FADE_SPEED * dt.asSeconds(), 0.f);
        fadeOverlay_.setFillColor(sf::Color(0, 0, 0, static_cast<std::uint8_t>(fadeAlpha_)));
    }
}

void GameScreen::draw(sf::RenderWindow& window) {
    background_.draw(window);

    narrativePanel_.draw(window);
    portrait_.draw(window);

    for (auto& b : choiceButtons_) b.draw(window);

    if (showingTerminal_) {
        for (auto& b : terminalButtons_) b.draw(window);
    }

    window.draw(hudBar_);
    window.draw(mapNameText_);
    window.draw(coordBarBg_);
    window.draw(coordBarFill_);
    window.draw(coordText_);
    window.draw(controlBarBg_);
    window.draw(controlBarFill_);
    window.draw(controlText_);

    if (fadeAlpha_ > 0.f) window.draw(fadeOverlay_);
}