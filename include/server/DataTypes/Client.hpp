#ifndef CLIENT_HPP
#define CLIENT_HPP

#include "ClientState.hpp"
#include <SFML/Network/TcpSocket.hpp>
#include <SFML/Network/Packet.hpp>
#include <memory>
#include <optional>
#include <deque>
#include <string>
#include <cstdint>


namespace Server
{
    struct Client
    {
        std::unique_ptr<sf::TcpSocket> socket;
        uint32_t id;
        ClientState state = ClientState::TitleState;
        std::optional<std::string> username;
        std::optional<std::string> currentMatchCode;
        std::deque<sf::Packet> outgoingQueue;
    };
}

#endif