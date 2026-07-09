#include "GUI/Component.hpp"


GUI::Component::Component()
    : mIsHovered(false)
    , mIsPressed(false)
    , mIsFocused(false)
{
}

bool GUI::Component::isHovered() const
{
    return mIsHovered; 
}

void GUI::Component::hover()
{
    mIsHovered = true;
}

void GUI::Component::unhover()
{
    mIsHovered = false;
}

bool GUI::Component::isPressed() const
{
    return mIsPressed;
}

void GUI::Component::press(std::optional<sf::Vector2f> worldMousePos)
{
    mIsPressed = true;
}

void GUI::Component::release(std::optional<sf::Vector2f> worldMousePos)
{
    mIsPressed = false;
}

bool GUI::Component::isFocused() const
{
    return mIsFocused;
}
		
void GUI::Component::focus()
{
    mIsFocused = true;
}

void GUI::Component::unfocus()	
{
    mIsFocused = false;
}

void GUI::Component::update(sf::Time)
{
};

std::optional<sf::Cursor::Type> GUI::Component::getMouseCursorType() const
{
    return std::nullopt;
}