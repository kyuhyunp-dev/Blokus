#include <gtest/gtest.h>
#include "shared/Referee.hpp"
#include "TestBase.hpp"

class RefereeTest : public PolyominoTestBase {};

TEST_F(RefereeTest, Boundary) {
    Referee referee(sLibrary); 
    
    EXPECT_FALSE(referee.isValid(0, {-1, 0}, Team::Red));
    EXPECT_FALSE(referee.isValid(0, {0, -1}, Team::Red));
    EXPECT_FALSE(referee.isValid(0, {Config::BoardSize, 0}, Team::Red));
    EXPECT_FALSE(referee.isValid(0, {0, Config::BoardSize}, Team::Red));

    EXPECT_FALSE(referee.isValid(0, {-1, 0}, Team::Blue));
    EXPECT_FALSE(referee.isValid(0, {0, -1}, Team::Blue));
    EXPECT_FALSE(referee.isValid(0, {Config::BoardSize, 0}, Team::Blue));
    EXPECT_FALSE(referee.isValid(0, {0, Config::BoardSize}, Team::Blue));

    EXPECT_FALSE(referee.isValid(0, {-1, 0}, Team::Yellow));
    EXPECT_FALSE(referee.isValid(0, {0, -1}, Team::Yellow));
    EXPECT_FALSE(referee.isValid(0, {Config::BoardSize, 0}, Team::Yellow));
    EXPECT_FALSE(referee.isValid(0, {0, Config::BoardSize}, Team::Yellow));

    EXPECT_FALSE(referee.isValid(0, {-1, 0}, Team::Green));
    EXPECT_FALSE(referee.isValid(0, {0, -1}, Team::Green));
    EXPECT_FALSE(referee.isValid(0, {Config::BoardSize, 0}, Team::Green));
    EXPECT_FALSE(referee.isValid(0, {0, Config::BoardSize}, Team::Green));
}

TEST_F(RefereeTest, Start) {
    Referee referee(sLibrary);
    
    // Piece 0 (monomino)
    EXPECT_TRUE(referee.isValid(0, {0, 0}, Team::Red));
    EXPECT_FALSE(referee.isValid(0, {Config::BoardSize - 1, 0}, Team::Red));
    EXPECT_FALSE(referee.isValid(0, {0, Config::BoardSize - 1}, Team::Red));
    EXPECT_FALSE(referee.isValid(0, {Config::BoardSize - 1, Config::BoardSize - 1}, Team::Red)); 
    
    EXPECT_FALSE(referee.isValid(0, {0, 0}, Team::Blue));
    EXPECT_TRUE(referee.isValid(0, {Config::BoardSize - 1, 0}, Team::Blue));
    EXPECT_FALSE(referee.isValid(0, {0, Config::BoardSize - 1}, Team::Blue));
    EXPECT_FALSE(referee.isValid(0, {Config::BoardSize - 1, Config::BoardSize - 1}, Team::Blue));

    EXPECT_FALSE(referee.isValid(0, {0, 0}, Team::Yellow));
    EXPECT_FALSE(referee.isValid(0, {Config::BoardSize - 1, 0}, Team::Yellow));
    EXPECT_TRUE(referee.isValid(0, {0, Config::BoardSize - 1}, Team::Yellow));
    EXPECT_FALSE(referee.isValid(0, {Config::BoardSize - 1, Config::BoardSize - 1}, Team::Yellow));

    EXPECT_FALSE(referee.isValid(0, {0, 0}, Team::Green));
    EXPECT_FALSE(referee.isValid(0, {Config::BoardSize - 1, 0}, Team::Green));
    EXPECT_FALSE(referee.isValid(0, {0, Config::BoardSize - 1}, Team::Green));
    EXPECT_TRUE(referee.isValid(0, {Config::BoardSize - 1, Config::BoardSize - 1}, Team::Green));

    // Piece 1 (horizontal domino)
    EXPECT_TRUE(referee.isValid(1, {0, 0}, Team::Red));
    EXPECT_FALSE(referee.isValid(1, {Config::BoardSize - 1, 0}, Team::Red));
    EXPECT_FALSE(referee.isValid(1, {0, Config::BoardSize - 1}, Team::Red));
    EXPECT_FALSE(referee.isValid(1, {Config::BoardSize - 1, Config::BoardSize - 1}, Team::Red)); 
    
    EXPECT_FALSE(referee.isValid(1, {0, 0}, Team::Blue));
    EXPECT_TRUE(referee.isValid(1, {Config::BoardSize - 1, 0}, Team::Blue));
    EXPECT_FALSE(referee.isValid(1, {0, Config::BoardSize - 1}, Team::Blue));
    EXPECT_FALSE(referee.isValid(1, {Config::BoardSize - 1, Config::BoardSize - 1}, Team::Blue));

    EXPECT_FALSE(referee.isValid(1, {0, 0}, Team::Yellow));
    EXPECT_FALSE(referee.isValid(1, {Config::BoardSize - 1, 0}, Team::Yellow));
    EXPECT_TRUE(referee.isValid(1, {0, Config::BoardSize - 2}, Team::Yellow));
    EXPECT_FALSE(referee.isValid(1, {Config::BoardSize - 1, Config::BoardSize - 1}, Team::Yellow));

    EXPECT_FALSE(referee.isValid(1, {0, 0}, Team::Green));
    EXPECT_FALSE(referee.isValid(1, {Config::BoardSize - 1, 0}, Team::Green));
    EXPECT_FALSE(referee.isValid(1, {0, Config::BoardSize - 1}, Team::Green));
    EXPECT_TRUE(referee.isValid(1, {Config::BoardSize - 1, Config::BoardSize - 2}, Team::Green));
}

TEST_F(RefereeTest, ValidCornerContact) {
    Referee referee(sLibrary);
    
    referee.place(0, {0, 0}, Team::Red);

    EXPECT_TRUE(referee.isValid(0, {1, 1}, Team::Red));
    
    referee.place(0, {1, 1}, Team::Red);
    EXPECT_TRUE(referee.isValid(0, {2, 2}, Team::Red));
}

TEST_F(RefereeTest, InvalidEdgeContact) {
    Referee referee(sLibrary);
    
    referee.place(0, {0, 0}, Team::Red);

    EXPECT_FALSE(referee.isValid(0, {1, 0}, Team::Red)); // Right edge
    EXPECT_FALSE(referee.isValid(0, {0, 1}, Team::Red)); // Bottom edge
    
    referee.place(0, {1, 1}, Team::Red);
    
    EXPECT_FALSE(referee.isValid(0, {1, 0}, Team::Red)); 
}

TEST_F(RefereeTest, DisconnectedPlacement) {
    Referee referee(sLibrary);
    
    referee.place(0, {0, 0}, Team::Red);

    EXPECT_FALSE(referee.isValid(0, {2, 2}, Team::Red));
    EXPECT_FALSE(referee.isValid(0, {0, 5}, Team::Red));
    EXPECT_FALSE(referee.isValid(0, {5, 5}, Team::Red));
}

TEST_F(RefereeTest, OverlapAndCollision) {
    Referee referee(sLibrary);
    
    referee.place(0, {0, 0}, Team::Red);

    EXPECT_FALSE(referee.isValid(0, {0, 0}, Team::Red));
}

TEST_F(RefereeTest, LargerPieceContact) {
    Referee referee(sLibrary);
    
    referee.place(0, {0, 0}, Team::Red);

    EXPECT_TRUE(referee.isValid(1, {1, 1}, Team::Red));
    
    // Placing pieces on the edge of the larger piece.
    EXPECT_FALSE(referee.isValid(1, {1, 2}, Team::Red));
    EXPECT_FALSE(referee.isValid(1, {2, 2}, Team::Red)); 
}

TEST_F(RefereeTest, OpponentInteraction) {
    Referee referee(sLibrary);
    
    referee.place(0, {0, 0}, Team::Red);
    referee.place(0, {Config::BoardSize - 1, 0}, Team::Blue);

    // Vertical domino occupying {1,1} and {1,2}
    referee.place(1, {1, 1}, Team::Red); 

    EXPECT_TRUE(referee.isValid(0, {Config::BoardSize - 2, 1}, Team::Blue));

    EXPECT_FALSE(referee.isValid(0, {2, 2}, Team::Blue)); // Touches Red's corner, but no Blue corner
    EXPECT_FALSE(referee.isValid(0, {2, 0}, Team::Blue)); // Touches Red's edge, but no Blue corner
}
 