#ifndef MATCH_HPP
#define MATCH_HPP

#include <string>
#include <vector>
#include <SFML/Network/TcpSocket.hpp>
#include "shared/Network/NetworkProtocol.hpp" 
#include "shared/Team.hpp"

namespace Server 
{
    struct Player 
    {
        int id;
        sf::TcpSocket* socketPtr;
    };

    struct Match 
    {
        std::string matchCode; 
        std::vector<Player> players;
    };
}
#endif