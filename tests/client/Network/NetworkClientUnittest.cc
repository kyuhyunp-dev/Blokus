#include <gtest/gtest.h>
#include "Network/NetworkClient.hpp"
#include <SFML/Network/TcpListener.hpp>
#include <SFML/Network/TcpSocket.hpp>
#include <SFML/Network/Packet.hpp>
#include <thread>
#include <chrono>
#include <atomic>
        

class NetworkClientTest : public ::testing::Test {
protected:
    NetworkClient mClient;

    // Helper function to create a temporary dummy server for testing connections.
    // It binds to port 0 (letting the OS pick a free port) and returns that port.
    unsigned short startDummyServer(sf::TcpListener& listener, sf::TcpSocket& serverSideSocket, std::atomic<bool>& clientAccepted) 
    {
        // Listen on any available port
        EXPECT_EQ(listener.listen(0), sf::Socket::Status::Done);
        unsigned short port = listener.getLocalPort();

        // Spin up a background thread to accept the incoming connection
        std::thread([&listener, &serverSideSocket, &clientAccepted]() {
            if (listener.accept(serverSideSocket) == sf::Socket::Status::Done) {
                clientAccepted = true;
            }
        }).detach();

        return port;
    }
};

TEST_F(NetworkClientTest, InitiallyDisconnected) 
{
    EXPECT_FALSE(mClient.isConnected());
}

TEST_F(NetworkClientTest, ConnectFailsOnInvalidPort) 
{
    // Port 54321 is arbitrary and extremely likely to be dead
    bool success = mClient.connect("127.0.0.1", 54321);
    
    EXPECT_FALSE(success);
    EXPECT_FALSE(mClient.isConnected());
}

TEST_F(NetworkClientTest, ConnectSuccessAndDisconnect) 
{
    sf::TcpListener listener;
    sf::TcpSocket serverSideSocket;
    std::atomic<bool> clientAccepted{false};
    
    unsigned short port = startDummyServer(listener, serverSideSocket, clientAccepted);
    bool success = mClient.connect("127.0.0.1", port);
    
    // Tiny window for the background thread to finish accepting
    std::this_thread::sleep_for(std::chrono::milliseconds(50));

    EXPECT_TRUE(success);
    EXPECT_TRUE(mClient.isConnected());
    EXPECT_TRUE(clientAccepted);

    // Explicit disconnect
    mClient.disconnect();
    EXPECT_FALSE(mClient.isConnected());
}

TEST_F(NetworkClientTest, SendAndPollPacket) 
{
    sf::TcpListener listener;
    sf::TcpSocket serverSideSocket;
    std::atomic<bool> clientAccepted{false};
    
    unsigned short port = startDummyServer(listener, serverSideSocket, clientAccepted);
    ASSERT_TRUE(mClient.connect("127.0.0.1", port));
    
    std::this_thread::sleep_for(std::chrono::milliseconds(50));

    // Sending (Client -> Server) 
    sf::Packet outPacket;
    outPacket << "Ping";
    mClient.sendPacket(outPacket);

    sf::Packet receivedByServer;

    // Server socket is blocking by default, so it will wait for the packet
    ASSERT_EQ(serverSideSocket.receive(receivedByServer), sf::Socket::Status::Done);
    
    std::string serverMsg;
    receivedByServer >> serverMsg;
    EXPECT_EQ(serverMsg, "Ping");

    // Polling (Server -> Client) 
    sf::Packet serverReply;
    serverReply << "Pong";
    sf::Socket::Status status = serverSideSocket.send(serverReply);
    ASSERT_TRUE(status == sf::Socket::Status::Done);
    
    // Because pollPacket is non-blocking, we need to poll a few times in case 
    sf::Packet receivedByClient;
    bool gotPacket = false;
    int numTries = 0;
    while (!gotPacket || numTries < 3) 
    {
        if (mClient.pollPacket(receivedByClient)) 
        {
            gotPacket = true;
            break;  
        }
        
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }

    ASSERT_TRUE(gotPacket);
    std::string clientMsg;
    receivedByClient >> clientMsg;
    EXPECT_EQ(clientMsg, "Pong");
}

TEST_F(NetworkClientTest, HandlesServerDisconnect) 
{
    sf::TcpListener listener;
    sf::TcpSocket serverSideSocket;
    std::atomic<bool> clientAccepted{false};
    
    unsigned short port = startDummyServer(listener, serverSideSocket, clientAccepted);
    ASSERT_TRUE(mClient.connect("127.0.0.1", port));
    
    std::this_thread::sleep_for(std::chrono::milliseconds(50));

    // Force the server to abruptly kill the connection
    serverSideSocket.disconnect();
    
    // Give the OS a moment to register the broken pipe
    std::this_thread::sleep_for(std::chrono::milliseconds(50));

    sf::Packet emptyPacket;
    
    // The poll should fail, detect the disconnect, and update internal state
    EXPECT_FALSE(mClient.pollPacket(emptyPacket));
    EXPECT_FALSE(mClient.isConnected());
}