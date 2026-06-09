#include <gtest/gtest.h>
#include "Mock/GUI/MockComponent.hpp"
#include "GUI/Container.hpp"
#include "GUI/Component.hpp"
#include <SFML/Window/Event.hpp>
#include <memory>
#include <optional>


namespace GUI
{
    class ContainerTest : public ::testing::Test 
    {
    protected:
        Container container;

        testing::NiceMock<MockComponent>* child1Ptr = nullptr;
        testing::NiceMock<MockComponent>* child2Ptr = nullptr;

        void SetUp() override 
        {
            auto child1 = std::make_unique<testing::NiceMock<MockComponent>>();
            auto child2 = std::make_unique<testing::NiceMock<MockComponent>>();
            
            child1Ptr = child1.get();
            child2Ptr = child2.get();

            container.pack(std::move(child1));
            container.pack(std::move(child2));
        }

        std::size_t getChildCount() const
        {
            return container.mChildren.size();
        }

        int getSelectedChildIndex() const
        {
            return container.mSelectedChild;
        }
    };

    TEST_F(ContainerTest, Pack) 
    {
        EXPECT_EQ(getChildCount(), 2);
        EXPECT_EQ(getSelectedChildIndex(), -1); 
    }

    TEST_F(ContainerTest, handleEvents)
    {
        ON_CALL(*child1Ptr, isSelectable()).WillByDefault(testing::Return(true));
        ON_CALL(*child1Ptr, getGlobalBounds()).WillByDefault(
            testing::Return(sf::FloatRect({50.f, 50.f}, {100.f, 100.f}))
        );

        ON_CALL(*child2Ptr, isSelectable()).WillByDefault(testing::Return(true));
        ON_CALL(*child2Ptr, getGlobalBounds()).WillByDefault(
            testing::Return(sf::FloatRect({150.f, 150.f}, {100.f, 100.f}))
        );

        ON_CALL(*child1Ptr, isActive()).WillByDefault(testing::Return(false));
        ON_CALL(*child2Ptr, isActive()).WillByDefault(testing::Return(false));

        // Move mouse to point A
        sf::Vector2i posA(100, 100);
        sf::Event::MouseMoved moveA;
        moveA.position = posA;
        sf::Vector2f worldPosA {100.f, 100.f};

        EXPECT_CALL(*child1Ptr, select()).Times(1);
        EXPECT_CALL(*child2Ptr, select()).Times(0);
        container.handleEvent(moveA, worldPosA);

        // Click mouse at point B
        sf::Vector2i posB{200, 200};
        sf::Event::MouseButtonPressed clickB;
        clickB.button = sf::Mouse::Button::Left;
        clickB.position = posB;
        sf::Vector2f worldPosB {200.f, 200.f};
       
        EXPECT_CALL(*child1Ptr, deselect()).Times(1);
        EXPECT_CALL(*child2Ptr, select()).Times(1);
        EXPECT_CALL(*child2Ptr, activate()).Times(1);
        container.handleEvent(clickB, worldPosB); 

        ON_CALL(*child2Ptr, isActive()).WillByDefault(testing::Return(true));
        // Move mouse to point A
        EXPECT_CALL(*child1Ptr, select()).Times(0);
        EXPECT_CALL(*child1Ptr, deselect()).Times(0);
        container.handleEvent(moveA, worldPosA);

        // Enter Text at B
        sf::Event::TextEntered enterTextB;
        enterTextB.unicode = 'A';

        EXPECT_CALL(*child1Ptr, handleEvent(testing::_, testing::_)).Times(0);
        EXPECT_CALL(*child2Ptr, handleEvent(testing::_, testing::_)).Times(1);
        container.handleEvent(enterTextB, std::nullopt);
    }


    TEST_F(ContainerTest, handleUnselectableEvents)
    {
        ON_CALL(*child1Ptr, isSelectable()).WillByDefault(testing::Return(false));
        ON_CALL(*child1Ptr, getGlobalBounds()).WillByDefault(
            testing::Return(sf::FloatRect({50.f, 50.f}, {100.f, 100.f}))
        );

        ON_CALL(*child2Ptr, isSelectable()).WillByDefault(testing::Return(false));
        ON_CALL(*child2Ptr, getGlobalBounds()).WillByDefault(
            testing::Return(sf::FloatRect({150.f, 150.f}, {100.f, 100.f}))
        );

        // Move mouse to point A
        sf::Vector2i posA(100, 100);
        sf::Event::MouseMoved moveA; 
        moveA.position = posA;
        sf::Vector2f worldPosA {100.f, 100.f};

        EXPECT_CALL(*child1Ptr, select()).Times(0);
        EXPECT_CALL(*child2Ptr, select()).Times(0);
        container.handleEvent(moveA, worldPosA);

        // Click mouse at point B
        sf::Vector2i posB{200, 200};
        sf::Event::MouseButtonPressed clickB; 
        clickB.button = sf::Mouse::Button::Left;
        clickB.position = posB;
        sf::Vector2f worldPosB {200.f, 200.f};
       
        EXPECT_CALL(*child1Ptr, select()).Times(0);
        EXPECT_CALL(*child1Ptr, activate()).Times(0);
        EXPECT_CALL(*child2Ptr, select()).Times(0);
        EXPECT_CALL(*child2Ptr, activate()).Times(0);
        container.handleEvent(clickB, worldPosB); 
    }
    
    TEST_F(ContainerTest, handleOutofBoundsEvents)
    {
        ON_CALL(*child1Ptr, isSelectable()).WillByDefault(testing::Return(true));
        ON_CALL(*child1Ptr, getGlobalBounds()).WillByDefault(
            testing::Return(sf::FloatRect({50.f, 50.f}, {100.f, 100.f}))
        );

        ON_CALL(*child2Ptr, isSelectable()).WillByDefault(testing::Return(true));
        ON_CALL(*child2Ptr, getGlobalBounds()).WillByDefault(
            testing::Return(sf::FloatRect({150.f, 150.f}, {100.f, 100.f}))
        );

        // Move mouse to point A
        sf::Vector2i posA(300, 300);
        sf::Event::MouseMoved moveA;
        moveA.position = posA;
        sf::Vector2f worldPosA {300.f, 300.f};

        EXPECT_CALL(*child1Ptr, select()).Times(0);
        EXPECT_CALL(*child2Ptr, select()).Times(0);
        container.handleEvent(moveA, worldPosA);

        // Click mouse at point B
        sf::Event::MouseButtonPressed clickA;
        clickA.button = sf::Mouse::Button::Left;
        clickA.position = posA;
 
        EXPECT_CALL(*child1Ptr, select()).Times(0);
        EXPECT_CALL(*child1Ptr, activate()).Times(0);
        EXPECT_CALL(*child2Ptr, select()).Times(0);
        EXPECT_CALL(*child2Ptr, activate()).Times(0);      
        container.handleEvent(clickA, worldPosA); 
    }
};