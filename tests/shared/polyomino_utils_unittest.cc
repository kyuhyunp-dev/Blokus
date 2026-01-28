#include "gtest/gtest.h"
#include "gmock/gmock.h"

using ::testing::ThrowsMessage;
using ::testing::HasSubstr;

#include "shared/polyomino_util.hpp"
#include "shared/constants.hpp"
#include "polyomino_generated.h"

#include <iostream>



namespace 
{
    int GetNumBlocks(int id) 
    { 
        int numBlocks = 0;
        int remaining_id = id;

        while (remaining_id > 0) 
        {
            remaining_id /= 26;
            ++numBlocks;
        }

        return numBlocks;
    }
    
    TEST(PolyominoGenerationTest, CanonicalIdsCount)
    {   
        const auto library = Blokus::PolyominoGenerator::GetData();
        const std::set<int> &canonical_ids = library.canonical_ids;
        
        EXPECT_EQ(21, canonical_ids.size());

        std::unordered_map<int, int> numPiecesByNumBlocks;
        for (int x : canonical_ids)
        {
            ++numPiecesByNumBlocks[GetNumBlocks(x)];
        }

        EXPECT_EQ(1, numPiecesByNumBlocks[1]);
        EXPECT_EQ(1, numPiecesByNumBlocks[2]);
        EXPECT_EQ(2, numPiecesByNumBlocks[3]);
        EXPECT_EQ(5, numPiecesByNumBlocks[4]);
        EXPECT_EQ(12, numPiecesByNumBlocks[5]);
    }

    TEST(PolyominoGenerationTest, Rotation)
    {
        const auto library = Blokus::PolyominoGenerator::GetData();
        
        // O tetromino
        EXPECT_EQ(19091, library.clockwise_rotated_ids.at(19091));

        // domino
        EXPECT_EQ(32, library.clockwise_rotated_ids.at(28));
        EXPECT_EQ(28, library.clockwise_rotated_ids.at(32));

        // L tromino        
        EXPECT_EQ(839, library.clockwise_rotated_ids.at(734));
        EXPECT_EQ(1515, library.clockwise_rotated_ids.at(839));
        EXPECT_EQ(735, library.clockwise_rotated_ids.at(1515));
        EXPECT_EQ(734, library.clockwise_rotated_ids.at(735));
    }

    TEST(PolyominoGenerationTest, Reflection)
    {
        const auto library = Blokus::PolyominoGenerator::GetData();
       
        // domino
        EXPECT_EQ(28, library.horizontally_reflected_ids.at(28));
        EXPECT_EQ(32, library.horizontally_reflected_ids.at(32));

        // L Tromino
        EXPECT_EQ(839, library.horizontally_reflected_ids.at(734));
        EXPECT_EQ(734, library.horizontally_reflected_ids.at(839));
        EXPECT_EQ(735, library.horizontally_reflected_ids.at(1515));
        EXPECT_EQ(1515, library.horizontally_reflected_ids.at(735));
    }

    TEST(PolyominoGenerationTest, TransformedId)
    {
        const auto library = Blokus::PolyominoGenerator::GetData();
        
        // O tetromino
        EXPECT_EQ(19091, library.transformed_to_canonical.at(19091));

        // domino
        EXPECT_EQ(28, library.transformed_to_canonical.at(28));
        EXPECT_EQ(28, library.transformed_to_canonical.at(32));
        
        // L Tromino
        EXPECT_EQ(734, library.transformed_to_canonical.at(734));
        EXPECT_EQ(734, library.transformed_to_canonical.at(735));
        EXPECT_EQ(734, library.transformed_to_canonical.at(839));
        EXPECT_EQ(734, library.transformed_to_canonical.at(1515));
    }

    TEST(PolyominoDecryptionTest, Normalization)
    {
        Blokus::PolyominoGenerator generator;

        int MAX_SIZE = pow(26, 5);
        int count = 0;
        for (int id = 0; id < MAX_SIZE; ++id)
        {
            if (generator.IsNormalized(id))
            {
                ++count;
            }
        }
        EXPECT_EQ(91, count);

        EXPECT_FALSE(generator.IsNormalized(0));
        EXPECT_FALSE(generator.IsNormalized(2));

        EXPECT_TRUE(generator.IsNormalized(734));
        EXPECT_TRUE(generator.IsNormalized(1515));
    }

    TEST(PolyominoDecryptionTest, RoundTripIntegrity)
    {
        const auto& library = Blokus::PolyominoGenerator::GetData();
        
        Blokus::PolyominoGenerator generator;
        for (int canon_id : library.canonical_ids)
        {
            Blokus::Polyomino piece;
            EXPECT_NO_THROW({
                piece = generator.Decode(canon_id);
            });
        }
    }

    TEST(PolyominoDecryptionTest, RandomValues)
    {
        Blokus::PolyominoGenerator generator;
        
        // One Monomino
        Blokus::Polyomino monomino;
        EXPECT_NO_THROW({        
            monomino = generator.Decode(1);
        });

        EXPECT_EQ(1, monomino.size());

        Blokus::Polyomino piece;
        EXPECT_NO_THROW({      
            piece = generator.Decode(494346);
        });
        // Verifies (0, 0), (0, 1), (0, 2), (1, 1), (1, 2)
        EXPECT_EQ(1, piece.count({0, 0}));
        EXPECT_EQ(1, piece.count({0, 1}));
        EXPECT_EQ(1, piece.count({0, 2}));
        EXPECT_EQ(1, piece.count({1, 1}));
        EXPECT_EQ(1, piece.count({1, 2}));

        EXPECT_NO_THROW({      
            piece = generator.Decode(1938309);
        });

        EXPECT_EQ(5, piece.size());
    }
    
}