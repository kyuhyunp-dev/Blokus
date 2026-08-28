#include <gtest/gtest.h>
#include "Network/NetworkClient.hpp"
#include "Mock/Network/MockTcpSocket.hpp"

#include <SFML/Network/TcpListener.hpp>
#include <SFML/Network/Packet.hpp>
#include <thread>
#include <chrono>
#include <atomic>


class NetworkClientTest : public ::testing::Test 
{
protected:
    // We instantiate the template using our Mock socket instead of the real one
    NetworkClientImpl<MockTcpSocket> mClient;
    MockTcpSocket& mMockSocket = mClient.getSocket();
};


TEST_F(NetworkClientTest, InitiallyDisconnected) 
{
    EXPECT_FALSE(mClient.isConnected());

    sf::Packet outPacket;
    EXPECT_NO_THROW(mClient.sendPacket(outPacket));

    sf::Packet receivedPacket;
    EXPECT_FALSE(mClient.pollPacket(receivedPacket));
}

TEST_F(NetworkClientTest, ConnectFailsAtTcpLevel) 
{
    EXPECT_CALL(mMockSocket, setBlocking(true)).Times(1);
    
    // Simulate a hard TCP rejection (e.g., dead port)
    EXPECT_CALL(mMockSocket, connect(_, _, _))
        .WillOnce(Return(sf::Socket::Status::Error));

    bool success = mClient.connect("127.0.0.1", 54321, "localhost", false);
    
    EXPECT_FALSE(success);
    EXPECT_FALSE(mClient.isConnected());
}

TEST_F(NetworkClientTest, SendPollSuccess) 
{
    // Expect the setup sequence
    EXPECT_CALL(mMockSocket, setBlocking(true)).Times(1);
    EXPECT_CALL(mMockSocket, connect(_, _, _)).WillOnce(Return(sf::Socket::Status::Done));
    EXPECT_CALL(mMockSocket, setBlocking(false)).Times(1);
    
    // Simulate an instant, perfect TLS handshake. 
    // This bypasses the 5-second selector.wait() loop completely!
    EXPECT_CALL(mMockSocket, setupTlsClient(_, _))
        .WillOnce(Return(sf::TcpSocket::TlsStatus::HandshakeComplete));

    // Execute
    EXPECT_TRUE(mClient.connect("127.0.0.1", 8080, "localhost", false));
    EXPECT_TRUE(mClient.isConnected());
    
    // Send and poll packet
    sf::Packet outPacket;
    outPacket << "Client to Server";
    EXPECT_NO_THROW(mClient.sendPacket(outPacket));

    sf::Packet simulatedServerPacket;
    simulatedServerPacket << "Server to Client";
    
    EXPECT_CALL(mMockSocket, receive(_))
        .WillOnce(DoAll(
            SetArgReferee<0>(simulatedServerPacket),
            Return(sf::Socket::Status::Done)
        ));

    sf::Packet receivedPacket;
    EXPECT_TRUE(mClient.pollPacket(receivedPacket));

    std::string receivedMessage;
    receivedPacket >> receivedMessage;
    EXPECT_EQ(receivedMessage, "Server to Client");

    EXPECT_CALL(mMockSocket, disconnect()).Times(1);
    mClient.disconnect();
    EXPECT_FALSE(mClient.isConnected());
}

TEST_F(NetworkClientTest, SendAndPollFailure) 
{
    // Expect the setup sequence
    EXPECT_CALL(mMockSocket, setBlocking(true)).Times(1);
    EXPECT_CALL(mMockSocket, connect(_, _, _)).WillOnce(Return(sf::Socket::Status::Done));
    EXPECT_CALL(mMockSocket, setBlocking(false)).Times(1);
    
    // Simulate an instant, perfect TLS handshake. 
    // This bypasses the 5-second selector.wait() loop completely!
    EXPECT_CALL(mMockSocket, setupTlsClient(_, _))
        .WillOnce(Return(sf::TcpSocket::TlsStatus::HandshakeComplete));

    // Execute
    EXPECT_TRUE(mClient.connect("127.0.0.1", 8080, "localhost", false));
    EXPECT_TRUE(mClient.isConnected());

    sf::Packet outPacket;
    outPacket << "Client to Server";
    EXPECT_NO_THROW(mClient.sendPacket(outPacket));

    sf::Packet simulatedServerPacket;
    simulatedServerPacket << "Server to Client";
    
    EXPECT_CALL(mMockSocket, receive(_))
        .WillOnce(Return(sf::Socket::Status::Disconnected));
    EXPECT_CALL(mMockSocket, disconnect()).Times(1);

    sf::Packet receivedPacket;
    EXPECT_FALSE(mClient.pollPacket(receivedPacket));

    EXPECT_FALSE(mClient.isConnected());
}

TEST_F(NetworkClientTest, HandlesTlsHandshakeFailure) 
{
    EXPECT_CALL(mMockSocket, setBlocking(true)).Times(1);
    EXPECT_CALL(mMockSocket, connect(_, _, _)).WillOnce(Return(sf::Socket::Status::Done));
    EXPECT_CALL(mMockSocket, setBlocking(false)).Times(1);    

    // Simulate a TLS failure (e.g., bad certificate)
    EXPECT_CALL(mMockSocket, setupTlsClient(_, _))
        .WillOnce(Return(sf::TcpSocket::TlsStatus::Error));
    
    // The client should cleanly disconnect upon TLS failure
    EXPECT_CALL(mMockSocket, disconnect()).Times(1); // Once at start, once on fail

    EXPECT_FALSE(mClient.connect("127.0.0.1", 8080, "localhost", false));
    EXPECT_FALSE(mClient.isConnected());
}

TEST_F(NetworkClientTest, HandlesServerDisconnectDuringPoll) 
{
    // Setup a connected state directly for testing poll behavior
    EXPECT_CALL(mMockSocket, connect(_, _, _)).WillOnce(Return(sf::Socket::Status::Done));
    EXPECT_CALL(mMockSocket, setupTlsClient(_, _)).WillOnce(Return(sf::TcpSocket::TlsStatus::HandshakeComplete));
    ASSERT_TRUE(mClient.connect("127.0.0.1", 8080, "localhost", false));

    sf::Packet emptyPacket;
    
    // Simulate the server closing the connection abruptly during a read
    EXPECT_CALL(mMockSocket, receive(_))
        .WillOnce(Return(sf::Socket::Status::Disconnected));

    EXPECT_FALSE(mClient.pollPacket(emptyPacket));
    EXPECT_FALSE(mClient.isConnected());
}