#include <gtest/gtest.h>
#include <filesystem>
#include <fstream>
#include "shared/PolyominoLoader.hpp"

namespace Blokus {

class BinaryIOTest : public ::testing::Test 
{
protected:
    const std::string validTestFile = "test_pieces.bin";
    const std::string emptyTestFile = "empty_pieces.bin";
    const std::string missingFile = "does_not_exist.bin";

    void SetUp() override 
    {
    }

    void TearDown() override 
    {
        if (std::filesystem::exists(validTestFile)) std::filesystem::remove(validTestFile);
        if (std::filesystem::exists(emptyTestFile)) std::filesystem::remove(emptyTestFile);
    }

    // Helper to create a dummy valid definition
    PolyominoDefinition createMockDefinition() 
    {
        PolyominoDefinition def;
        Blocks monomino = { sf::Vector2i{0, 0} };

        Polyomino piece (monomino);
        piece.sensors.edges.insert({-1, 0});
        piece.sensors.corners.insert({0, -1});

        Blocks domino = { sf::Vector2i{0, 0}, sf::Vector2i{0, 1} };

        def.polyominoById[0] = piece;
        def.idByPolyomino[monomino] = 0;

        for (int i = 1; i < PolyominoCount ; ++i) 
        {
            def.polyominoById[i] = domino;
            def.idByPolyomino[domino] = i;
            def.clockwiseRotatedIds[i] = (i + 1) % 91;
            def.horizontallyReflectedIds[i] = i;
            def.transformedToCanonical[i] = 0;
        }

        return def;
    }
};

// --- Error Handling Tests ---

TEST_F(BinaryIOTest, ThrowsOnMissingFile) 
{
    EXPECT_THROW(loadBinaryFile(missingFile), std::runtime_error);
}

TEST_F(BinaryIOTest, ThrowsOnEmptyFile) 
{
    // Create an actual file with 0 bytes
    std::ofstream ofs(emptyTestFile);
    ofs.close();

    EXPECT_THROW(validateBinaryFile(emptyTestFile), std::runtime_error);
}

TEST_F(BinaryIOTest, ThrowsOnEmptyDefinitionSave) 
{
    PolyominoDefinition empty_def;
    // Your code has: if (data.idByPolyomino.empty()) throw...
    EXPECT_THROW(saveToBinary(validTestFile, empty_def), std::runtime_error);
}

// --- Round Trip Tests ---

TEST_F(BinaryIOTest, RoundTripDataIntegrity) 
{
    PolyominoDefinition original = createMockDefinition();

    // Save 
    ASSERT_NO_THROW(saveToBinary(validTestFile, original));
    ASSERT_TRUE(std::filesystem::exists(validTestFile));

    // Load
    PolyominoDefinition loaded = loadPieceLibrary(validTestFile);

    // Verify coordinates for a sample ID eXPECT_EQ(loaded.polyominoById[42].blocks, original.polyominoById[42].blocks);
    
    // Verify rotation mapping (checking if the -1 fill logic was overwritten correctly)
    EXPECT_EQ(loaded.clockwiseRotatedIds[10], original.clockwiseRotatedIds[10]);
    
    // Verify reverse lookup map was populated
    Blocks monomino = { sf::Vector2i{0, 0} };
    EXPECT_EQ(loaded.idByPolyomino.at(monomino), 0); // Last ID assigned in mock
}

TEST_F(BinaryIOTest, SensorBinaryPersistence) {
    PolyominoDefinition original = createMockDefinition();
    
    // Save 
    ASSERT_NO_THROW(saveToBinary(validTestFile, original));

    // Load back
    PolyominoDefinition loaded = loadPieceLibrary(validTestFile);
    
    // Check a sample piece (e.g., ID 10)
    EXPECT_FALSE(loaded.polyominoById[0].sensors.edges.empty());
    EXPECT_EQ(loaded.polyominoById[0].sensors.edges.size(), 1);
    EXPECT_TRUE(loaded.polyominoById[0].sensors.edges.find({-1, 0})
                != loaded.polyominoById[0].sensors.edges.end());
    
    EXPECT_EQ(loaded.polyominoById[0].sensors.corners.size(), 1);
    EXPECT_TRUE(loaded.polyominoById[0].sensors.corners.find({0, -1})
                != loaded.polyominoById[0].sensors.corners.end());
}

} // namespace Blokus