#ifndef NETWORK_GAME_STATE_HPP
#define NETWORK_GAME_STATE_HPP

#include "States/State.hpp"
#include "Command/CommandQueue.hpp"
#include "Nodes/IArena.hpp"

#include "shared/Referee.hpp"
#include "Resource/ResourceHolder.hpp"
#include "Resource/ResourceIdentifiers.hpp"
#include "Player.hpp"

#include <SFML/System/Time.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Text.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/Font.hpp>

#include <cstdint>
#include <memory>
#include <map>


class NetworkGameState : public State
{
    public:
        // Non Copyable 
        NetworkGameState(const NetworkGameState&) = delete;
        NetworkGameState& operator=(const NetworkGameState&) = delete;

        explicit NetworkGameState(StateStack& stack, Context context);

        virtual void draw() override;
        virtual bool update(sf::Time dt) override;
        virtual bool handleEvent(const sf::Event& event) override;

        virtual void onActivate() override;

    protected:
        virtual std::unique_ptr<IArena> createArena();
        CommandQueue& getCommandQueue();
        virtual void setQuery();

    private:
        using PlayerPtr = std::unique_ptr<Player>;

        void loadTextures();

    private:
        CommandQueue mCommandQueue;
        Referee mReferee;
        std::unique_ptr<IArena> mArenaPtr;

        std::map<int32_t, PlayerPtr> mPlayers;
        uint32_t mLocalPlayerIdentifier;
};

#endif