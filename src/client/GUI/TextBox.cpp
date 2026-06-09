#include "GUI/TextBox.hpp"
#include <SFML/Window/Event.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/System/Time.hpp>
#include <iostream>


namespace GUI 
{
    TextBox::TextBox(const sf::Font& font, const sf::Vector2f& size)
        : mShape(size)
        , mText(font, "", 16)
        , mShowCursor(false)
        , mCharacterLimit(10) 
    {
        mShape.setFillColor(UNPRESSED);
        mShape.setOutlineColor(NON_HOVER_OUTLINE);
        mShape.setOutlineThickness(OUTLINE_WIDTH);

        mText.setFillColor(sf::Color::Black);
        
        mCursor.setSize({ 2.f, 18.f }); 
        mCursor.setFillColor(sf::Color::Black);
       
        mText.setString("A");
        sf::FloatRect sampleBounds = mText.getLocalBounds();
        float fixedCenterY = (size.y - sampleBounds.size.y) / 2.0f - sampleBounds.position.y;

        mText.setPosition({ 0.f, fixedCenterY });
        mCursor.setPosition({ size.x / 2.0f, size.y / 2.0f - mCursor.getSize().y / 2.0f });

        mText.setString("");
        updateLayout();
    }

    bool TextBox::isSelectable() const 
    {
        return true;
    }

    void TextBox::select() 
    {
        Component::select();
        mShape.setOutlineColor(HOVER_OUTLINE);
    }

    void TextBox::deselect() 
    {
        Component::deselect();
        mShape.setOutlineColor(NON_HOVER_OUTLINE);
    }

    void TextBox::activate() 
    {
        Component::activate();
        
        // Reset the cursor blinker so it immediately shows up when clicked
        mShowCursor = true;
        mCursorTimer.restart(); 
    }

    void TextBox::deactivate() 
    {
        Component::deactivate();
        mShowCursor = false;
        
        // Revert to hover or non-hover outline depending on mouse state
        if (isSelected())
        {
            mShape.setOutlineColor(HOVER_OUTLINE);
        }
        else
        {
            mShape.setOutlineColor(NON_HOVER_OUTLINE);
        }
    }

    void TextBox::handleEvent(const sf::Event& event, std::optional<sf::Vector2f> /*worldMousePos*/) 
    {
        assert(isActive());

        if (auto text = event.getIf<sf::Event::TextEntered>()) 
        {
            uint32_t unicode = text->unicode;

            // Handle Backspace (ASCII 8)
            if (unicode == 8) 
            {
                if (!mString.empty()) 
                {
                    mString.pop_back();
                }
            }
            // Handle Enter/Return (ASCII 13 or 10) to finalize typing
            else if (unicode == 13 || unicode == 10) 
            {
                deactivate();
                return; 
            }
            // Handle standard printable characters
            else if (unicode >= 32 && unicode < 128) 
            {
                if (mString.size() < mCharacterLimit) 
                {
                    mString += static_cast<char>(unicode);
                }
            }

            mText.setString(mString);
            updateLayout();

            mShowCursor = true;
            mCursorTimer.restart();
        }
    }

    sf::FloatRect TextBox::getGlobalBounds() const 
    {
        return getTransform().transformRect(mShape.getLocalBounds());
    }

    std::string TextBox::getText() const 
    {
        return mString;
    }

    void TextBox::updateLayout()
    {
        sf::FloatRect textBounds = mText.getLocalBounds();
        sf::Vector2f boxSize = mShape.getSize();

        float startX = (boxSize.x - textBounds.size.x) / 2.0f;
        mText.setPosition({ startX, mText.getPosition().y });

        sf::Vector2f nextCharPos = mText.findCharacterPos(mString.size());
        mCursor.setPosition({ nextCharPos.x, mCursor.getPosition().y });
    }

    void TextBox::draw(sf::RenderTarget& target, sf::RenderStates states) const 
    {
        states.transform *= getTransform();
        target.draw(mShape, states);
        target.draw(mText, states);

        // Blinking Cursor Logic
        if (isActive()) 
        {
            if (mCursorTimer.getElapsedTime().asSeconds() >= 0.5f) 
            {
                mShowCursor = !mShowCursor;
                mCursorTimer.restart();
            }

            if (mShowCursor) 
            {
                target.draw(mCursor, states);
            }
        }
    }
}