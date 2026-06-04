#ifndef COMPONENT_H
#define COMPONENT_H

#include <SFML/Graphics/Drawable.hpp>
#include <SFML/Graphics/Transformable.hpp>

#include <memory>

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

		virtual bool isSelectable() const = 0;
		bool isSelected() const;
		virtual void select();
		virtual void deselect();

		virtual bool isActive() const;
		virtual void activate();
		virtual void deactivate();

		virtual void handleEvent(const sf::Event& event, 
            std::optional<sf::Vector2f> worldMousePos) = 0;

        virtual sf::FloatRect getGlobalBounds() const = 0;

	private:
		bool mIsSelected; // Hovered
		bool mIsActive; // Used
	};
}

#endif