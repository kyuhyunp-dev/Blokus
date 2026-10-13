#ifndef NETWORK_PROTOCOL_HPP
#define NETWORK_PROTOCOL_HPP

#include <SFML/Network/Packet.hpp>
#include <string>
#include <cstdint>


namespace NetworkProtocol 
{
    enum class PacketType : uint8_t 
    {
        // Matchmaking 
        CreateMatch,
        JoinMatch,
        
        // Ack
        MatchJoined, 
        MatchJoinFailed,

        // Username
        CreateUsername,
        
        // Ack
        UsernameCreated,
        AlreadyRegistered

    };


    // Operators for sending and receiving PacketTypes
    inline sf::Packet& operator<<(sf::Packet& packet, PacketType type) 
    {
        return packet << static_cast<uint8_t>(type);
    }

    inline sf::Packet& operator>>(sf::Packet& packet, PacketType& type) 
    {
        uint8_t typeValue;
        packet >> typeValue;
        type = static_cast<PacketType>(typeValue);
        return packet;
    }

// ----------------------------------------------------------------    

    // Packet JoinMatch
    struct JoinMatchRequest 
    { // Client -> Server
        std::string matchCode;
    };

    inline sf::Packet& operator<<(sf::Packet& packet, const JoinMatchRequest& request) 
    {
        return packet << request.matchCode;
    }

    inline sf::Packet& operator>>(sf::Packet& packet, JoinMatchRequest& request) 
    {
        return packet >> request.matchCode;
    }

    // Packet MatchJoined
    struct MatchJoinedResponse 
    { // Server -> Client
        std::string matchCode;
    };

    inline sf::Packet& operator<<(sf::Packet& packet, const MatchJoinedResponse& response) 
    {
        return packet << response.matchCode;
    }

    inline sf::Packet& operator>>(sf::Packet& packet, MatchJoinedResponse& response) 
    {
        return packet >> response.matchCode;
    }

    // Reasons for MatchJoinFailed
    enum class JoinError : uint8_t 
    {
        MatchFull,
        MatchNotFound,
        AlreadyInMatch
    };

    // Packet MatchJoinFailed
    struct MatchJoinFailedResponse 
    { // Server -> Client
        JoinError reason;
    };

    inline sf::Packet& operator<<(sf::Packet& packet, const MatchJoinFailedResponse& response) 
    {
        return packet << static_cast<uint8_t>(response.reason);
    }

    inline sf::Packet& operator>>(sf::Packet& packet, MatchJoinFailedResponse& response) 
    {
        uint8_t reason;
        packet >> reason;
        response.reason = static_cast<JoinError>(reason);
        return packet;
    }


// ----------------------------------------------------------------    
    struct CreateUsernameRequest
    {
        std::string username;
    };

    inline sf::Packet& operator<<(sf::Packet& packet, const CreateUsernameRequest& request) 
    {
        return packet << request.username;
    }

    inline sf::Packet& operator>>(sf::Packet& packet, CreateUsernameRequest& request) 
    {
        return packet >> request.username;
    }

    struct UsernameCreatedResponse 
    {
        std::string username;
    };

    inline sf::Packet& operator<<(sf::Packet& packet, const UsernameCreatedResponse& response) 
    {
        return packet << response.username;
    }

    inline sf::Packet& operator>>(sf::Packet& packet, UsernameCreatedResponse& response) 
    {
        return packet >> response.username;
    }
}


#endif
