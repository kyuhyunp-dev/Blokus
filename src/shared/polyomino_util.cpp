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
    std::deque<Blocks> source_canonical_pieces;
    source_canonical_pieces.push_back({sf::Vector2i{0, 0}});

    // Store monomino into piece_library_
    piece_library_.id_by_polyomino[source_canonical_pieces.front()] = 0;
    piece_library_.polyomino_by_id[0] = source_canonical_pieces.front();
    AddSensors(0);

    piece_library_.clockwise_rotated_ids[0] = 0;
    piece_library_.horizontally_reflected_ids[0] = 0;
    for (int id = 0; id < kCanonicalCount; ++id)
    {
        piece_library_.transformed_to_canonical[id] = id;
    }

    // Start BFS search
    size_t piece_size = 1;
    while (!source_canonical_pieces.empty() && piece_size < kMaxBlocks)
    { // Generate all polyominoes of piece_size
        size_t piece_size_count = source_canonical_pieces.size();
        for (size_t i = 0; i < piece_size_count; ++i)
        {
            Blocks current_piece = std::move(source_canonical_pieces.front());
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
    std::deque<Blocks>& source_canonical_pieces, const Blocks &piece)
{
    // Compute normalized form of the piece
    Blocks current = Normalize(piece);
    if (piece_library_.id_by_polyomino.find(current) 
        != piece_library_.id_by_polyomino.end())
    {
        return;
    }
    
    // Compute rotated, reflected polyominoes
    std::set<Blocks, PolyominoComparator> found_polyominoes;
    std::map<Blocks, Blocks, PolyominoComparator> rotated_polyomino;
    std::map<Blocks, Blocks, PolyominoComparator> reflected_polyomino;
    SearchTransformation(current, found_polyominoes, 
        rotated_polyomino, reflected_polyomino);

    if (found_polyominoes.empty()) 
    {
        return;
    }

    Record(source_canonical_pieces, found_polyominoes, rotated_polyomino, reflected_polyomino);
} // RegisterUniquePolyominoes

void PolyominoGenerator::SearchTransformation(const Blocks& piece,
    std::set<Blocks, PolyominoComparator>& found_polyominoes,
    std::map<Blocks, Blocks, PolyominoComparator> &rotated_polyomino,
    std::map<Blocks, Blocks, PolyominoComparator> &reflected_polyomino) const
{
    std::vector<Blocks> pieces {piece};

    while (!pieces.empty())
    {
        Blocks current = std::move(pieces.back());
        found_polyominoes.insert(current);
        pieces.pop_back();

        Blocks rotated = RotateClockwise(current);
        rotated_polyomino[current] = rotated;
        if (found_polyominoes.find(rotated) == found_polyominoes.end())
        {
            pieces.push_back(rotated);
        }

        Blocks reflected = ReflectHorizontally(current);
        reflected_polyomino[current] = reflected;
        if (found_polyominoes.find(reflected) == found_polyominoes.end())
        {
            pieces.push_back(reflected);
        }
    }
}

void PolyominoGenerator::Record(std::deque<Blocks>& source_canonical_pieces,
    std::set<Blocks, PolyominoComparator> &found_polyominoes,
    const std::map<Blocks, Blocks, PolyominoComparator>& rotated_polyomino,
    const std::map<Blocks, Blocks, PolyominoComparator>& reflected_polyomino)
{
    auto start = found_polyominoes.begin();

    source_canonical_pieces.push_back(*start);
    piece_library_.polyomino_by_id[canonical_pointer_] = *start;
    piece_library_.id_by_polyomino[*start] = canonical_pointer_;
    AddSensors(canonical_pointer_);

    found_polyominoes.erase(start);

    // Record transformed id
    int transformation_begin = transformation_pointer_;
    for (const Blocks piece : found_polyominoes) 
    {
        piece_library_.polyomino_by_id[transformation_pointer_] = piece;
        piece_library_.id_by_polyomino[piece] = transformation_pointer_;
        piece_library_.transformed_to_canonical[transformation_pointer_] = canonical_pointer_;
        AddSensors(transformation_pointer_);

        ++transformation_pointer_;
    }

    RecordTransformations(canonical_pointer_, reflected_polyomino, rotated_polyomino);
    for (int id = transformation_begin; id < transformation_pointer_; ++id)
    {
        RecordTransformations(id, reflected_polyomino, rotated_polyomino);
    }

    ++canonical_pointer_;
}

void PolyominoGenerator::RecordTransformations(int id, 
    const std::map<Blocks, Blocks, PolyominoComparator>& reflected_polyomino,
    const std::map<Blocks, Blocks, PolyominoComparator>& rotated_polyomino)
{
    Blocks current = piece_library_.polyomino_by_id[id].blocks;
    
    Blocks rotated = rotated_polyomino.at(current);
    int rotated_id = piece_library_.id_by_polyomino[rotated];
    piece_library_.clockwise_rotated_ids[id] = rotated_id;

    Blocks reflected = reflected_polyomino.at(current);
    int reflected_id = piece_library_.id_by_polyomino[reflected];
    piece_library_.horizontally_reflected_ids[id] = reflected_id;
}

Blocks PolyominoGenerator::RotateClockwise(const Blocks& piece) const
{
    Blocks rotated_piece;
    for (const sf::Vector2i block : piece)
    { // rotate across the origin : (-y, x)
        rotated_piece.insert({ -block.y, block.x });
    }

    return Normalize(rotated_piece);
}

Blocks PolyominoGenerator::ReflectHorizontally(const Blocks& piece) const
{
    Blocks reflected_piece;
    for (sf::Vector2i block : piece)
    { // reflect horizontally: (-x, y)
        reflected_piece.insert({ -block.x, block.y });
    }

    return Normalize(reflected_piece);
}

sf::Vector2i PolyominoGenerator::GetOrigin(const Blocks& piece) const
{ // Find minimum x and y (origin)
     sf::Vector2i origin = *piece.begin();

    for (const sf::Vector2i block : piece)
    {
        origin.x = std::min(origin.x, block.x);
        origin.y = std::min(origin.y, block.y);
    }

    return origin;
}

bool PolyominoGenerator::IsNormalized(int id) const
{
    return id >= 0 && id < kPolyominoCount;
}

bool PolyominoGenerator::IsCanonical(int id) const
{
    return id >= 0 && id < kCanonicalCount;
}

Blocks PolyominoGenerator::Normalize(const Blocks& piece) const
{ // Returns the normalized (canonical) form of the piece
    Blocks normalized;
    sf::Vector2i origin = GetOrigin(piece);
    
    for (const sf::Vector2i block : piece)
    {
        normalized.insert(block - origin);
    }
    
    return normalized;
}

void PolyominoGenerator::AddSensors(int id)
{
    Polyomino& piece = piece_library_.polyomino_by_id[id];
    // Calculate edge blocks (neighbors of the polyomino that are not part of it)
    for (const sf::Vector2i block : piece.blocks)
    {
        for (const auto dir : kCardinalOffsets)
        {
            sf::Vector2i neighbor = block + dir;
            if (piece.blocks.find(neighbor) == piece.blocks.end())
            {
                piece.sensors.edges.insert(neighbor);
            }
        }
    }

    // Calculate corner blocks (diagonal neighbors of the polyomino that are not part of it or edge)
    for (const sf::Vector2i block : piece.blocks)
    {
        for (const auto dir : kDiagonalOffsets)
        {
            sf::Vector2i neighbor = block + dir;
            if (piece.blocks.find(neighbor) == piece.blocks.end() &&
                piece.sensors.edges.find(neighbor) == piece.sensors.edges.end())
            {
                piece.sensors.corners.insert(neighbor);
            }
        }
    }
}
} // namespace Blokus
