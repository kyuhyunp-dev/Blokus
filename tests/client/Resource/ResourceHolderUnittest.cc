#include "Resource/ResourceHolder.hpp"
#include "Mock/Resource/MockResource.hpp"

#include "gtest/gtest.h"
#include <memory>
#include <stdexcept>

namespace
{ 
    TEST(ResourceHolderTest, CorrectBranchingByErrorMessage) {
        // 1. Test the Font branch (if branch)
        ResourceHolder<sf::Font, ResourceID> fontHolder;
        try 
        {
            fontHolder.load(ResourceID::First, "non_existent.ttf");
            FAIL() << "Should have thrown an exception";
        } 
        catch (const std::runtime_error& e) 
        {
            // This confirms it reached: "Failed to open font"
            EXPECT_STREQ(e.what(), "ResourceHolder::load - Failed to open font non_existent.ttf");
        }

        // 2. Test the Texture branch (else branch)
        ResourceHolder<sf::Texture, ResourceID> textureHolder;
        try {
            textureHolder.load(ResourceID::First, "non_existent.png");
            FAIL() << "Should have thrown an exception";
        } catch (const std::runtime_error& e) {
            // This confirms it reached: "Failed to load resource"
            EXPECT_STREQ(e.what(), "ResourceHolder::load - Failed to load resource non_existent.png");
        }
    }
    
    // Test: Round-trip integrity test with open() and get()
    TEST(ResourceHolderTest, LoadAndGetRoundTrip)
    {
        ResourceHolder<MockResource, ResourceID> holder;

        // Load resource using open()
        holder.load(ResourceID::First, "test_file.txt");

        // Get the resource and verify it was loaded correctly
        const MockResource& resource = holder.get(ResourceID::First);
        EXPECT_TRUE(resource.isLoaded());
        EXPECT_EQ(resource.getFileName(), "test_file.txt");
    }

    // Test: Round-trip integrity test with load(id, filename, param) and get()
    TEST(ResourceHolderTest, LoadWithParameterAndGetRoundTrip)
    {
        ResourceHolder<MockResource, ResourceID> holder;

        // Load resource using load(id, filename, parameter)
        holder.load(ResourceID::First, "resource_param.txt", 42);

        // Get the resource and verify it was loaded correctly
        const MockResource& resource = holder.get(ResourceID::First);
        EXPECT_TRUE(resource.isLoaded());
        EXPECT_EQ(resource.getFileName(), "resource_param.txt");
        EXPECT_EQ(resource.getParameter(), "42");
    }

    // Test: Error handling for load(id, filename) with invalid file
    TEST(ResourceHolderTest, LoadErrorHandling)
    {
        ResourceHolder<MockResource, ResourceID> holder;

        // Attempt to load an invalid file should throw
        EXPECT_THROW(
            holder.load(ResourceID::First, "invalid.txt"),
            std::runtime_error
        );
    }

    // Test: Error handling for load(id, filename, param) with invalid file
    TEST(ResourceHolderTest, LoadWithParameterErrorHandling)
    {
        ResourceHolder<MockResource, ResourceID> holder;

        // Attempt to load an invalid file should throw
        EXPECT_THROW(
            holder.load(ResourceID::First, "invalid_param.txt", 99),
            std::runtime_error
        );
    }

    TEST(ResourceHolderDeathTest, AssertsOnMissingResource) 
    {
        GTEST_FLAG_SET(death_test_style, "threadsafe");

        ResourceHolder<sf::Texture, ResourceID> textures;
        
        // We expect the program to 'die' when we call get() for a missing ID
        EXPECT_DEBUG_DEATH({
            const auto& first = textures.get(ResourceID::First);
        }, "");
        
        EXPECT_DEBUG_DEATH({
            auto first = textures.get(ResourceID::First);
        }, ""); 
    }
}