#include "shared/polyomino_util.hpp"

#include <vector>
#include <algorithm>
#include <utility>
#include <cassert>
#include <iostream>

namespace Blokus
{

PolyominoGenerator::PolyominoGenerator()
{
    // BFS to generate all polyominoes up to kMaxBlocks
    std::deque<Polyomino> source_canonical_pieces;
    source_canonical_pieces.push_back({sf::Vector2i{0, 0}});

    // Store monomino into piece_library_
    piece_library_.canonical_ids.insert(1);
    piece_library_.clockwise_rotated_ids[1] = 1;
    piece_library_.horizontally_reflected_ids[1] = 1;
    piece_library_.transformed_to_canonical[1] = 1;

    // Start BFS search
    size_t piece_size = 1;
    while (!source_canonical_pieces.empty() && piece_size < kMaxBlocks)
    { // Generate all polyominoes of piece_size
        size_t piece_size_count = source_canonical_pieces.size();
        for (size_t i = 0; i < piece_size_count; ++i)
        {
            Polyomino current_piece = std::move(source_canonical_pieces.front());
            source_canonical_pieces.pop_front();

            for (const sf::Vector2i block : current_piece)
            { // For every block, try adding another block next to it
                sf::Vector2i next_block = block;
                for (const auto dir : kCardinalOffsets)
                {
                    next_block += dir;

                    if (current_piece.find(next_block) == current_piece.end())
                    { // Not part of the parent
                        current_piece.insert(next_block);

                        RegisterUniquePolyominoes(source_canonical_pieces, 
                        current_piece);;

                        current_piece.erase(next_block);
                    }
                    
                    next_block -= dir;
                }
            }
        }
        
        
        ++piece_size;
    }

} // PolyominoGenerator::PolyominoGenerator

void PolyominoGenerator::RegisterUniquePolyominoes(
    std::deque<Polyomino>& source_canonical_pieces, const Polyomino &piece)
{
    // Compute id of normalized piece
    std::set<int> found_ids;
    int original_id;
    if (!RegisterNewPolyomino(piece, found_ids, original_id))
    {
        return;
    }

    // Compute rotated, reflected id
    int reflected_id, rotated_id;
    Polyomino reflected, rotated = piece;
    for (int rotation = 0 ;rotation < 4; ++rotation)
    {
        reflected = ReflectHorizontally(rotated);
        if (RegisterNewPolyomino(reflected, found_ids, reflected_id))
        {
            piece_library_.horizontally_reflected_ids[original_id] = reflected_id;
        }

        rotated = RotateClockwise(rotated);
        if (RegisterNewPolyomino(rotated, found_ids, rotated_id))
        {
            piece_library_.clockwise_rotated_ids[original_id] = rotated_id;
        }
        
        original_id = rotated_id;
    }
  
    // Record other information
    RecordIds(source_canonical_pieces, found_ids);
} // RegisterUniquePolyominoes

bool PolyominoGenerator::RegisterNewPolyomino(const Polyomino& piece, 
    std::set<int>& found_ids, int& id)
{ // returns true if piece was new
    id = Normalize(piece);
    if (piece_library_.transformed_to_canonical.find(id) 
        != piece_library_.transformed_to_canonical.end())
    { // piece has been recorded
        return false;
    }

    found_ids.insert(id);
    return true;
}

Polyomino PolyominoGenerator::RotateClockwise(const Polyomino& piece) const
{
    Polyomino rotated_piece;
    for (const sf::Vector2i block : piece)
    { // rotate across the origin : (-y, x)
        rotated_piece.insert({ -block.y, block.x });
    }

    return rotated_piece;
}

Polyomino PolyominoGenerator::ReflectHorizontally(const Polyomino& piece) const
{
    Polyomino reflected_piece;
    for (sf::Vector2i block : piece)
    { // reflect horizontally: (-x, y)
        reflected_piece.insert({ -block.x, block.y });
    }

    return reflected_piece;
}

void PolyominoGenerator::RecordIds(
    std::deque<Polyomino>& source_canonical_pieces,
    const std::set<int>& found_ids)
{
    if (found_ids.empty())
    {
        return;
    }

    int canonical_id = *found_ids.begin();
    source_canonical_pieces.push_back(Decode(canonical_id));
    piece_library_.canonical_ids.insert(canonical_id);

    for (int id: found_ids)
    {    
        piece_library_.transformed_to_canonical[id] = canonical_id;
    }
}

sf::Vector2i PolyominoGenerator::GetOrigin(const Polyomino& piece) const
{ // Find minimum x and y (origin)
     sf::Vector2i origin ( {INT32_MAX, INT32_MAX} );

    for (const sf::Vector2i block : piece)
    {
        origin.x = std::min(origin.x, block.x);
        origin.y = std::min(origin.y, block.y);
    }

    return origin;
}

int PolyominoGenerator::Normalize(const Polyomino& piece) const
{ // Because Polyomino is sorted, linearly normalize
    sf::Vector2i origin = GetOrigin(piece);

    int id = 0;
    int base = (kMaxBlocks * kMaxBlocks + 1); // (4, 5) creates 25
    for (const sf::Vector2i block : piece) 
    { // Encoding with offset 1
        id *= base;
        id += Encode(block - origin);
    }

    return id;
}

int PolyominoGenerator::Encode(const sf::Vector2i& position) const
{ // Use Base-5 Encoding with offset 1
    return position.x * kMaxBlocks + position.y + 1;
} 

bool PolyominoGenerator::IsNormalized(int id) const
{
    return !(piece_library_.transformed_to_canonical.count(id) == 0);
}


Polyomino PolyominoGenerator::Decode(int id) const
{
    Polyomino piece;
    int remaining_id = id;

    while (remaining_id > 0) 
    {
        int val = (remaining_id % (kMaxBlocks * kMaxBlocks + 1)) - 1; 
        remaining_id /= (kMaxBlocks * kMaxBlocks + 1);

        if (val >= 0) 
        {
            int x = val / kMaxBlocks;
            int y = val % kMaxBlocks;

            piece.insert({ x, y });
        }
    }

    return piece;
}
} // namespace Blokus
