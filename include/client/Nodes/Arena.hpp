#ifndef ARENA_HPP
#define ARENA_HPP

#include "Nodes/SceneNode.hpp"
#include "Resource/ResourceHolder.hpp"
#include "Resource/ResourceIdentifiers.hpp"
#include "shared/Team.hpp"
#include "shared/Constants.hpp"

 
class CommandQueue;
class BoardNode;
class TrayNode;
class ActiveNode;

namespace sf
{
    class RenderTarget;
}

class Arena : public SceneNode
{
public:
    enum Layer 
    { 
        World, 
        Action,
        LayerCount 
    };

public:
    Arena(sf::RenderTarget& window, TextureHolder& textures, std::array<int, Blokus::DeckSize> deck, CommandQueue &commands);

    void buildScene();

    unsigned int getCategory() const override;

    void grabPiece(int id, sf::Vector2f worldPos);

    TrayNode* getTrayNode() const;

    BoardNode* getBoardNode() const; 

    SceneNode* getLayer(Layer layer) const;

private:
    sf::RenderTarget& mTarget;
    TextureHolder& mTextures; 
    
    std::array<SceneNode*, LayerCount> mSceneLayers;
    CommandQueue& mCommands;

    // The Visual Layers (Added as children in constructor)
    BoardNode* mBoard;
    TrayNode* mTray;

    Team mActiveTeam;
    const Team mTeam;
    std::array<int, Blokus::DeckSize> mDeck;
};

#endif