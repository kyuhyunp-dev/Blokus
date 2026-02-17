#include "Resource/ResourceHolder.hpp"

#include "gtest/gtest.h"
#include <memory>
#include <stdexcept>

namespace
{
    // Mock Resource class for testing
    class MockResource
    {
    public:
        MockResource() : mLoaded(false), mOpened(false) {}

        bool loadFromFile(const std::string& filename)
        {
            // Simulate failure for specific filenames
            if (filename == "invalid.txt")
                return false;
            mLoaded = true;
            mFileName = filename;
            return true;
        }

        template <typename Parameter>
        bool loadFromFile(const std::string& filename, const Parameter& param)
        {
            // Simulate failure for specific filenames
            if (filename == "invalid_param.txt")
                return false;
            mLoaded = true;
            mFileName = filename;
            mParameter = std::to_string(param);
            return true;
        }

        bool openFromFile(const std::string& filename)
        {
            // Simulate failure for specific filenames
            if (filename == "invalid_open.txt")
                return false;
            mOpened = true;
            mFileName = filename;
            return true;
        }

        bool isLoaded() const { return mLoaded; }
        bool isOpened() const { return mOpened; }
        const std::string& getFileName() const { return mFileName; }
        const std::string& getParameter() const { return mParameter; }

    private:
        bool mLoaded;
        bool mOpened;
        std::string mFileName;
        std::string mParameter;
    };

    // Identifier enum for testing
    enum class ResourceID
    {
        First = 0,
        Second = 1,
        Third = 2
    };

    // Test: Round-trip integrity test with open() and get()
    TEST(ResourceHolderTest, OpenAndGetRoundTrip)
    {
        ResourceHolder<MockResource, ResourceID> holder;

        // Load resource using open()
        holder.open(ResourceID::First, "test_file.txt");

        // Get the resource and verify it was loaded correctly
        const MockResource& resource = holder.get(ResourceID::First);
        EXPECT_TRUE(resource.isOpened());
        EXPECT_EQ(resource.getFileName(), "test_file.txt");
    }

    // Test: Round-trip integrity test with load(id, filename) and get()
    TEST(ResourceHolderTest, LoadAndGetRoundTrip)
    {
        ResourceHolder<MockResource, ResourceID> holder;

        // Load resource using load(id, filename)
        holder.load(ResourceID::Second, "resource.txt");

        // Get the resource and verify it was loaded correctly
        const MockResource& resource = holder.get(ResourceID::Second);
        EXPECT_TRUE(resource.isLoaded());
        EXPECT_EQ(resource.getFileName(), "resource.txt");
    }

    // Test: Round-trip integrity test with load(id, filename, param) and get()
    TEST(ResourceHolderTest, LoadWithParameterAndGetRoundTrip)
    {
        ResourceHolder<MockResource, ResourceID> holder;

        // Load resource using load(id, filename, parameter)
        holder.load(ResourceID::Third, "resource_param.txt", 42);

        // Get the resource and verify it was loaded correctly
        const MockResource& resource = holder.get(ResourceID::Third);
        EXPECT_TRUE(resource.isLoaded());
        EXPECT_EQ(resource.getFileName(), "resource_param.txt");
        EXPECT_EQ(resource.getParameter(), "42");
    }

    // Test: Error handling for open() with invalid file
    TEST(ResourceHolderTest, OpenErrorHandling)
    {
        ResourceHolder<MockResource, ResourceID> holder;

        // Attempt to open an invalid file should throw
        EXPECT_THROW(
            holder.open(ResourceID::First, "invalid_open.txt"),
            std::runtime_error
        );
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
}