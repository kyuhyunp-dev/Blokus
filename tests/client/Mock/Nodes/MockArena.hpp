#ifndef MOCK_ARENA_HPP
#define MOCK_ARENA_HPP

#include <gmock/gmock.h>
#include "Nodes/IArena.hpp"


class MockArena : public IArena 
{ // The constructor for Arena has too many arguments + buildScene
public:
    MOCK_METHOD(unsigned int, getCategory, (), (const, override));
    MOCK_METHOD(void, buildScene, (), (override));

    MOCK_METHOD(void, grabPiece, (int, sf::Vector2f), (override));
    MOCK_METHOD(void, placePiece, (sf::Vector2i), (override));
    MOCK_METHOD(void, returnPiece, (), (override));

    MOCK_METHOD(TrayNode*, getTrayNodePtr, (), (const, override));
    MOCK_METHOD(BoardNode*, getBoardNodePtr, (), (const, override));
    
    // Arena is a SceneNode 
    MOCK_METHOD(void, updateCurrent, (sf::Time), (override));
    MOCK_METHOD(void, onCommand, (const Command&, sf::Time), (override));
    MOCK_METHOD(void, draw, (sf::RenderTarget&, sf::RenderStates), (const, override));   
}; 

#endif