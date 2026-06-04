#ifndef MOCK_COMPONENT_HPP
#define MOCK_COMPONENT_HPP

#include <gmock/gmock.h>
#include <SFML/Graphics/RenderStates.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include "GUI/Component.hpp"


namespace GUI
{
    class MockComponent : public GUI::Component 
    {
    public:
        MOCK_METHOD(bool, isSelectable, (), (const, override));
        MOCK_METHOD(sf::FloatRect, getGlobalBounds, (), (const, override));
        MOCK_METHOD(void, handleEvent, (const sf::Event&, std::optional<sf::Vector2f>), (override));
        MOCK_METHOD(void, draw, (sf::RenderTarget&, sf::RenderStates), (const, override));

        MOCK_METHOD(void, select, (), (override));
        MOCK_METHOD(void, deselect, (), (override));
        MOCK_METHOD(void, activate, (), (override));
        MOCK_METHOD(void, deactivate, (), (override));
        MOCK_METHOD(bool, isActive, (), (const, override));
    };
}

#endif