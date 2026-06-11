#ifndef NETWORK_PROTOCOL_HPP
#define NETWORK_PROTOCOL_HPP

#include <SFML/Network/Packet.hpp>
#include <string>
#include <cstdint>


const unsigned short ServerPort = 49152;

namespace NetworkProtocol 
{
    enum class PacketType : uint8_t 
    {
        // Matchmaking 
        CreateMatch,
        JoinMatch,
        MatchJoined,
        MatchJoinFailed 
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
        uint8_t localPlayerId;
        std::string matchCode;
    };

    inline sf::Packet& operator<<(sf::Packet& packet, const MatchJoinedResponse& response) 
    {
        return packet << response.localPlayerId << response.matchCode;
    }

    inline sf::Packet& operator>>(sf::Packet& packet, MatchJoinedResponse& response) 
    {
        return packet >> response.localPlayerId >> response.matchCode;
    }


    struct MatchJoinFailedResponse 
    { // Server -> Client
        std::string reason;
    };

    inline sf::Packet& operator<<(sf::Packet& packet, const MatchJoinFailedResponse& response) 
    {
        return packet << response.reason;
    }

    inline sf::Packet& operator>>(sf::Packet& packet, MatchJoinFailedResponse& response) 
    {
        return packet >> response.reason;
    }
}

#endif