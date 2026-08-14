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
    TestableGameServer(): GameServer(0) {}

    // Expose protected methods to public
    using GameServer::handleIncomingConnections;
    using GameServer::handleIncomingPackets;
    using GameServer::flushOutgoingQueues;
    using GameServer::removeClient;

    // Expose protected state via getters
    auto& getClients() { return mClients; }
    const auto& getClients() const { return mClients; }
    const auto& getMatches() const { return mMatches; }
    sf::SocketSelector& getSelector() { return mSelector; }
    sf::TcpListener& getListener() { return mListener; }
    const auto& getClientsToDisconnect() const { return mClientsToDisconnect; }

    unsigned short getBoundPort() const
    {
        return mListener.getLocalPort();
    }

    void processDisconnects()
    {
        for (uint32_t idToRemove : mClientsToDisconnect)
        {
            removeClient(idToRemove);
        }
        mClientsToDisconnect.clear();
    }
};



TEST(GameServerTest, ConnectCreateJoinDisconnect) 
{
    TestableGameServer server; 

    // Host connects
    sf::TcpSocket host;
    sf::Socket::Status status = host.connect(sf::IpAddress::LocalHost, server.getBoundPort());
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

    std::string matchCode = server.getMatches().begin()->first;
    EXPECT_EQ(matchCode, server.getMatches().begin()->second.matchCode);
    EXPECT_EQ(server.getMatches().begin()->second.playerIDs.size(), 1);

    // Guest connects
    sf::TcpSocket guest;
    ASSERT_EQ(guest.connect(sf::IpAddress::LocalHost, server.getBoundPort()), sf::Socket::Status::Done);

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
    EXPECT_EQ(matches.size(), 1) << "Match count altered unexpectedly.";

    EXPECT_EQ(matchCode, server.getMatches().begin()->second.matchCode);
    EXPECT_EQ(server.getMatches().begin()->second.playerIDs.size(), 2);

    // Host disconnects
    host.disconnect();
    ASSERT_TRUE(server.getSelector().wait(sf::milliseconds(50)));
    server.handleIncomingPackets();
    server.processDisconnects();

    EXPECT_EQ(server.getClients().size(), 1);
    EXPECT_EQ(matches.begin()->second.playerIDs.size(), 1); 

    // Guest disconnects
    guest.disconnect();
    ASSERT_TRUE(server.getSelector().wait(sf::milliseconds(50)));
    server.handleIncomingPackets();
    server.processDisconnects();
    
    EXPECT_EQ(server.getClients().size(), 0);
    EXPECT_EQ(matches.size(), 0) << "Match was not cleaned up after everyone left.";
}

TEST(GameServerTest, StateAndOutgoingQueue) 
{
    TestableGameServer server;

    // Verify Creator state
    sf::TcpSocket creatorSocket;
    ASSERT_EQ(creatorSocket.connect(sf::IpAddress::LocalHost, server.getBoundPort()), sf::Socket::Status::Done);
    
    ASSERT_TRUE(server.getSelector().wait(sf::milliseconds(50)));
    server.handleIncomingConnections();
    
    uint32_t creatorID = server.getClients().begin()->first;
    auto& creatorObj = server.getClients().begin()->second;

    EXPECT_EQ(creatorObj.state, Server::ClientState::TitleState);

    sf::Packet createReq;
    createReq << NetworkProtocol::PacketType::CreateMatch;
    ASSERT_EQ(creatorSocket.send(createReq), sf::Socket::Status::Done);

    ASSERT_TRUE(server.getSelector().wait(sf::milliseconds(50)));
    server.handleIncomingPackets();

    EXPECT_EQ(creatorObj.state, Server::ClientState::UsernameState);
    EXPECT_EQ(creatorObj.outgoingQueue.size(), 1);

    server.flushOutgoingQueues();
    EXPECT_EQ(creatorObj.outgoingQueue.size(), 0);

    sf::Packet response;
    ASSERT_EQ(creatorSocket.receive(response), sf::Socket::Status::Done);
    
    NetworkProtocol::PacketType type;
    NetworkProtocol::MatchJoinedResponse matchResp;
    ASSERT_TRUE(response >> type >> matchResp);
    EXPECT_EQ(type, NetworkProtocol::PacketType::MatchJoined);
    EXPECT_FALSE(matchResp.matchCode.empty());
    
    // Verify Joiner state
    sf::TcpSocket joinerSocket;
    ASSERT_EQ(joinerSocket.connect(sf::IpAddress::LocalHost, server.getBoundPort()), sf::Socket::Status::Done);
    
    ASSERT_TRUE(server.getSelector().wait(sf::milliseconds(50)));
    server.handleIncomingConnections();
    
    uint32_t joinerID = 0;
    for (const auto& [id, client] : server.getClients()) 
    {
        if (id != creatorID) 
        {
            joinerID = id;
            break;
        }
    }

    const auto& joinerObj = server.getClients().at(joinerID);

    sf::Packet joinReq;
    NetworkProtocol::JoinMatchRequest joinPayload { matchResp.matchCode };
    joinReq << NetworkProtocol::PacketType::JoinMatch << joinPayload;
    ASSERT_EQ(joinerSocket.send(joinReq), sf::Socket::Status::Done);

    ASSERT_TRUE(server.getSelector().wait(sf::milliseconds(50)));
    server.handleIncomingPackets();

    EXPECT_EQ(joinerObj.state, Server::ClientState::UsernameState);
    EXPECT_EQ(joinerObj.outgoingQueue.size(), 1);

    server.flushOutgoingQueues();
    EXPECT_EQ(joinerObj.outgoingQueue.size(), 0);

    response.clear();
    ASSERT_EQ(joinerSocket.receive(response), sf::Socket::Status::Done);
    
    ASSERT_TRUE(response >> type >> matchResp);
    EXPECT_EQ(type, NetworkProtocol::PacketType::MatchJoined);
    EXPECT_FALSE(matchResp.matchCode.empty());

    creatorSocket.disconnect();
    joinerSocket.disconnect();
    ASSERT_TRUE(server.getSelector().wait(sf::milliseconds(50)));
    server.handleIncomingPackets();
    server.processDisconnects();
}

TEST(GameServerTest, JoinErrorPackets) 
{
    TestableGameServer server;

    auto receivePacket = [](sf::TcpSocket& socket) {
        sf::Packet packet;
        EXPECT_EQ(socket.receive(packet), sf::Socket::Status::Done);
        return packet;
    };

    // Host creates a match.
    sf::TcpSocket host;
    ASSERT_EQ(host.connect(sf::IpAddress::LocalHost, server.getBoundPort()), sf::Socket::Status::Done);
    ASSERT_TRUE(server.getSelector().wait(sf::milliseconds(50)));
    server.handleIncomingConnections();
    ASSERT_EQ(server.getClients().size(), 1);
    const uint32_t hostID = server.getClients().begin()->first;

    sf::Packet createRequest;
    createRequest << NetworkProtocol::PacketType::CreateMatch;
    ASSERT_EQ(host.send(createRequest), sf::Socket::Status::Done);
    ASSERT_TRUE(server.getSelector().wait(sf::milliseconds(50)));
    server.handleIncomingPackets();
    server.flushOutgoingQueues();

    NetworkProtocol::PacketType outType;
    NetworkProtocol::MatchJoinedResponse successfulResponse;
    sf::Packet outboundCreate = receivePacket(host);
    ASSERT_TRUE(outboundCreate >> outType >> successfulResponse);
    EXPECT_EQ(outType, NetworkProtocol::PacketType::MatchJoined);
    const std::string matchCode = successfulResponse.matchCode;

    // The create path transitions the client to UsernameState. Restore title state
    // to exercise the AlreadyInMatch branch in handleTitleStatePackets.
    server.getClients().at(hostID).state = Server::ClientState::TitleState;
    ASSERT_EQ(host.send(createRequest), sf::Socket::Status::Done);
    ASSERT_TRUE(server.getSelector().wait(sf::milliseconds(50)));
    server.handleIncomingPackets();
    server.flushOutgoingQueues();

    NetworkProtocol::MatchJoinFailedResponse failedResponse;
    sf::Packet alreadyInMatch = receivePacket(host);
    ASSERT_TRUE(alreadyInMatch >> outType >> failedResponse);
    EXPECT_EQ(outType, NetworkProtocol::PacketType::MatchJoinFailed);
    EXPECT_EQ(failedResponse.reason, NetworkProtocol::JoinError::AlreadyInMatch);

    // A guest using an unknown code receives MatchNotFound.
    sf::TcpSocket guest;
    ASSERT_EQ(guest.connect(sf::IpAddress::LocalHost, server.getBoundPort()), sf::Socket::Status::Done);
    ASSERT_TRUE(server.getSelector().wait(sf::milliseconds(50)));
    server.handleIncomingConnections();

    std::string wrongCode = matchCode == "0000" ? "0001" : "0000";
    sf::Packet wrongCodeRequest;
    wrongCodeRequest << NetworkProtocol::PacketType::JoinMatch
                     << NetworkProtocol::JoinMatchRequest{wrongCode};
    ASSERT_EQ(guest.send(wrongCodeRequest), sf::Socket::Status::Done);
    ASSERT_TRUE(server.getSelector().wait(sf::milliseconds(50)));
    server.handleIncomingPackets();
    server.flushOutgoingQueues();

    sf::Packet wrongCodeResponse = receivePacket(guest);
    ASSERT_TRUE(wrongCodeResponse >> outType >> failedResponse);
    EXPECT_EQ(outType, NetworkProtocol::PacketType::MatchJoinFailed);
    EXPECT_EQ(failedResponse.reason, NetworkProtocol::JoinError::MatchNotFound);

    // The guest joins successfully, filling the two-player match.
    sf::Packet joinRequest;
    joinRequest << NetworkProtocol::PacketType::JoinMatch
                << NetworkProtocol::JoinMatchRequest{matchCode};
    ASSERT_EQ(guest.send(joinRequest), sf::Socket::Status::Done);
    ASSERT_TRUE(server.getSelector().wait(sf::milliseconds(50)));
    server.handleIncomingPackets();
    server.flushOutgoingQueues();
    sf::Packet successfulJoin = receivePacket(guest);
    ASSERT_TRUE(successfulJoin >> outType >> successfulResponse);
    EXPECT_EQ(outType, NetworkProtocol::PacketType::MatchJoined);
    EXPECT_EQ(successfulResponse.matchCode, matchCode);

    // A third client receives MatchFull.
    sf::TcpSocket thirdClient;
    ASSERT_EQ(thirdClient.connect(sf::IpAddress::LocalHost, server.getBoundPort()), sf::Socket::Status::Done);
    ASSERT_TRUE(server.getSelector().wait(sf::milliseconds(50)));
    server.handleIncomingConnections();
    ASSERT_EQ(thirdClient.send(joinRequest), sf::Socket::Status::Done);
    ASSERT_TRUE(server.getSelector().wait(sf::milliseconds(50)));
    server.handleIncomingPackets();
    server.flushOutgoingQueues();

    sf::Packet fullMatchResponse = receivePacket(thirdClient);
    ASSERT_TRUE(fullMatchResponse >> outType >> failedResponse);
    EXPECT_EQ(outType, NetworkProtocol::PacketType::MatchJoinFailed);
    EXPECT_EQ(failedResponse.reason, NetworkProtocol::JoinError::MatchFull);
}

TEST(GameServerTest, Security_AntiSpamRateLimiting) 
{
    TestableGameServer server;
    sf::TcpSocket spammer;
    ASSERT_EQ(spammer.connect(sf::IpAddress::LocalHost, server.getBoundPort()), sf::Socket::Status::Done);
    
    ASSERT_TRUE(server.getSelector().wait(sf::milliseconds(50)));
    server.handleIncomingConnections();
    
    // Flood the server with 25 packets (Limit is 20)
    sf::Packet dummyPacket;
    dummyPacket << NetworkProtocol::PacketType::CreateMatch;
    for (int i = 0; i < 25; ++i)
    {
        ASSERT_EQ(spammer.send(dummyPacket), sf::Socket::Status::Done);
    }

    // TCP may split these writes across several readiness notifications. Keep
    // polling until the server has observed the rate-limit violation, but do
    // not make the test depend on a particular delivery batch or latency.
    constexpr int maxPolls = 100;
    for (int poll = 0;
         poll < maxPolls && server.getClientsToDisconnect().empty();
         ++poll)
    {
        if (server.getSelector().wait(sf::milliseconds(10)))
        {
            server.handleIncomingPackets();
        }
    }

    ASSERT_EQ(server.getClientsToDisconnect().size(), 1)
        << "Server did not observe all packets and enforce the rate limit.";
    server.processDisconnects();
    EXPECT_EQ(server.getClients().size(), 0) << "Spammer bypassed rate limit and was not dropped.";
}

TEST(GameServerTest, Security_MalformedPacket) 
{
    TestableGameServer server;
    sf::TcpSocket hacker;
    ASSERT_EQ(hacker.connect(sf::IpAddress::LocalHost, server.getBoundPort()), sf::Socket::Status::Done);
    
    ASSERT_TRUE(server.getSelector().wait(sf::milliseconds(50)));
    server.handleIncomingConnections();

    // Send invalid data (Injecting a raw string instead of a valid enum)
    sf::Packet badPacket;
    badPacket << "corrupted_garbage_data"; 
    ASSERT_EQ(hacker.send(badPacket), sf::Socket::Status::Done);

    ASSERT_TRUE(server.getSelector().wait(sf::milliseconds(50)));
    server.handleIncomingPackets();

    // packet >> type extraction should fail and instantly kill the client
    EXPECT_EQ(server.getClientsToDisconnect().size(), 1);
    
    server.processDisconnects();
    EXPECT_EQ(server.getClients().size(), 0) << "Malformed packet did not trigger a disconnect.";
} 

TEST(GameServerTest, Security_PhantomConnectionHandling) 
{
    TestableGameServer server;
    
    // Simulate a scanner or phantom connection that connects and instantly dies
    {
        sf::TcpSocket phantom;
        ASSERT_EQ(phantom.connect(sf::IpAddress::LocalHost, server.getBoundPort()), sf::Socket::Status::Done);
        // phantom instantly goes out of scope and destroys its socket
    } 
    
    // Accepting the connection and observing its FIN are separate events. Poll
    // both with a finite bound so neither event is tied to one 50 ms window.
    constexpr int maxPolls = 100;
    for (int poll = 0;
         poll < maxPolls && server.getClientsToDisconnect().empty();
         ++poll)
    {
        if (!server.getSelector().wait(sf::milliseconds(10)))
        {
            continue;
        }

        if (server.getSelector().isReady(server.getListener()))
        {
            server.handleIncomingConnections();
        }
        server.handleIncomingPackets();
    }

    ASSERT_EQ(server.getClientsToDisconnect().size(), 1)
        << "Server did not observe the phantom peer disconnect.";
    server.processDisconnects();
    EXPECT_EQ(server.getClients().size(), 0) << "Server tracked a ghost connection.";
}
