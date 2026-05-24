#include "ui/statsScreen.h"
#include "logger.h"
#include <sstream>
#include <algorithm>

StatsScreen::StatsScreen(sf::Vector2u windowSize, int& totalRounds, int& totalWins)
    : winSize_(windowSize)
    , totalRounds_(totalRounds)
    , totalWins_(totalWins)
    , resultText_(*AssetManager::instance().getFont("data/fonts/Rajdhani-Bold.ttf"), "", 58)
    , subtitleText_(*AssetManager::instance().getFont("data/fonts/Rajdhani-Bold.ttf"), "", 22)
{
    font_ = AssetManager::instance().getFont("data/fonts/Rajdhani-Bold.ttf");
    if (!font_) font_ = AssetManager::instance().getFont("data/fonts/font.ttf");

    for (int i = 0; i < 6; ++i) {
        statLines_.emplace_back(*font_);
    }

    auto bgTex = AssetManager::instance().getTexture("data/images/stats_bg.png");
    background_.loadTexture(bgTex, winSize_);
    background_.setOverlayAlpha(160);

    buildLayout();
}

void StatsScreen::parsePayload(const std::string& payload) {
    std::istringstream ss(payload);
    std::string tok;
    std::vector<std::string> parts;

    while (std::getline(ss, tok, '|')) {
        parts.push_back(tok);
    }

    won_ = (!parts.empty() && parts[0] == "win");
    agentId_ = (parts.size() > 1) ? parts[1] : "Unknown";
    mapId_ = (parts.size() > 2) ? parts[2] : "Unknown";

    try {
        teamCoord_ = (parts.size() > 3 && !parts[3].empty()) ? std::stoi(parts[3]) : 5;
    }
    catch (...) { teamCoord_ = 5; }

    try {
        mapControl_ = (parts.size() > 4 && !parts[4].empty()) ? std::stoi(parts[4]) : 5;
    }
    catch (...) { mapControl_ = 5; }
}

void StatsScreen::buildLayout() {
    float W = static_cast<float>(winSize_.x);
    float H = static_cast<float>(winSize_.y);

    resultBanner_.setSize({ W * 0.7f, 110.f });
    resultBanner_.setOrigin({ W * 0.7f / 2.f, 55.f });
    resultBanner_.setPosition({ W / 2.f, 130.f });
    resultBanner_.setFillColor(sf::Color(30, 30, 40, 230));
    resultBanner_.setOutlineThickness(3.f);

    resultText_.setString("ROUND WON");
    resultText_.setStyle(sf::Text::Bold);
    {
        sf::FloatRect b = resultText_.getLocalBounds();
        resultText_.setOrigin({ b.position.x + b.size.x / 2.f,
                                b.position.y + b.size.y / 2.f });
        resultText_.setPosition({ W / 2.f, 120.f });
    }

    subtitleText_.setString("");
    subtitleText_.setFillColor(Theme::TextSecondary);
    {
        sf::FloatRect b = subtitleText_.getLocalBounds();
        subtitleText_.setOrigin({ b.position.x + b.size.x / 2.f, b.position.y });
        subtitleText_.setPosition({ W / 2.f, 168.f });
    }

    statPanel_.setSize({ 420.f, 240.f });
    statPanel_.setPosition({ W / 2.f - 210.f, 220.f });
    statPanel_.setFillColor(Theme::PanelBg);
    statPanel_.setOutlineThickness(1.f);
    statPanel_.setOutlineColor(sf::Color(80, 80, 120));

    for (int i = 0; i < 6; ++i) {
        statLines_[i].setString("");
        statLines_[i].setCharacterSize(20);
        statLines_[i].setFillColor(Theme::TextPrimary);
        statLines_[i].setPosition({ W / 2.f - 190.f, 234.f + i * 36.f });
    }

    playAgainBtn_ = std::make_unique<Button>(
        *font_, "PLAY AGAIN", "R",
        sf::Vector2f(W / 2.f - 330.f, H - 110.f),
        sf::Vector2f(200.f, 55.f));

    mainMenuBtn_ = std::make_unique<Button>(
        *font_, "MAIN MENU", "M",
        sf::Vector2f(W / 2.f - 100.f, H - 110.f),
        sf::Vector2f(200.f, 55.f));

    quitBtn_ = std::make_unique<Button>(
        *font_, "QUIT", "Q",
        sf::Vector2f(W / 2.f + 130.f, H - 110.f),
        sf::Vector2f(200.f, 55.f));
}

void StatsScreen::onEnter(const std::string& payload) {
    result_ = { ScreenStatus::Running, "", "" };
    bannerScale_ = 0.f;
    animDone_ = false;

    parsePayload(payload);

    ++totalRounds_;
    if (won_) ++totalWins_;

    resultText_.setString(won_ ? "ROUND WON" : "ROUND LOST");
    resultText_.setFillColor(won_ ? sf::Color(80, 255, 120) : Theme::AccentRed);
    resultBanner_.setOutlineColor(won_ ? sf::Color(80, 255, 120) : Theme::AccentRed);

    std::string sub = won_ ? "Excellent execution!" : "Better luck next round.";
    subtitleText_.setString(sub);

    {
        float W = static_cast<float>(winSize_.x);
        sf::FloatRect b = resultText_.getLocalBounds();
        resultText_.setOrigin({ b.position.x + b.size.x / 2.f,
                                b.position.y + b.size.y / 2.f });
        resultText_.setPosition({ W / 2.f, 120.f });
        sf::FloatRect sb = subtitleText_.getLocalBounds();
        subtitleText_.setOrigin({ sb.position.x + sb.size.x / 2.f, sb.position.y });
        subtitleText_.setPosition({ W / 2.f, 168.f });
    }

    statLines_[0].setString("Agent      :  " + agentId_);
    statLines_[1].setString("Map        :  " + mapId_);
    statLines_[2].setString("Team Coord :  " + std::to_string(teamCoord_) + " / 10");
    statLines_[3].setString("Map Ctrl   :  " + std::to_string(mapControl_) + " / 10");
    statLines_[4].setString("Session W  :  " + std::to_string(totalWins_) +
        " / " + std::to_string(totalRounds_));

    float wr = totalRounds_ > 0
        ? (100.f * totalWins_ / totalRounds_)
        : 0.f;
    std::ostringstream wrss;
    wrss << "Win Rate   :  " << static_cast<int>(wr) << "%";
    statLines_[5].setString(wrss.str());
    statLines_[5].setFillColor(wr >= 50.f ? sf::Color(80, 255, 120) : Theme::AccentRed);

    int total = teamCoord_ + mapControl_;
    std::string rating;
    sf::Color ratingColor;
    if (total >= 14) { rating = "S TIER - Flawless";  ratingColor = sf::Color(255, 215, 0); }
    else if (total >= 10) { rating = "A TIER - Clean";      ratingColor = sf::Color(80, 255, 120); }
    else if (total >= 6) { rating = "B TIER - Sloppy";     ratingColor = sf::Color(255, 160, 40); }
    else { rating = "C TIER - Chaotic";    ratingColor = sf::Color(255, 70, 70); }
    statLines_[5].setString("Rating     :  " + rating);
    statLines_[5].setFillColor(ratingColor);

    LOG_INFO("StatsScreen: " + std::string(won_ ? "WIN" : "LOSE") +
        " agent=" + agentId_ + " map=" + mapId_);
}

void StatsScreen::handleEvent(const sf::Event& event) {
    if (playAgainBtn_ && playAgainBtn_->handleEvent(event))
        result_ = { ScreenStatus::ReplaceScreen, "game_same", "" };

    if (mainMenuBtn_ && mainMenuBtn_->handleEvent(event))
        result_ = { ScreenStatus::ReplaceScreen, "menu", "" };

    if (quitBtn_ && quitBtn_->handleEvent(event))
        result_ = { ScreenStatus::Quit, "", "" };

    if (const auto* kp = event.getIf<sf::Event::KeyPressed>()) {
        if (kp->code == sf::Keyboard::Key::R)
            result_ = { ScreenStatus::ReplaceScreen, "game_same", "" };
        if (kp->code == sf::Keyboard::Key::M)
            result_ = { ScreenStatus::ReplaceScreen, "menu", "" };
        if (kp->code == sf::Keyboard::Key::Q)
            result_ = { ScreenStatus::Quit, "", "" };
    }
}

void StatsScreen::update(sf::Time dt) {
    if (!animDone_) {
        bannerScale_ += dt.asSeconds() * 2.5f;
        if (bannerScale_ >= 1.f) {
            bannerScale_ = 1.f;
            animDone_ = true;
        }
        resultBanner_.setScale({ bannerScale_, bannerScale_ });
        resultText_.setScale({ bannerScale_, bannerScale_ });
    }
}

void StatsScreen::draw(sf::RenderWindow& window) {
    background_.draw(window);
    window.draw(resultBanner_);
    window.draw(resultText_);
    window.draw(subtitleText_);
    window.draw(statPanel_);
    for (auto& l : statLines_) window.draw(l);
    if (playAgainBtn_) playAgainBtn_->draw(window);
    if (mainMenuBtn_)  mainMenuBtn_->draw(window);
    if (quitBtn_)      quitBtn_->draw(window);
}