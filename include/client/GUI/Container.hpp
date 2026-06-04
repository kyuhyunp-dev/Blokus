#ifndef GUI_CONTAINER_HPP
#define GUI_CONTAINER_HPP

#include "GUI/Component.hpp"
#include <SFML/Graphics/RenderStates.hpp>
#include <vector>
#include <memory>


namespace GUI
{
    class Container: public Component
    {
    public:
        Container();

        void pack(Component::Ptr component);

    	bool isSelectable() const override;
		
        void handleEvent(const sf::Event& event, 
            std::optional<sf::Vector2f> worldMousePos) override;

        sf::FloatRect getGlobalBounds() const override;
    
    private:
        void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

        bool hasSelection() const;

    private:
        static const int UNSELECTED = -1;

        std::vector<Component::Ptr> mChildren;
        int mSelectedChild;

        friend class ContainerTest;
    };  
};
#endif