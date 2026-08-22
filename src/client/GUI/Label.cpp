#include "GUI/Label.hpp"
#include <SFML/Graphics/RenderStates.hpp>
#include <SFML/Graphics/RenderTarget.hpp>


GUI::Label::Label(const std::string& text, const sf::Font& font, unsigned int size)
    : mText(font, "", size)
{
    setText(text);
}

bool GUI::Label::isInteractive() const
{
    return false;
}

void GUI::Label::handleEvent(const sf::Event& event, 
    std::optional<sf::Vector2f> worldMousePos)
{
    return;
}

sf::FloatRect GUI::Label::getGlobalBounds() const
{
    return getTransform().transformRect(mText.getLocalBounds());
}

void GUI::Label::setText(const std::string& text)
{
    mText.setString(text);

    sf::FloatRect bounds = mText.getLocalBounds();
    setOrigin(bounds.position + bounds.size / 2.f);
}

void GUI::Label::draw(sf::RenderTarget& target, sf::RenderStates states) const 
{
    states.transform *= getTransform();
    target.draw(mText, states);
}
    
