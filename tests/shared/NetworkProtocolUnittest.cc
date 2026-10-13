#include <gtest/gtest.h>
#include "shared/Network/NetworkProtocol.hpp"

// Test case for the PacketType enum serialization
TEST(NetworkProtocolTest, PacketTypeRoundTrip) 
{
    sf::Packet packet;
    { /* ... test CreateMatch ... */
        NetworkProtocol::PacketType originalType = NetworkProtocol::PacketType::CreateMatch;
        packet << originalType;
        
        NetworkProtocol::PacketType deserializedType;
        packet >> deserializedType;
        
        EXPECT_EQ(originalType, deserializedType);
    }
    packet.clear();

    { /* ... test MatchJoined ... */    
        NetworkProtocol::PacketType originalType = NetworkProtocol::PacketType::MatchJoined;
        packet << originalType;
        
        NetworkProtocol::PacketType deserializedType;
        packet >> deserializedType;
        
        EXPECT_EQ(originalType, deserializedType);
    }
    packet.clear();

    { // Test CreateUsername
        NetworkProtocol::PacketType originalType = NetworkProtocol::PacketType::CreateUsername;
        packet << originalType;
        
        NetworkProtocol::PacketType deserializedType;
        packet >> deserializedType;
        
        EXPECT_EQ(originalType, deserializedType);
    }
    packet.clear();

    { // Test UsernameCreated
        NetworkProtocol::PacketType originalType = NetworkProtocol::PacketType::UsernameCreated;
        packet << originalType;
        
        NetworkProtocol::PacketType deserializedType;
        packet >> deserializedType;
        
        EXPECT_EQ(originalType, deserializedType);
    }
    packet.clear();

    { // Test UsernameAlreadyRegistered
        NetworkProtocol::PacketType originalType = NetworkProtocol::PacketType::AlreadyRegistered;
        packet << originalType;
        
        NetworkProtocol::PacketType deserializedType;
        packet >> deserializedType;
        
        EXPECT_EQ(originalType, deserializedType);
    }
}

// Test case for JoinMatchRequest (Client -> Server)
TEST(NetworkProtocolTest, JoinMatchRequestRoundTrip) 
{
    sf::Packet packet;
    NetworkProtocol::JoinMatchRequest originalRequest{"ROOM"};
    
    packet << originalRequest;
    
    NetworkProtocol::JoinMatchRequest deserializedRequest;
    packet >> deserializedRequest;
    
    EXPECT_EQ(originalRequest.matchCode, deserializedRequest.matchCode);
}

// Test case for JoinMatchRequest edge case (Empty String)
TEST(NetworkProtocolTest, JoinMatchRequestEmptyCode) 
{
    sf::Packet packet;
    NetworkProtocol::JoinMatchRequest originalRequest{""};
    
    packet << originalRequest;
    
    NetworkProtocol::JoinMatchRequest deserializedRequest;
    packet >> deserializedRequest;
    
    EXPECT_EQ(deserializedRequest.matchCode, "");
}

// Test case for MatchJoinedResponse (Server -> Client)
TEST(NetworkProtocolTest, MatchJoinedResponseRoundTrip) 
{
    sf::Packet packet;
    NetworkProtocol::MatchJoinedResponse originalResponse{"LOBBY_A"};
    
    packet << originalResponse;
    
    NetworkProtocol::MatchJoinedResponse deserializedResponse;
    packet >> deserializedResponse;
    
    EXPECT_EQ(originalResponse.matchCode, deserializedResponse.matchCode);
}

// Test case for MatchJoinFailedResponse (Server -> Client)
TEST(NetworkProtocolTest, MatchJoinFailedResponseRoundTrip) 
{
    sf::Packet packet;
    NetworkProtocol::MatchJoinFailedResponse originalResponse{NetworkProtocol::JoinError::AlreadyInMatch};
    
    packet << originalResponse;
    
    NetworkProtocol::MatchJoinFailedResponse deserializedResponse;
    packet >> deserializedResponse;
    
    EXPECT_EQ(originalResponse.reason, deserializedResponse.reason);
}

// Test case for CreateUsernameRequest (Client -> Server)
TEST(NetworkProtocolTest, CreateUsernameRequestRoundTrip) 
{
    sf::Packet packet;
    NetworkProtocol::CreateUsernameRequest originalRequest{"PlayerOne"};
    
    packet << originalRequest;
    
    NetworkProtocol::CreateUsernameRequest deserializedRequest;
    packet >> deserializedRequest;
    
    EXPECT_EQ(originalRequest.username, deserializedRequest.username);
}

// Test case for CreateUsernameRequest edge case (Empty String)
TEST(NetworkProtocolTest, CreateUsernameRequestEmpty) 
{
    sf::Packet packet;
    NetworkProtocol::CreateUsernameRequest originalRequest{""};
    
    packet << originalRequest;
    
    NetworkProtocol::CreateUsernameRequest deserializedRequest;
    packet >> deserializedRequest;
    
    EXPECT_EQ(deserializedRequest.username, "");
}

// Test case for UsernameCreatedResponse (Server -> Client)
TEST(NetworkProtocolTest, UsernameCreatedResponseRoundTrip) 
{
    sf::Packet packet;
    NetworkProtocol::UsernameCreatedResponse originalResponse{"PlayerOne"};
    
    packet << originalResponse;
    
    NetworkProtocol::UsernameCreatedResponse deserializedResponse;
    packet >> deserializedResponse;
    
    EXPECT_EQ(originalResponse.username, deserializedResponse.username);
}

// Full pipeline test: Simulates a real chained network transaction
TEST(NetworkProtocolTest, ChainedPipelineRoundTrip) 
{
    sf::Packet packet;
    { 
        NetworkProtocol::PacketType originalType = NetworkProtocol::PacketType::JoinMatch;
        NetworkProtocol::JoinMatchRequest originalRequest{"ROOM"};
        
        // Chain serialization exactly like the real game loop
        packet << originalType << originalRequest;
        
        // Sequential deserialization
        NetworkProtocol::PacketType deserializedType;
        NetworkProtocol::JoinMatchRequest deserializedRequest;
        
        packet >> deserializedType >> deserializedRequest;
        packet.clear();

        EXPECT_EQ(originalType, deserializedType);
        EXPECT_EQ(originalRequest.matchCode, deserializedRequest.matchCode);
    }
    {
        NetworkProtocol::PacketType originalType = NetworkProtocol::PacketType::CreateUsername;
        NetworkProtocol::CreateUsernameRequest originalRequest{"BlokusMaster"};
        
        packet << originalType << originalRequest;

        // Sequential deserialization
        NetworkProtocol::PacketType deserializedType;
        NetworkProtocol::CreateUsernameRequest deserializedRequest;
        
        packet >> deserializedType >> deserializedRequest;
        packet.clear();
        
        EXPECT_EQ(originalType, deserializedType);
        EXPECT_EQ(originalRequest.username, deserializedRequest.username);
    }
}