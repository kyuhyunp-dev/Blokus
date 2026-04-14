#include <string>


class MockResource
{
public:
    MockResource() 
    : mLoaded(false)
    {}

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

    bool isLoaded() const { return mLoaded; }
    const std::string& getFileName() const { return mFileName; }
    const std::string& getParameter() const { return mParameter; }

private:
    bool mLoaded;
    std::string mFileName;
    std::string mParameter;
};

 // Identifier enum for testing
enum class ResourceID
{
    First = 0 
};