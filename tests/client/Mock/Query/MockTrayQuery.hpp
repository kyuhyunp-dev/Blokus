#include "Query/TrayQuery.hpp"
#include "Mock/Resource/MockTextureHolder.hpp"
#include "shared/Team.hpp"
#include "Nodes/PieceNode.hpp"


// A simple mock for the TrayQuery interface
class MockTrayQuery : public TrayQuery {
public:
    MockTrayQuery(MockTextureHolder& textures)
        : mTeam (Team::Red)
        , mTextures(textures)
    {
    }

    void setPiece(int id) 
    {
        // Make sure all piece ptrs are alive during test
        auto piece = std::make_unique<PieceNode>(id, mTeam, mTextures);
        responsePiece = piece.get();
        mPieceVault.push_back(std::move(piece));
    }

    PieceNode* getPieceAt(sf::Vector2f worldPos) const 
    {
        return responsePiece;
    }

private:
    PieceNode* responsePiece;
    const Team mTeam;
    MockTextureHolder& mTextures;
    std::vector<std::unique_ptr<PieceNode>> mPieceVault; 
};