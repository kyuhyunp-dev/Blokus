#include "Mock/Resource/MockTextureHolder.hpp"

#include <gtest/gtest.h>
#include "Nodes/BoardNode.hpp" 
#include "Config.hpp"

#include "SFML/Graphics/Texture.hpp"


class BoardNodeTest : public ::testing::Test 
{
protected:
    BoardNodeTest() 
        : textures()           // Initialize textures first
        , boardNode(textures)              // Default construct board
    {
    }

    void SetUp() override {
        // Assume Config::GridSize is 40.f for these calculations
        // We set the board at an offset to test the InverseTransform
        boardNode.setPosition({100.f, 100.f}); 
    }

    MockTextureHolder textures;
    BoardNode boardNode;
};

TEST_F(BoardNodeTest, GridSize)
{
    EXPECT_EQ(Config::GridSize, 40.f);
}

TEST_F(BoardNodeTest, PerfectSnap) 
{
    sf::Vector2f centroid(20.f, 20.f);
    
    // Mouse is at (120, 120) in world space.
    // Board is at (100, 100), so local mouse is (20, 20).
    // localTopLeft = (20, 20) - (20, 20) = (0, 0).
    sf::Vector2f worldMouse(120.f, 120.f);

    sf::Vector2i result = boardNode.getMinSnappedGrid(worldMouse, centroid);

    EXPECT_EQ(result.x, 0);
    EXPECT_EQ(result.y, 0);
}

TEST_F(BoardNodeTest, RoundingUpThreshold) 
{
    sf::Vector2f centroid(0.f, 0.f); 
    
    sf::Vector2f worldMouse(119.9f, 119.9f); 
    sf::Vector2i result = boardNode.getMinSnappedGrid(worldMouse, centroid);
    EXPECT_EQ(result.x, 0);
    EXPECT_EQ(result.y, 0);

    for (int pos = 120; pos < 160; ++pos) {
        worldMouse = { 
            static_cast<float> (pos), 
            static_cast<float> (pos) 
        };

        result = boardNode.getMinSnappedGrid(worldMouse, centroid);
        EXPECT_EQ(result.x, 1);
        EXPECT_EQ(result.y, 1); 
    }

    worldMouse = { 160.f, 160.f };
    result = boardNode.getMinSnappedGrid(worldMouse, centroid);
    EXPECT_EQ(result.x, 2);
    EXPECT_EQ(result.y, 2);
}

TEST_F(BoardNodeTest, NegativeRounding) 
{
    sf::Vector2f centroid(0.f, 0.f);
    
    sf::Vector2f worldMouse(80.f, 80.f); 
    sf::Vector2i result = boardNode.getMinSnappedGrid(worldMouse, centroid);
    EXPECT_EQ(result.x, -1);
    EXPECT_EQ(result.y, -1);

    for (int pos = 80; pos < 100; ++pos) {
        worldMouse = { 
            static_cast<float> (pos) + 0.01f, 
            static_cast<float> (pos) + 0.01f
        };

        result = boardNode.getMinSnappedGrid(worldMouse, centroid);
        EXPECT_EQ(result.x, 0);
        EXPECT_EQ(result.y, 0); 
    }
}