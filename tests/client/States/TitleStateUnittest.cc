#include <gtest/gtest.h>
#include "States/TitleState.hpp"

#include "Mock/Resource/MockTextureHolder.hpp"
#include "Mock/Resource/MockFontHolder.hpp"

#include "shared/TestBase.hpp"
#include "States/TestableStateStack.hpp"

#include <SFML/Graphics/RenderWindow.hpp>


class TestableTitleState : public TitleState 
{
public:
    using TitleState::getShowText;
    using TitleState::getTextEffectTime;
    
public:
    TestableTitleState(StateStack& stack, Context context)
        : TitleState(stack, context) 
        {}

};

class TitleStateTest : public PolyominoTestBase 
{
protected:
    sf::RenderWindow mWindow;
    MockTextureHolder mTextureHolder; 
    MockFontHolder mFontHolder;
    GameSessionData mGameSessionData;
    NetworkClient mNetworkClient;
    
    State::Context mContext;
    TestableStateStack mStack;

    TitleStateTest()
        : mContext(mWindow, mTextureHolder, mFontHolder, mGameSessionData, mNetworkClient, sLibrary)
        , mStack(mContext)
    {
        mTextureHolder.load(Textures::ID::TitleScreen, "dummy/path.png");
        mFontHolder.load(Fonts::ID::Sansation, "dummy/path.ttf");
    }
};


TEST_F(TitleStateTest, Update) 
{
    TestableTitleState state(mStack, mContext);
   
    EXPECT_TRUE(state.getShowText());
    EXPECT_EQ(state.getTextEffectTime(), sf::Time::Zero);

    state.update(sf::seconds(0.1f));

    EXPECT_TRUE(state.getShowText());
    EXPECT_EQ(state.getTextEffectTime(), sf::seconds(0.1f));

    state.update(sf::seconds(0.4f));
    EXPECT_FALSE(state.getShowText());
    EXPECT_EQ(state.getTextEffectTime(), sf::Time::Zero);
 
    state.update(sf::seconds(0.5f));
    EXPECT_TRUE(state.getShowText());
}

TEST_F(TitleStateTest, StateTransition) 
{
    TestableTitleState state(mStack, mContext);

    // Left click does not cause transition
    sf::Event::MouseButtonPressed leftClick;
    leftClick.button = sf::Mouse::Button::Left;
    leftClick.position = {100, 100};

    state.handleEvent(leftClick);

    auto pendingChanges = mStack.getPendingChanges();

    EXPECT_EQ(pendingChanges.size(), 0);

    // Any Key causes transition 
    sf::Event::KeyReleased spaceReleased;
    spaceReleased.code = sf::Keyboard::Key::Space;

    state.handleEvent(spaceReleased);

    pendingChanges = mStack.getPendingChanges();

    EXPECT_EQ(pendingChanges.size(), 2);
    EXPECT_EQ(pendingChanges[0].action, StateStack::Action::Pop);   
    EXPECT_EQ(pendingChanges[1].action, StateStack::Action::Push);
    EXPECT_EQ(pendingChanges[1].stateID, States::ID::Lobby);
}

