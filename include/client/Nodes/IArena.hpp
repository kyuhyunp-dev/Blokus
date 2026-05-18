#ifndef IARENA_HPP
#define IARENA_HPP

#include "Nodes/SceneNode.hpp"
#include <SFML/System/Vector2.hpp>


// Forward declarations to avoid heavy includes
class TrayNode;
class BoardNode;

class IArena : public SceneNode 
{
public:
    virtual ~IArena() = default;

    // Core lifecycle
    virtual void buildScene() = 0;

    // Actions
    virtual unsigned int getCategory() const = 0;
                    
    virtual void grabPiece(int id, sf::Vector2f worldPos) = 0;
    virtual void placePiece(sf::Vector2i gridCoord) = 0;
    virtual void returnPiece() = 0;

    void onCommand(const Command& command, sf::Time dt) override = 0;

    // Queries
    virtual TrayNode* getTrayNodePtr() const = 0;
    virtual BoardNode* getBoardNodePtr() const = 0;
};

#endif