#include <gtest/gtest.h>
#include "shared/Referee.hpp"


TEST(RefereeTest, Boundary) {
    Referee referee; 
    
    EXPECT_FALSE(referee.isValid(0, {-1, 0}, Team::Red));
    EXPECT_FALSE(referee.isValid(0, {0, -1}, Team::Red));
    EXPECT_FALSE(referee.isValid(0, {Blokus::BoardSize, 0}, Team::Red));
    EXPECT_FALSE(referee.isValid(0, {0, Blokus::BoardSize}, Team::Red));

    EXPECT_FALSE(referee.isValid(0, {-1, 0}, Team::Blue));
    EXPECT_FALSE(referee.isValid(0, {0, -1}, Team::Blue));
    EXPECT_FALSE(referee.isValid(0, {Blokus::BoardSize, 0}, Team::Blue));
    EXPECT_FALSE(referee.isValid(0, {0, Blokus::BoardSize}, Team::Blue));

    EXPECT_FALSE(referee.isValid(0, {-1, 0}, Team::Yellow));
    EXPECT_FALSE(referee.isValid(0, {0, -1}, Team::Yellow));
    EXPECT_FALSE(referee.isValid(0, {Blokus::BoardSize, 0}, Team::Yellow));
    EXPECT_FALSE(referee.isValid(0, {0, Blokus::BoardSize}, Team::Yellow));

    EXPECT_FALSE(referee.isValid(0, {-1, 0}, Team::Green));
    EXPECT_FALSE(referee.isValid(0, {0, -1}, Team::Green));
    EXPECT_FALSE(referee.isValid(0, {Blokus::BoardSize, 0}, Team::Green));
    EXPECT_FALSE(referee.isValid(0, {0, Blokus::BoardSize}, Team::Green));
}

TEST(RefereeTest, Start) {
    Referee referee;
    
    // Piece 0 (monomino)
    EXPECT_TRUE(referee.isValid(0, {0, 0}, Team::Red));
    EXPECT_FALSE(referee.isValid(0, {Blokus::BoardSize - 1, 0}, Team::Red));
    EXPECT_FALSE(referee.isValid(0, {0, Blokus::BoardSize - 1}, Team::Red));
    EXPECT_FALSE(referee.isValid(0, {Blokus::BoardSize - 1, Blokus::BoardSize - 1}, Team::Red)); 
    
    EXPECT_FALSE(referee.isValid(0, {0, 0}, Team::Blue));
    EXPECT_TRUE(referee.isValid(0, {Blokus::BoardSize - 1, 0}, Team::Blue));
    EXPECT_FALSE(referee.isValid(0, {0, Blokus::BoardSize - 1}, Team::Blue));
    EXPECT_FALSE(referee.isValid(0, {Blokus::BoardSize - 1, Blokus::BoardSize - 1}, Team::Blue));

    EXPECT_FALSE(referee.isValid(0, {0, 0}, Team::Yellow));
    EXPECT_FALSE(referee.isValid(0, {Blokus::BoardSize - 1, 0}, Team::Yellow));
    EXPECT_TRUE(referee.isValid(0, {0, Blokus::BoardSize - 1}, Team::Yellow));
    EXPECT_FALSE(referee.isValid(0, {Blokus::BoardSize - 1, Blokus::BoardSize - 1}, Team::Yellow));

    EXPECT_FALSE(referee.isValid(0, {0, 0}, Team::Green));
    EXPECT_FALSE(referee.isValid(0, {Blokus::BoardSize - 1, 0}, Team::Green));
    EXPECT_FALSE(referee.isValid(0, {0, Blokus::BoardSize - 1}, Team::Green));
    EXPECT_TRUE(referee.isValid(0, {Blokus::BoardSize - 1, Blokus::BoardSize - 1}, Team::Green));

    // Piece 1 (horizontal domino)
    EXPECT_TRUE(referee.isValid(1, {0, 0}, Team::Red));
    EXPECT_FALSE(referee.isValid(1, {Blokus::BoardSize - 1, 0}, Team::Red));
    EXPECT_FALSE(referee.isValid(1, {0, Blokus::BoardSize - 1}, Team::Red));
    EXPECT_FALSE(referee.isValid(1, {Blokus::BoardSize - 1, Blokus::BoardSize - 1}, Team::Red)); 
    
    EXPECT_FALSE(referee.isValid(1, {0, 0}, Team::Blue));
    EXPECT_TRUE(referee.isValid(1, {Blokus::BoardSize - 1, 0}, Team::Blue));
    EXPECT_FALSE(referee.isValid(1, {0, Blokus::BoardSize - 1}, Team::Blue));
    EXPECT_FALSE(referee.isValid(1, {Blokus::BoardSize - 1, Blokus::BoardSize - 1}, Team::Blue));

    EXPECT_FALSE(referee.isValid(1, {0, 0}, Team::Yellow));
    EXPECT_FALSE(referee.isValid(1, {Blokus::BoardSize - 1, 0}, Team::Yellow));
    EXPECT_TRUE(referee.isValid(1, {0, Blokus::BoardSize - 2}, Team::Yellow));
    EXPECT_FALSE(referee.isValid(1, {Blokus::BoardSize - 1, Blokus::BoardSize - 1}, Team::Yellow));

    EXPECT_FALSE(referee.isValid(1, {0, 0}, Team::Green));
    EXPECT_FALSE(referee.isValid(1, {Blokus::BoardSize - 1, 0}, Team::Green));
    EXPECT_FALSE(referee.isValid(1, {0, Blokus::BoardSize - 1}, Team::Green));
    EXPECT_TRUE(referee.isValid(1, {Blokus::BoardSize - 1, Blokus::BoardSize - 2}, Team::Green));
}
 