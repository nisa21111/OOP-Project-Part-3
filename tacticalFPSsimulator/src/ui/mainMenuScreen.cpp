#include "ui/mainMenuScreen.h"
#include "logger.h"
#include "agentFactory.h"
#include <algorithm>

static const std::vector<std::string> STRAT_IDS = { "balanced", "aggressive", "coordinated" };

MainMenuScreen::MainMenuScreen(sf::Vector2u windowSize,
    const std::vector<std::string>& availableMaps,
    const std::vector<std::string>& availableAgents)
    : winSize_(windowSize)
    , mapIds_(availableMaps)
    , agentIds_(availableAgents)
    , stratIds_(STRAT_IDS)
    , font_(AssetManager::instance().getFont("data/fonts/Rajdhani-Bold.ttf"))
    , titleText_(*font_, "", 42)
    , subtitleText_(*font_, "", 18)
    , mapLabel_(*font_, "", 22)
    , agentLabel_(*font_, "", 22)
    , stratLabel_(*font_, "", 22)
{
    font_ = AssetManager::instance().getFont("data/fonts/Rajdhani-Bold.ttf");
    if (!font_) font_ = AssetManager::instance().getFont("data/fonts/font.ttf");

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
    titleText_.setStyle(sf::Text::Style::Bold);
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
    mapLabel_.setPosition({ 60.f, 100.f });

    mapButtons_.clear();
    for (size_t i = 0; i < mapIds_.size(); ++i) {
        std::string label = mapIds_[i];
        auto slash = label.find_last_of("/\\");
        if (slash != std::string::npos) label = label.substr(slash + 1);
        std::transform(label.begin(), label.end(), label.begin(), ::toupper);
        mapButtons_.emplace_back(*font_, label, std::to_string(i + 1),
            sf::Vector2f(60.f + i * 200.f, 132.f),
            sf::Vector2f(180.f, 50.f));
    }

    agentLabel_ = sf::Text(*font_, "SELECT AGENT", 22);
    agentLabel_.setFillColor(Theme::AccentGold);
    agentLabel_.setPosition({ 60.f, 210.f });

    agentButtons_.clear();
    for (size_t i = 0; i < agentIds_.size(); ++i) {
        std::string label = agentIds_[i];
        std::transform(label.begin(), label.end(), label.begin(), ::toupper);
        agentButtons_.emplace_back(*font_, label, std::to_string(i + 1),
            sf::Vector2f(60.f + i * 200.f, 242.f),
            sf::Vector2f(180.f, 50.f));
    }

    agentInfoPanel_ = std::make_unique<TextPanel>(
        *font_,
        sf::Vector2f(60.f, 308.f),
        sf::Vector2f(760.f, 90.f),
        16);
    agentInfoPanel_->setTitle("Select an agent");
    updateAgentInfo();

    stratInfoPanel_ = std::make_unique<TextPanel>(
        *font_,
        sf::Vector2f(60.f, 510.f),
        sf::Vector2f(760.f, 75.f),
        15);
    stratInfoPanel_->setTitle("Strategy");
    updateStratInfo();

    stratLabel_ = sf::Text(*font_, "SELECT STRATEGY", 22);
    stratLabel_.setFillColor(Theme::AccentGold);
    stratLabel_.setPosition({ 60.f, 418.f });

    stratButtons_.clear();
    for (size_t i = 0; i < stratIds_.size(); ++i) {
        std::string label = stratIds_[i];
        std::transform(label.begin(), label.end(), label.begin(), ::toupper);
        stratButtons_.emplace_back(*font_, label, std::to_string(i + 1),
            sf::Vector2f(60.f + i * 220.f, 450.f),
            sf::Vector2f(200.f, 50.f));
    }

    startButton_ = std::make_unique<Button>(
        *font_, "START ROUND", "ENTER",
        sf::Vector2f(W / 2.f - 150.f, H - 120.f),
        sf::Vector2f(300.f, 60.f));

    quitButton_ = std::make_unique<Button>(
        *font_, "QUIT", "Q",
        sf::Vector2f(W / 2.f - 150.f, H - 50.f),
        sf::Vector2f(300.f, 40.f));

    updateSelectionVisuals();
}

void MainMenuScreen::updateAgentInfo() {
    if (!agentInfoPanel_ || agentIds_.empty()) return;
    try {
        auto agent = AgentFactory::instance().create(agentIds_[selectedAgent_]);
        std::string abils;
        for (auto& a : agent->abilities()) abils += "  > " + a + "    ";
        agentInfoPanel_->setTitle(agent->name() + "  |  " + agent->roleStr());
        agentInfoPanel_->setText(abils);
    }
    catch (...) {}
}

void MainMenuScreen::updateStratInfo() {
    if (!stratInfoPanel_) return;
    static const std::array<std::string, 3> descs = {
        "Balanced: Both map control and team coordination matter equally.",
        "Aggressive: Map control is key. High risk, high reward. Spike drops hurt less.",
        "Coordinated: Team coordination is everything. Any miscommunication is costly."
    };
    stratInfoPanel_->setTitle("STRATEGY: " + stratIds_[selectedStrat_]);
    stratInfoPanel_->setText(descs[selectedStrat_]);
}

void MainMenuScreen::updateSelectionVisuals() {
    for (size_t i = 0; i < mapButtons_.size(); ++i)
        mapButtons_[i].setSelected(static_cast<int>(i) == selectedMap_);
    for (size_t i = 0; i < agentButtons_.size(); ++i)
        agentButtons_[i].setSelected(static_cast<int>(i) == selectedAgent_);
    for (size_t i = 0; i < stratButtons_.size(); ++i)
        stratButtons_[i].setSelected(static_cast<int>(i) == selectedStrat_);
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
            updateAgentInfo();
        }
    }

    for (size_t i = 0; i < stratButtons_.size(); ++i) {
        if (stratButtons_[i].handleEvent(event)) {
            selectedStrat_ = static_cast<int>(i);
            updateSelectionVisuals();
            updateStratInfo();
        }
    }

    if (startButton_ && startButton_->handleEvent(event)) {
        std::string payload = selectedMapId() + "|" + selectedAgentId() + "|" + selectedStratId();
        result_ = { ScreenStatus::ReplaceScreen, "game", payload };
        LOG_INFO("MainMenu -> GameScreen: " + payload);
    }

    if (quitButton_ && quitButton_->handleEvent(event))
        result_ = { ScreenStatus::Quit, "", "" };

    if (const auto* kp = event.getIf<sf::Event::KeyPressed>()) {
        if (kp->code == sf::Keyboard::Key::Escape ||
            kp->code == sf::Keyboard::Key::Q)
            result_ = { ScreenStatus::Quit, "", "" };

        if (kp->code == sf::Keyboard::Key::Enter ||
            kp->code == sf::Keyboard::Key::Space) {
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

    if (agentInfoPanel_) agentInfoPanel_->draw(window);

    window.draw(stratLabel_);
    for (auto& b : stratButtons_) b.draw(window);

    if (startButton_) startButton_->draw(window);
    if (stratInfoPanel_) stratInfoPanel_->draw(window);
    if (quitButton_)  quitButton_->draw(window);
}