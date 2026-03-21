#include "MockTextureHolder.hpp"

#include <gtest/gtest.h>
#include "Nodes/TrayNode.hpp" 
#include "Config.hpp"

#include "SFML/Graphics/Texture.hpp"


void testOccupied(const std::vector<SlotInfo>& slots, const std::array<bool, Blokus::DeckSize>& occupied)
{
    for (int i = 0; i < Blokus::DeckSize; ++i) 
    {
        const SlotInfo& slot = slots[i];
            
        if (occupied[i]) 
        {
            EXPECT_TRUE(slot.pieceId);
        }
        else 
        {
            EXPECT_FALSE(slot.pieceId);
        }   
    }
}

TEST(TrayNodeTest, addPieceRoundTrip) 
{
    MockTextureHolder testTextures;
    TrayNode tray(testTextures);
    std::array<bool, Blokus::DeckSize> occupied;
    occupied.fill(false);

    std::vector<SlotInfo> slots = tray.getSlotManifest();
    EXPECT_EQ(slots.size(), Blokus::DeckSize);
    
    testOccupied(slots, occupied);

    auto monomino = std::make_unique<PieceNode>(0, Team::Red, testTextures);
    tray.addPiece(2, std::move(monomino));
    occupied[2] = true;

    testOccupied(slots, occupied);
}