#include "gtest/gtest.h"
#include "gmock/gmock.h"

using ::testing::ThrowsMessage;
using ::testing::HasSubstr;

#include "shared/PolyominoUtil.hpp"
#include "shared/Constants.hpp"

#include <iostream>
#include <array>
#include <queue>


namespace 
{
    TEST(PolyominoGenerationTest, Integrity) {
        // 1. Generate or load the library
        Blokus::PolyominoDefinition library = Blokus::PolyominoGenerator::getData();

        // 2. Verify Bidirectional Mapping
        for (int i = 0; i < Blokus::PolyominoCount; ++i) {
            // get the shape associated with this ID
            const Blokus::Blocks& piece = library.polyominoById[i].blocks;

            // Ensure the piece isn't empty (sanity check for the generator)
            ASSERT_FALSE(piece.empty()) << "Piece at ID " << i << " is empty!";

            // Look up the ID for that specific shape in the map
            auto it = library.idByPolyomino.find(piece);

            // Test A: The piece must actually exist in the map
            ASSERT_NE(it, library.idByPolyomino.end()) 
                << "Shape for ID " << i << " was not found in the reverse lookup map!";

            // Test B: The ID found in the map must match the current loop index
            int mappedId = it->second;
            EXPECT_EQ(mappedId, i) 
                << "Mapping mismatch! ID " << i << " points to a shape, "
                << "but that shape points back to ID " << mappedId;
        }        
    }

    TEST(PolyominoGenerationTest, Rotation)
    {
        Blokus::PolyominoDefinition library = Blokus::PolyominoGenerator::getData();
        
        // We track globally visited IDs to ensure every single ID 
        // is part of a valid rotation set.
        std::vector<bool> visited(Blokus::PolyominoCount, false);

        for (int i = 0; i < Blokus::PolyominoCount; ++i) {
            if (visited[i]) 
                continue;

            // Start a traversal for a new cycle
            std::vector<int> currentCycle;
            int currentId = i;
            
            // Track visited nodes within THIS specific cycle to detect loops
            std::vector<bool> localVisited(Blokus::PolyominoCount, false);

            while (!localVisited[currentId]) {
                localVisited[currentId] = true;
                visited[currentId] = true;
                currentCycle.push_back(currentId);
                
                // Move to the next rotation
                currentId = library.clockwiseRotatedIds[currentId];
            }

            // Logic Check: A valid rotation cycle MUST return to the starting ID
            // and the length must be a divisor of 4 (1, 2, or 4).
            EXPECT_EQ(currentId, i) 
                << "Piece " << i << " rotated into a 'sink' or wrong cycle at ID " << currentId;
            
            EXPECT_TRUE(currentCycle.size() == 1 || currentCycle.size() == 2 || currentCycle.size() == 4)
                << "Piece " << i << " has an invalid cycle length of " << currentCycle.size();
        }
    }

    TEST(PolyominoGenerationTest, Reflection) {
        Blokus::PolyominoDefinition library = Blokus::PolyominoGenerator::getData();

        
        // Globally track which IDs we've already verified
        std::vector<bool> visited(Blokus::PolyominoCount, false);

        for (int i = 0; i < Blokus::PolyominoCount; ++i) 
        {
            if (visited[i]) 
                continue;

            int startId = i;
            int reflectedId = library.horizontallyReflectedIds[startId];

            // Mark both pieces in the pair as visited
            visited[startId] = true;
            visited[reflectedId] = true;

            // Test 1: The "Round Trip" property
            // Reflecting the reflected piece must lead back to the start
            EXPECT_EQ(library.horizontallyReflectedIds[reflectedId], startId)
                << "Reflection cycle broken: ID " << startId << " -> " << reflectedId 
                << " but " << reflectedId << " -> " << library.horizontallyReflectedIds[reflectedId];
        }
    }

   TEST(PolyominoGenerationTest, CanonicalMappingIntegrity) {
        Blokus::PolyominoDefinition library = Blokus::PolyominoGenerator::getData();

        for (int i = 0; i < Blokus::PolyominoCount; ++i) {
            int canonicalId = library.transformedToCanonical[i];

            // 1. Range Check: Must be between 0 and 20 (the 21 free polyominoes)
            EXPECT_GE(canonicalId, 0) 
                << "ID " << i << " maps to an invalid negative canonical ID.";
            EXPECT_LT(canonicalId, 21) 
                << "ID " << i << " maps to " << canonicalId 
                << ", which is outside the canonical range (0-20).";

            // 2. Identity Check: If the piece is already a canonical piece, it should map to itself
            if (i < 21) {
                EXPECT_EQ(canonicalId, i) 
                    << "Canonical piece " << i << " should map to itself, but maps to " << canonicalId;
            }
        }
    }

    TEST(PolyominoGenerationTest, O_Tetromino) 
    {
        Blokus::PolyominoDefinition library = Blokus::PolyominoGenerator::getData();

        // Create a 2x2 Square (O-Tetromino)
        // [(0,0), (1,0), (0,1), (1,1)]
        Blokus::Blocks square = {
            {0, 0}, {1, 0},
            {0, 1}, {1, 1}
        };
        int squareId = library.idByPolyomino.at(square);

        // 1. Test Rotation
        int rotated = library.clockwiseRotatedIds.at(squareId);
        // After rotation and normalization, it should be identical
        EXPECT_EQ(rotated, squareId) << "Square rotated into a different shape!";

        // 2. Test Reflection
        int reflected = library.horizontallyReflectedIds.at(squareId);
        EXPECT_EQ(reflected, squareId) << "Square reflected into a different shape!";

        EXPECT_EQ(library.transformedToCanonical.at(rotated), squareId);
        EXPECT_EQ(library.transformedToCanonical.at(reflected), squareId);
    }

    TEST(PolyominoGenerationTest, P_Pentomino) 
    {
        Blokus::PolyominoDefinition library = Blokus::PolyominoGenerator::getData();


        // 1. Find the ID for the P-Pentomino
        // [(0,0), (1,0), (0,1), (1,1), (0,2)]
        Blokus::Blocks pShape = {{0, 0}, {1, 0}, {0, 1}, {1, 1}, {0, 2}};
        ASSERT_TRUE(library.idByPolyomino.count(pShape)) << "P-Pentomino not found in library!";
        
        int startId = library.idByPolyomino.at(pShape);
        int canonicalId = library.transformedToCanonical[startId];

        // 2. BFS Setup
        std::queue<int> q;
        std::unordered_set<int> visited;

        q.push(startId);
        visited.insert(startId);

        while (!q.empty()) {
            int current = q.front();
            q.pop();

            // Every ID we reach MUST belong to the same canonical piece
            EXPECT_EQ(library.transformedToCanonical[current], canonicalId)
                << "BFS reached ID " << current << " which belongs to a different canonical piece!";

            // Define neighbors based on the two types of transformations
            std::array<int, 2> neighbors = {
                library.clockwiseRotatedIds[current],
                library.horizontallyReflectedIds[current]
            };

            for (int neighbor : neighbors) {
                if (visited.find(neighbor) == visited.end()) {
                    visited.insert(neighbor);
                    q.push(neighbor);
                }

                EXPECT_EQ(library.transformedToCanonical.at(neighbor), startId);
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
            EXPECT_FALSE(generator.isCanonical(i));
        } 

        for (int i = 0; i < 21; ++i)
        {
            EXPECT_TRUE(generator.isCanonical(i));
        }

        for (int i = 21; i < 50; ++i)
        {
            EXPECT_FALSE(generator.isCanonical(i));
        }
    }

    TEST(PolyominoDecryptionTest, Normalization)
    {
        Blokus::PolyominoGenerator generator;
        for (int i = -10; i < 0; ++i)
        {
            EXPECT_FALSE(generator.isNormalized(i));
        } 

        for (int i = 0; i < 91; ++i)
        {
            EXPECT_TRUE(generator.isNormalized(i));
        }

        for (int i = 91; i < 100; ++i)
        {
            EXPECT_FALSE(generator.isNormalized(i));
        }
    }

    TEST(PolyominoSensorTest, MonominoSensorCalculation) {
        Blokus::PolyominoDefinition library = Blokus::PolyominoGenerator::getData();

        // Create a 1x1 piece at (0,0)
        Blokus::Blocks monomino = { {0, 0} };
        
        // Assume you have a function that generates sensors from blocks
        Blokus::Polyomino p = library.polyominoById[library.idByPolyomino.at(monomino)];

        // Verify edges(Cardinal directions)
        EXPECT_EQ(p.sensors.edges.size(), 4);
        EXPECT_TRUE(std::find(p.sensors.edges.begin(), p.sensors.edges.end(), sf::Vector2i(1, 0)) != p.sensors.edges.end());
        EXPECT_TRUE(std::find(p.sensors.edges.begin(), p.sensors.edges.end(), sf::Vector2i(0, 1)) != p.sensors.edges.end());

        // Verify corners(Diagonal directions)
        EXPECT_EQ(p.sensors.corners.size(), 4);
        EXPECT_TRUE(std::find(p.sensors.corners.begin(), p.sensors.corners.end(), sf::Vector2i(1, 1)) != p.sensors.corners.end());
    }

    TEST(PolyominoSensorTest, RotationConsistency) {
        Blokus::PolyominoDefinition library = Blokus::PolyominoGenerator::getData();
        // Use an L-shape (asymmetric)
        Blokus::Blocks lBlocks = { {0,0}, {0,1}, {0,2}, {1,2} };
        int id = library.idByPolyomino.at(lBlocks);

        int rotatedId = library.clockwiseRotatedIds[id];
        Blokus::Polyomino rotatedObject = library.polyominoById.at(rotatedId);

        // Manually calculated rotated edges
        Blokus::Blocks expectedEdges = { {-1, 0}, {-1, 1}, {0, -1}, {0, 2},
                                {1, -1}, {1, 1}, {2, -1}, {2, 1}, {3, 0}};
        EXPECT_EQ(rotatedObject.sensors.edges, expectedEdges);

        // Manually calculated rotated corners
        Blokus::Blocks expectedCorners = { {-1, -1}, {-1, 2}, {1, 2}, 
                                {3, -1}, {3, 1} };
        EXPECT_EQ(rotatedObject.sensors.corners, expectedCorners);
    }

    TEST(PolyominoSensorTest, ReflectionConsistency) {
        Blokus::PolyominoDefinition library = Blokus::PolyominoGenerator::getData();
        // Use an L-shape (asymmetric)
        Blokus::Blocks lBlocks = { {0,0}, {0,1}, {0,2}, {1,2} };
        int id = library.idByPolyomino.at(lBlocks);

        int reflectedId = library.horizontallyReflectedIds[id];
        Blokus::Polyomino reflectedObject = library.polyominoById.at(reflectedId);

        // Manually calculated rotated edges
        Blokus::Blocks expectedEdges = { {-1, 2}, {0, 0}, {0, 1}, {0, 3},
                                {1, -1}, {1, 3}, {2, 0}, {2, 1}, {2, 2}};
        EXPECT_EQ(reflectedObject.sensors.edges, expectedEdges);

        // Manually calculated rotated corners
        Blokus::Blocks expectedCorners = { {-1, 1}, {-1, 3}, {0, -1}, 
                                {2, -1}, {2, 3} };
        EXPECT_EQ(reflectedObject.sensors.corners, expectedCorners);
    }

    TEST(PolyominoSensorTest, SensorExclusion) {
        Blokus::PolyominoDefinition library = Blokus::PolyominoGenerator::getData();

        for (int i = 0; i < Blokus::PolyominoCount; ++i) {
            const auto& p = library.polyominoById[i];
            
            // Ensure no Edge is inside the Blocks set
            for (const auto& edge : p.sensors.edges) {
                EXPECT_EQ(p.blocks.find(edge), p.blocks.end()) 
                    << "ID " << i << " has an edge sensor overlapping a block at " << edge.x << "," << edge.y;
            }

            // Ensure no Corner is an Edge (Edges take precedence in Blokus rules)
            for (const auto& corner : p.sensors.corners) {
                auto it = std::find(p.blocks.begin(), p.blocks.end(), corner);
                EXPECT_EQ(it, p.blocks.end());

                it = std::find(p.sensors.edges.begin(), p.sensors.edges.end(), corner);
                EXPECT_EQ(it, p.sensors.edges.end()) 
                    << "ID " << i << " has a corner sensor overlapping an edge sensor at " << corner.x << "," << corner.y;
            }
        }
    }

    TEST(PolyominoSensorTest, FPentominoAdjacency) {
        Blokus::PolyominoDefinition library = Blokus::PolyominoGenerator::getData();
        // F-Pentomino: A very 'crowded' piece
        Blokus::Blocks fBlocks = { {1,0}, {2,0}, {0,1}, {1,1}, {1,2} };
        int id = library.idByPolyomino.at(fBlocks);
        Blokus::Polyomino fPolyomino = library.polyominoById[id];

        // Test: (1,1) is the center block. 
        // Its neighbors are (1,0), (1,2), (0,1), (2,1).
        // Three of those are BLOCKS. Therefore, they cannot be SENSORS.
        for (const auto& block : fPolyomino.blocks) {
            auto edgeIt = std::find(fPolyomino.sensors.edges.begin(), fPolyomino.sensors.edges.end(), block);
            auto cornerIt = std::find(fPolyomino.sensors.corners.begin(), fPolyomino.sensors.corners.end(), block);
            
            EXPECT_EQ(edgeIt, fPolyomino.sensors.edges.end()) << "Block at " << block.x << "," << block.y << " is also an edge!";
            EXPECT_EQ(cornerIt, fPolyomino.sensors.corners.end()) << "Block at " << block.x << "," << block.y << " is also a corner!";
        }
    }

    TEST(PolyominoSensorTest, SensorBoundingBox) {
        Blokus::PolyominoDefinition library = Blokus::PolyominoGenerator::getData();
        Blokus::Blocks monomino = { {0,0} };
        Blokus::Polyomino p = library.polyominoById[library.idByPolyomino[monomino]];

        int minX = 0, maxX = 0, minY = 0, maxY = 0;
        
        // Check all sensors (edges and corners)
        auto check_bounds = [&](const Blokus::Blocks& sensors) {
            for(const auto& v : sensors) {
                minX = std::min(minX, v.x);
                maxX = std::max(maxX, v.x);
                minY = std::min(minY, v.y);
                maxY = std::max(maxY, v.y);
            }
        };

        check_bounds(p.sensors.edges);
        check_bounds(p.sensors.corners);

        // For a monomino at (0,0), sensors should span from (-1,-1) to (1,1)
        EXPECT_EQ(minX, -1);
        EXPECT_EQ(maxX, 1);
        EXPECT_EQ(minY, -1);
        EXPECT_EQ(maxY, 1);
    }
}