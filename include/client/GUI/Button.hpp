#ifndef GUI_BUTTON_HPP
#define GUI_BUTTON_HPP

#include "GUI/Component.hpp"
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/Text.hpp>
#include <SFML/Graphics/Font.hpp>
#include <functional>
#include <string>


namespace GUI
{
    class Button : public Component
    {
        public:
            using Callback = std::function<void()>;
            
            Button(const std::string&text,
                const sf::Font& font, const sf::Vector2f& size);

            void setCallback(Callback callback);

            bool isInteractive() const override;

            void hover() override;
            void unhover() override;

            void press(std::optional<sf::Vector2f> worldMousePos = std::nullopt) override;
            void release(std::optional<sf::Vector2f> worldMousePos = std::nullopt) override;

            void focus() override;
            
            void handleEvent(const sf::Event& event, 
                std::optional<sf::Vector2f> worldMousePos) override;
            
            sf::FloatRect getGlobalBounds() const override;
        
            std::optional<sf::Cursor::Type> getMouseCursorType() const override;

        private:
            void draw(sf::RenderTarget& target,
                sf::RenderStates states) const override;

            void updateVisuals();

        private:
            static inline const sf::Color PRESSED = sf::Color(128, 128, 128); // Gray
            static inline const sf::Color UNPRESSED =  sf::Color(255, 255, 200); // Slightly Yellow
            static inline const sf::Color HOVER_OUTLINE = sf::Color::Black;
            static inline const sf::Color NON_HOVER_OUTLINE = sf::Color::Transparent;
            static inline const float OUTLINE_WIDTH = 4.f;

            Callback mCallback;
            sf::RectangleShape mShape;
            sf::Text mText;
    };
}


#endif