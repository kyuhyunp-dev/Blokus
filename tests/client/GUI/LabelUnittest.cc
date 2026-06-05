#include <gtest/gtest.h>
#include "GUI/Label.hpp"
#include <SFML/Window/Event.hpp>
#include <SFML/Graphics/Font.hpp>


class LabelTest : public ::testing::Test 
{
protected:
    sf::Font font;
    
    void SetUp() override 
    {
        if (!font.openFromFile("assets/client/fonts/sansation.ttf")) 
        {
            FAIL() << "Setup Failed: Could not load font required for LabelTest.";
        }
    }
};

TEST_F(LabelTest, LabelIsNeverSelectable) 
{
    GUI::Label label("Score: 0", font);
    EXPECT_FALSE(label.isSelectable());
}

TEST_F(LabelTest, HandleEventDoesNotModifyStateOrCrash) 
{
    GUI::Label label("Main Menu", font);
    
    sf::Event::MouseButtonPressed click; 
    click.button = sf::Mouse::Button::Left;
    click.position = {10, 10};
    
    sf::Vector2f worldPos{10.f, 10.f};

    EXPECT_NO_THROW(label.handleEvent(click, worldPos));
    
    EXPECT_FALSE(label.isSelected());
    EXPECT_FALSE(label.isActive());
}

TEST_F(LabelTest, SetTextUpdatesGlobalBoundsSize) 
{
    GUI::Label label("Short", font);
    sf::FloatRect initialBounds = label.getGlobalBounds();

    label.setText("This is a much longer string of text");
    sf::FloatRect updatedBounds = label.getGlobalBounds();

    EXPECT_GT(updatedBounds.size.x, initialBounds.size.x);
}

TEST_F(LabelTest, MovingLabelShiftsGlobalBounds) 
{
    GUI::Label label("Player 1", font);

    sf::FloatRect originalBounds = label.getGlobalBounds();

    label.setPosition({250.f, 400.f});
    sf::FloatRect movedBounds = label.getGlobalBounds();

    EXPECT_EQ(movedBounds.position.x, originalBounds.position.x + 250.f);
    EXPECT_EQ(movedBounds.position.y, originalBounds.position.y + 400.f);
    
    EXPECT_EQ(movedBounds.size.x, originalBounds.size.x);
    EXPECT_EQ(movedBounds.size.y, originalBounds.size.y);
}