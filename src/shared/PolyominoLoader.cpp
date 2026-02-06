#include "shared/PolyominoLoader.hpp"
#include "polyomino_generated.h"

#include <fstream>
#include <iostream>
#include <vector>
#include <flatbuffers/flatbuffers.h>
#include <filesystem>


namespace Blokus
 {

void saveToBinary(const std::string& filepath, 
    const Blokus::PolyominoDefinition& data) 
{
    if (data.idByPolyomino.empty())
    {
        throw std::runtime_error("No pieces to save");
    }

    flatbuffers::FlatBufferBuilder builder(1024);

    // 1. Build polyominoes vector
    std::vector<flatbuffers::Offset<Blokus::Data::Polyomino>> polyominoOffsets;
    polyominoOffsets.reserve(PolyominoCount);
    
    for (int id = 0; id < PolyominoCount; ++id)
    {
        const Polyomino& piece = data.polyominoById[id];
        const Blocks& blocks = piece.blocks;
        
        if (blocks.empty())
        {
            throw std::runtime_error("Empty piece");
        }

        std::vector<Blokus::Data::Coordinate> coords;
        coords.reserve(blocks.size());
        for (const auto block : blocks)
        {
            coords.push_back(Blokus::Data::Coordinate{block.x, block.y});
        }

        std::vector<Blokus::Data::Coordinate> edges;
        edges.reserve(piece.sensors.edges.size());
        for (const auto block : piece.sensors.edges)
        {
            edges.push_back(Blokus::Data::Coordinate{block.x, block.y});
        }

        std::vector<Blokus::Data::Coordinate> corners;
        corners.reserve(piece.sensors.corners.size());
        for (const auto block : piece.sensors.corners)
        {
            corners.push_back(Blokus::Data::Coordinate{block.x, block.y});
        }
        
        auto coordsOffset = builder.CreateVectorOfStructs(coords);
        auto edgesOffset = builder.CreateVectorOfStructs(edges);
        auto cornersOffset = builder.CreateVectorOfStructs(corners);
        auto pieceOffset = Blokus::Data::CreatePolyomino(builder, id, coordsOffset, edgesOffset, cornersOffset);
        polyominoOffsets.push_back(pieceOffset);
    }

    auto polyominoesOffset = builder.CreateVector(polyominoOffsets);

    // 2. Build rotations, reflections, transformations
    auto arrToOffsetVec = [&](const std::array<int32_t, PolyominoCount>& sourceArr) {
        std::vector<flatbuffers::Offset<Blokus::Data::IdEntry>> offsets;
        offsets.reserve(sourceArr.size());
        for (int id = 0; id < PolyominoCount; ++id) {
            offsets.push_back(Blokus::Data::CreateIdEntry(builder, id, sourceArr[id]));
        }

        return builder.CreateVectorOfSortedTables(&offsets);
    };
    
    auto rotationsOffset = arrToOffsetVec(data.clockwiseRotatedIds);
    auto reflectionsOffset = arrToOffsetVec(data.horizontallyReflectedIds);
    auto transformationsOffset = arrToOffsetVec(data.transformedToCanonical);

    // 3. Build the root PieceLibrary table
    Blokus::Data::PieceLibraryBuilder libBuilder(builder);
    libBuilder.add_polyominoes(polyominoesOffset);
    libBuilder.add_rotations(rotationsOffset);
    libBuilder.add_reflections(reflectionsOffset);
    libBuilder.add_transformations(transformationsOffset);
    
    auto rootOffset = libBuilder.Finish();

    builder.Finish(rootOffset);

    std::ofstream outfile(filepath, std::ios::binary);
    outfile.write(
        reinterpret_cast<const char*>(builder.GetBufferPointer()), 
        builder.GetSize());
    outfile.close();
}// saveToBinary

void validateBinaryFile(const std::string& filepath) 
{
    if (filepath.empty()) 
    {
        throw std::runtime_error("File path is empty");
    }

    std::ifstream infile(filepath, std::ios::binary);
    if (!infile) 
    {
        throw std::runtime_error("Binary file not found");
    }

    uintmax_t size = std::filesystem::file_size(filepath);
    
    if (size == 0)
    {
        throw std::runtime_error("Binary file is empty");
    }
} // validateBinaryFile

std::vector<char> loadBinaryFile(const std::string& filepath)
{
    validateBinaryFile(filepath);
    
    // Read the file into a buffer
    std::ifstream infile(filepath, std::ios::binary);
    uintmax_t size = std::filesystem::file_size(filepath);

    std::vector<char> buffer(size);
    if (!infile.read(buffer.data(), size)) 
    {
        throw std::runtime_error("Failed to read binary file data");
    }

    return buffer;
} // loadBinaryFile

PolyominoDefinition loadPieceLibrary(const std::string& filepath) 
{
    // 1. Read the file into memory ONCE
    std::vector<char> buffer = loadBinaryFile(filepath);
    auto pieceLib = Blokus::Data::GetPieceLibrary(buffer.data());
    
    PolyominoDefinition data;

    // 2. Load Polyomino Shapes
    if (pieceLib->polyominoes()) 
    {
        for (auto polyTable : *pieceLib->polyominoes()) 
        {
            int id = polyTable->id();
            if (id >= 0 && id < PolyominoCount && polyTable->coordinates()) 
            {
                Polyomino poly;
                for (auto coord : *polyTable->coordinates()) 
                {
                    poly.blocks.insert(sf::Vector2i{coord->x(), coord->y()});
                }

                for (auto coord : *polyTable->edges()) 
                {
                    poly.sensors.edges.insert(sf::Vector2i{coord->x(), coord->y()});
                }

                for (auto coord : *polyTable->corners()) 
                {
                    poly.sensors.corners.insert(sf::Vector2i{coord->x(), coord->y()});
                }

                data.polyominoById[id] = poly;
                data.idByPolyomino[poly.blocks] = id; // Also populate the reverse map!
            }
        }
    }

    // 3. Helper to load the simple maps (arrays)
    auto fillArray = [](const flatbuffers::Vector<flatbuffers::Offset<Blokus::Data::IdEntry>>* vec, 
                         std::array<int32_t, PolyominoCount>& target) 
                         {
        target.fill(-1);
        if (vec) 
        {
            for (auto entry : *vec) 
            {
                if (entry->id() >= 0 && entry->id() < PolyominoCount) 
                {
                    target[entry->id()] = entry->result();
                }
            }
        }
    };

    fillArray(pieceLib->rotations(), data.clockwiseRotatedIds);
    fillArray(pieceLib->reflections(), data.horizontallyReflectedIds);
    fillArray(pieceLib->transformations(), data.transformedToCanonical);

    return data; 
    // Because buffer is destroyed here, and you copied data into 'data', this is safe.
}
} // namespace Blokus


