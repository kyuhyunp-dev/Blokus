#include "GUI/Container.hpp"        

#include <SFML/Window/Event.hpp>
#include <SFML/Graphics/RenderTarget.hpp>


GUI::Container::Container()
    : mChildren()
    , mSelectedChild(UNSELECTED)
    {
    }

void GUI::Container::pack(Component::Ptr component)
{
    mChildren.push_back(std::move(component));
}

bool GUI::Container::isSelectable() const 
{
    return false;
}
		
void GUI::Container::handleEvent(const sf::Event& event, 
    std::optional<sf::Vector2f> worldMousePos) 
{
    if (auto textEntered = event.getIf<sf::Event::TextEntered>())
    {
        if (hasSelection() && mChildren[mSelectedChild]->isActive())
        {
            mChildren[mSelectedChild]->handleEvent(event, worldMousePos);
        }
    }

    if (!worldMousePos.has_value())
    {
        return;
    }

    if (auto mouseClick = event.getIf<sf::Event::MouseButtonPressed>())
    {
        if (mouseClick->button == sf::Mouse::Button::Left)
        {
            for (std::size_t i = 0; i < mChildren.size(); ++i)
            {
                if (mChildren[i]->isSelectable() && 
                mChildren[i]->getGlobalBounds().contains(worldMousePos.value()))
                {
                    if (hasSelection()) 
                    {
                        mChildren[mSelectedChild]->deselect();
                        mChildren[mSelectedChild]->deactivate();
                    }

                    mSelectedChild = i;
                    mChildren[mSelectedChild]->select();
                    mChildren[mSelectedChild]->activate();
                    break;
                }
            }
        }
    }
    else if (auto mouseMoved = event.getIf<sf::Event::MouseMoved>())
    {
        if (hasSelection() && mChildren[mSelectedChild]->isActive())
        {
            return;
        }

        for (std::size_t i = 0; i < mChildren.size(); ++i)
        {
            if (mChildren[i]->isSelectable() &&
                mChildren[i]->getGlobalBounds().contains(worldMousePos.value()))
            {
                if (hasSelection()) 
                {
                    mChildren[mSelectedChild]->deselect();
                }

                mSelectedChild = i;
                mChildren[mSelectedChild]->select();
                break;
            }
        }
    }
}
    
void GUI::Container::draw(sf::RenderTarget& target, sf::RenderStates states) const 
{
    states.transform *= getTransform();

    for (const auto& child : mChildren)
    {
        target.draw(*child, states);
    }
}

bool GUI::Container::hasSelection() const
{
    return mSelectedChild != UNSELECTED;
}

sf::FloatRect GUI::Container::getGlobalBounds() const
{
    return {};
}