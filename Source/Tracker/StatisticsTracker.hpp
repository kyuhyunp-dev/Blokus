#ifndef TRACKER_STATISTICS_TRACKER_HPP
#define TRACKER_STATISTICS_TRACKER_HPP

#include <SFML/System/Time.hpp>
#include <string>


class StatisticsTracker 
{
    public:
        StatisticsTracker();

        void update(sf::Time dt);

    private:
        sf::Time mUpdateTime;
        std::size_t mNumFrames;
};

#endif