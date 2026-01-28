#include "pregame.hpp"

#include "polyomino_generated.h"
#include "shared/polyomino_loader.hpp"

#include <iostream>
#include <fstream>
#include <filesystem>
#include <vector>


PreGame::PreGame(const std::string& polyomino_path)
{
    Blokus::ValidateBinaryFile(polyomino_path);

    auto canonical_ids = Blokus::LoadCanonicalIds(polyomino_path);
    auto rotations = Blokus::LoadRotations(polyomino_path);
    auto reflections = Blokus::LoadReflections(polyomino_path);
    auto transformations = Blokus::LoadTransformations(polyomino_path);

    std::cout << "=== Canonical IDs ===" << std::endl;
    std::cout << "Total canonical pieces: " << canonical_ids.size() << std::endl;
    for (int32_t id : canonical_ids)
    {
        std::cout << "  Canonical ID: " << id << std::endl;
    }

    std::cout << "\n=== Rotations ===" << std::endl;
    std::cout << "Total rotations: " << rotations.size() << std::endl;
    for (const auto& [original_id, rotated_id] : rotations)
    {
        std::cout << "  ID " << original_id << " rotates to " << rotated_id << std::endl;
    }

    std::cout << "\n=== Reflections ===" << std::endl;
    std::cout << "Total reflections: " << reflections.size() << std::endl;
    for (const auto& [original_id, reflected_id] : reflections)
    {
        std::cout << "  ID " << original_id << " reflects to " << reflected_id << std::endl;
    }

    std::cout << "\n=== Transformations ===" << std::endl;
    std::cout << "Total transformations: " << transformations.size() << std::endl;
    for (const auto& [transformed_id, canonical_id] : transformations)
    {
        std::cout << "  ID " << transformed_id << " transforms to canonical " << canonical_id << std::endl;
    }
}