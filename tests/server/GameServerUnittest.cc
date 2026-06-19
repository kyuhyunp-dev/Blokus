#include <gtest/gtest.h>
#include <SFML/Network.hpp>
#include "server/GameServer.hpp"
#include "shared/Network/NetworkProtocol.hpp"

// ---------------------------------------------------------
// THE WRAPPER: Exposes protected members for testing
// ---------------------------------------------------------
class TestableGameServer : public GameServer 
{
public:
    // Expose protected methods to public
    using GameServer::handleIncomingConnections;
    using GameServer::handleIncomingPackets;
    using GameServer::processPacket;

    // Expose protected state via getters
    const auto& getClients() const { return mClients; }
    const sf::Packet& getOutboundPacket() const { return mOutboundPacket; }
    const auto& getMatches() const { return mMatches; }
    sf::SocketSelector& getSelector() { return mSelector; }
};

TEST(GameServerTest, ConnectCreateJoinDisconnect) 
{
    TestableGameServer server; 

    // Host connects
    sf::TcpSocket host;
    sf::IpAddress address = sf::IpAddress::LocalHost;
    sf::Socket::Status status = host.connect(address, ServerPort);
    ASSERT_EQ(status, sf::Socket::Status::Done) << "Host failed to connect locally.";

    ASSERT_TRUE(server.getSelector().wait(sf::milliseconds(50)));
    server.handleIncomingConnections();
    EXPECT_EQ(server.getClients().size(), 1);

    // Host sends CreateMatch
    sf::Packet createRequest;
    createRequest << NetworkProtocol::PacketType::CreateMatch;
    ASSERT_EQ(host.send(createRequest), sf::Socket::Status::Done);

    const auto& matches = server.getMatches();
    ASSERT_TRUE(server.getSelector().wait(sf::milliseconds(50)));
    server.handleIncomingPackets();
    EXPECT_EQ(matches.size(), 1) << "Match was not created.";

    // Check Server::Match
    std::string matchCode = server.getMatches().begin()->first;
    EXPECT_EQ(matchCode, server.getMatches().begin()->second.matchCode);
    EXPECT_EQ(server.getMatches().begin()->second.players.size(), 1);

    // Guest connects
    sf::TcpSocket guest;
    ASSERT_EQ(guest.connect(address, ServerPort), sf::Socket::Status::Done);

    ASSERT_TRUE(server.getSelector().wait(sf::milliseconds(50)));
    server.handleIncomingConnections();
    EXPECT_EQ(server.getClients().size(), 2);

    // Guest sends JoinMatch
    sf::Packet joinRequest;
    NetworkProtocol::JoinMatchRequest joinPayload { matchCode };
    joinRequest << NetworkProtocol::PacketType::JoinMatch << joinPayload;
    ASSERT_EQ(guest.send(joinRequest), sf::Socket::Status::Done);

    ASSERT_TRUE(server.getSelector().wait(sf::milliseconds(50)));
    server.handleIncomingPackets();
    EXPECT_EQ(matches.size(), 1) << "Match was modified.";

    EXPECT_EQ(matchCode, server.getMatches().begin()->second.matchCode);
    EXPECT_EQ(server.getMatches().begin()->second.players.size(), 2);

    // Host disconnects
    host.disconnect();

    ASSERT_TRUE(server.getSelector().wait(sf::milliseconds(50)));
    server.handleIncomingPackets();
    EXPECT_EQ(server.getClients().size(), 1);
    
    EXPECT_EQ(matches.size(), 1) << "Match was modified.";
    EXPECT_EQ(matchCode, server.getMatches().begin()->second.matchCode);
    EXPECT_EQ(server.getMatches().begin()->second.players.size(), 1);

    // Guest disconnects
    guest.disconnect();
    ASSERT_TRUE(server.getSelector().wait(sf::milliseconds(50)));
    server.handleIncomingPackets();
    EXPECT_EQ(server.getClients().size(), 0);
    EXPECT_EQ(matches.size(), 0) << "Match still exists.";
}

TEST(GameServerTest, CreateOutboundPackets) 
{ // Test for every packet
    TestableGameServer server;
    sf::TcpSocket dummySender; 

    // CreateMatch
    sf::Packet createRequest;
    createRequest << NetworkProtocol::PacketType::CreateMatch;

    server.processPacket(createRequest, dummySender);
    
    sf::Packet outboundCreate = server.getOutboundPacket(); 
    NetworkProtocol::PacketType outType;
    NetworkProtocol::MatchJoinedResponse createResponse;
    
    // Extract MatchJoinedResponse
    ASSERT_TRUE(outboundCreate >> outType >> createResponse) << "Failed to extract CreateMatch outbound packet.";
    EXPECT_EQ(outType, NetworkProtocol::PacketType::MatchJoined);
    EXPECT_FALSE(createResponse.matchCode.empty());

    std::string matchCode = createResponse.matchCode;

    // JoinMatch
    sf::Packet joinRequest;
    NetworkProtocol::JoinMatchRequest joinPayload { matchCode };
    joinRequest << NetworkProtocol::PacketType::JoinMatch << joinPayload;

    server.processPacket(joinRequest, dummySender);

    sf::Packet outboundJoin = server.getOutboundPacket();
    NetworkProtocol::PacketType joinOutType;
    NetworkProtocol::MatchJoinedResponse joinResponse;

    // Extract MatchJoinedResponse
    ASSERT_TRUE(outboundJoin >> joinOutType >> joinResponse) << "Failed to extract JoinMatch outbound packet.";
    EXPECT_EQ(joinOutType, NetworkProtocol::PacketType::MatchJoined);
    EXPECT_EQ(joinResponse.matchCode, matchCode);

    // Invalid JoinMatch
    sf::Packet failRequest;
    NetworkProtocol::JoinMatchRequest failPayload { "INVALID_CODE" };
    failRequest << NetworkProtocol::PacketType::JoinMatch << failPayload;

    server.processPacket(failRequest, dummySender);

    sf::Packet outboundFail = server.getOutboundPacket();
    NetworkProtocol::PacketType failOutType;
    NetworkProtocol::MatchJoinFailedResponse failResponse;

    // Extract MatchJoinFailedResponse
    ASSERT_TRUE(outboundFail >> failOutType >> failResponse) << "Failed to extract JoinMatch failure packet.";
    EXPECT_EQ(failOutType, NetworkProtocol::PacketType::MatchJoinFailed);
}