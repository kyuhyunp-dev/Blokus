#include "MockTextureHolder.hpp"

#include <gtest/gtest.h>
#include "Nodes/PieceNode.hpp" 
#include "SFML/Graphics/Texture.hpp"
#include <SFML/Window/Context.hpp>


TEST(PieceNodeTest, NumberOfBlocks) 
{
    // Use the real type PieceNode expects
    MockTextureHolder testTextures;
    // Create a blank 1x1 texture in memory (no file needed!)
    sf::Texture dummy;
    bool resized = dummy.resize({1, 1}); 
    EXPECT_TRUE(resized);
 
    // Insert it manually (assuming your ResourceHolder has a 'load' or 'insert' method)
    testTextures.load(Textures::Tiles, "dummy_data");
    // If your load() calls sf::Texture::loadFromFile, you might need a 
    // specific 'insert' method in your ResourceHolder for testing.

    std::array<int, Blokus::PolyominoCount> expectedNumBlocks;
    auto pieces = Blokus::PolyominoGenerator::getData().polyominoById;
    for (int id = 0; id < Blokus::PolyominoCount; ++id)
    {
        expectedNumBlocks[id] = pieces[id].blocks.size();
    }
     
    for (int id = 0; id < Blokus::PolyominoCount; ++id)
    {  
        const Team blue = Team::Blue;
        PieceNode piece(id, blue, testTextures);
        EXPECT_EQ(piece.getChildCount(), expectedNumBlocks[id]);
    } 
}

TEST(PieceNodeTest, IdentityManagement) 
{
    MockTextureHolder testTextures;
    testTextures.load(Textures::Tiles, "dummy_data");
    
    // Constructor test
    PieceNode piece(0, Team::Red, testTextures);
    EXPECT_EQ(piece.getId(), 0);

    // setId() test
    piece.setId(5);
    EXPECT_EQ(piece.getId(), 5); 
}

TEST(PieceNodeTest, Color) 
{
    MockTextureHolder testTextures;
    testTextures.load(Textures::Tiles, "dummy_data");

    PieceNode yellow(1, Team::Yellow, testTextures);
    EXPECT_EQ(yellow.getTeam(), Team::Yellow);

    PieceNode green(1, Team::Green, testTextures);
    EXPECT_EQ(green.getTeam(), Team::Green);

    PieceNode blue(1, Team::Blue, testTextures);
    EXPECT_EQ(blue.getTeam(), Team::Blue);

    PieceNode red(1, Team::Red, testTextures);
    EXPECT_EQ(red.getTeam(), Team::Red);
}

TEST(PieceNodeTest, SetIdUpdatesBlocks) 
{
    MockTextureHolder testTextures;
    testTextures.load(Textures::Tiles, "dummy_data");

    // Start with ID 0 (1 block)
    PieceNode piece(0, Team::Blue, testTextures);
    EXPECT_EQ(piece.getChildCount(), 1);

    // Change to ID 1 (Suppose it has 2 blocks)
    piece.setId(1);
    EXPECT_EQ(piece.getChildCount(), 2); // The test now proves the layout updated!
}