#include "shared/polyomino_loader.hpp"
#include "polyomino_generated.h"

#include <fstream>
#include <iostream>
#include <vector>
#include <flatbuffers/flatbuffers.h>


namespace Blokus
 {

    void save_to_binary(const std::string& filepath, 
        const std::vector<PolyominoDefinition>& pieces) 
    {
        if (filepath.empty()) 
        {
            throw std::runtime_error("File path is empty");
        }

        if (pieces.empty()) 
        {
            throw std::runtime_error("No pieces to save");
        }

        flatbuffers::FlatBufferBuilder builder(1024);
        std::vector<flatbuffers::Offset<Data::PolyominoMetadata>> piece_offsets;

        for (const auto& piece : pieces) 
        {
            Blokus::Data::PolyominoMetadataBuilder piece_builder(builder);
            piece_builder.add_id(piece.id);

            auto piece_offset = piece_builder.Finish();
            piece_offsets.push_back(piece_offset);
        }

        auto pieces_vec_offset = builder.CreateVector(piece_offsets);

        Blokus::Data::PieceLibraryBuilder library_builder(builder);
        library_builder.add_pieces(pieces_vec_offset);
        auto root_offset = library_builder.Finish();

        builder.Finish(root_offset);

        std::ofstream outfile(filepath, std::ios::binary);
        outfile.write(reinterpret_cast<const char*>(builder.GetBufferPointer()), builder.GetSize());
        outfile.close();
    }

    std::vector<char> load_binary_file(const std::string& filepath) 
    {
       // Read the binary file into a buffer
        std::ifstream infile(filepath, std::ios::binary);
        if (!infile) 
        {
            throw std::runtime_error("Binary file not found");
        }

        // Read the file into a buffer
        uintmax_t size = std::filesystem::file_size(filepath);

        std::vector<char> buffer(size);
        if (!infile.read(buffer.data(), size)) 
        {
            throw std::runtime_error("Failed to read data");
        }

        return buffer;
    }
}


