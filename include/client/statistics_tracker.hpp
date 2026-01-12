#ifndef STATISTICS_TRACKER_HPP
#define STATISTICS_TRACKER_HPP

#include <SFML/System/Time.hpp>
#include <string>


class StatisticsTracker 
{
    public:
        StatisticsTracker();

        void Update(sf::Time dt);

    private:
        sf::Time update_time_;
        std::size_t num_frames_;
};

#endif