#ifndef PLAYER_INFO_HPP
#define PLAYER_INFO_HPP

#include "shared/Team.hpp"
#include <cstdint>
#include <string>


struct PlayerInfo 
{
    int playerId;
    Team team;
    std::string username;
    bool isConnected;
    float remainingTime;    
};

#endif