#include "Resource/Path.hpp"

#include <filesystem>
namespace fs = std::filesystem;

#include <iostream>


// Static variable to hold the root discovered at runtime
static fs::path ResourceRoot;

void initializeResourceRoot(const char* argv0) {
    // fs::absolute(argv0) turns "./bin/BlokusClient" into "/home/user/Blokus/build/bin/BlokusClient"
    // .parent_path() leaves us with "/home/user/Blokus/build/bin/"
    ResourceRoot = fs::absolute(argv0).parent_path() / "assets";
}


// The "Safe" way to get assets
std::string getAssetPath(const std::string& filename) {
    return (ResourceRoot / filename).string();
}