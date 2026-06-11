#include <gtest/gtest.h>
#include "shared/NetworkProtocol.hpp"

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

    { /* ... test MatchJoined ... */    
        NetworkProtocol::PacketType originalType = NetworkProtocol::PacketType::MatchJoined;
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
    NetworkProtocol::MatchJoinedResponse originalResponse{3, "LOBBY_A"};
    
    packet << originalResponse;
    
    NetworkProtocol::MatchJoinedResponse deserializedResponse;
    packet >> deserializedResponse;
    
    EXPECT_EQ(originalResponse.localPlayerId, deserializedResponse.localPlayerId);
    EXPECT_EQ(originalResponse.matchCode, deserializedResponse.matchCode);
}

// Test case for MatchJoinFailedResponse (Server -> Client)
TEST(NetworkProtocolTest, MatchJoinFailedResponseRoundTrip) 
{
    sf::Packet packet;
    NetworkProtocol::MatchJoinFailedResponse originalResponse{"Lobby is completely full."};
    
    packet << originalResponse;
    
    NetworkProtocol::MatchJoinFailedResponse deserializedResponse;
    packet >> deserializedResponse;
    
    EXPECT_EQ(originalResponse.reason, deserializedResponse.reason);
}

// Full pipeline test: Simulates a real chained network transaction
TEST(NetworkProtocolTest, ChainedPipelineRoundTrip) 
{
    sf::Packet packet;
    
    NetworkProtocol::PacketType originalType = NetworkProtocol::PacketType::JoinMatch;
    NetworkProtocol::JoinMatchRequest originalRequest{"ROOM"};
    
    // Chain serialization exactly like the real game loop
    packet << originalType << originalRequest;
    
    // Sequential deserialization
    NetworkProtocol::PacketType deserializedType;
    NetworkProtocol::JoinMatchRequest deserializedRequest;
    
    packet >> deserializedType >> deserializedRequest;
    
    EXPECT_EQ(originalType, deserializedType);
    EXPECT_EQ(originalRequest.matchCode, deserializedRequest.matchCode);
}