#ifndef MOCK_COMPONENT_HPP
#define MOCK_COMPONENT_HPP

#include <gmock/gmock.h>
#include <SFML/Graphics/RenderStates.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include "GUI/Component.hpp"

using ::testing::_;
using ::testing::Invoke;
using ::testing::Return;


class MockComponent : public GUI::Component 
{
public:
    MockComponent() 
    {
        ON_CALL(*this, isInteractive()).WillByDefault(testing::Return(true));

        // Use explicit scope resolution in lambdas to bypass the vtable
        ON_CALL(*this, focus()).WillByDefault(testing::Invoke([this]() { 
            this->GUI::Component::focus(); 
        }));
        
        ON_CALL(*this, unfocus()).WillByDefault(testing::Invoke([this]() { 
            this->GUI::Component::unfocus(); 
        }));
        
        ON_CALL(*this, press(testing::_)).WillByDefault(testing::Invoke([this](std::optional<sf::Vector2f> p) { 
            this->GUI::Component::press(p); 
        }));
        
        ON_CALL(*this, release(testing::_)).WillByDefault(testing::Invoke([this](std::optional<sf::Vector2f> p) { 
            this->GUI::Component::release(p); 
        }));
        
        // Add these if you want to test hover state accurately as well
        ON_CALL(*this, hover()).WillByDefault(testing::Invoke([this]() { 
            this->GUI::Component::hover(); 
        }));
        
        ON_CALL(*this, unhover()).WillByDefault(testing::Invoke([this]() { 
            this->GUI::Component::unhover(); 
        }));
    }

    MOCK_METHOD(bool, isInteractive, (), (const, override));
    MOCK_METHOD(void, hover, (), (override));
    MOCK_METHOD(void, unhover, (), (override));
    MOCK_METHOD(void, press, (std::optional<sf::Vector2f>), (override));
    MOCK_METHOD(void, release, (std::optional<sf::Vector2f>), (override));
    MOCK_METHOD(void, focus, (), (override));
    MOCK_METHOD(void, unfocus, (), (override));
    MOCK_METHOD(void, handleEvent, (const sf::Event&, std::optional<sf::Vector2f>), (override));
    
    MOCK_METHOD(sf::FloatRect, getGlobalBounds, (), (const, override));
    MOCK_METHOD(std::optional<sf::Cursor::Type>, getMouseCursorType, (), (const, override));

    MOCK_METHOD(void, draw, (sf::RenderTarget&, sf::RenderStates), (const, override));   
};

#endif