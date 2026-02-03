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
    const static int kCanonicalCount = 21;
    const static int kPolyominoCount = 91;
    // For storage in flatbuffer
    // IDs 0-20: canonical polyominoes
    // IDs 21-90: transformations (rotations and reflections)
    struct PolyominoDefinition
    {
        std::array<Polyomino, kPolyominoCount> polyomino_by_id;  // All polyominoes indexed by ID
        std::map<Polyomino, int, PolyominoComparator> id_by_polyomino; // All IDs indexed by Polyomino
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
                std::deque<Polyomino>& source_canonical_pieces,
                 const Polyomino &parent);

            void SearchTransformation(const Polyomino& piece,
                std::set<Polyomino, PolyominoComparator>& found_polyominoes,
                std::map<Polyomino, Polyomino, PolyominoComparator> &rotated_polyomino,
                std::map<Polyomino, Polyomino, PolyominoComparator> &reflected_polyomino) const;

            void Record(std::deque<Polyomino>& source_canonical_pieces,
                std::set<Polyomino, PolyominoComparator> &found_polyominoes,
                const std::map<Polyomino, Polyomino, PolyominoComparator>& reflected_polyomino,
                const std::map<Polyomino, Polyomino, PolyominoComparator>& rotated_polyomino);

            void RecordTransformations(int id,
                const std::map<Polyomino, Polyomino, PolyominoComparator>& reflected_polyomino,
                const std::map<Polyomino, Polyomino, PolyominoComparator>& rotated_polyomino);

                            
            Polyomino Normalize(const Polyomino& piece) const;

            sf::Vector2i GetOrigin(const Polyomino& piece) const;   

            Polyomino RotateClockwise(const Polyomino& piece) const;

            Polyomino ReflectHorizontally(const Polyomino& piece) const;

            // Two pointers to record polyominos at two places
            int canonical_pointer_ = 1;
            int transformation_pointer_ = 21;

            // Return object
            PolyominoDefinition piece_library_;

            
    }; // class PolyominoGenerator
} // namespace Blokus


#endif // POLYOMINO_UTIL_HPP