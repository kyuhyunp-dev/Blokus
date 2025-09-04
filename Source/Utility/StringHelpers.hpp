#ifndef UTILITY_STRINGHELPERS_HPP
#define UTILITY_STRINGHELPERS_HPP

#include <sstream>

// Since std::to_string doesn't work on MinGW we have to implement
// our own to support all platforms.
template <typename T>
std::string toString(const T& value);

#include "Utility/StringHelpers.inl"
#endif