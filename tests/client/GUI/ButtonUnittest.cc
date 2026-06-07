#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "GUI/Button.hpp"
#include <SFML/Window/Event.hpp>
#include <SFML/Graphics/Font.hpp>
#include <iostream>

// Mock class to verify callback execution
class MockCallback 
{
public:
    MOCK_METHOD(void, onActivate, ());
};

class ButtonTest : public ::testing::Test 
{
protected:
    sf::Font font;
    sf::Vector2f buttonSize{150.f, 50.f};
    const std::string text = "Click Me";
    
    void SetUp() override 
    {
        if (!font.openFromFile("assets/client/fonts/sansation.ttf")) 
        {
            FAIL() << "Setup Failed: Could not load font required for ButtonTest.";
        }
    }
};

TEST_F(ButtonTest, ConstructorInitializesCorrectly) 
{
    GUI::Button button(text, font, buttonSize);
    
    EXPECT_TRUE(button.isSelectable());
    EXPECT_FALSE(button.isSelected());
    EXPECT_FALSE(button.isActive());
}

TEST_F(ButtonTest, DeselectUpdatesState) 
{
    GUI::Button button(text, font, buttonSize);
    
    button.select();
    EXPECT_TRUE(button.isSelected());

    button.deselect();
    EXPECT_FALSE(button.isSelected());
}

TEST_F(ButtonTest, ActivateTriggersCallbackAndResetsState) 
{
    GUI::Button button(text, font, buttonSize);
    MockCallback mock;

    EXPECT_CALL(mock, onActivate())
        .Times(1);

    button.setCallback([&mock]() {
        mock.onActivate();
    });

    button.activate();
    EXPECT_FALSE(button.isActive());
}

TEST_F(ButtonTest, HandleEventIsNoOp) 
{
    GUI::Button button(text, font, buttonSize);
    
    sf::Event::MouseButtonPressed click; 
    click.button = sf::Mouse::Button::Left;
    click.position = {10, 10};
    
    sf::Vector2f worldPos{10.f, 10.f};

    EXPECT_NO_THROW(button.handleEvent(click, worldPos));
    
    EXPECT_FALSE(button.isSelected());
    EXPECT_FALSE(button.isActive());
}

TEST_F(ButtonTest, GetGlobalBoundsReflectsTransformAndSize) 
{
    GUI::Button button(text, font, buttonSize);
    
    sf::FloatRect initialBounds = button.getGlobalBounds();
    
    EXPECT_EQ(initialBounds.size.x, buttonSize.x);
    EXPECT_EQ(initialBounds.size.y, buttonSize.y);

    button.setPosition({250.f, 400.f});
    sf::FloatRect movedBounds = button.getGlobalBounds();

    EXPECT_EQ(movedBounds.position.x, initialBounds.position.x + 250.f);
    EXPECT_EQ(movedBounds.position.y, initialBounds.position.y + 400.f);
    
    EXPECT_EQ(movedBounds.size.x, initialBounds.size.x);
    EXPECT_EQ(movedBounds.size.y, initialBounds.size.y);
}