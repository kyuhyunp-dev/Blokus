#include "shared/PolyominoUtil.hpp"

#include <vector>
#include <algorithm>
#include <utility>
#include <cassert>
#include <iostream>


PolyominoGenerator::PolyominoGenerator()
{
    // BFS to generate all polyominoes up to MaxBlocks
    std::deque<Blocks> sourceCanonicalPieces;
    sourceCanonicalPieces.push_back({sf::Vector2i{0, 0}});

    // Store monomino into mPieceLibrary
    mPieceLibrary.idByPolyomino[sourceCanonicalPieces.front()] = 0;
    mPieceLibrary.polyominoById[0] = sourceCanonicalPieces.front();
    addSensors(0);

    mPieceLibrary.clockwiseRotatedIds[0] = 0;
    mPieceLibrary.horizontallyReflectedIds[0] = 0;
    for (int id = 0; id < Config::CanonicalCount; ++id)
    {
        mPieceLibrary.transformedToCanonical[id] = id;
    }
 
    // Start BFS search
    size_t pieceSize = 1;
    while (!sourceCanonicalPieces.empty() && pieceSize < Config::MaxBlocks)
    { // Generate all polyominoes of pieceSize
        size_t pieceSizeCount = sourceCanonicalPieces.size();
        for (size_t i = 0; i < pieceSizeCount; ++i)
        {
            Blocks currentPiece = std::move(sourceCanonicalPieces.front());
            sourceCanonicalPieces.pop_front();

            for (const sf::Vector2i block : currentPiece)
            { // For every block, try adding another block next to it
                sf::Vector2i nextBlock = block;
                for (const auto dir : Config::CardinalOffsets)
                {
                    nextBlock += dir;

                    if (currentPiece.find(nextBlock) == currentPiece.end())
                    { // Not part of the parent
                        currentPiece.insert(nextBlock);

                        registerUniquePolyominoes(sourceCanonicalPieces, 
                        currentPiece);;

                        currentPiece.erase(nextBlock);
                    }
                    
                    nextBlock -= dir;
                }
            }
        }
        
        ++pieceSize;
    }

} // PolyominoGenerator::PolyominoGenerator

void PolyominoGenerator::registerUniquePolyominoes(
    std::deque<Blocks>& sourceCanonicalPieces, const Blocks &piece)
{
    // Compute normalized form of the piece
    Blocks current = normalize(piece);
    if (mPieceLibrary.idByPolyomino.find(current) 
        != mPieceLibrary.idByPolyomino.end())
    {
        return;
    }
    
    // Compute rotated, reflected polyominoes
    std::set<Blocks, PolyominoComparator> foundPolyominoes;
    std::map<Blocks, Blocks, PolyominoComparator> rotatedPolyomino;
    std::map<Blocks, Blocks, PolyominoComparator> reflectedPolyomino;
    searchTransformation(current, foundPolyominoes, 
        rotatedPolyomino, reflectedPolyomino);

    if (foundPolyominoes.empty()) 
    {
        return;
    }

    record(sourceCanonicalPieces, foundPolyominoes, rotatedPolyomino, reflectedPolyomino);
} // registerUniquePolyominoes

void PolyominoGenerator::searchTransformation(const Blocks& piece,
    std::set<Blocks, PolyominoComparator>& foundPolyominoes,
    std::map<Blocks, Blocks, PolyominoComparator> &rotatedPolyomino,
    std::map<Blocks, Blocks, PolyominoComparator> &reflectedPolyomino) const
{
    std::vector<Blocks> pieces {piece};

    while (!pieces.empty())
    {
        Blocks current = std::move(pieces.back());
        foundPolyominoes.insert(current);
        pieces.pop_back();

        Blocks rotated = rotateClockwise(current);
        rotatedPolyomino[current] = rotated;
        if (foundPolyominoes.find(rotated) == foundPolyominoes.end())
        {
            pieces.push_back(rotated);
        }

        Blocks reflected = reflectHorizontally(current);
        reflectedPolyomino[current] = reflected;
        if (foundPolyominoes.find(reflected) == foundPolyominoes.end())
        {
            pieces.push_back(reflected);
        }
    }
}

void PolyominoGenerator::record(std::deque<Blocks>& sourceCanonicalPieces,
    std::set<Blocks, PolyominoComparator> &foundPolyominoes,
    const std::map<Blocks, Blocks, PolyominoComparator>& rotatedPolyomino,
    const std::map<Blocks, Blocks, PolyominoComparator>& reflectedPolyomino)
{
    auto start = foundPolyominoes.begin();

    sourceCanonicalPieces.push_back(*start);
    mPieceLibrary.polyominoById[mCanonicalPointer] = *start;
    mPieceLibrary.idByPolyomino[*start] = mCanonicalPointer;
    addSensors(mCanonicalPointer);

    foundPolyominoes.erase(start);

    // Record transformed id
    int transformationBegin = mTransformationPointer;
    for (const Blocks piece : foundPolyominoes) 
    {
        mPieceLibrary.polyominoById[mTransformationPointer] = piece;
        mPieceLibrary.idByPolyomino[piece] = mTransformationPointer;
        mPieceLibrary.transformedToCanonical[mTransformationPointer] = mCanonicalPointer;
        addSensors(mTransformationPointer);

        ++mTransformationPointer;
    }

    recordTransformations(mCanonicalPointer, reflectedPolyomino, rotatedPolyomino);
    for (int id = transformationBegin; id < mTransformationPointer; ++id)
    {
        recordTransformations(id, reflectedPolyomino, rotatedPolyomino);
    }

    ++mCanonicalPointer;
}

void PolyominoGenerator::recordTransformations(int id, 
    const std::map<Blocks, Blocks, PolyominoComparator>& reflectedPolyomino,
    const std::map<Blocks, Blocks, PolyominoComparator>& rotatedPolyomino)
{
    Blocks current = mPieceLibrary.polyominoById[id].blocks;
    
    Blocks rotated = rotatedPolyomino.at(current);
    int rotated_id = mPieceLibrary.idByPolyomino[rotated];
    mPieceLibrary.clockwiseRotatedIds[id] = rotated_id;

    Blocks reflected = reflectedPolyomino.at(current);
    int reflected_id = mPieceLibrary.idByPolyomino[reflected];
    mPieceLibrary.horizontallyReflectedIds[id] = reflected_id;
}

Blocks PolyominoGenerator::rotateClockwise(const Blocks& piece) const
{
    Blocks rotated_piece;
    for (const sf::Vector2i block : piece)
    { // rotate across the origin : (-y, x)
        rotated_piece.insert({ -block.y, block.x });
    }

    return normalize(rotated_piece);
}

Blocks PolyominoGenerator::reflectHorizontally(const Blocks& piece) const
{
    Blocks reflected_piece;
    for (sf::Vector2i block : piece)
    { // reflect horizontally: (-x, y)
        reflected_piece.insert({ -block.x, block.y });
    }

    return normalize(reflected_piece);
}

sf::Vector2i PolyominoGenerator::getOrigin(const Blocks& piece) const
{ // Find minimum x and y (origin)
     sf::Vector2i origin = *piece.begin();

    for (const sf::Vector2i block : piece)
    {
        origin.x = std::min(origin.x, block.x);
        origin.y = std::min(origin.y, block.y);
    }

    return origin;
}

bool PolyominoGenerator::isNormalized(int id) const
{
    return id >= 0 && id < Config::PolyominoCount;
}

bool PolyominoGenerator::isCanonical(int id) const
{
    return id >= 0 && id < Config::CanonicalCount;
}

Blocks PolyominoGenerator::normalize(const Blocks& piece) const
{ // Returns the normalized (canonical) form of the piece
    Blocks normalized;
    sf::Vector2i origin = getOrigin(piece);
    
    for (const sf::Vector2i block : piece)
    {
        normalized.insert(block - origin);
    }
    
    return normalized;
}

void PolyominoGenerator::addSensors(int id)
{
    Polyomino& piece = mPieceLibrary.polyominoById[id];
    // Calculate edge blocks (neighbors of the polyomino that are not part of it)
    for (const sf::Vector2i block : piece.blocks)
    {
        for (const auto dir : Config::CardinalOffsets)
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
        for (const auto dir : Config::DiagonalOffsets)
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
