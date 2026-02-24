#include "Resource/ResourceHolder.hpp"
#include "Resource/ResourceIdentifiers.hpp"


class MockTextureHolder : public ResourceHolder<sf::Texture, Textures::ID> 
{
public:
    // Override load to do absolutely nothing
    void load(Textures::ID id, const std::string& filename) override 
    {
        // We catch the call and stay silent. 
        // Then we manually insert a dummy texture so 'get()' doesn't crash.
        auto dummy = std::make_unique<sf::Texture>();
        insertResource(id, std::move(dummy));
    }
};