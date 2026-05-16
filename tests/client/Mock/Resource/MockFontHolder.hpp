#ifndef MOCK_FONTHOLDER_HPP
#define MOCK_FONTHOLDER_HPP

#include "Resource/ResourceHolder.hpp"
#include "Resource/ResourceIdentifiers.hpp"


class MockFontHolder : public ResourceHolder<sf::Font, Fonts::ID> 
{
public:
    // Override load to do absolutely nothing
    void load(Fonts::ID id, const std::string& filename) override 
    {
        // We catch the call and stay silent. 
        // Then we manually insert a dummy texture so 'get()' doesn't crash.
        auto dummy = std::make_unique<sf::Font>();
        insertResource(id, std::move(dummy));
    }
};

#endif