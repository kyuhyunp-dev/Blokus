#ifndef POLYOMINO_UTIL_HPP
#define POLYOMINO_UTIL_HPP

#include "constants.hpp"

#include <unordered_map>
#include <unordered_set>
#include <set>
#include <map>
#include <deque>


namespace Blokus
{
    // For storage in flatbuffer
    // IDs 0-20: canonical polyominoes
    // IDs 21-90: transformations (rotations and reflections)
    struct PolyominoDefinition
    {
        std::array<Polyomino, kPolyominoCount> polyomino_by_id;  // All polyominoes indexed by ID
        std::map<Blocks, int, PolyominoComparator> id_by_polyomino; // All IDs indexed by Polyomino
        std::array<int, kPolyominoCount> clockwise_rotated_ids;
        std::array<int, kPolyominoCount> horizontally_reflected_ids;
        std::array<int, kPolyominoCount> transformed_to_canonical;
    };


    class PolyominoGenerator
    {
        public:
            explicit PolyominoGenerator();

            static const PolyominoDefinition& GetData()
            {
                static PolyominoGenerator generator;
                return generator.piece_library_;
            }
            
            bool IsCanonical(int id) const;

            bool IsNormalized(int id) const;

        private:
            void RegisterUniquePolyominoes(
                std::deque<Blocks>& source_canonical_pieces,
                 const Blocks &parent);

            void SearchTransformation(const Blocks& piece,
                std::set<Blocks, PolyominoComparator>& found_polyominoes,
                std::map<Blocks, Blocks, PolyominoComparator> &rotated_polyomino,
                std::map<Blocks, Blocks, PolyominoComparator> &reflected_polyomino) const;

            void Record(std::deque<Blocks>& source_canonical_pieces,
                std::set<Blocks, PolyominoComparator> &found_polyominoes,
                const std::map<Blocks, Blocks, PolyominoComparator>& reflected_polyomino,
                const std::map<Blocks, Blocks, PolyominoComparator>& rotated_polyomino);

            void RecordTransformations(int id,
                const std::map<Blocks, Blocks, PolyominoComparator>& reflected_polyomino,
                const std::map<Blocks, Blocks, PolyominoComparator>& rotated_polyomino);

            void AddSensors(int id);
                            
            Blocks Normalize(const Blocks& piece) const;

            sf::Vector2i GetOrigin(const Blocks& piece) const;   

            Blocks RotateClockwise(const Blocks& piece) const;

            Blocks ReflectHorizontally(const Blocks& piece) const;

            // Two pointers to record polyominos at two places
            int canonical_pointer_ = 1;
            int transformation_pointer_ = 21;

            // Return object
            PolyominoDefinition piece_library_;

            
    }; // class PolyominoGenerator
} // namespace Blokus


#endif // POLYOMINO_UTIL_HPP