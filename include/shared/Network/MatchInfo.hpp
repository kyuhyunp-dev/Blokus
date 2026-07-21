#ifndef MATCH_INFO_HPP
#define MATCH_INFO_HPP

struct MatchInfo 
{
    std::string matchCode;
    int targetPlayerCount = 2;  
    int currentPlayerCount = 0;  
};

#endif