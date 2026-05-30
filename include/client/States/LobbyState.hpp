#ifndef LOBBY_STATE_HPP
#define LOBBY_STATE_HPP

#include "States/State.hpp"
#include "shared/Game/GameSessionData.hpp"

#include <SFML/Graphics/Text.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Texture.hpp>

#include <map>


class LobbyState : public State
{
public:
	LobbyState(StateStack& stack, Context context);

	virtual void draw();
	virtual bool update(sf::Time dt);
	virtual bool handleEvent(const sf::Event& event);

protected:
	// Inside LobbyState.hpp
	struct LobbySlot {
		int id;
		sf::Text text;
	};

    void addPlayer(int id, const std::string& username);
	void removePlayer(int id);

	const sf::Text& getTitleText() const;
	const sf::Text& getStatusText() const;
	const std::vector<LobbySlot>& getUsernameTexts() const;

	void recalculateLayout();
	const sf::Vector2f getOffset(int pos) const;
	
private:
	sf::Text mTitleText;
    sf::Text mStatusText;
  
	std::vector<LobbySlot> mUsernameTexts;
};

#endif
