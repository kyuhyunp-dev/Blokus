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
    struct PolyominoDefinition
    {
        std::set<int> canonical_ids;
        std::map<int, int> clockwise_rotated_ids;
        std::map<int, int> horizontally_reflected_ids;
        std::map<int, int> transformed_to_canonical;
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

            bool IsNormalized(int id) const;

            Polyomino Decode(int id) const;

        private:
            void RegisterUniquePolyominoes(
                std::deque<Polyomino>& source_canonical_pieces,
                 const Polyomino &parent);
            
            bool RegisterNewPolyomino(const Polyomino& piece, 
                std::set<int>& found_ids, int& transformed_id);

            void RecordIds(std::deque<Polyomino>& source_canonical_pieces, 
                const std::set<int>& found_ids);

            int GetBase() const;    

            sf::Vector2i GetOrigin(const Polyomino& piece) const;   

            int Normalize(const Polyomino& piece) const;

            int Encode(const sf::Vector2i& position) const;
            
            Polyomino RotateClockwise(const Polyomino& piece) const;

            Polyomino ReflectHorizontally(const Polyomino& piece) const;


            PolyominoDefinition piece_library_;

            
    }; // class PolyominoGenerator
} // namespace Blokus


#endif // POLYOMINO_UTIL_HPP