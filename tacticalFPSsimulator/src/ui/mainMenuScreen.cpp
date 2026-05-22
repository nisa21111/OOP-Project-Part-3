#include "ui/mainMenuScreen.h"
#include "logger.h"
#include <algorithm>

static const std::vector<std::string> STRAT_IDS = { "balanced", "aggressive", "coordinated" };

MainMenuScreen::MainMenuScreen(sf::Vector2u windowSize,
    const std::vector<std::string>& availableMaps,
    const std::vector<std::string>& availableAgents)
    : winSize_(windowSize)
    , mapIds_(availableMaps)
    , agentIds_(availableAgents)
    , stratIds_(STRAT_IDS)
    , font_(AssetManager::instance().getFont("data/fonts/Rajdhani-Bold.ttf") ?
        AssetManager::instance().getFont("data/fonts/Rajdhani-Bold.ttf") :
        AssetManager::instance().getFont("data/fonts/font.ttf"))
    , titleText_(*font_, "", 42) 
    , subtitleText_(*font_, "", 18) 
    , mapLabel_(*font_, "", 22) 
    , agentLabel_(*font_, "", 22) 
    , stratLabel_(*font_, "", 22)
{
    auto bgTex = AssetManager::instance().getTexture("data/images/menu_bg.png");
    background_.loadTexture(bgTex, winSize_);
    background_.setOverlayAlpha(150);

    buildLayout();
}

void MainMenuScreen::buildLayout() {
    float W = static_cast<float>(winSize_.x);
    float H = static_cast<float>(winSize_.y);

    titleBar_.setSize({ W, 80.f });
    titleBar_.setFillColor(sf::Color(10, 10, 15, 220));
    titleBar_.setPosition({ 0.f, 0.f });

    titleText_ = sf::Text(*font_, "TACTICAL FPS SIMULATOR", 42);
    titleText_.setFillColor(Theme::AccentRed);
    titleText_.setStyle(sf::Text::Bold);
    sf::FloatRect tb = titleText_.getLocalBounds();
    titleText_.setOrigin({ tb.position.x + tb.size.x / 2.f, tb.position.y });
    titleText_.setPosition({ W / 2.f, 14.f });

    subtitleText_ = sf::Text(*font_, "VALORANT CHOOSE-YOUR-OWN-STORY", 18);
    subtitleText_.setFillColor(Theme::TextSecondary);
    sf::FloatRect sb = subtitleText_.getLocalBounds();
    subtitleText_.setOrigin({ sb.position.x + sb.size.x / 2.f, sb.position.y });
    subtitleText_.setPosition({ W / 2.f, 58.f });

    mapLabel_ = sf::Text(*font_, "SELECT MAP", 22);
    mapLabel_.setFillColor(Theme::AccentGold);
    mapLabel_.setPosition({ 60.f, 120.f });

    mapButtons_.clear();
    for (size_t i = 0; i < mapIds_.size(); ++i) {
        std::string label = mapIds_[i];
        std::transform(label.begin(), label.end(), label.begin(), ::toupper);
        mapButtons_.emplace_back(*font_, label, std::to_string(i + 1),
            sf::Vector2f(60.f + i * 200.f, 155.f),
            sf::Vector2f(180.f, 50.f));
    }

    agentLabel_ = sf::Text(*font_, "SELECT AGENT", 22);
    agentLabel_.setFillColor(Theme::AccentGold);
    agentLabel_.setPosition({ 60.f, 240.f });

    agentButtons_.clear();
    for (size_t i = 0; i < agentIds_.size(); ++i) {
        std::string label = agentIds_[i];
        std::transform(label.begin(), label.end(), label.begin(), ::toupper);
        agentButtons_.emplace_back(*font_, label, std::to_string(i + 1),
            sf::Vector2f(60.f + i * 200.f, 275.f),
            sf::Vector2f(180.f, 50.f));
    }

    stratLabel_ = sf::Text(*font_, "SELECT STRATEGY", 22);
    stratLabel_.setFillColor(Theme::AccentGold);
    stratLabel_.setPosition({ 60.f, 360.f });

    stratButtons_.clear();
    for (size_t i = 0; i < stratIds_.size(); ++i) {
        std::string label = stratIds_[i];
        std::transform(label.begin(), label.end(), label.begin(), ::toupper);
        stratButtons_.emplace_back(*font_, label, std::to_string(i + 1),
            sf::Vector2f(60.f + i * 220.f, 395.f),
            sf::Vector2f(200.f, 50.f));
    }

    startButton_ = std::make_unique<Button>(
        *font_, "START ROUND", "ENTER",
        sf::Vector2f(W / 2.f - 150.f, H - 120.f),
        sf::Vector2f(300.f, 60.f));

    updateSelectionVisuals();
}

void MainMenuScreen::updateSelectionVisuals() {
    for (size_t i = 0; i < mapButtons_.size(); ++i)
        mapButtons_[i].setDisabled(static_cast<int>(i) != selectedMap_);
    for (size_t i = 0; i < agentButtons_.size(); ++i)
        agentButtons_[i].setDisabled(static_cast<int>(i) != selectedAgent_);
    for (size_t i = 0; i < stratButtons_.size(); ++i)
        stratButtons_[i].setDisabled(static_cast<int>(i) != selectedStrat_);
}

void MainMenuScreen::onEnter(const std::string&) {
    result_ = { ScreenStatus::Running, "", "" };
}

void MainMenuScreen::handleEvent(const sf::Event& event) {
    for (size_t i = 0; i < mapButtons_.size(); ++i) {
        if (mapButtons_[i].handleEvent(event)) {
            selectedMap_ = static_cast<int>(i);
            updateSelectionVisuals();
        }
    }
    for (size_t i = 0; i < agentButtons_.size(); ++i) {
        if (agentButtons_[i].handleEvent(event)) {
            selectedAgent_ = static_cast<int>(i);
            updateSelectionVisuals();
        }
    }
    for (size_t i = 0; i < stratButtons_.size(); ++i) {
        if (stratButtons_[i].handleEvent(event)) {
            selectedStrat_ = static_cast<int>(i);
            updateSelectionVisuals();
        }
    }
    if (startButton_ && startButton_->handleEvent(event)) {
        std::string payload = selectedMapId() + "|" + selectedAgentId() + "|" + selectedStratId();
        result_ = { ScreenStatus::ReplaceScreen, "game", payload };
        LOG_INFO("MainMenu -> GameScreen: " + payload);
    }

    if (const auto* kp = event.getIf<sf::Event::KeyPressed>()) {
        if (kp->code == sf::Keyboard::Key::Escape)
            result_ = { ScreenStatus::Quit, "", "" };
        if (kp->code == sf::Keyboard::Key::Enter || kp->code == sf::Keyboard::Key::Space) {
            std::string payload = selectedMapId() + "|" + selectedAgentId() + "|" + selectedStratId();
            result_ = { ScreenStatus::ReplaceScreen, "game", payload };
        }
    }
}

void MainMenuScreen::update(sf::Time) {}

void MainMenuScreen::draw(sf::RenderWindow& window) {
    background_.draw(window);
    window.draw(titleBar_);
    window.draw(titleText_);
    window.draw(subtitleText_);

    window.draw(mapLabel_);
    for (auto& b : mapButtons_)   b.draw(window);

    window.draw(agentLabel_);
    for (auto& b : agentButtons_) b.draw(window);

    window.draw(stratLabel_);
    for (auto& b : stratButtons_) b.draw(window);

    if (startButton_) startButton_->draw(window);
}
