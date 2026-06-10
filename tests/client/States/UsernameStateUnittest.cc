#include <gtest/gtest.h>
#include "States/UsernameState.hpp"

#include "Mock/Resource/MockTextureHolder.hpp"
#include "Mock/Resource/MockFontHolder.hpp"

#include "shared/TestBase.hpp"
#include "States/TestableStateStack.hpp"

#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Window/Event.hpp>

class TestableUsernameState : public UsernameState 
{
public:
    using UsernameState::UsernameState;
};

class UsernameStateTest : public PolyominoTestBase 
{
protected:
    sf::RenderWindow mWindow;
    MockTextureHolder mTextureHolder; 
    MockFontHolder mFontHolder;
    GameSessionData mGameSessionData;
    NetworkClient mNetworkClient;
    
    State::Context mContext;
    TestableStateStack mStack;

    UsernameStateTest()
        // Initialize sf::VideoMode here so the windowSize > 0
        : mWindow(sf::VideoMode({800, 600}), "Test Window") 
        , mContext(mWindow, mTextureHolder, mFontHolder, mGameSessionData, mNetworkClient, sLibrary)
        , mStack(mContext)
    {
        mFontHolder.load(Fonts::ID::Sansation, "dummy/path.ttf");
    }

    void simulateMouseClick(State& state, sf::Vector2i position)
    {
        // Mouse Moved (Usually necessary for GUI components to trigger hover states)
        sf::Event::MouseMoved mouseMoved;
        mouseMoved.position = position;
        state.handleEvent(mouseMoved);

        // Mouse Pressed
        sf::Event::MouseButtonPressed mousePressed;
        mousePressed.button = sf::Mouse::Button::Left;
        mousePressed.position = position;
        state.handleEvent(mousePressed);
    }

    sf::Vector2u getWindowSize() const
    {
        return mWindow.getSize();
    }
};

TEST_F(UsernameStateTest, LeaveButtonPopsAndPushesTitle) 
{
    TestableUsernameState state(mStack, mContext);

    sf::Vector2u windowSize = getWindowSize();

    // Leave button is at (windowSize.x - 120.f, 20.f). No origin adjustment was made.
    // In an 800x600 window, the top left of Leave button is (680, 20). 
    // We add +10 to x and y to click safely inside the button's bounds.
    simulateMouseClick(state, { 690, 30 });

    auto pendingChanges = mStack.getPendingChanges();

    ASSERT_EQ(pendingChanges.size(), 2);
    EXPECT_EQ(pendingChanges[0].action, StateStack::Action::Pop);   
    EXPECT_EQ(pendingChanges[1].action, StateStack::Action::Push);
    EXPECT_EQ(pendingChanges[1].stateID, States::ID::Title);
}

TEST_F(UsernameStateTest, JoinButtonFailsWithEmptyUsername) 
{
    TestableUsernameState state(mStack, mContext);

    // Join button's origin is centered. Positioned at (W/2, H/2 + 50).
    // In an 800x600 window, this is exactly (400, 350).
    simulateMouseClick(state, { 400, 350 });

    auto pendingChanges = mStack.getPendingChanges();

    // The stack should remain untouched because validation blocked it
    EXPECT_EQ(pendingChanges.size(), 0);
}

TEST_F(UsernameStateTest, JoinButtonSucceedsWithValidUsername) 
{
    TestableUsernameState state(mStack, mContext);

    // Click the TextBox to activate it. 
    // Origin is centered. Positioned at (W/2, H/2 - 25).
    // In an 800x600 window, this is exactly (400, 275).
    simulateMouseClick(state, { 400, 275 });

    // Simulate typing a valid character
    sf::Event::TextEntered textEntered;
    textEntered.unicode = U'A'; 
    state.handleEvent(textEntered);

    // Click the Join button
    simulateMouseClick(state, { 400, 350 });

    auto pendingChanges = mStack.getPendingChanges();

    // The validation should pass and push the Lobby
    ASSERT_EQ(pendingChanges.size(), 2);
    EXPECT_EQ(pendingChanges[0].action, StateStack::Action::Pop);   
    EXPECT_EQ(pendingChanges[1].action, StateStack::Action::Push);
    EXPECT_EQ(pendingChanges[1].stateID, States::ID::Lobby);
}