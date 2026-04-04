#ifndef COMMAND_HPP
#define COMMAND_HPP


#include "Player.hpp"
#include "Category.hpp"

#include "SFML/System/Time.hpp"
#include <functional>

class SceneNode;

struct Command 
{
    using ActionFunc = std::function<void(SceneNode&, sf::Time)>;
    
    Command();

    ActionFunc action;
    unsigned int category = Category::None; // Filters which nodes listen
};

template <typename GameObject, typename Function>
std::function<void(SceneNode&, sf::Time)> derivedAction(Function fn)
{
	return [=] (SceneNode& node, sf::Time dt)
	{
		// Check if cast is safe
		assert(dynamic_cast<GameObject*>(&node) != nullptr);

		// Downcast node and invoke function on it
		fn(static_cast<GameObject&>(node), dt);
	};
}

#endif