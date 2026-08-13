#ifndef MATCH_HPP
#define MATCH_HPP

#include <vector>


namespace Server
{
    struct Match
    {
        std::string matchCode;
        std::vector<uint32_t> playerIDs;
    };
}

#endif