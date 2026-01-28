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
    if (filepath.empty()) 
    {
        throw std::runtime_error("File path is empty");
    }

    if (data.canonical_ids.empty())
    {
        throw std::runtime_error("No pieces to save");
    }

    flatbuffers::FlatBufferBuilder builder(1024);

    std::vector<int32_t> canonical_vec(data.canonical_ids.begin(), data.canonical_ids.end());
    auto canonical_offset = builder.CreateVector(canonical_vec);

    auto map_to_offset_vec = [&](const std::map<int32_t, int32_t>& source_map) {
        std::vector<flatbuffers::Offset<Blokus::Data::IdEntry>> offsets;
        offsets.reserve(source_map.size());
        for (auto const& [id, res] : source_map) {
            offsets.push_back(Blokus::Data::CreateIdEntry(builder, id, res));
        }
        // Since std::map is sorted, this creates a searchable binary index
        return builder.CreateVectorOfSortedTables(&offsets);
    };
    
    auto rotations_offset = map_to_offset_vec(data.clockwise_rotated_ids);
    auto reflections_offset = map_to_offset_vec(data.horizontally_reflected_ids);
    auto transformations_offset = map_to_offset_vec(data.transformed_to_canonical);

    // 3. Build the root PieceLibrary table
    Blokus::Data::PieceLibraryBuilder lib_builder(builder);
    lib_builder.add_canonical_ids(canonical_offset);
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

std::set<int32_t> LoadCanonicalIds(const std::string& filepath)
{
    std::vector<char> buffer = LoadBinaryFile(filepath);
    
    auto piece_lib = Blokus::Data::GetPieceLibrary(buffer.data());
    
    std::set<int32_t> canonical_ids;
    if (piece_lib->canonical_ids() != nullptr)
    {
        for (int i = 0; i < piece_lib->canonical_ids()->size(); ++i)
        {
            canonical_ids.insert(piece_lib->canonical_ids()->Get(i));
        }
    }
    
    return canonical_ids;
} // LoadCanonicalIds

std::map<int32_t, int32_t> LoadRotations(const std::string& filepath)
{
    std::vector<char> buffer = LoadBinaryFile(filepath);
    
    auto piece_lib = Blokus::Data::GetPieceLibrary(buffer.data());
    
    std::map<int32_t, int32_t> rotations;
    if (piece_lib->rotations() != nullptr)
    {
        for (int i = 0; i < piece_lib->rotations()->size(); ++i)
        {
            auto entry = piece_lib->rotations()->Get(i);
            rotations[entry->id()] = entry->result();
        }
    }
    
    return rotations;
} // LoadRotations

std::map<int32_t, int32_t> LoadReflections(const std::string& filepath)
{
    std::vector<char> buffer = LoadBinaryFile(filepath);
    
    auto piece_lib = Blokus::Data::GetPieceLibrary(buffer.data());
    
    std::map<int32_t, int32_t> reflections;
    if (piece_lib->reflections() != nullptr)
    {
        for (int i = 0; i < piece_lib->reflections()->size(); ++i)
        {
            auto entry = piece_lib->reflections()->Get(i);
            reflections[entry->id()] = entry->result();
        }
    }
    
    return reflections;
} // LoadReflections

std::map<int32_t, int32_t> LoadTransformations(const std::string& filepath)
{
    std::vector<char> buffer = LoadBinaryFile(filepath);
    
    auto piece_lib = Blokus::Data::GetPieceLibrary(buffer.data());
    
    std::map<int32_t, int32_t> transformations;
    if (piece_lib->transformations() != nullptr)
    {
        for (int i = 0; i < piece_lib->transformations()->size(); ++i)
        {
            auto entry = piece_lib->transformations()->Get(i);
            transformations[entry->id()] = entry->result();
        }
    }
    
    return transformations;
} // LoadTransformations
} // namespace Blokus


