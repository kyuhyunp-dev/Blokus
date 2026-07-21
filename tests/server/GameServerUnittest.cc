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
    const auto& getMatches() const { return mMatches; }
    sf::SocketSelector& getSelector() { return mSelector; }

    const sf::Packet& getOutboundPacket() const 
    {
        return mSpyPacket;
    }

protected:
    sf::Socket::Status sendToClient(sf::TcpSocket& client, sf::Packet& packet) override 
    {
        mSpyPacket = packet; 
        return sf::Socket::Status::Done; 
    }

private:
    sf::Packet mSpyPacket;
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
    sf::TcpSocket dummyHost; 
    sf::TcpSocket dummyGuest; 
    sf::TcpSocket dummyGuest2; 

    NetworkProtocol::PacketType outType;
    NetworkProtocol::MatchJoinedResponse successfulResponse;
    NetworkProtocol::MatchJoinFailedResponse failedResponse; 

    // CreateMatch
    sf::Packet createRequest;
    createRequest << NetworkProtocol::PacketType::CreateMatch;

    server.processPacket(createRequest, dummyHost);
    
    sf::Packet outboundCreate = server.getOutboundPacket(); 
    
    ASSERT_TRUE(outboundCreate >> outType >> successfulResponse) << "Failed to extract CreateMatch outbound packet.";
    EXPECT_EQ(outType, NetworkProtocol::PacketType::MatchJoined);
    EXPECT_FALSE(successfulResponse.matchCode.empty());

    std::string matchCode = successfulResponse.matchCode;

    // Create Existing Match 
    sf::Packet createExistingRequest;
    createExistingRequest << NetworkProtocol::PacketType::CreateMatch;
    server.processPacket(createExistingRequest, dummyHost);
     
    sf::Packet outboundExistingCreate = server.getOutboundPacket(); 

    ASSERT_TRUE(outboundExistingCreate >> outType >> failedResponse) << "Failed to extract CreateMatch outbound packet.";
    EXPECT_EQ(outType, NetworkProtocol::PacketType::MatchJoinFailed);
    EXPECT_EQ(failedResponse.reason, NetworkProtocol::JoinError::AlreadyInMatch);

    // Join Match using incorrect code
    sf::Packet wrongCodeRequest;
     
    std::string code = "0000";
    if (matchCode == code)
    {
        code = "0001";
    }
     
    NetworkProtocol::JoinMatchRequest incorrectJoinPayload { code };
    wrongCodeRequest << NetworkProtocol::PacketType::JoinMatch << incorrectJoinPayload;

    server.processPacket(wrongCodeRequest, dummyGuest);

    sf::Packet wrongCodeOutbound = server.getOutboundPacket();

    ASSERT_TRUE(wrongCodeOutbound >> outType >> failedResponse) << "Failed to extract JoinMatch outbound packet.";
    EXPECT_EQ(outType, NetworkProtocol::PacketType::MatchJoinFailed);
    EXPECT_EQ(failedResponse.reason, NetworkProtocol::JoinError::MatchNotFound);

    // JoinMatch
    sf::Packet joinRequest;
    NetworkProtocol::JoinMatchRequest joinPayload { matchCode };
    joinRequest << NetworkProtocol::PacketType::JoinMatch << joinPayload;

    server.processPacket(joinRequest, dummyGuest);

    sf::Packet outboundJoin = server.getOutboundPacket();

    ASSERT_TRUE(outboundJoin >> outType >> successfulResponse) << "Failed to extract JoinMatch outbound packet.";
    EXPECT_EQ(outType, NetworkProtocol::PacketType::MatchJoined);
    EXPECT_EQ(successfulResponse.matchCode, matchCode);

    // Create Match using Guest
    sf::Packet guestCreateRequest;
    guestCreateRequest << NetworkProtocol::PacketType::CreateMatch;

    server.processPacket(guestCreateRequest, dummyGuest);

    sf::Packet guestCreate = server.getOutboundPacket();

    ASSERT_TRUE(guestCreate >> outType >> failedResponse) << "Failed to extract JoinMatch outbound packet.";
    EXPECT_EQ(outType, NetworkProtocol::PacketType::MatchJoinFailed);
    EXPECT_EQ(failedResponse.reason, NetworkProtocol::JoinError::AlreadyInMatch);

    // Join Match using incorrect code
    sf::Packet fullMatchRequest;
    fullMatchRequest << NetworkProtocol::PacketType::JoinMatch << joinPayload;

    server.processPacket(fullMatchRequest, dummyGuest2);

    sf::Packet fullMatchOutbound = server.getOutboundPacket();

    ASSERT_TRUE(fullMatchOutbound >> outType >> failedResponse) << "Failed to extract JoinMatch outbound packet.";
    EXPECT_EQ(outType, NetworkProtocol::PacketType::MatchJoinFailed);
    EXPECT_EQ(failedResponse.reason, NetworkProtocol::JoinError::MatchFull);
}
