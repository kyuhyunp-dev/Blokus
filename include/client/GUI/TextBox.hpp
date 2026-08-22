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
        TextBox(const std::string placeholder, const sf::Font& font, const sf::Vector2f& size);

        bool isInteractive() const override;
        
		void press(std::optional<sf::Vector2f> worldMousePos = std::nullopt) override;
		void release(std::optional<sf::Vector2f> worldMousePos = std::nullopt) override;

		void unfocus() override;	
        
        void handleEvent(const sf::Event& event, std::optional<sf::Vector2f> worldMousePos) override;
        void update(sf::Time dt) override;

        sf::FloatRect getGlobalBounds() const override;
        std::optional<sf::Cursor::Type> getMouseCursorType() const override;

        std::string getText() const;

    public:
        enum class RepeatAction { None, Backspace, MoveLeft, MoveRight };
        
    public: // Test-only accessors to safely read private coordinates
        sf::Vector2f getTextPosition() const { return mText.getPosition(); }
        sf::Vector2f getCursorPosition() const { return mCursor.getPosition(); }
        bool getShowCursor() const { return mShowCursor; }
        bool hasHighlight() const;

    private:
        void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

        void updateHighlight();
        bool deleteSelectedText(); 
        void updateLayout();
        
        std::size_t getCharacterIndexAt(sf::Vector2f worldMousePos) const;
        void resetCursorBlink();

        void executeBackspace();
        void executeMoveLeft();
        void executeMoveRight();
        
        // For standard movements (Left/Right arrows, clicks, backspace)
        void setCursorPosition(std::size_t index);

        // For highlighting (Mouse dragging)
        void setSelectionEnd(std::size_t index);

        void updateString(const std::string& newString, std::size_t newCursorPos);

    private:
        RepeatAction mRepeatAction;
        sf::Time mRepeatTime;
        bool mInInitialDelay;
                
        // Text Selection during Drag
        std::optional<std::size_t> mSelectionStart;
        std::optional<std::size_t> mSelectionEnd; 
        sf::RectangleShape mSelectionHighlight;

        sf::RectangleShape mShape;
        sf::Text mText;
        std::string mString;
        const std::string mPlaceholder;   
        // Blinking Cursor mechanics
        sf::RectangleShape mCursor;
        sf::Time mCursorTime;
        std::size_t mCursorIndex;
        bool mShowCursor;

        unsigned int mCharacterLimit;
    };
}

#endif