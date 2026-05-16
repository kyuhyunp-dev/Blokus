#include "StatisticsTracker.hpp"

#include "gtest/gtest.h"
#include <regex>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/ostream_sink.h>

namespace
{
    TEST(StatisticsTrackerTest, LessThanSecond)
    {
        std::ostringstream oss;
        auto sink = std::make_shared<spdlog::sinks::ostream_sink_mt>(oss);
        auto logger = std::make_shared<spdlog::logger>("test_logger", sink);

        // Swap the default logger
        auto sourceLogger = spdlog::default_logger();
        spdlog::set_default_logger(logger);

        StatisticsTracker statistics;
        for (int frame = 0; frame < 60; ++frame)
        {
            statistics.update(sf::milliseconds(16));
            EXPECT_TRUE(oss.str().empty());
        }

        // Restore the old logger
        spdlog::set_default_logger(sourceLogger);
    }

    TEST(StatisticsTrackerTest, MoreThanSecond)
    {
        std::ostringstream oss;
        auto sink = std::make_shared<spdlog::sinks::ostream_sink_mt>(oss);
        auto logger = std::make_shared<spdlog::logger>("test_logger", sink);

        // Swap the default logger
        auto sourceLogger = spdlog::default_logger();
        spdlog::set_default_logger(logger);

        StatisticsTracker statistics;
        //[<timestamp>] [<logger name>] [info] Frames / Second = 1.00
        std::regex pattern(
            R"(\[.*\] \[test_logger\] \[info\] Frames / Second = \d+(\.\d+)?)"
        );

        // One pattern for 1 second
        statistics.update(sf::seconds(1));
        EXPECT_TRUE(std::regex_search(oss.str(), pattern));
        
        // Two patterns for 2 seconds
        statistics.update(sf::seconds(2));
        std::string output = oss.str();

        auto begin = std::sregex_iterator(output.begin(), output.end(), pattern);
        auto end = std::sregex_iterator();

        size_t count = std::distance(begin, end);
        EXPECT_EQ(count, 2);

        // Restore the old logger
        spdlog::set_default_logger(sourceLogger);
    }
}