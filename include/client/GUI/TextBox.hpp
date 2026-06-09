#ifndef GUI_TEXTBOX_HPP
#define GUI_TEXTBOX_HPP

#include "GUI/Component.hpp"
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/Text.hpp>
#include <SFML/System/Clock.hpp>
#include <string>


namespace GUI 
{
    class TextBox : public Component 
    {
    public:
        TextBox(const sf::Font& font, const sf::Vector2f& size);

        bool isSelectable() const override;
        void select() override;
        void deselect() override;
        void activate() override;
        void deactivate() override;
        
        void handleEvent(const sf::Event& event, std::optional<sf::Vector2f> worldMousePos) override;
        sf::FloatRect getGlobalBounds() const override;

        std::string getText() const;

    public: // Test-only accessors to safely read private coordinates
        sf::Vector2f getTextPosition() const { return mText.getPosition(); }
        sf::Vector2f getCursorPosition() const { return mCursor.getPosition(); }
        bool getShowCursor() const { return mShowCursor; }
    
    private:
        void draw(sf::RenderTarget& target, sf::RenderStates states) const override;
        void updateLayout();

    private:
        static inline const sf::Color UNPRESSED{255, 255, 255};
        static inline const sf::Color HOVER_OUTLINE{100, 100, 100};
        static inline const sf::Color NON_HOVER_OUTLINE{200, 200, 200};
        static inline const float OUTLINE_WIDTH = -2.f; 
    
    private:
        sf::RectangleShape mShape;
        sf::Text mText;
        std::string mString;
        
        // Blinking Cursor mechanics
        sf::RectangleShape mCursor;
        mutable sf::Clock mCursorTimer; 
        mutable bool mShowCursor;

        unsigned int mCharacterLimit;
    };
}

#endif