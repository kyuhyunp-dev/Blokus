#ifndef RESOURCEIDENTIFIERS_HPP
#define RESOURCEIDENTIFIERS_HPP


// Forward declaration of SFML classes
namespace sf
{
    class Font;
	class Texture;
}

namespace Fonts
{
    enum ID
    {
        Sansation
    };
}

namespace Textures
{
	enum ID
	{
		Tiles
	};
}

// Forward declaration and a few type definitions
template <typename Resource, typename Identifier>
class ResourceHolder;

typedef ResourceHolder<sf::Font, Fonts::ID> FontHolder;
typedef ResourceHolder<sf::Texture, Textures::ID> TextureHolder;

#endif