#include "gtest/gtest.h"
#include "gmock/gmock.h"

using ::testing::ThrowsMessage;
using ::testing::HasSubstr;

#include "shared/polyomino_loader.hpp"
#include "shared/polyomino_util.hpp"
#include "polyomino_generated.h"

#include <filesystem>
#include <fstream>


namespace
{
    std::vector<Blokus::PolyominoDefinition> create_test_definitions()
    {
        std::vector<Blokus::PolyominoDefinition> definitions;
        for (int i = 0; i < 3; ++i) {
            Blokus::PolyominoDefinition definition;
            definition.id = i;
            definitions.push_back(definition);
        }
        return definitions;
    }
    
    TEST(RoundTripTest, ExistingFile)
    {   
        // File created
        const std::string file_path = "test_pieces.bin";
        auto definitions = create_test_definitions();
        EXPECT_NO_THROW(Blokus::save_to_binary(file_path, definitions));
        EXPECT_TRUE(std::filesystem::exists(file_path));

        // Load file to buffer
        std::vector<char> buffer;
        EXPECT_NO_THROW({ 
            buffer = Blokus::load_binary_file(file_path); 
        });

        auto piece_library = Blokus::Data::GetPieceLibrary(buffer.data());

        // Access the data
        auto pieces = piece_library->pieces(); 
        EXPECT_EQ(pieces->size(), definitions.size());

        for (unsigned int i = 0; i < pieces->size(); ++i) 
        {
            auto piece = pieces->Get(i);
            EXPECT_EQ(piece->id(), definitions[i].id);
        }
        
        std::filesystem::remove(file_path);
    }

    TEST(SaveToBinaryTest, EmptyFileName)
    {  
        // Empty filename created with no pieces
        const std::string empty_path = "";
        auto definitions = create_test_definitions();

        EXPECT_THAT(
            [&]() { 
                using namespace Blokus;
                save_to_binary(empty_path, definitions); 
            }, 
            ThrowsMessage<std::runtime_error>(HasSubstr("File path is empty"))
        );
    }

    TEST(SaveToBinaryTest, NoPieces)
    { 
        // File created with no pieces
        const std::string file_path = "test_empty.bin";

        EXPECT_THAT(
            [&]() { 
                using namespace Blokus;
                std::vector<PolyominoDefinition> empty_vec;
                save_to_binary(file_path, empty_vec); 
            }, 
            ThrowsMessage<std::runtime_error>(HasSubstr("No pieces to save"))
        );
    }

    TEST(LoadBinaryTest, EmptyFileName)
    {  
        // Empty filename created with no pieces
        const std::string non_existing_path = "non_existing_file.bin";

        EXPECT_THAT(
            [&]() { 
                using namespace Blokus;
                load_binary_file(non_existing_path); 
            }, 
            ThrowsMessage<std::runtime_error>(HasSubstr("Binary file not found"))
        );
    } 

    // Not testing read failure that hasn't been found 
}