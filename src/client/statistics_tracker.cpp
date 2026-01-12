#include "statistics_tracker.hpp"
#include <utility>
#include <sstream>
#include <iomanip>
#include <spdlog/spdlog.h>


StatisticsTracker::StatisticsTracker()
    : update_time_(sf::Time::Zero)
    , num_frames_(0)
{
}

void StatisticsTracker::Update(sf::Time dt) 
{
    update_time_ += dt;
    num_frames_ += 1;

    if (update_time_ >= sf::seconds(1.0f)) {
        float fps = static_cast<float>(num_frames_) / update_time_.asSeconds();

        std::ostringstream stream;
        stream << std::fixed << std::setprecision(2);
        stream << "Frames / Second = " << fps << "\n";
        stream << "Time / Update = "
               << (update_time_.asMicroseconds() / num_frames_) << "us";

        spdlog::info(stream.str());

        // reset counters
        update_time_ -= sf::seconds(1.0f);
        num_frames_ = 0;
    }
}

