#include "GUI/Button.hpp"
#include <SFML/Graphics/RenderTarget.hpp>


namespace GUI 
{
    Button::Button(const std::string& text, const sf::Font& font, const sf::Vector2f& size)
        : mText(font, text, 16)
        , mShape(size)
    {
        mShape.setFillColor(UNPRESSED);
        mShape.setOutlineColor(NON_HOVER_OUTLINE);
        mShape.setOutlineThickness(-OUTLINE_WIDTH);

        mText.setFillColor(sf::Color::Black);

        // Centering text that is shifted by its position
        sf::FloatRect textBounds = mText.getLocalBounds();
        mText.setOrigin({
            textBounds.position.x + textBounds.size.x / 2.0f,
            textBounds.position.y + textBounds.size.y / 2.0f
        });

        mText.setPosition({ size.x / 2.0f, size.y / 2.0f });
    }

    void Button::setCallback(Callback callback) 
    {
        mCallback = std::move(callback);
    }

    bool Button::isSelectable() const 
    {
        return true;
    }

    void Button::select() 
    {
        Component::select(); 
        mShape.setOutlineColor(HOVER_OUTLINE);
    }

    void Button::deselect() 
    {
        Component::deselect(); 
        mShape.setOutlineColor(NON_HOVER_OUTLINE);
    }

    void Button::activate() 
    {
        Component::activate(); 
        mShape.setFillColor(PRESSED);

        if (mCallback) 
        {
            mCallback();
        }

        mShape.setFillColor(UNPRESSED);
        Component::deactivate(); 
    }

    void Button::handleEvent(const sf::Event& /*event*/, std::optional<sf::Vector2f> /*worldMousePos*/) 
    {
        // Handled entirely by the Container
        return; 
    }

    sf::FloatRect Button::getGlobalBounds() const 
    {
        return getTransform().transformRect(mShape.getLocalBounds());
    }

    void Button::draw(sf::RenderTarget& target, sf::RenderStates states) const 
    {
        states.transform *= getTransform();
        target.draw(mShape, states);
        target.draw(mText, states);
    }
}
