#include "gtest/gtest.h"
#include "gmock/gmock.h"

using ::testing::ThrowsMessage;
using ::testing::HasSubstr;

#include "shared/polyomino_util.hpp"
#include "shared/constants.hpp"
#include "polyomino_generated.h"

#include <iostream>
#include <array>



namespace 
{
    TEST(PolyominoGenerationTest, Integrity) {
        // 1. Generate or load the library
        Blokus::PolyominoGenerator generator;
        const auto& library = generator.GetData();

        // 2. Verify Bidirectional Mapping
        for (int i = 0; i < Blokus::kPolyominoCount; ++i) {
            // Get the shape associated with this ID
            const Blokus::Polyomino& piece = library.polyomino_by_id[i];

            // Ensure the piece isn't empty (sanity check for the generator)
            ASSERT_FALSE(piece.empty()) << "Piece at ID " << i << " is empty!";

            // Look up the ID for that specific shape in the map
            auto it = library.id_by_polyomino.find(piece);

            // Test A: The piece must actually exist in the map
            ASSERT_NE(it, library.id_by_polyomino.end()) 
                << "Shape for ID " << i << " was not found in the reverse lookup map!";

            // Test B: The ID found in the map must match the current loop index
            int mapped_id = it->second;
            EXPECT_EQ(mapped_id, i) 
                << "Mapping mismatch! ID " << i << " points to a shape, "
                << "but that shape points back to ID " << mapped_id;
        }
    }

    TEST(PolyominoGenerationTest, Rotation)
    {
        Blokus::PolyominoDefinition library = Blokus::PolyominoGenerator::GetData();
        
        // We track globally visited IDs to ensure every single ID 
        // is part of a valid rotation set.
        std::vector<bool> visited(Blokus::kPolyominoCount, false);


        for (int i = 0; i < Blokus::kPolyominoCount; ++i) {
            if (visited[i]) 
                continue;

            // Start a traversal for a new cycle
            std::vector<int> current_cycle;
            int current_id = i;
            
            // Track visited nodes within THIS specific cycle to detect loops
            std::vector<bool> local_visited(Blokus::kPolyominoCount, false);

            while (!local_visited[current_id]) {
                local_visited[current_id] = true;
                visited[current_id] = true;
                current_cycle.push_back(current_id);
                
                // Move to the next rotation
                current_id = library.clockwise_rotated_ids[current_id];
            }

            // Logic Check: A valid rotation cycle MUST return to the starting ID
            // and the length must be a divisor of 4 (1, 2, or 4).
            EXPECT_EQ(current_id, i) 
                << "Piece " << i << " rotated into a 'sink' or wrong cycle at ID " << current_id;
            
            EXPECT_TRUE(current_cycle.size() == 1 || current_cycle.size() == 2 || current_cycle.size() == 4)
                << "Piece " << i << " has an invalid cycle length of " << current_cycle.size();
        }
    }

    TEST(PolyominoGenerationTest, Reflection) {
        Blokus::PolyominoGenerator generator;
        const auto& library = generator.GetData();
        
        // Globally track which IDs we've already verified
        std::vector<bool> visited(Blokus::kPolyominoCount, false);

        for (int i = 0; i < Blokus::kPolyominoCount; ++i) 
        {
            if (visited[i]) 
                continue;

            int start_id = i;
            int reflected_id = library.horizontally_reflected_ids[start_id];

            // Mark both pieces in the pair as visited
            visited[start_id] = true;
            visited[reflected_id] = true;

            // Test 1: The "Round Trip" property
            // Reflecting the reflected piece must lead back to the start
            EXPECT_EQ(library.horizontally_reflected_ids[reflected_id], start_id)
                << "Reflection cycle broken: ID " << start_id << " -> " << reflected_id 
                << " but " << reflected_id << " -> " << library.horizontally_reflected_ids[reflected_id];
        }
    }

   TEST(PolyominoGenerationTest, CanonicalMappingIntegrity) {
        Blokus::PolyominoGenerator generator;
        const auto& library = generator.GetData();

        for (int i = 0; i < Blokus::kPolyominoCount; ++i) {
            int canonical_id = library.transformed_to_canonical[i];

            // 1. Range Check: Must be between 0 and 20 (the 21 free polyominoes)
            EXPECT_GE(canonical_id, 0) 
                << "ID " << i << " maps to an invalid negative canonical ID.";
            EXPECT_LT(canonical_id, 21) 
                << "ID " << i << " maps to " << canonical_id 
                << ", which is outside the canonical range (0-20).";

            // 2. Identity Check: If the piece is already a canonical piece, it should map to itself
            if (i < 21) {
                EXPECT_EQ(canonical_id, i) 
                    << "Canonical piece " << i << " should map to itself, but maps to " << canonical_id;
            }
        }
    }

    TEST(PolyominoGenerationTest, O_Tetromino) 
    {
        Blokus::PolyominoGenerator generator;
        auto &library = generator.GetData();

        // Create a 2x2 Square (O-Tetromino)
        // [(0,0), (1,0), (0,1), (1,1)]
        Blokus::Polyomino square = {
            {0, 0}, {1, 0},
            {0, 1}, {1, 1}
        };
        int square_id = library.id_by_polyomino.at(square);

        // 1. Test Rotation
        int rotated = library.clockwise_rotated_ids.at(square_id);
        // After rotation and normalization, it should be identical
        EXPECT_EQ(rotated, square_id) << "Square rotated into a different shape!";

        // 2. Test Reflection
        int reflected = library.horizontally_reflected_ids.at(square_id);
        EXPECT_EQ(reflected, square_id) << "Square reflected into a different shape!";

        EXPECT_EQ(library.transformed_to_canonical.at(rotated), square_id);
        EXPECT_EQ(library.transformed_to_canonical.at(reflected), square_id);
    }

    TEST(PolyominoGenerationTest, P_Pentomino) 
    {
        Blokus::PolyominoGenerator generator;
        const auto& library = generator.GetData();

        // 1. Find the ID for the P-Pentomino
        // [(0,0), (1,0), (0,1), (1,1), (0,2)]
        Blokus::Polyomino p_shape = {{0, 0}, {1, 0}, {0, 1}, {1, 1}, {0, 2}};
        ASSERT_TRUE(library.id_by_polyomino.count(p_shape)) << "P-Pentomino not found in library!";
        
        int start_id = library.id_by_polyomino.at(p_shape);
        int canonical_id = library.transformed_to_canonical[start_id];

        // 2. BFS Setup
        std::queue<int> q;
        std::unordered_set<int> visited;

        q.push(start_id);
        visited.insert(start_id);

        while (!q.empty()) {
            int current = q.front();
            q.pop();

            // Every ID we reach MUST belong to the same canonical piece
            EXPECT_EQ(library.transformed_to_canonical[current], canonical_id)
                << "BFS reached ID " << current << " which belongs to a different canonical piece!";

            // Define neighbors based on the two types of transformations
            std::array<int, 2> neighbors = {
                library.clockwise_rotated_ids[current],
                library.horizontally_reflected_ids[current]
            };

            for (int neighbor : neighbors) {
                if (visited.find(neighbor) == visited.end()) {
                    visited.insert(neighbor);
                    q.push(neighbor);
                }

                EXPECT_EQ(library.transformed_to_canonical.at(neighbor), start_id);
            }
        }

        // 3. Final Assertion
        // For a P-Pentomino, the BFS must find exactly 8 unique IDs.
        EXPECT_EQ(visited.size(), 8) 
            << "P-Pentomino BFS should have found 8 orientations, but found " << visited.size();
    }

    TEST(PolyominoDecryptionTest, Canonicalization)
    {
        Blokus::PolyominoGenerator generator;
        for (int i = -10; i < 0; ++i)
        {
            EXPECT_FALSE(generator.IsCanonical(i));
        } 

        for (int i = 0; i < 21; ++i)
        {
            EXPECT_TRUE(generator.IsCanonical(i));
        }

        for (int i = 21; i < 50; ++i)
        {
            EXPECT_FALSE(generator.IsCanonical(i));
        }
    }

    TEST(PolyominoDecryptionTest, Normalization)
    {
        Blokus::PolyominoGenerator generator;
        for (int i = -10; i < 0; ++i)
        {
            EXPECT_FALSE(generator.IsNormalized(i));
        } 

        for (int i = 0; i < 91; ++i)
        {
            EXPECT_TRUE(generator.IsNormalized(i));
        }

        for (int i = 91; i < 100; ++i)
        {
            EXPECT_FALSE(generator.IsNormalized(i));
        }
    }
}