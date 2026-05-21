#ifndef GAME_SESSION_DATA_HPP
#define GAME_SESSION_DATA_HPP

#include "MatchInfo.hpp"
#include "PlayerInfo.hpp"

#include <map>


struct GameSessionData 
{
    int localPlayerId;    
    MatchInfo match;
    std::map<int, PlayerInfo> playerById; 
};

#endif