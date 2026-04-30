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

TEST(RefereeTest, ValidCornerContact) {
    Referee referee;
    
    referee.place(0, {0, 0}, Team::Red);

    EXPECT_TRUE(referee.isValid(0, {1, 1}, Team::Red));
    
    referee.place(0, {1, 1}, Team::Red);
    EXPECT_TRUE(referee.isValid(0, {2, 2}, Team::Red));
}

TEST(RefereeTest, InvalidEdgeContact) {
    Referee referee;
    
    referee.place(0, {0, 0}, Team::Red);

    EXPECT_FALSE(referee.isValid(0, {1, 0}, Team::Red)); // Right edge
    EXPECT_FALSE(referee.isValid(0, {0, 1}, Team::Red)); // Bottom edge
    
    referee.place(0, {1, 1}, Team::Red);
    
    EXPECT_FALSE(referee.isValid(0, {1, 0}, Team::Red)); 
}

TEST(RefereeTest, DisconnectedPlacement) {
    Referee referee;
    
    referee.place(0, {0, 0}, Team::Red);

    EXPECT_FALSE(referee.isValid(0, {2, 2}, Team::Red));
    EXPECT_FALSE(referee.isValid(0, {0, 5}, Team::Red));
    EXPECT_FALSE(referee.isValid(0, {5, 5}, Team::Red));
}

TEST(RefereeTest, OverlapAndCollision) {
    Referee referee;
    
    referee.place(0, {0, 0}, Team::Red);

    EXPECT_FALSE(referee.isValid(0, {0, 0}, Team::Red));
}

TEST(RefereeTest, LargerPieceContact) {
    Referee referee;
    
    referee.place(0, {0, 0}, Team::Red);

    EXPECT_TRUE(referee.isValid(1, {1, 1}, Team::Red));
    
    // Placing pieces on the edge of the larger piece.
    EXPECT_FALSE(referee.isValid(1, {1, 2}, Team::Red));
    EXPECT_FALSE(referee.isValid(1, {2, 2}, Team::Red)); 
}

TEST(RefereeTest, OpponentInteraction) {
    Referee referee;
    
    referee.place(0, {0, 0}, Team::Red);
    referee.place(0, {Blokus::BoardSize - 1, 0}, Team::Blue);

    // Vertical domino occupying {1,1} and {1,2}
    referee.place(1, {1, 1}, Team::Red); 

    EXPECT_TRUE(referee.isValid(0, {Blokus::BoardSize - 2, 1}, Team::Blue));

    EXPECT_FALSE(referee.isValid(0, {2, 2}, Team::Blue)); // Touches Red's corner, but no Blue corner
    EXPECT_FALSE(referee.isValid(0, {2, 0}, Team::Blue)); // Touches Red's edge, but no Blue corner
}
 