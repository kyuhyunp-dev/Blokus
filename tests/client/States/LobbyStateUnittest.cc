#include <gtest/gtest.h>
#include "States/LobbyState.hpp"

#include "Mock/Resource/MockTextureHolder.hpp"
#include "Mock/Resource/MockFontHolder.hpp"

#include "shared/TestBase.hpp"
#include "States/TestableStateStack.hpp"

#include <SFML/Graphics/RenderWindow.hpp>


class TestableLobbyState : public LobbyState 
{
public:
    using LobbyState::LobbyState;   

    using LobbyState::addPlayer;
    using LobbyState::removePlayer;

    using LobbyState::getTitleText;
    using LobbyState::getStatusText;
    using LobbyState::getUsernameTexts;
};

class LobbyStateTest : public PolyominoTestBase 
{
protected:
    sf::RenderWindow mWindow;
    MockTextureHolder mTextureHolder; 
    MockFontHolder mFontHolder;
    GameSessionData mGameSessionData;
    NetworkClient mNetworkClient;
    
    State::Context mContext;
    TestableStateStack mStack;

    LobbyStateTest()
        : mContext(mWindow, mTextureHolder, mFontHolder, mGameSessionData, mNetworkClient, sLibrary)
        , mStack(mContext)
    {
        mFontHolder.load(Fonts::ID::Sansation, "dummy/path.ttf");
    }
};

TEST_F(LobbyStateTest, PlayerLayout) 
{
    TestableLobbyState state(mStack, mContext);

    EXPECT_EQ(state.getTitleText().getString(), "LOBBY");
    EXPECT_EQ(state.getStatusText().getString(), "Waiting for players...");

    float yBegin = state.getStatusText().getPosition().y;

    // Capture a LIVE REFERENCE to the vector to avoid copies.
    const auto& slots = state.getUsernameTexts();
    ASSERT_EQ(slots.size(), 0);

    // Test Insertions
    for (int i = 0; i < 3; ++i)
    {
        state.addPlayer(i, "user" + std::to_string(i));
        
        // 'slots' automatically updates because it is a reference!
        const auto& slot = slots.back();

        EXPECT_EQ(slot.id, i);
        EXPECT_EQ(slot.text.getString(), "user" + std::to_string(i));
        EXPECT_EQ(slot.text.getPosition().x, 500.f); // Verify X anchoring
        EXPECT_EQ(slot.text.getPosition().y - yBegin, (i + 1) * 150.f);
    } 

    ASSERT_EQ(slots.size(), 3);

    // Remove player from front
    state.removePlayer(0);
    ASSERT_EQ(slots.size(), 2);

    for (int i = 0; i < slots.size(); ++i)
    {
        EXPECT_EQ(slots[i].id, i + 1);
        EXPECT_EQ(slots[i].text.getString(), "user" + std::to_string(i + 1));
        EXPECT_EQ(slots[i].text.getPosition().x, 500.f);
        EXPECT_EQ(slots[i].text.getPosition().y - yBegin, (i + 1) * 150.f);
    } 

    // Remove all players from back
    for (int i = 2; i > 0; --i)
    {
        state.removePlayer(i);
        
        for (int j = 0; j < slots.size(); ++j)
        {
            EXPECT_EQ(slots[j].id, j + 1);
            EXPECT_EQ(slots[j].text.getString(), "user" + std::to_string(j + 1));
            EXPECT_EQ(slots[j].text.getPosition().x, 500.f);
            EXPECT_EQ(slots[j].text.getPosition().y - yBegin, (j + 1) * 150.f);
        }
    }
    
    EXPECT_EQ(slots.size(), 0);
}