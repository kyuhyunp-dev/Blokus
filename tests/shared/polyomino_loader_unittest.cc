#include <gtest/gtest.h>
#include <filesystem>
#include <fstream>
#include "shared/polyomino_loader.hpp"

namespace Blokus {

class BinaryIOTest : public ::testing::Test 
{
protected:
    const std::string valid_test_file = "test_pieces.bin";
    const std::string empty_test_file = "empty_pieces.bin";
    const std::string missing_file = "does_not_exist.bin";

    void SetUp() override 
    {
        // Ensure no leftover files from previous crashes
        TearDown();
    }

    void TearDown() override 
    {
        if (std::filesystem::exists(valid_test_file)) std::filesystem::remove(valid_test_file);
        if (std::filesystem::exists(empty_test_file)) std::filesystem::remove(empty_test_file);
    }

    // Helper to create a dummy valid definition
    PolyominoDefinition CreateMockDefinition() 
    {
        PolyominoDefinition def;
        Polyomino monomino = { sf::Vector2i{0, 0} };
        Polyomino domino = { sf::Vector2i{0, 0}, sf::Vector2i{0, 1} };

        def.polyomino_by_id[0] = monomino;
        def.id_by_polyomino[monomino] = 0;

        for (int i = 1; i < kPolyominoCount ; ++i) 
        {
            def.polyomino_by_id[i] = domino;
            def.id_by_polyomino[domino] = i;
            def.clockwise_rotated_ids[i] = (i + 1) % 91;
            def.horizontally_reflected_ids[i] = i;
            def.transformed_to_canonical[i] = 0;
        }

        return def;
    }
};

// --- Error Handling Tests ---

TEST_F(BinaryIOTest, ThrowsOnMissingFile) 
{
    EXPECT_THROW(LoadBinaryFile(missing_file), std::runtime_error);
}

TEST_F(BinaryIOTest, ThrowsOnEmptyFile) 
{
    // Create an actual file with 0 bytes
    std::ofstream ofs(empty_test_file);
    ofs.close();

    EXPECT_THROW(ValidateBinaryFile(empty_test_file), std::runtime_error);
}

TEST_F(BinaryIOTest, ThrowsOnEmptyDefinitionSave) 
{
    PolyominoDefinition empty_def;
    // Your code has: if (data.id_by_polyomino.empty()) throw...
    EXPECT_THROW(SaveToBinary(valid_test_file, empty_def), std::runtime_error);
}

// --- Round Trip Tests ---

TEST_F(BinaryIOTest, RoundTripDataIntegrity) 
{
    PolyominoDefinition original = CreateMockDefinition();

    // Save
    ASSERT_NO_THROW(SaveToBinary(valid_test_file, original));
    ASSERT_TRUE(std::filesystem::exists(valid_test_file));

    // Load
    PolyominoDefinition loaded = LoadPieceLibrary(valid_test_file);

    // Verify coordinates for a sample ID
    EXPECT_EQ(loaded.polyomino_by_id[42], original.polyomino_by_id[42]);
    
    // Verify rotation mapping (checking if the -1 fill logic was overwritten correctly)
    EXPECT_EQ(loaded.clockwise_rotated_ids[10], original.clockwise_rotated_ids[10]);
    
    // Verify reverse lookup map was populated
    Polyomino monomino = { sf::Vector2i{0, 0} };
    EXPECT_EQ(loaded.id_by_polyomino.at(monomino), 0); // Last ID assigned in mock
}

} // namespace Blokus