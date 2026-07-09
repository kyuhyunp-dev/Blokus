#include "GUI/Container.hpp"        

#include <SFML/Window/Event.hpp>
#include <SFML/Graphics/RenderTarget.hpp>


GUI::Container::Container()
    : mChildren()
    , mHoveredChild(std::nullopt)
    , mPressedChild(std::nullopt)
    , mFocusedChild(std::nullopt)
    , mIsDraggingBackground(false)
{
}

void GUI::Container::pack(Component::Ptr component)
{
    mChildren.push_back(std::move(component));
}

bool GUI::Container::isInteractive() const 
{
    return false;
}
		
void GUI::Container::handleEvent(const sf::Event& event, 
    std::optional<sf::Vector2f> worldMousePos) 
{
    if (mFocusedChild && (
        event.is<sf::Event::TextEntered>() || 
        event.is<sf::Event::KeyPressed>() ||
        event.is<sf::Event::KeyReleased>() 
    ))
    { // Only Focused Textbox can handle Key Events
        mChildren[*mFocusedChild]->handleEvent(event, worldMousePos);
        return; 
    }

    // All Mouse Events require mouse position
    if (!worldMousePos.has_value())
    {
        return;
    }

    if (auto mousePress = event.getIf<sf::Event::MouseButtonPressed>())
    {
        if (mousePress->button == sf::Mouse::Button::Left)
        { // Does not handle hover because MouseMoved already does before pressed
            std::optional<std::size_t> targetChild = getComponentIndexAt(*worldMousePos);
            if (targetChild)
            { // Pressed a Component
                if (mFocusedChild && *mFocusedChild != *targetChild)
                {
                    mChildren[*mFocusedChild]->unfocus();
                    mFocusedChild = std::nullopt;
                }

                // Includes more than one press
                mChildren[*targetChild]->press(worldMousePos);
                mPressedChild = targetChild;
                return; 
            }

            // Pressed the background
            mIsDraggingBackground = true;
            if (mFocusedChild)
            {
                mChildren[*mFocusedChild]->unfocus();
                mFocusedChild = std::nullopt;
            }
        }
    }
    else if (auto mouseRelease = event.getIf<sf::Event::MouseButtonReleased>())
    {
        if (mouseRelease->button == sf::Mouse::Button::Left)
        { // Handles hover when mouse had been pressed (mIsDraggingBackground || mPressedChild)
            // mPressedChild or mIsDraggingBackground but mutually exclusive
            assert(mPressedChild || mIsDraggingBackground);
            assert(!(mPressedChild && mIsDraggingBackground));

            std::optional<std::size_t> targetChild = getComponentIndexAt(*worldMousePos);
            if (mIsDraggingBackground)
            { // Dragged from Background to Component or Background
                mIsDraggingBackground = false;
                
                if (targetChild)
                { // Dragged from Background to Component
                    mChildren[*targetChild]->hover();
                    mHoveredChild = targetChild;
                }
            }
            else if (mPressedChild) 
            { // Dragged from Component A to A, A to B, A to background 
                if (mPressedChild == targetChild)
                { // From Component A to A: Hover already handled by MouseMoved
                    if (!mChildren[*mPressedChild]->isFocused())
                    {
                        mChildren[*mPressedChild]->focus();
                        if (mChildren[*mPressedChild]->isFocused())
                        { // Child's Persistent Focus State
                            mFocusedChild = mPressedChild;
                        }
                        else
                        { // Child's Transient Focus State
                            mFocusedChild = std::nullopt;
                        } 
                    }
                    
                    mChildren[*mPressedChild]->release(worldMousePos);
                    mPressedChild = std::nullopt;
                    return;
                }
                else
                { 
                    mChildren[*mPressedChild]->unhover();

                    if (targetChild)
                    { // From Component A to B
                        mChildren[*targetChild]->hover();
                        mHoveredChild = targetChild;
                    }
                    else
                    { // From Component A to Background
                        mHoveredChild = std::nullopt;
                    }
                }
                    
                mChildren[*mPressedChild]->release();
                mPressedChild = std::nullopt;
            }
       }
    }
    else if (auto mouseMoved = event.getIf<sf::Event::MouseMoved>())
    {
        if (mIsDraggingBackground)
        { // Should not hover when a mouse is pressed
            return;
        }
        if (mPressedChild)
        { // Should handle actions like highlight
            mChildren[*mPressedChild]->handleEvent(event, worldMousePos); 
            return;
        }
        else if (mHoveredChild && mChildren[*mHoveredChild]->getGlobalBounds().contains(*worldMousePos))
        { // Hovering the same Component
            return;
        }
        
        std::optional<std::size_t> targetChild = getComponentIndexAt(*worldMousePos);
        if (targetChild)
        { // The hovered component changed: Between Component or from Background
            if (mHoveredChild)
            { // Hovered Child exists: Mouse moved from Component A to B
                mChildren[*mHoveredChild]->unhover();
            }

            mChildren[*targetChild]->hover();
            mHoveredChild = targetChild;
            return;
        }

        if (mHoveredChild)
        { // Mouse is over the background
            mChildren[*mHoveredChild]->unhover();
            mHoveredChild = std::nullopt;
        }
    }
}

void GUI::Container::update(sf::Time dt)
{
    for (auto& child : mChildren)
    {
        child->update(dt);
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

std::optional<std::size_t> GUI::Container::getComponentIndexAt(sf::Vector2f worldMousePos) const
{
    if (mChildren.empty())
    {
        return std::nullopt;
    }

    // Loop backwards so elements drawn on top (highest Z-index) are checked first
    for (std::size_t i = mChildren.size(); i-- > 0; )
    {
        if (mChildren[i]->isInteractive() && 
            mChildren[i]->getGlobalBounds().contains(worldMousePos))
        {
            return i;
        }
    }

    return std::nullopt;
}

sf::FloatRect GUI::Container::getGlobalBounds() const
{
    return {};
}

std::optional<sf::Cursor::Type> GUI::Container::getMouseCursorType(sf::Vector2f worldMousePos) const
{
    if (mPressedChild)
    {
        return mChildren[*mPressedChild]->getMouseCursorType();
    }
    
    if (mHoveredChild)
    {
        return mChildren[*mHoveredChild]->getMouseCursorType();
    }

    return std::nullopt;
}
