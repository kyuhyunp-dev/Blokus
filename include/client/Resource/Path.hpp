#ifndef PATH_HPP
#define PATH_HPP

#include <string>


void initializeResourceRoot(const char* argv0);
// The "Safe" way to get assets
std::string getAssetPath(const std::string& filename);

#endif
