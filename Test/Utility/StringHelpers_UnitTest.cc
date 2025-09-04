#include "Utility/StringHelpers.hpp"

#include "gtest/gtest.h"


namespace
{
    TEST(ToStringTest, Numbers)
    {
        EXPECT_EQ(toString(2), "2");
        EXPECT_EQ(toString(3000000000LL), "3000000000");

        EXPECT_EQ(toString(-3), "-3");
        EXPECT_EQ(toString(-3000000000LL), "-3000000000");

        EXPECT_EQ(toString(2.5), "2.5");
    } 

    TEST(ToStringTest, String)
    {
        EXPECT_EQ(toString("string str"), "string str");
    } 
}