#include <gtest/gtest.h>
#include "States/NetworkGameState.hpp"
#include "States/StateStack.hpp"
#include "Mock/Nodes/MockArena.hpp"
#include "Mock/Resource/MockTextureHolder.hpp"
#include "Mock/Resource/MockFontHolder.hpp"
#include <SFML/Graphics/RenderWindow.hpp>

#include "Mock/MockPlayer.hpp"


class TestableNetworkGameState : public NetworkGameState 
{
public:
    using NetworkGameState::getCommandQueue;
     
public:
    MockArena* mockArenaPtr = nullptr; 

    TestableNetworkGameState(StateStack& stack, Context context)
        : NetworkGameState(stack, context) 
        {}

protected:
    // Inject the Mock Arena
    std::unique_ptr<IArena> createArena() override 
    {
        auto mock = std::make_unique<testing::NiceMock<MockArena>>();
        mockArenaPtr = mock.get();
        return mock;
    }

    void setQuery() override 
    { // Do nothing
    }
};


class NetworkGameStateTest : public ::testing::Test 
{
protected:
    sf::RenderWindow mWindow;
    MockTextureHolder mTextureHolder; 
    MockFontHolder mFontHolder;
    State::Context mContext;
    StateStack mStack;

    NetworkGameStateTest()
        : mContext(mWindow, mTextureHolder, mFontHolder)
        , mStack(mContext)
    {
    }
};


TEST_F(NetworkGameStateTest, UpdatePassesTimeToArena) 
{
    TestableNetworkGameState state(mStack, mContext);
    state.onActivate();
    ASSERT_NE(state.mockArenaPtr, nullptr); 
     
    EXPECT_CALL(*state.mockArenaPtr, updateCurrent(sf::Time::Zero))
        .Times(1);
    
    state.update(sf::Time::Zero);
}

TEST_F(NetworkGameStateTest, UpdatePassesCommandsToArena) 
{
    TestableNetworkGameState state(mStack, mContext);
    state.onActivate(); 

    std::array<Category::Type, 3> categories = 
        {Category::Scene, Category::Arena, Category::Board};

    for (int i = 0; i < categories.size(); ++i) {
        Command dummyCommand;
        dummyCommand.category = Category::Arena; 
        dummyCommand.action = [](SceneNode&, sf::Time) {};  

        state.getCommandQueue().push(dummyCommand);
    }
    
    EXPECT_CALL(*state.mockArenaPtr, onCommand(testing::_, sf::Time::Zero))
        .Times(3);
    
    state.update(sf::Time::Zero);
}

TEST_F(NetworkGameStateTest, DrawPassesArena) 
{
    TestableNetworkGameState state(mStack, mContext);
    state.onActivate(); 
    
    EXPECT_CALL(*state.mockArenaPtr, draw(testing::_, testing::_))
        .Times(1);
    
    state.draw();
}
