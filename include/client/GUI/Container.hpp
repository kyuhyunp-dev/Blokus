#ifndef GUI_CONTAINER_HPP
#define GUI_CONTAINER_HPP

#include "GUI/Component.hpp"
#include <SFML/Graphics/RenderStates.hpp>
#include <vector>
#include <memory>
#include <optional>


namespace GUI
{
    class Container: public Component
    {
    public:
        Container();

        void pack(Component::Ptr component);

    	bool isInteractive() const override;
		
        void handleEvent(const sf::Event& event, 
            std::optional<sf::Vector2f> worldMousePos) override;
        void update(sf::Time dt) override; 

        sf::FloatRect getGlobalBounds() const override;

        std::optional<sf::Cursor::Type> getMouseCursorType(sf::Vector2f worldMousePos) const;
 
    private:
        void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

        std::optional<std::size_t> getComponentIndexAt(sf::Vector2f worldMousePos) const;
        
    private:
        std::vector<Component::Ptr> mChildren;
        std::optional<int> mHoveredChild;
        std::optional<int> mFocusedChild;
        std::optional<int> mPressedChild;
        bool mIsDraggingBackground;

        friend class ContainerTest;
    };  
}
#endif