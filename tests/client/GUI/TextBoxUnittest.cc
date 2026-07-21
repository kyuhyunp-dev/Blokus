#include <gtest/gtest.h>
#include "GUI/TextBox.hpp"
#include "GUI/Button.hpp"
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

        // Helper lambdas to keep event firing clean
        void sendText(TextBox& box, uint32_t unicode)
        {
            sf::Event::TextEntered text;
            text.unicode = unicode;
            box.handleEvent(text, std::nullopt);
        }

        void sendKeyPress(TextBox& box, sf::Keyboard::Key code)
        {
            sf::Event::KeyPressed key;
            key.code = code;
            box.handleEvent(key, std::nullopt);
        }

        void sendKeyRelease(TextBox& box, sf::Keyboard::Key code)
        {
            sf::Event::KeyReleased key;
            key.code = code;
            box.handleEvent(key, std::nullopt);
        }

        void sendMouseMove(TextBox& box, sf::Vector2f pos)
        {
            sf::Event::MouseMoved mouse;
            box.handleEvent(mouse, pos);
        }
    };
    
    TEST_F(TextBoxTest, ComponentTest) 
    {
        TextBox box("Enter 4 digit code", font, {200.f, 50.f});

        // Constructor state
        EXPECT_TRUE(box.isInteractive());
        EXPECT_FALSE(box.isHovered());
        EXPECT_FALSE(box.isPressed());
        EXPECT_FALSE(box.isFocused());

        box.hover();
        EXPECT_TRUE(box.isHovered());
        EXPECT_FALSE(box.isPressed());
        EXPECT_FALSE(box.isFocused());

        box.press();
        EXPECT_TRUE(box.isHovered());
        EXPECT_TRUE(box.isPressed());
        EXPECT_FALSE(box.isFocused());

        box.focus();
        EXPECT_TRUE(box.isHovered());
        EXPECT_TRUE(box.isPressed());
        EXPECT_TRUE(box.isFocused());

        box.release();
        EXPECT_TRUE(box.isHovered());
        EXPECT_FALSE(box.isPressed());
        EXPECT_TRUE(box.isFocused());

        box.unhover();
        EXPECT_FALSE(box.isHovered());
        EXPECT_FALSE(box.isPressed());
        EXPECT_TRUE(box.isFocused());

        box.unfocus();
        EXPECT_FALSE(box.isHovered());
        EXPECT_FALSE(box.isPressed());
        EXPECT_FALSE(box.isFocused());
    }

    TEST_F(TextBoxTest, Cursor)
    {
        TextBox box("Enter 4 digit code", font, {200.f, 50.f});
        box.setPosition({ 100.f, 25.f });

        Button button("button", font, {100.f, 100.f});
        button.setPosition({50.f, 50.f});

        sf::Vector2f mid = sf::Vector2f(100.f, 25.f);
        sf::Vector2f out = sf::Vector2f(300.f, 150.f);
        sf::Vector2f inButton = sf::Vector2f(250.f, 50.f);

        // Pressing and Hovering the TextBox
        box.hover();
        box.press(mid);
        EXPECT_EQ(box.getMouseCursorType(), sf::Cursor::Type::Text);   

        box.release(mid);
        EXPECT_EQ(box.getMouseCursorType(), sf::Cursor::Type::Text);
    
        // Pressing and Hovering the Background
        box.press(mid);

        box.unhover();
        sendMouseMove(box, out);
        EXPECT_EQ(box.getMouseCursorType(), sf::Cursor::Type::Text);

        box.release(mid);
        EXPECT_EQ(box.getMouseCursorType(), std::nullopt);
    
        // Pressing and Hovering the Button 
        box.hover();
        box.press(mid);

        box.unhover();
        sendMouseMove(box, inButton);
        EXPECT_EQ(box.getMouseCursorType(), sf::Cursor::Type::Text);

        box.release();
        EXPECT_EQ(box.getMouseCursorType(), std::nullopt);
    }

   TEST_F(TextBoxTest, TextEntryAndLimits) 
    {
        TextBox box("Enter 4 digit code", font, {200.f, 50.f});
        box.focus(); 

        // Regular typing
        sendText(box, 'C');
        sendText(box, 'A');
        sendText(box, 'T');
        EXPECT_EQ(box.getText(), "CAT");

        // Backspace (ASCII 8)
        sendKeyPress(box, sf::Keyboard::Key::Backspace); 
        EXPECT_EQ(box.getText(), "CA");

        // Prevent popping an empty string
        for (int i = 0; i < 3; ++i)
        {
            sendKeyPress(box, sf::Keyboard::Key::Backspace); 
        }

        EXPECT_EQ(box.getText(), "");

        // Character limits (Assuming limit is 10)
        for (int i = 0; i < 15; ++i) 
        {
            sendText(box, 'W');
        }
        EXPECT_EQ(box.getText().size(), 10); 

        // Enter/Return key (ASCII 13)
        sendKeyRelease(box, sf::Keyboard::Key::Enter); 
        EXPECT_FALSE(box.isFocused()); // Hitting enter should deactivate the box
    }

    TEST_F(TextBoxTest, CursorBlinkingOverTime) 
    {
        TextBox box("Enter 4 digit code", font, {200.f, 50.f});
        box.setPosition({100.f, 25.f});
        sf::Vector2f mid = sf::Vector2f(100.f, 25.f);

        // Activate the box
        box.press(mid);
        box.release(mid);
        box.focus();

        EXPECT_FALSE(box.getShowCursor());

        box.update(sf::milliseconds(550));
        EXPECT_TRUE(box.getShowCursor());
        
        box.update(sf::milliseconds(550));
        EXPECT_FALSE(box.getShowCursor());
    }

    TEST_F(TextBoxTest, Highlight)
    {
        TextBox box("Enter 4 digit code", font, {200.f, 50.f});
        box.setPosition({100.f, 25.f});
        sf::Vector2f mid = sf::Vector2f(100.f, 25.f);

        box.press(mid);
        box.release(mid);
        box.focus();

        sendText(box, 'H');
        sendText(box, 'E');
        sendText(box, 'L');
        sendText(box, 'L');
        sendText(box, 'O');
        
        EXPECT_FALSE(box.hasHighlight());
        EXPECT_TRUE(box.getShowCursor());

        // Press at index 0 (far left)
        box.press(sf::Vector2f(0.f, 25.f));
        ASSERT_TRUE(box.isPressed());

        //  Move mouse after press -> mSelectionEnd updates
        sendMouseMove(box, sf::Vector2f(1000.f, 25.f)); // Drag to far right

        // Highlight content within the box
        EXPECT_TRUE(box.hasHighlight());

        // During highlight, the TextBox doesn't show any cursor
        EXPECT_FALSE(box.getShowCursor());
        
        box.release();
        
        EXPECT_TRUE(box.hasHighlight());
        EXPECT_FALSE(box.getShowCursor());
    }

    TEST_F(TextBoxTest, HighlightAndErase) 
    {
        TextBox box("Enter 4 digit code", font, {200.f, 50.f});
        box.setPosition({100.f, 25.f});

        box.focus();

        sendText(box, 'T');
        sendText(box, 'E');
        sendText(box, 'S');
        sendText(box, 'T');

        // Highlight the entire word "TEST"
        box.press(sf::Vector2f(0.f, 25.f));
        sendMouseMove(box, sf::Vector2f(1000.f, 25.f));
        box.release();

        EXPECT_TRUE(box.hasHighlight());

        //  Type during highlight -> highlighted text is erased, new text inserted
        sendText(box, 'X');
        EXPECT_EQ(box.getText(), "X");

        EXPECT_FALSE(box.hasHighlight());
        EXPECT_TRUE(box.getShowCursor());

        // Repopulate and test Backspace
        sendText(box, 'Y');
        sendText(box, 'Z');
        
        // Highlight "XYZ"
        box.press(sf::Vector2f(0.f, 25.f));
        sendMouseMove(box, sf::Vector2f(1000.f, 25.f));
        box.release();

        // Backspace during highlight -> highlighted text is erased
        sendKeyPress(box, sf::Keyboard::Key::Backspace);
        EXPECT_EQ(box.getText(), "");
        EXPECT_FALSE(box.hasHighlight());
    }

    TEST_F(TextBoxTest, HighlightArrowSpace) 
    {
        TextBox box("Enter 4 digit code", font, {200.f, 50.f});
        box.setPosition({100.f, 25.f});
        box.focus();

        sendText(box, 'A');
        sendText(box, 'B');
        sendText(box, 'C');

        // Highlight the entire string (Indices 0 to 3)
        box.press(sf::Vector2f(0.f, 25.f));
        sendMouseMove(box, sf::Vector2f(1000.f, 25.f));
        box.release();
        EXPECT_TRUE(box.hasHighlight());

        // Left arrow during highlight -> cursor index is minimum of region
        sendKeyPress(box, sf::Keyboard::Key::Left);
        EXPECT_FALSE(box.hasHighlight());
        EXPECT_TRUE(box.getShowCursor());

        // Cursor is now at 0, so backspace should do nothing
        sendKeyPress(box, sf::Keyboard::Key::Backspace);
        EXPECT_EQ(box.getText(), "ABC"); 

        // Highlight again (Indices 0 to 3)
        box.press(sf::Vector2f(0.f, 25.f));
        sendMouseMove(box, sf::Vector2f(1000.f, 25.f));
        box.release();

        //  Right arrow during highlight -> cursor index is maximum of region
        sendKeyPress(box, sf::Keyboard::Key::Right);
        EXPECT_FALSE(box.hasHighlight());
        EXPECT_TRUE(box.getShowCursor());

        // Cursor is now at 3, backspace should delete 'C'
        sendKeyPress(box, sf::Keyboard::Key::Backspace);
        EXPECT_EQ(box.getText(), "AB");
    }

    TEST_F(TextBoxTest, RepeatActionBackspace) 
    {
        TextBox box("Enter 4 digit code", font, {200.f, 50.f});
        box.setPosition({100.f, 25.f});

        box.focus();
        
        // Type 5 characters
        for (int i = 0; i < 5; ++i) {
            sendText(box, 'W');
        }
        EXPECT_EQ(box.getText(), "WWWWW");

        // Handles repeat action that repeats faster after one action
        sendKeyPress(box, sf::Keyboard::Key::Backspace); 

        // Initial press triggers one immediate backspace
        EXPECT_EQ(box.getText(), "WWWW");

        // Simulate time passing (Initial Delay is 260ms)
        box.update(sf::milliseconds(250));
        EXPECT_EQ(box.getText(), "WWWW"); // Should not delete yet (In initial delay)

        box.update(sf::milliseconds(40)); 
        EXPECT_EQ(box.getText(), "WWWW");
        // Total time > 260ms, initial delay is cleared. 
        
        // Repeat delay is 40ms. Passing 45ms should trigger one repeat action.
        box.update(sf::milliseconds(40));
        EXPECT_EQ(box.getText(), "WWW");

        box.update(sf::milliseconds(40));
        EXPECT_EQ(box.getText(), "WW");
       
        // Releasing the key stops the action
        sendKeyRelease(box, sf::Keyboard::Key::Backspace);
        box.update(sf::milliseconds(100));
        EXPECT_EQ(box.getText(), "WW");
    }
    
    TEST_F(TextBoxTest, RepeatActionArrow) 
    {
        TextBox box("Enter 4 digit code", font, {200.f, 50.f});
        box.setPosition({100.f, 25.f});
        box.focus();
        
        // Type 5 characters
        for (int i = 0; i < 5; ++i) {
            sendText(box, 'W');
        }
        EXPECT_EQ(box.getText(), "WWWWW");

        // Handles repeat action that repeats faster after one action
        sendKeyPress(box, sf::Keyboard::Key::Left); 
        sendKeyRelease(box, sf::Keyboard::Key::Left);
        box.update(sf::milliseconds(40));

        // Insert A in index 4 
        sendText(box, 'A');
        EXPECT_EQ(box.getText(), "WWWWAW");

        // Simulate time passing (Initial Delay is 260ms, 40ms after)
        sendKeyPress(box, sf::Keyboard::Key::Left); 
        box.update(sf::milliseconds(260));
        box.update(sf::milliseconds(40));
        sendKeyRelease(box, sf::Keyboard::Key::Left);

        // Insert A in index 4 
        sendText(box, 'B');
        EXPECT_EQ(box.getText(), "WWWBWAW");

        sendKeyPress(box, sf::Keyboard::Key::Left);
        box.update(sf::milliseconds(260));
        for (int i = 0; i < 2; ++i)
        {
            box.update(sf::milliseconds(40));
        }
        sendKeyRelease(box, sf::Keyboard::Key::Left);

        // Insert B in index 1
        sendText(box, 'C');
        EXPECT_EQ(box.getText(), "WCWWBWAW");
       
        sendKeyPress(box, sf::Keyboard::Key::Right);

        box.update(sf::milliseconds(260));
        for (int i = 0; i < 4; ++i)
        {
            box.update(sf::milliseconds(40));
        }
        sendKeyRelease(box, sf::Keyboard::Key::Right);

        sendText(box, 'D');
        EXPECT_EQ(box.getText(), "WCWWBWADW");
    }
    
    TEST_F(TextBoxTest, Unfocus) 
    {
        TextBox box("Enter 4 digit code", font, {200.f, 50.f});
        box.setPosition({100.f, 25.f});
        sf::Vector2f mid = sf::Vector2f(100.f, 25.f);

        box.press(mid);
        box.release(mid);
        box.focus();
        
        sendText(box, 'A');
        sendText(box, 'N');
        sendText(box, 'N');
        sendText(box, 'A');

        box.press(sf::Vector2f(0.f, 25.f));
        sendMouseMove(box, mid);
        box.release(mid);

        ASSERT_TRUE(box.hasHighlight());
        ASSERT_FALSE(box.getShowCursor());

        box.unfocus();

        EXPECT_FALSE(box.hasHighlight());
        EXPECT_FALSE(box.getShowCursor());  
    }

    TEST_F(TextBoxTest, DynamicLayoutUpdates) 
    {
        TextBox box("Enter 4 digit code", font, {200.f, 50.f});
        box.setPosition({100.f, 25.f});
        box.focus();

        // Capture initial placeholder positions (empty string displays "Enter 4 Digit Code")
        float emptyTextX = box.getTextPosition().x;
        float emptyCursorX = box.getCursorPosition().x;

        // The cursor should start exactly in the middle of the 200px box
        EXPECT_LE(emptyTextX, 200.f);
        EXPECT_FLOAT_EQ(emptyCursorX, 100.f); 

        float previousTextX = emptyTextX;
        float previousCursorX = emptyCursorX;

        for (int i = 0; i < 10; ++i) 
        { 
            sendText(box, 'W');

            float typedTextX = box.getTextPosition().x;
            float typedCursorX = box.getCursorPosition().x;

            if (i == 0)
            {
                // Edge Case 1: First character typed!
                // It replaces the wide placeholder with a single 'W'.
                // Because the text shrunk significantly, its starting X must move RIGHT.
                EXPECT_GT(typedTextX, previousTextX);
            }
            else
            {
                // Standard Case: "W" -> "WW" -> "WWW"
                // The string is getting wider. To remain perfectly centered, 
                // the left anchor of the text MUST shift LEFT.
                EXPECT_LT(typedTextX, previousTextX);
            }

            // The cursor must always advance to the right of its previous position
            EXPECT_GT(typedCursorX, previousCursorX);

            // The cursor must always be positioned to the right of the text's starting X
            EXPECT_GT(typedCursorX, typedTextX);

            // Update tracking variables for the next loop iteration
            previousTextX = typedTextX;
            previousCursorX = typedCursorX;
        }
    }
}
