#ifndef GUI_LABEL_HPP
#define GUI_LABEL_HPP

#include "GUI/Component.hpp"
#include <SFML/Graphics/Text.hpp>
#include <SFML/Graphics/Font.hpp>
#include <string>


namespace GUI
{
    class Label : public Component
    {
        public:
            Label(const std::string& text, const sf::Font& font);

            bool isSelectable() const override;
            void handleEvent(const sf::Event& event, 
                std::optional<sf::Vector2f> worldMousePos) override;
            sf::FloatRect getGlobalBounds() const override;

            void setText(const std::string& text);

        private:
            void draw(sf::RenderTarget& target, sf::RenderStates states) const override;
    
        private:
            static const int CHAR_SIZE = 16;
            sf::Text mText;
    };
}


#endif