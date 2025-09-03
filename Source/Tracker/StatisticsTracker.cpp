// StatisticsTracker.cpp
#include "Tracker/StatisticsTracker.hpp"
#include <utility>
#include <sstream>
#include <iomanip>
#include <spdlog/spdlog.h>


StatisticsTracker::StatisticsTracker()
    : mUpdateTime(sf::Time::Zero)
    , mNumFrames(0)
{
}

void StatisticsTracker::update(sf::Time dt) 
{
    mUpdateTime += dt;
    mNumFrames += 1;

    if (mUpdateTime >= sf::seconds(1.0f)) {
        float fps = static_cast<float>(mNumFrames) / mUpdateTime.asSeconds();

        std::ostringstream stream;
        stream << std::fixed << std::setprecision(2);
        stream << "Frames / Second = " << fps << "\n";
        stream << "Time / Update = "
               << (mUpdateTime.asMicroseconds() / mNumFrames) << "us";

        spdlog::info(stream.str());

        // reset counters
        mUpdateTime -= sf::seconds(1.0f);
        mNumFrames = 0;
    }
}

