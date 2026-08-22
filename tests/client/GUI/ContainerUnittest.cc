#include <gtest/gtest.h>
#include "Mock/GUI/MockComponent.hpp"
#include "GUI/Container.hpp"
#include "GUI/Component.hpp"
#include <SFML/Window/Event.hpp>
#include <memory>
#include <optional>


TEST(ContainerTest, SameComponentType)
{
    GUI::Container container;

    auto componentA = std::make_unique<testing::NiceMock<MockComponent>>();
    auto componentB = std::make_unique<testing::NiceMock<MockComponent>>();

    ON_CALL(*componentA, getGlobalBounds()).WillByDefault(Return(sf::FloatRect({0.f, 0.f}, {100.f, 50.f})));
    ON_CALL(*componentB, getGlobalBounds()).WillByDefault(Return(sf::FloatRect({0.f, 60.f}, {100.f, 50.f})));

    auto rawCompA = componentA.get();
    auto rawCompB = componentB.get();

    container.pack(std::move(componentA));
    container.pack(std::move(componentB));

    sf::Vector2f posA(50.f, 25.f);

    // Moving the mouse within the component
    EXPECT_CALL(*rawCompA, hover()).Times(1);
    container.handleEvent(sf::Event::MouseMoved{{50, 25}}, posA);

    // Pressing the component
    EXPECT_CALL(*rawCompA, press(std::make_optional(posA))).Times(1);
    container.handleEvent(sf::Event::MouseButtonPressed{sf::Mouse::Button::Left, {50, 25}}, posA);
    
    // Releasing the component
    EXPECT_CALL(*rawCompA, focus()).Times(1);
    EXPECT_CALL(*rawCompA, release(std::make_optional(posA))).Times(1);
    container.handleEvent(sf::Event::MouseButtonReleased{sf::Mouse::Button::Left, {50, 25}}, posA);

    EXPECT_CALL(*rawCompA, handleEvent(testing::_, testing::_)).Times(1);
    EXPECT_CALL(*rawCompB, handleEvent(testing::_, testing::_)).Times(0);
    container.handleEvent(sf::Event::TextEntered{'A'}, std::nullopt); 

    sf::Vector2f posB(50.f, 75.f);

    EXPECT_CALL(*rawCompA, unhover()).Times(1);
    EXPECT_CALL(*rawCompB, hover()).Times(1);
    container.handleEvent(sf::Event::MouseMoved{{50, 75}}, posB);

    EXPECT_CALL(*rawCompA, unfocus()).Times(1);
    EXPECT_CALL(*rawCompB, press(std::make_optional(posB))).Times(1);
    container.handleEvent(sf::Event::MouseButtonPressed{sf::Mouse::Button::Left, {50, 75}}, posB);

    EXPECT_CALL(*rawCompB, focus()).Times(1);
    EXPECT_CALL(*rawCompB, release(std::make_optional(posB))).Times(1);
    container.handleEvent(sf::Event::MouseButtonReleased{sf::Mouse::Button::Left, {50, 75}}, posB);
}

TEST(ContainerTest, BackgroundDrag)
{
    GUI::Container container;
    auto comp = std::make_unique<testing::NiceMock<MockComponent>>(); 
    
    ON_CALL(*comp, getGlobalBounds()).WillByDefault(Return(sf::FloatRect({100.f, 100.f}, {50.f, 50.f})));

    auto rawComp = comp.get();
    container.pack(std::move(comp));

    sf::Vector2f backgroundPos(10.f, 10.f);
    sf::Vector2f componentPos(125.f, 125.f);

    EXPECT_CALL(*rawComp, hover()).Times(0);
    EXPECT_CALL(*rawComp, focus()).Times(0);
    container.handleEvent(sf::Event::MouseButtonPressed{sf::Mouse::Button::Left, {10, 10}}, backgroundPos);

    EXPECT_CALL(*rawComp, hover()).Times(0);
    EXPECT_CALL(*rawComp, focus()).Times(0);
    container.handleEvent(sf::Event::MouseMoved{{125, 125}}, componentPos);

    EXPECT_CALL(*rawComp, hover()).Times(1);
    EXPECT_CALL(*rawComp, focus()).Times(0);
    container.handleEvent(sf::Event::MouseButtonReleased{sf::Mouse::Button::Left, {125, 125}}, componentPos);
}

TEST(ContainerTest, DragOut)
{
    GUI::Container container;
    auto comp = std::make_unique<testing::NiceMock<MockComponent>>(); 
    
    ON_CALL(*comp, getGlobalBounds()).WillByDefault(Return(sf::FloatRect({100.f, 100.f}, {50.f, 50.f})));

    auto rawComp = comp.get();
    container.pack(std::move(comp));

    sf::Vector2f backgroundPos(10.f, 10.f);
    sf::Vector2f componentPos(125.f, 125.f);

    // TEST not unhover after clicking the component and drags out 
    EXPECT_CALL(*rawComp, hover()).Times(1);
    container.handleEvent(sf::Event::MouseMoved{{125, 125}}, componentPos);

    EXPECT_CALL(*rawComp, press(std::make_optional(componentPos))).Times(1);
    container.handleEvent(sf::Event::MouseButtonPressed{sf::Mouse::Button::Left, {125, 125}}, componentPos);

    EXPECT_CALL(*rawComp, unhover()).Times(0);
    container.handleEvent(sf::Event::MouseMoved{{10, 10}}, backgroundPos);

    EXPECT_CALL(*rawComp, unhover()).Times(1);
    EXPECT_CALL(*rawComp, release(testing::Eq(std::nullopt))).Times(1);
    container.handleEvent(sf::Event::MouseButtonReleased{sf::Mouse::Button::Left, {10, 10}}, backgroundPos);
}
