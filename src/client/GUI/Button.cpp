#include "GUI/Button.hpp"
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Window/Event.hpp>


GUI::Button::Button(const std::string& text, const sf::Font& font, const sf::Vector2f& size)
    : mText(font, text, 16)
    , mShape(size)
{
    mShape.setFillColor(UNPRESSED);
    mShape.setOutlineColor(NON_HOVER_OUTLINE);
    mShape.setOutlineThickness(0.f);

    mText.setFillColor(sf::Color::Black);

    // Centering text that is shifted by its position
    sf::FloatRect textBounds = mText.getLocalBounds();
    mText.setOrigin({
        textBounds.position.x + textBounds.size.x / 2.0f,
        textBounds.position.y + textBounds.size.y / 2.0f
    });

    mText.setPosition({ size.x / 2.0f, size.y / 2.0f });
}

void GUI::Button::setCallback(Callback callback) 
{
    mCallback = std::move(callback);
}

bool GUI::Button::isInteractive() const 
{
    return true;
}

void GUI::Button::hover() 
{
    Component::hover();
    updateVisuals();    
}

void GUI::Button::unhover() 
{
    Component::unhover();
    updateVisuals();
}

void GUI::Button::press(std::optional<sf::Vector2f> worldMousePos) 
{
    Component::press();
    updateVisuals();
}

void GUI::Button::release(std::optional<sf::Vector2f> worldMousePos) 
{
    Component::release(worldMousePos);
    updateVisuals();
}

void GUI::Button::focus() 
{
    Component::focus(); 

    if (mCallback) 
    {
        mCallback();
    }

    Component::unfocus(); 
}

void GUI::Button::handleEvent(const sf::Event& event, std::optional<sf::Vector2f> worldMousePos)
{
}

sf::FloatRect GUI::Button::getGlobalBounds() const 
{
    return getTransform().transformRect(mShape.getLocalBounds());
}

std::optional<sf::Cursor::Type> GUI::Button::getMouseCursorType() const
{
    if (isPressed() || isHovered()) 
    {
        return sf::Cursor::Type::Hand;
    }
    
    return std::nullopt;
}

void GUI::Button::draw(sf::RenderTarget& target, sf::RenderStates states) const 
{
    states.transform *= getTransform();
    target.draw(mShape, states);
    target.draw(mText, states);
}

void GUI::Button::updateVisuals()
{
    if (isPressed())
    {
        mShape.setOutlineThickness(-OUTLINE_WIDTH); 
    }
    else
    {
        mShape.setOutlineThickness(0.0f);
    }
}
