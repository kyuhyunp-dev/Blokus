#ifndef POLYOMINO_UTIL_HPP
#define POLYOMINO_UTIL_HPP

#include "Constants.hpp"

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
        std::array<Polyomino, PolyominoCount> polyominoById;  // All polyominoes indexed by ID
        std::map<Blocks, int, PolyominoComparator> idByPolyomino; // All IDs indexed by Polyomino
        std::array<int, PolyominoCount> clockwiseRotatedIds;
        std::array<int, PolyominoCount> horizontallyReflectedIds;
        std::array<int, PolyominoCount> transformedToCanonical;
    };


    class PolyominoGenerator
    {
        public:
            explicit PolyominoGenerator();

            static const PolyominoDefinition& getData()
            {
                static PolyominoGenerator generator;
                return generator.mPieceLibrary;
            }
            
            bool isCanonical(int id) const;

            bool isNormalized(int id) const;

        private:
            void registerUniquePolyominoes(
                std::deque<Blocks>& sourceCanonicalPieces,
                 const Blocks &parent);

            void searchTransformation(const Blocks& piece,
                std::set<Blocks, PolyominoComparator>& foundPolyominoes,
                std::map<Blocks, Blocks, PolyominoComparator> &rotatedPolyomino,
                std::map<Blocks, Blocks, PolyominoComparator> &reflectedPolyomino) const;

            void record(std::deque<Blocks>& sourceCanonicalPieces,
                std::set<Blocks, PolyominoComparator> &foundPolyominoes,
                const std::map<Blocks, Blocks, PolyominoComparator>& reflectedPolyomino,
                const std::map<Blocks, Blocks, PolyominoComparator>& rotatedPolyomino);

            void recordTransformations(int id,
                const std::map<Blocks, Blocks, PolyominoComparator>& reflectedPolyomino,
                const std::map<Blocks, Blocks, PolyominoComparator>& rotatedPolyomino);

            void addSensors(int id);
                            
            Blocks normalize(const Blocks& piece) const;

            sf::Vector2i getOrigin(const Blocks& piece) const;   

            Blocks rotateClockwise(const Blocks& piece) const;

            Blocks reflectHorizontally(const Blocks& piece) const;

            // Two pointers to record polyominos at two places
            int mCanonicalPointer = 1;
            int mTransformationPointer = 21;

            // Return object
            PolyominoDefinition mPieceLibrary;

            
    }; // class PolyominoGenerator
} // namespace Blokus


#endif // POLYOMINO_UTIL_HPP