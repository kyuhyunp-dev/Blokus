#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "Mock/States/MockState.hpp"
#include "Mock/Resource/MockTextureHolder.hpp"
#include "Mock/Resource/MockFontHolder.hpp"

#include "States/StateStack.hpp"
#include "States/State.hpp"
#include "States/StateIdentifiers.hpp"
#include "Resource/ResourceHolder.hpp"
#include "Resource/ResourceIdentifiers.hpp"

#include "SFML/Graphics/RenderWindow.hpp"


class StateStackTest : public ::testing::Test {
protected:
    StateStackTest()
        // We can pass dummy context objects for testing the stack
        : mContext(mWindow, mTextures, mFonts)
        , mStates(mContext)
    {
    }

    void SetUp() override {
        // NiceMock silences warnings
        mStates.registerState<testing::NiceMock<MockState>>(States::ID::None);
        mStates.registerState<testing::NiceMock<MockState>>(States::ID::NetworkGame);
    }

    void pushDirectly(std::unique_ptr<MockState> state) {
        mStates.mStack.push_back(std::move(state));
    }

    std::vector<State::Ptr>::reverse_iterator rbegin() {
        return mStates.mStack.rbegin();
    }

    size_t size() {
        return mStates.mStack.size();
    }

    sf::RenderWindow mWindow;
    MockTextureHolder mTextures; 
    MockFontHolder mFonts;
    State::Context mContext;
    StateStack mStates;
};

TEST_F(StateStackTest, DeferredPush) {
    EXPECT_TRUE(mStates.isEmpty());

    mStates.pushState(States::ID::None);
    
    // The stack should still be empty because applyPendingChanges hasn't run
    EXPECT_TRUE(mStates.isEmpty()); 

    // Trigger applyPendingChanges via a dummy update
    mStates.update(sf::Time::Zero);

    EXPECT_FALSE(mStates.isEmpty());
}

TEST_F(StateStackTest, DeferredPop) {
    EXPECT_TRUE(mStates.isEmpty());

    for (int i = 0; i < 3; ++i) {
        mStates.pushState(States::ID::None);
    }
    mStates.update(sf::Time::Zero);
    EXPECT_EQ(size(), 3);

    mStates.popState();
    EXPECT_EQ(size(), 3);

    mStates.update(sf::Time::Zero);
    EXPECT_EQ(size(), 2);
}

TEST_F(StateStackTest, DeferredClear) {
    EXPECT_TRUE(mStates.isEmpty());

    for (int i = 0; i < 3; ++i) {
        mStates.pushState(States::ID::None);
    }
    mStates.update(sf::Time::Zero);
    EXPECT_EQ(size(), 3);

    mStates.clearStates();
    EXPECT_EQ(size(), 3);

    mStates.update(sf::Time::Zero);
    EXPECT_TRUE(mStates.isEmpty());
}

TEST_F(StateStackTest, UpdateStops) 
{
    auto bottomState = std::make_unique<testing::StrictMock<MockState>>(mStates, mContext);
    auto topState = std::make_unique<testing::StrictMock<MockState>>(mStates, mContext);

    // If top state's update returns false, bottom state doesn't execute
    EXPECT_CALL(*bottomState, update(testing::_)).Times(0);
    EXPECT_CALL(*topState, update(testing::_)).WillOnce(testing::Return(false));

    pushDirectly(std::move(bottomState));
    pushDirectly(std::move(topState));

    mStates.update(sf::Time::Zero);
}

TEST_F(StateStackTest, HandleEventStops) 
{
    auto bottomState = std::make_unique<testing::StrictMock<MockState>>(mStates, mContext);
    auto topState = std::make_unique<testing::StrictMock<MockState>>(mStates, mContext);

    // If top state's handleEvent returns false, bottom state doesn't execute
    EXPECT_CALL(*bottomState, handleEvent(testing::_)).Times(0);
    EXPECT_CALL(*topState, handleEvent(testing::_)).WillOnce(testing::Return(false));

    pushDirectly(std::move(bottomState));
    pushDirectly(std::move(topState));

    sf::Event::Closed event; // Random event
    mStates.handleEvent(event);
}

TEST_F(StateStackTest, RequestsForwarded) {
    // Can't push another State until a State exists
    EXPECT_TRUE(mStates.isEmpty());
    mStates.pushState(States::ID::None);
    mStates.update(sf::Time::Zero);
    EXPECT_FALSE(mStates.isEmpty());

    auto getActiveState = [this]() -> MockState* {
        return static_cast<MockState*>((*rbegin()).get());
    };

    // Multiple Push
    for (int i = 0; i < 4; ++i) {
        getActiveState()->requestStackPush(States::ID::NetworkGame);
    }
    mStates.update(sf::Time::Zero); 
    EXPECT_EQ(size(), 5);

    // Pop
    getActiveState()->requestStackPop();
    mStates.update(sf::Time::Zero); 
    EXPECT_EQ(size(), 4);
    
    // Clear
    getActiveState()->requestStateClear();
    mStates.update(sf::Time::Zero); 
    EXPECT_TRUE(mStates.isEmpty());
}


