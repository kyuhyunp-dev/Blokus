#include <gtest/gtest.h>
#include "States/TitleState.hpp"
#include "shared/Network/NetworkProtocol.hpp"

#include "Mock/Resource/MockTextureHolder.hpp"
#include "Mock/Resource/MockFontHolder.hpp"
#include "Mock/Network/MockNetworkClient.hpp"

#include "shared/TestBase.hpp"
#include "States/TestableStateStack.hpp"

#include <SFML/Graphics/RenderWindow.hpp>


class TestableTitleState : public TitleState 
{
public:
    TestableTitleState(StateStack& stack, Context context)
        : TitleState(stack, context) 
        {}

    using TitleState::requestCreateMatch;
    using TitleState::requestJoinMatch;
    using TitleState::processPacket;

    GUI::Label* getStatusLabel()
    {
        return mStatusLabel;
    }
};

class TitleStateTest : public PolyominoTestBase 
{
    protected:
        sf::RenderWindow mWindow;
        MockTextureHolder mTextureHolder; 
        MockFontHolder mFontHolder;
        GameSessionData mGameSessionData;
        testing::NiceMock<MockNetworkClient> mNetworkClient;
        
        State::Context mContext;
        TestableStateStack mStack;

        TitleStateTest()
            : mContext(mWindow, mTextureHolder, mFontHolder, mGameSessionData, mNetworkClient, sLibrary)
            , mStack(mContext)
        {
            mWindow.create(sf::VideoMode({800, 600}), "Test Window");
            mTextureHolder.load(Textures::ID::TitleScreen, "dummy/path.png");
            mFontHolder.load(Fonts::ID::Sansation, "dummy/path.ttf");

            ON_CALL(mNetworkClient, isConnected()).WillByDefault(testing::Return(true));
        }
};

TEST_F(TitleStateTest, CreateMatchPacket) 
{ // Request Create generates correct packet
    TestableTitleState state(mStack, mContext);

    sf::Packet sentPacket;
    EXPECT_CALL(mNetworkClient, sendPacket(testing::_))
        .WillOnce(testing::SaveArg<0>(&sentPacket));

    state.requestCreateMatch();

    NetworkProtocol::PacketType type;
    sentPacket >> type;
    EXPECT_EQ(type, NetworkProtocol::PacketType::CreateMatch);
}

TEST_F(TitleStateTest, JoinMatchPacket) 
{ // Request Join generates correct packet
    TestableTitleState state(mStack, mContext);

    sf::Packet sentPacket;
    EXPECT_CALL(mNetworkClient, sendPacket(testing::_))
        .WillOnce(testing::SaveArg<0>(&sentPacket));

    state.requestJoinMatch("ABCD");

    NetworkProtocol::PacketType type;
    NetworkProtocol::JoinMatchRequest request;
    sentPacket >> type >> request;

    EXPECT_EQ(type, NetworkProtocol::PacketType::JoinMatch);
    EXPECT_EQ(request.matchCode, "ABCD");
}

TEST_F(TitleStateTest, ProcessMatchJoinedPacket) 
{
    TestableTitleState state(mStack, mContext);

    sf::Packet packet;
    NetworkProtocol::MatchJoinedResponse response{"XYZ9"};
    packet << NetworkProtocol::PacketType::MatchJoined << response;

    state.processPacket(packet);

    // Saves match code
    EXPECT_EQ(mGameSessionData.match.matchCode, "XYZ9");

    // Transitions state
    auto pendingChanges = mStack.getPendingChanges();
    ASSERT_EQ(pendingChanges.size(), 2);
    EXPECT_EQ(pendingChanges[0].action, StateStack::Action::Pop);   
    EXPECT_EQ(pendingChanges[1].action, StateStack::Action::Push);
    EXPECT_EQ(pendingChanges[1].stateID, States::ID::Username);
}

TEST_F(TitleStateTest, ProcessMatchJoinFailed) 
{
    TestableTitleState state(mStack, mContext);

    sf::Packet packet;
    NetworkProtocol::MatchJoinFailedResponse alreadyInMathcResponse { NetworkProtocol::JoinError::AlreadyInMatch };
    packet << NetworkProtocol::PacketType::MatchJoinFailed << alreadyInMathcResponse;

    state.processPacket(packet);

    EXPECT_EQ(mStack.getPendingChanges().size(), 0);

    packet.clear();
    NetworkProtocol::MatchJoinFailedResponse matchFullResponse { NetworkProtocol::JoinError::MatchFull };
    packet << NetworkProtocol::PacketType::MatchJoinFailed << matchFullResponse;

    state.processPacket(packet);

    EXPECT_EQ(mStack.getPendingChanges().size(), 0);

    packet.clear();
    NetworkProtocol::MatchJoinFailedResponse invalidCodeResponse{ NetworkProtocol::JoinError::MatchNotFound};
    packet << NetworkProtocol::PacketType::MatchJoinFailed << invalidCodeResponse;

    state.processPacket(packet);

    EXPECT_EQ(mStack.getPendingChanges().size(), 0);
}