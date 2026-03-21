#include "Nodes/SceneNode.hpp"
#include "Resource/ResourceHolder.hpp"
#include "Resource/ResourceIdentifiers.hpp"
#include "shared/Team.hpp"
#include "shared/Constants.hpp"


class BoardNode;
class TrayNode;

namespace sf
{
    class RenderWindow;
}

class Arena
{
public:
    Arena(std::array<int, Blokus::DeckSize> deck);

    void buildScene();
    
    // The "Engine" - Processes the commands sent by the Player
    void update(sf::Time dt);
    
    void draw(sf::RenderTarget& target, sf::RenderStates states) const;

private:
    void loadTextures();

    enum Layer 
    { 
        World, 
        LayerCount 
    };

private:
    TextureHolder mTextures; 
    
    std::array<SceneNode*, LayerCount> mSceneLayers;
    SceneNode mSceneGraph;

    // The Visual Layers (Added as children in constructor)
    BoardNode* mBoard;
    TrayNode* mTray;

    Team mActiveTeam;
    Team mTeam;
    std::array<int, Blokus::DeckSize> mDeck;
};