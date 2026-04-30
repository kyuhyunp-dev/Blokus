#include "Mock/Resource/MockTextureHolder.hpp"

#include <gtest/gtest.h>
#include "Nodes/TrayNode.hpp" 
#include "Config.hpp"

#include "SFML/Graphics/Texture.hpp"
    

TEST(TrayNodeTest, WithdrawalAndHitTest) 
{
    // 1. Setup
    MockTextureHolder testTextures;
    testTextures.load(Textures::Tiles, "dummy_data");

    TrayNode tray(testTextures);
    
    int testSlotId = 0;
    int testPieceId = 42;
    sf::Vector2f clickInside(10.f, 10.f);
    sf::Vector2f clickOutside(500.f, 500.f);

    // 2. Initial state: Should be empty
    EXPECT_FALSE(tray.getPieceAt(clickInside));

    // 3. Add a piece
    auto piece = std::make_unique<PieceNode>(testPieceId, Team::Red, testTextures);
    // Manually set position/size for the test if your TrayNode doesn't layout automatically
    piece->setPosition({0.f, 0.f}); 
    auto piecePtr = piece.get();
    
    tray.addPiece(testSlotId, std::move(piece));

    // 4. Test getPieceIdAt
    // Test hit inside
    auto foundPiece = tray.getPieceAt(clickInside);
    EXPECT_TRUE(foundPiece);
    EXPECT_EQ(foundPiece->getId(), piecePtr->getId());

    // Test hit outside
    EXPECT_FALSE(tray.getPieceAt(clickOutside));

    // 5. Test withdrawPiece (Success)
    auto withdrawn = tray.withdrawPiece(testPieceId);
    ASSERT_NE(withdrawn, nullptr);
    EXPECT_EQ(withdrawn->getId(), testPieceId);
    EXPECT_EQ(withdrawn->getSlotId(), testSlotId); 

    // 6. Post-withdrawal state: Should be empty again
    EXPECT_FALSE(tray.getPieceAt(clickInside));
}

TEST(TrayNodeTest, WithdrawInvalidIdCrashes) 
{
    GTEST_FLAG_SET(death_test_style, "threadsafe");
    
    MockTextureHolder testTextures;
    TrayNode tray(testTextures);
    // Requesting an ID that was never added
    ASSERT_DEATH(tray.withdrawPiece(99), "Piece ID not found in Tray slots!");
}