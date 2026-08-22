#ifndef NETWORK_PROTOCOL_HPP
#define NETWORK_PROTOCOL_HPP

#include <SFML/Network/Packet.hpp>
#include <string>
#include <cstdint>


inline constexpr int ServerPort = 49152;
inline constexpr std::string_view IpAddress = "127.0.0.1"; // Home 
//"10.167.189.28"; // Library
//"127.0.0.1"; // Local

namespace NetworkProtocol 
{
    enum class PacketType : uint8_t 
    {
        // Matchmaking 
        CreateMatch,
        JoinMatch,
        MatchJoined, // Ack
        MatchJoinFailed 
    };
    
    enum class JoinError : uint8_t 
    {
        MatchFull,
        MatchNotFound,
        AlreadyInMatch
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
}

#endif
