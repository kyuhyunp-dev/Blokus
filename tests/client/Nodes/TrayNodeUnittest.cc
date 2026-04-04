#include "MockTextureHolder.hpp"

#include <gtest/gtest.h>
#include "Nodes/TrayNode.hpp" 
#include "Config.hpp"

#include "SFML/Graphics/Texture.hpp"
    

TEST(TrayNodeTest, WithdrawalAndHitTest) 
{
    // 1. Setup
    MockTextureHolder testTextures;
    TrayNode tray(testTextures);
    
    int testSlotId = 5;
    int testPieceId = 42;
    sf::Vector2f clickInside(10.f, 10.f);
    sf::Vector2f clickOutside(500.f, 500.f);

    // 2. Initial state: Should be empty
    EXPECT_FALSE(tray.getPieceIdAt(clickInside).has_value());

    // 3. Add a piece
    auto piece = std::make_unique<PieceNode>(testPieceId, testTextures);
    // Manually set position/size for the test if your TrayNode doesn't layout automatically
    piece->setPosition({0.f, 0.f}); 
    
    tray.addPiece(testSlotId, std::move(piece));

    // 4. Test getPieceIdAt
    // Test hit inside
    auto foundId = tray.getPieceIdAt(clickInside);
    ASSERT_TRUE(foundId.has_value());
    EXPECT_EQ(*foundId, testPieceId);

    // Test hit outside
    EXPECT_FALSE(tray.getPieceIdAt(clickOutside).has_value());

    // 5. Test withdrawPiece (Success)
    auto withdrawn = tray.withdrawPiece(testPieceId);
    ASSERT_NE(withdrawn, nullptr);
    EXPECT_EQ(withdrawn->getId(), testPieceId);
    EXPECT_EQ(withdrawn->getSlotId(), testSlotId); // Verify it remembered its home!

    // 6. Post-withdrawal state: Should be empty again
    EXPECT_FALSE(tray.getPieceIdAt(clickInside).has_value());
}

TEST(TrayNodeTest, WithdrawInvalidIdCrashes) 
{
    MockTextureHolder testTextures;
    TrayNode tray(testTextures);
    // Requesting an ID that was never added
    EXPECT_DEATH(tray.withdrawPiece(99), "Piece ID not found in Tray slots!");
}