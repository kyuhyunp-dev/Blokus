#ifndef ARENA_HPP
#define ARENA_HPP

#include "Nodes/IArena.hpp"
#include "Nodes/SceneNode.hpp"
#include "Resource/ResourceHolder.hpp"
#include "Resource/ResourceIdentifiers.hpp"
#include "shared/Team.hpp"
#include "shared/Constants.hpp"

class CommandQueue;
namespace sf
{
    class RenderTarget;
}

class Arena : public IArena
{
public:
    enum Layer 
    { 
        World, 
        Action,
        LayerCount 
    };

public:
    Arena(sf::RenderTarget& window, TextureHolder& textures, 
        std::array<int, Blokus::DeckSize> deck, 
        CommandQueue &commands, Team team);

    void buildScene();

    unsigned int getCategory() const override;

    void grabPiece(int id, sf::Vector2f worldPos);
    virtual void placePiece(sf::Vector2i gridCoord);
    virtual void returnPiece();

    TrayNode* getTrayNodePtr() const;
    BoardNode* getBoardNodePtr() const; 

    SceneNode* getLayer(Layer layer) const;

private:
    sf::RenderTarget& mTarget;
    TextureHolder& mTextures; 
    
    std::array<SceneNode*, LayerCount> mSceneLayers;
    CommandQueue& mCommands;

    // The Visual Layers (Added as children in constructor)
    BoardNode* mBoardPtr;
    TrayNode* mTrayPtr;
    SceneNode* mActivePiecePtr;

    Team mActiveTeam;
    const Team mTeam;
    std::array<int, Blokus::DeckSize> mDeck;
};

#endif