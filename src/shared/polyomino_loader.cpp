#include "shared/polyomino_loader.hpp"
#include "polyomino_generated.h"

#include <fstream>
#include <iostream>
#include <vector>
#include <flatbuffers/flatbuffers.h>
#include <filesystem>


namespace Blokus
 {

void SaveToBinary(const std::string& filepath, 
    const Blokus::PolyominoDefinition& data) 
{
    if (data.id_by_polyomino.empty())
    {
        throw std::runtime_error("No pieces to save");
    }

    flatbuffers::FlatBufferBuilder builder(1024);

    // 1. Build polyominoes vector
    std::vector<flatbuffers::Offset<Blokus::Data::Polyomino>> polyomino_offsets;
    polyomino_offsets.reserve(kPolyominoCount);
    
    for (int id = 0; id < kPolyominoCount; ++id)
    {
        const Polyomino& piece = data.polyomino_by_id[id];
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
        
        auto coords_offset = builder.CreateVectorOfStructs(coords);
        auto edges_offset = builder.CreateVectorOfStructs(edges);
        auto corners_offset = builder.CreateVectorOfStructs(corners);
        auto piece_offset = Blokus::Data::CreatePolyomino(builder, id, coords_offset, edges_offset, corners_offset);
        polyomino_offsets.push_back(piece_offset);
    }

    auto polyominoes_offset = builder.CreateVector(polyomino_offsets);

    // 2. Build rotations, reflections, transformations
    auto arr_to_offset_vec = [&](const std::array<int32_t, kPolyominoCount>& source_arr) {
        std::vector<flatbuffers::Offset<Blokus::Data::IdEntry>> offsets;
        offsets.reserve(source_arr.size());
        for (int id = 0; id < kPolyominoCount; ++id) {
            offsets.push_back(Blokus::Data::CreateIdEntry(builder, id, source_arr[id]));
        }

        return builder.CreateVectorOfSortedTables(&offsets);
    };
    
    auto rotations_offset = arr_to_offset_vec(data.clockwise_rotated_ids);
    auto reflections_offset = arr_to_offset_vec(data.horizontally_reflected_ids);
    auto transformations_offset = arr_to_offset_vec(data.transformed_to_canonical);

    // 3. Build the root PieceLibrary table
    Blokus::Data::PieceLibraryBuilder lib_builder(builder);
    lib_builder.add_polyominoes(polyominoes_offset);
    lib_builder.add_rotations(rotations_offset);
    lib_builder.add_reflections(reflections_offset);
    lib_builder.add_transformations(transformations_offset);
    
    auto root_offset = lib_builder.Finish();

    builder.Finish(root_offset);

    std::ofstream outfile(filepath, std::ios::binary);
    outfile.write(
        reinterpret_cast<const char*>(builder.GetBufferPointer()), 
        builder.GetSize());
    outfile.close();
}// SaveToBinary

void ValidateBinaryFile(const std::string& filepath) 
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
} // ValidateBinaryFile

std::vector<char> LoadBinaryFile(const std::string& filepath)
{
    ValidateBinaryFile(filepath);
    
    // Read the file into a buffer
    std::ifstream infile(filepath, std::ios::binary);
    uintmax_t size = std::filesystem::file_size(filepath);

    std::vector<char> buffer(size);
    if (!infile.read(buffer.data(), size)) 
    {
        throw std::runtime_error("Failed to read binary file data");
    }

    return buffer;
} // LoadBinaryFile

PolyominoDefinition LoadPieceLibrary(const std::string& filepath) 
{
    // 1. Read the file into memory ONCE
    std::vector<char> buffer = LoadBinaryFile(filepath);
    auto piece_lib = Blokus::Data::GetPieceLibrary(buffer.data());
    
    PolyominoDefinition data;

    // 2. Load Polyomino Shapes
    if (piece_lib->polyominoes()) 
    {
        for (auto poly_table : *piece_lib->polyominoes()) 
        {
            int id = poly_table->id();
            if (id >= 0 && id < kPolyominoCount && poly_table->coordinates()) 
            {
                Polyomino poly;
                for (auto coord : *poly_table->coordinates()) 
                {
                    poly.blocks.insert(sf::Vector2i{coord->x(), coord->y()});
                }

                for (auto coord : *poly_table->edges()) 
                {
                    poly.sensors.edges.insert(sf::Vector2i{coord->x(), coord->y()});
                }

                for (auto coord : *poly_table->corners()) 
                {
                    poly.sensors.corners.insert(sf::Vector2i{coord->x(), coord->y()});
                }

                data.polyomino_by_id[id] = poly;
                data.id_by_polyomino[poly.blocks] = id; // Also populate the reverse map!
            }
        }
    }

    // 3. Helper to load the simple maps (arrays)
    auto fill_array = [](const flatbuffers::Vector<flatbuffers::Offset<Blokus::Data::IdEntry>>* vec, 
                         std::array<int32_t, kPolyominoCount>& target) 
                         {
        target.fill(-1);
        if (vec) 
        {
            for (auto entry : *vec) 
            {
                if (entry->id() >= 0 && entry->id() < kPolyominoCount) 
                {
                    target[entry->id()] = entry->result();
                }
            }
        }
    };

    fill_array(piece_lib->rotations(), data.clockwise_rotated_ids);
    fill_array(piece_lib->reflections(), data.horizontally_reflected_ids);
    fill_array(piece_lib->transformations(), data.transformed_to_canonical);

    return data; 
    // Because buffer is destroyed here, and you copied data into 'data', this is safe.
}
} // namespace Blokus


