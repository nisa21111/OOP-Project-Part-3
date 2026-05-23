#include "ui/widgets.h"
#include <algorithm>
#include <sstream>


Button::Button(const sf::Font& font, const std::string& text,
    const std::string& key, sf::Vector2f pos, sf::Vector2f size)
    : label_(font, text, 20)
    , keyHint_(font, "[" + key + "]", 14)
    , actionKey_(key)
{
    shape_.setSize(size);
    shape_.setPosition(pos);
    shape_.setOutlineThickness(2.f);

    sf::FloatRect lb = label_.getLocalBounds();
    label_.setOrigin({ lb.position.x + lb.size.x / 2.f,
                       lb.position.y + lb.size.y / 2.f });
    label_.setPosition({ pos.x + size.x / 2.f, pos.y + size.y / 2.f });

    keyHint_.setFillColor(Theme::AccentGold);
    keyHint_.setPosition({ pos.x + 8.f, pos.y + 5.f });

    refreshColors();
}

void Button::refreshColors() {
    if (disabled_) {
        shape_.setFillColor(sf::Color(30, 30, 40, 160));
        shape_.setOutlineColor(sf::Color(60, 60, 60));
        label_.setFillColor(Theme::TextSecondary);
        return;
    }
    if (pressed_) {
        shape_.setFillColor(Theme::BtnPressed);
        shape_.setOutlineColor(Theme::AccentGold);
    }
    else if (selected_) {
        shape_.setFillColor(sf::Color(40, 90, 50, 240));
        shape_.setOutlineColor(sf::Color(80, 255, 120));
    }
    else if (hovered_) {
        shape_.setFillColor(Theme::BtnHover);
        shape_.setOutlineColor(Theme::BtnBorderHov);
    }
    else {
        shape_.setFillColor(Theme::BtnNormal);
        shape_.setOutlineColor(Theme::BtnBorder);
    }
    label_.setFillColor(Theme::TextPrimary);
}

void Button::setSelected(bool s) {
    selected_ = s;
    refreshColors();
}

void Button::setDisabled(bool d) {
    disabled_ = d;
    refreshColors();
}

void Button::setPosition(sf::Vector2f pos) {
    sf::Vector2f size = shape_.getSize();
    shape_.setPosition(pos);
    sf::FloatRect lb = label_.getLocalBounds();
    label_.setOrigin({ lb.position.x + lb.size.x / 2.f,
                       lb.position.y + lb.size.y / 2.f });
    label_.setPosition({ pos.x + size.x / 2.f, pos.y + size.y / 2.f });
    keyHint_.setPosition({ pos.x + 8.f, pos.y + 5.f });
}

void Button::setSize(sf::Vector2f size) {
    shape_.setSize(size);
    setPosition(shape_.getPosition());
}

bool Button::handleEvent(const sf::Event& event) {
    if (disabled_) return false;

    if (const auto* mm = event.getIf<sf::Event::MouseMoved>()) {
        sf::Vector2f mp(static_cast<float>(mm->position.x),
            static_cast<float>(mm->position.y));
        hovered_ = shape_.getGlobalBounds().contains(mp);
        refreshColors();
    }
    if (const auto* mb = event.getIf<sf::Event::MouseButtonPressed>()) {
        if (mb->button == sf::Mouse::Button::Left) {
            sf::Vector2f mp(static_cast<float>(mb->position.x),
                static_cast<float>(mb->position.y));
            if (shape_.getGlobalBounds().contains(mp)) {
                pressed_ = true;
                refreshColors();
            }
        }
    }
    if (const auto* mb = event.getIf<sf::Event::MouseButtonReleased>()) {
        if (mb->button == sf::Mouse::Button::Left && pressed_) {
            pressed_ = false;
            refreshColors();
            sf::Vector2f mp(static_cast<float>(mb->position.x),
                static_cast<float>(mb->position.y));
            return shape_.getGlobalBounds().contains(mp);
        }
    }
    return false;
}

void Button::draw(sf::RenderWindow& window) const {
    window.draw(shape_);
    window.draw(keyHint_);
    window.draw(label_);
}


TextPanel::TextPanel(const sf::Font& font, sf::Vector2f pos, sf::Vector2f size,
    unsigned int charSize)
    : text_(font, "", charSize)
    , titleText_(font, "", charSize + 4)
{
    background_.setSize(size);
    background_.setPosition(pos);
    background_.setFillColor(Theme::PanelBg);
    background_.setOutlineThickness(1.f);
    background_.setOutlineColor(sf::Color(80, 80, 120));

    text_.setFillColor(Theme::TextPrimary);
    text_.setPosition({ pos.x + 16.f, pos.y + 16.f });

    titleText_.setFillColor(Theme::AccentGold);
    titleText_.setStyle(sf::Text::Style::Bold);
    titleText_.setPosition({ pos.x + 16.f, pos.y + 10.f });
}

void TextPanel::setText(const std::string& text) {
    text_.setString(text);
    float yOff = hasTitle_ ? 50.f : 16.f;
    sf::Vector2f pos = background_.getPosition();
    text_.setPosition({ pos.x + 16.f, pos.y + yOff });
}

void TextPanel::setTitle(const std::string& title) {
    titleText_.setString(title);
    hasTitle_ = !title.empty();
}

void TextPanel::setPosition(sf::Vector2f pos) {
    background_.setPosition(pos);
    float yOff = hasTitle_ ? 50.f : 16.f;
    text_.setPosition({ pos.x + 16.f, pos.y + yOff });
    titleText_.setPosition({ pos.x + 16.f, pos.y + 10.f });
}

void TextPanel::draw(sf::RenderWindow& window) const {
    window.draw(background_);
    if (hasTitle_) window.draw(titleText_);
    window.draw(text_);
}



AgentPortrait::AgentPortrait(const sf::Font& font, sf::Vector2f pos, sf::Vector2f portraitSize)
    : speechText_(font, "", 17)
    , nameText_(font, "", 16)
    , pos_(pos)
    , portraitSize_(portraitSize)
{
    bubble_.setSize({ 340.f, 110.f });
    bubble_.setFillColor(sf::Color(20, 20, 35, 220));
    bubble_.setOutlineThickness(2.f);
    bubble_.setOutlineColor(Theme::AccentGold);
    bubble_.setPosition({ pos.x + portraitSize.x + 20.f, pos.y + 10.f });

    speechText_.setFillColor(Theme::TextPrimary);
    speechText_.setPosition({ pos.x + portraitSize.x + 34.f, pos.y + 22.f });

    nameBar_.setSize({ portraitSize.x, 30.f });
    nameBar_.setFillColor(sf::Color(200, 50, 50, 210));
    nameBar_.setPosition({ pos.x + slideOffset_, pos.y + portraitSize.y - 30.f });

    nameText_.setFillColor(sf::Color::White);
    nameText_.setStyle(sf::Text::Style::Bold);
    nameText_.setPosition({ pos.x + slideOffset_ + 8.f, pos.y + portraitSize.y - 26.f });
}

void AgentPortrait::loadTexture(std::shared_ptr<sf::Texture> tex) {
    texture_ = std::move(tex);

    portrait_.emplace(*texture_);

    sf::Vector2u ts = texture_->getSize();
    float sx = ts.x > 0 ? portraitSize_.x / static_cast<float>(ts.x) : 1.f;
    float sy = ts.y > 0 ? portraitSize_.y / static_cast<float>(ts.y) : 1.f;
    portrait_->setScale({ sx, sy });
    portrait_->setPosition({ pos_.x + slideOffset_, pos_.y });
}

void AgentPortrait::setSpeech(const std::string& text) {
    std::string wrapped;
    std::istringstream iss(text);
    std::string word, line;
    while (iss >> word) {
        if (line.size() + word.size() + 1 > 42) {
            wrapped += line + "\n";
            line = word;
        }
        else {
            if (!line.empty()) line += " ";
            line += word;
        }
    }
    if (!line.empty()) wrapped += line;
    speechText_.setString(wrapped);
}

void AgentPortrait::setAgentName(const std::string& name) {
    nameText_.setString(name);
}

void AgentPortrait::update(sf::Time dt) {
    if (!animating_) return;

    float speed = 500.f;
    slideOffset_ -= speed * dt.asSeconds();
    if (slideOffset_ <= 0.f) {
        slideOffset_ = 0.f;
        animating_ = false;
    }

    if (portrait_)
        portrait_->setPosition({ pos_.x + slideOffset_, pos_.y });

    nameBar_.setPosition({ pos_.x + slideOffset_, pos_.y + portraitSize_.y - 30.f });
    nameText_.setPosition({ pos_.x + slideOffset_ + 8.f, pos_.y + portraitSize_.y - 26.f });
}

void AgentPortrait::resetAnimation() {
    slideOffset_ = 300.f;
    animating_ = true;

    if (portrait_)
        portrait_->setPosition({ pos_.x + slideOffset_, pos_.y });

    nameBar_.setPosition({ pos_.x + slideOffset_, pos_.y + portraitSize_.y - 30.f });
    nameText_.setPosition({ pos_.x + slideOffset_ + 8.f, pos_.y + portraitSize_.y - 26.f });
}

void AgentPortrait::draw(sf::RenderWindow& window) const {
    if (portrait_) window.draw(*portrait_);
    window.draw(nameBar_);
    window.draw(nameText_);
    window.draw(bubble_);
    window.draw(speechText_);
}



BackgroundLayer::BackgroundLayer() {
    overlay_.setFillColor(sf::Color(0, 0, 10, 120));
    vignette_.setFillColor(sf::Color(0, 0, 0, 0));
}

void BackgroundLayer::loadTexture(std::shared_ptr<sf::Texture> tex, sf::Vector2u windowSize) {
    texture_ = std::move(tex);

    sprite_.emplace(*texture_);

    sf::Vector2u ts = texture_->getSize();
    if (ts.x > 0 && ts.y > 0) {
        float sx = static_cast<float>(windowSize.x) / ts.x;
        float sy = static_cast<float>(windowSize.y) / ts.y;
        sprite_->setScale({ std::max(sx, sy), std::max(sx, sy) });
    }

    overlay_.setSize(sf::Vector2f(windowSize));
    vignette_.setSize(sf::Vector2f(windowSize));
}

void BackgroundLayer::setOverlayAlpha(std::uint8_t alpha) {
    sf::Color c = overlay_.getFillColor();
    c.a = alpha;
    overlay_.setFillColor(c);
}

void BackgroundLayer::draw(sf::RenderWindow& window) const {
    if (sprite_) window.draw(*sprite_);
    window.draw(overlay_);
    window.draw(vignette_);
}