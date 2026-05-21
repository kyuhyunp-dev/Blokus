#include "StatisticsTracker.hpp"
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
        auto timePerUpdateUs = mUpdateTime.asMicroseconds() / mNumFrames;

        spdlog::info("Frames / Second = {:.2f}\nTime / Update = {}us", fps, timePerUpdateUs);

        // reset counters
        mUpdateTime -= sf::seconds(1.0f);
        mNumFrames = 0;
    }
}

