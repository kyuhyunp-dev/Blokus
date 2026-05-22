#include <gtest/gtest.h>
#include "Nodes/Arena.hpp"
#include "Nodes/PieceNode.hpp"
#include "Nodes/TrayNode.hpp" 
#include "Nodes/BoardNode.hpp"
#include "shared/Referee.hpp"
#include "Command/CommandQueue.hpp"
#include "Mock/Resource/MockTextureHolder.hpp"
#include "shared/TestBase.hpp"

#include <SFML/Graphics/RenderTexture.hpp>

 
class ArenaTest : public PolyominoTestBase 
{
protected:
    void SetUp() override 
    {
        if (!target.resize({1024, 768})) {
            throw std::runtime_error("Failed to create RenderTexture for testing");
        }

        mockTextures.load(Textures::ID::Tiles, "unused_path");

        arena = std::make_unique<Arena>(target, mockTextures, sLibrary, deck, commands, Team::Blue);
        arena->buildScene();
    }

    // Mock/Real objects used in the test
    MockTextureHolder mockTextures;
    sf::RenderTexture target; // Headless or dummy for internal refs
    const std::array<int, Config::DeckSize> deck = 
    {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17}; 
    CommandQueue commands;
    std::unique_ptr<Arena> arena; 
};

TEST_F(ArenaTest, GrabPiece) {
    GTEST_FLAG_SET(death_test_style, "threadsafe");

    // 1. Arrange
    int targetId = 5;
    sf::Vector2f targetPos(150.f, 200.f);
    
    // Ensure the piece exists in the tray first
    auto* tray = arena->getTrayNodePtr();
    auto* actionLayer = arena->getLayer(Arena::Action);

    // 2. Act
    arena->grabPiece(targetId, targetPos);

    // 3. Assert: Position and Origin
    // We find the piece in the Action Layer now 
    ASSERT_EQ(actionLayer->getChildCount(), 1);
    auto* grabbedPiece = static_cast<PieceNode*>(actionLayer->getChildren().back());
    
    EXPECT_EQ(grabbedPiece->getId(), targetId);
    EXPECT_EQ(grabbedPiece->getPosition(), targetPos);

    // Verify Origin is set to Centroid
    // (Assuming PieceNode has a getCentroid method)
    EXPECT_EQ(grabbedPiece->getOrigin(), grabbedPiece->getCentroid());

    ASSERT_DEATH({
        arena->grabPiece(targetId, targetPos);
    }, "Piece ID not found"); 

    ASSERT_DEATH({
        tray->withdrawPiece(targetId);
    }, "Piece ID not found"); 
}

TEST_F(ArenaTest, GrabInvalidPiece) {
    GTEST_FLAG_SET(death_test_style, "threadsafe");

    int fakeId = 999; 
    // This expects the process to terminate because of the assert()
    ASSERT_DEATH({
        arena->grabPiece(fakeId, {0,0});
    }, "Invalid ID"); 
}

TEST_F(ArenaTest, PlacePiece) {
    int targetId = 5;
    arena->grabPiece(targetId, {150.f, 200.f}); 

    auto* actionLayer = arena->getLayer(Arena::Action);
    ASSERT_EQ(actionLayer->getChildCount(), 1);

    sf::Vector2i targetGrid(3, 3);

    arena->placePiece(targetGrid);
    EXPECT_EQ(actionLayer->getChildCount(), 0);

    auto* board = arena->getBoardNodePtr();
    ASSERT_GT(board->getChildCount(), 0); 
    
    auto* placedPiece = static_cast<PieceNode*>(board->getChildren().back());
    EXPECT_EQ(placedPiece->getId(), targetId);
    EXPECT_EQ(placedPiece->getOrigin(), sf::Vector2f(0.f, 0.f)); 
}

TEST_F(ArenaTest, PlacePieceWithoutActivePiece) {
    GTEST_FLAG_SET(death_test_style, "threadsafe");

    // We intentionally DO NOT call grabPiece() here. 
    sf::Vector2i targetGrid(3, 3);

    ASSERT_DEATH({
        arena->placePiece(targetGrid);
    }, ""); 
}

TEST_F(ArenaTest, ReturnPiece_SuccessfullyReturnsToTray) {
    int targetId = 1;
    sf::Vector2f targetPos(150.f, 200.f);
    arena->grabPiece(targetId, targetPos);

    auto* actionLayer = arena->getLayer(Arena::Action);
    ASSERT_EQ(actionLayer->getChildCount(), 1);

    auto* grabbedPiece = static_cast<PieceNode*>(actionLayer->getChildren().back());
    
    arena->returnPiece();

    EXPECT_EQ(actionLayer->getChildCount(), 0);

    EXPECT_NO_FATAL_FAILURE({
        arena->grabPiece(targetId, targetPos);
    });
}

TEST_F(ArenaTest, ReturnPiece_WithoutActivePiece_Crashes) {
    GTEST_FLAG_SET(death_test_style, "threadsafe");

    // Intentionally do NOT call grabPiece()
    ASSERT_DEATH({
        arena->returnPiece();
    }, "");
}