#include <gtest/gtest.h>
#include "GUI/TextBox.hpp"
#include <SFML/Window/Event.hpp>
#include <SFML/Graphics/Font.hpp>
#include <SFML/Graphics/RenderTexture.hpp>
#include <thread>
#include <chrono>


namespace GUI
{
    class TextBoxTest : public ::testing::Test 
    {
    protected:
        sf::Font font;
        
        void SetUp() override 
        {
            if (!font.openFromFile("assets/client/fonts/sansation.ttf")) 
            {
                FAIL() << "Failed to load font required for TextBox tests.";
            }
        }

        // Helper lambda to keep event firing clean
        void sendText(TextBox& box, uint32_t unicode)
        {
            sf::Event::TextEntered texts;
            texts.unicode = unicode;
            box.handleEvent(texts, std::nullopt);
        }
    };
    
    TEST_F(TextBoxTest, ComponentTest) 
    {
        TextBox box(font, {200.f, 50.f});

        // Constructor state
        EXPECT_TRUE(box.isSelectable());
        EXPECT_FALSE(box.isSelected());
        EXPECT_FALSE(box.isActive());

        // Hover selection
        box.select();
        EXPECT_TRUE(box.isSelected());
        EXPECT_FALSE(box.isActive());

        // Typing activation
        box.activate();
        EXPECT_TRUE(box.isSelected());
        EXPECT_TRUE(box.isActive());

        // Deactivation (like hitting Enter)
        box.deactivate();
        EXPECT_FALSE(box.isActive());
        EXPECT_TRUE(box.isSelected());

        // Moving mouse away
        box.deselect();
        EXPECT_FALSE(box.isSelected());
        EXPECT_FALSE(box.isActive());
    }

    TEST_F(TextBoxTest, TextEntryAndLimits) 
    {
        TextBox box(font, {200.f, 50.f});
        
        box.activate(); 

        // Regular typing
        sendText(box, 'C');
        sendText(box, 'A');
        sendText(box, 'T');
        EXPECT_EQ(box.getText(), "CAT");

        // Backspace (ASCII 8)
        sendText(box, 8); 
        EXPECT_EQ(box.getText(), "CA");

        // Prevent popping an empty string
        sendText(box, 8); 
        sendText(box, 8); 
        sendText(box, 8); // Should safely do nothing
        EXPECT_EQ(box.getText(), "");

        // Character limits (Assuming limit is 10)
        for (int i = 0; i < 15; ++i) 
        {
            sendText(box, 'W');
        }
        EXPECT_EQ(box.getText().size(), 10); 

        // Enter/Return key (ASCII 13)
        sendText(box, 13);
        EXPECT_FALSE(box.isActive()); // Hitting enter should deactivate the box
    }

    TEST_F(TextBoxTest, DynamicLayoutUpdates) 
    {
        TextBox box(font, {200.f, 50.f});
        box.activate();

        // Capture initial perfectly centered positions (empty string)
        float emptyTextX = box.getTextPosition().x;
        float emptyCursorX = box.getCursorPosition().x;

        // The cursor should start exactly in the middle of the 200px box
        EXPECT_FLOAT_EQ(emptyTextX, 100.f);
        EXPECT_FLOAT_EQ(emptyCursorX, 100.f); 

        for (int i = 0; i < 10; ++i) 
        { // Type 'W'
            sendText(box, 'W');

            float typedTextX = box.getTextPosition().x;
            float typedCursorX = box.getCursorPosition().x;

            // To maintain center, the text's left anchor must shift left
            EXPECT_LT(typedTextX, emptyTextX);

            // The cursor must advance to the right of the starting center point
            EXPECT_GT(typedCursorX, emptyCursorX);

            // The cursor must always be positioned to the right of the text's starting X
            EXPECT_GT(typedCursorX, typedTextX);
        }

        for (int i = 0; i < 9; ++i)
        { // Delete characters
            sendText(box, 8);

            float typedTextX = box.getTextPosition().x;
            float typedCursorX = box.getCursorPosition().x;

            // To maintain center, the text's left anchor must shift left
            EXPECT_LT(typedTextX, emptyTextX);

            // The cursor must advance to the right of the starting center point
            EXPECT_GT(typedCursorX, emptyCursorX);

            // The cursor must always be positioned to the right of the text's starting X
            EXPECT_GT(typedCursorX, typedTextX);
        }

        sendText(box, 8);

        // Everything should perfectly snap back to the initial empty center
        EXPECT_FLOAT_EQ(box.getTextPosition().x, emptyTextX);
        EXPECT_FLOAT_EQ(box.getCursorPosition().x, emptyCursorX);
    }

    TEST_F(TextBoxTest, CursorBlinkingOverTime) 
    {
        TextBox box(font, {200.f, 50.f});
        
        sf::RenderTexture dummyTarget;

        // Activate the box
        box.activate();

        // Frame 1: Trigger the draw method
        dummyTarget.draw(box);
        EXPECT_TRUE(box.getShowCursor());

        // Frame 2: Increment time
        std::this_thread::sleep_for(std::chrono::milliseconds(550));
        dummyTarget.draw(box);
        EXPECT_FALSE(box.getShowCursor());
        
        // Sleep again to verify it toggles back
        std::this_thread::sleep_for(std::chrono::milliseconds(550));
        dummyTarget.draw(box);
        EXPECT_TRUE(box.getShowCursor());
    }
}