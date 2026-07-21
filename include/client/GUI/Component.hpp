#ifndef COMPONENT_H
#define COMPONENT_H

#include <SFML/Graphics/Drawable.hpp>
#include <SFML/Graphics/Transformable.hpp>
#include <SFML/Window/Cursor.hpp>
#include <SFML/System/Time.hpp>
#include <SFML/System/Vector2.hpp>

#include <memory>
#include <optional>


namespace sf
{
	class Event;
}

namespace GUI
{
	class Component : public sf::Drawable
					, public sf::Transformable
	{
	public:
		// Non copyable
		Component(const Component&) = delete;
		Component& operator=(const Component&) = delete;

		typedef std::unique_ptr<Component> Ptr;
    
    public:
		Component();
		virtual ~Component() = default;

		virtual bool isInteractive() const = 0;

		bool isHovered() const;
		virtual void hover();
		virtual void unhover();

		bool isPressed() const;
		virtual void press(std::optional<sf::Vector2f> worldMousePos = std::nullopt);
		virtual void release(std::optional<sf::Vector2f> worldMousePos = std::nullopt);

		bool isFocused() const;
		virtual void focus();
		virtual void unfocus();	

		virtual void handleEvent(const sf::Event& event, std::optional<sf::Vector2f> worldMousePos) = 0;
		virtual void update(sf::Time dt);

        virtual sf::FloatRect getGlobalBounds() const = 0;
        virtual std::optional<sf::Cursor::Type> getMouseCursorType() const;

	private:
		bool mIsHovered;
		bool mIsPressed;
		bool mIsFocused; 
	};
}

#endif