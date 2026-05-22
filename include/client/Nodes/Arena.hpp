#ifndef ARENA_HPP
#define ARENA_HPP

#include "Nodes/IArena.hpp"
#include "Nodes/SceneNode.hpp"
#include "Resource/ResourceHolder.hpp"
#include "Resource/ResourceIdentifiers.hpp"
#include "shared/Team.hpp"
#include "shared/SharedConfig.hpp"
#include "shared/PolyominoUtil.hpp"


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
        const PolyominoDefinition& library, 
        std::array<int, Config::DeckSize> deck, 
        CommandQueue &commands, Team team);

    void buildScene() override;

    unsigned int getCategory() const override;

    void grabPiece(int id, sf::Vector2f worldPos) override;
    virtual void placePiece(sf::Vector2i gridCoord) override;
    virtual void returnPiece() override;

    TrayNode* getTrayNodePtr() const override;
    BoardNode* getBoardNodePtr() const override; 

    SceneNode* getLayer(Layer layer) const;
    
    void loadTextures() override;

private:
    sf::RenderTarget& mTarget;
    TextureHolder& mTextures; 
    const PolyominoDefinition& mLibrary;
     
    std::array<SceneNode*, LayerCount> mSceneLayers;
    CommandQueue& mCommands;

    // The Visual Layers (Added as children in constructor)
    BoardNode* mBoardPtr;
    TrayNode* mTrayPtr;
    SceneNode* mActivePiecePtr;

    Team mActiveTeam;
    const Team mTeam;
    std::array<int, Config::DeckSize> mDeck;
};

#endif