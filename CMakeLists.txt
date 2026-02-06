cmake_minimum_required(VERSION 3.28)
project(Blokus LANGUAGES CXX)

# specify the C++ standard
add_library(project_configs INTERFACE)
target_compile_features(project_configs INTERFACE cxx_std_17)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# ---------------- Dependencies ----------------
include(FetchContent)

# Fetch SFML
FetchContent_Declare(SFML
    GIT_REPOSITORY https://github.com/SFML/SFML.git
    GIT_TAG 3.0.1
    GIT_SHALLOW ON
    EXCLUDE_FROM_ALL
    SYSTEM)
FetchContent_MakeAvailable(SFML)

# Fetch spdlog
FetchContent_Declare(spdlog
    GIT_REPOSITORY https://github.com/gabime/spdlog.git
    GIT_TAG v1.14.1   # latest as of 2025
    GIT_SHALLOW ON
    EXCLUDE_FROM_ALL
    SYSTEM)
FetchContent_MakeAvailable(spdlog)

# Fetch flatbuffers
FetchContent_Declare(flatbuffers
  GIT_REPOSITORY https://github.com/google/flatbuffers.git
  GIT_TAG        v23.5.26
  GIT_SHALLOW    ON       
  EXCLUDE_FROM_ALL            
  SYSTEM                          
)

FetchContent_MakeAvailable(flatbuffers)

# Manually create the alias if it doesn't exist
if(NOT TARGET flatbuffers::flatbuffers)
    add_library(flatbuffers::flatbuffers ALIAS flatbuffers)
endif()

# All executables → bin/, all libraries → lib/
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# Apply also to multi-config generators (MSVC, Xcode)
foreach(OUTPUTCONFIG IN LISTS CMAKE_CONFIGURATION_TYPES)
    string(TOUPPER ${OUTPUTCONFIG} OUTPUTCONFIG_UPPER)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_${OUTPUTCONFIG_UPPER} ${CMAKE_BINARY_DIR}/bin)
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_${OUTPUTCONFIG_UPPER} ${CMAKE_BINARY_DIR}/lib)
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_${OUTPUTCONFIG_UPPER} ${CMAKE_BINARY_DIR}/lib)
endforeach()

set(CMAKE_SKIP_BUILD_RPATH FALSE)
set(CMAKE_BUILD_WITH_INSTALL_RPATH FALSE)
set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)
set(CMAKE_INSTALL_RPATH "$ORIGIN")

# ---- assets ----

# Generate the headers for pre-game polyominoes

list(APPEND CMAKE_MODULE_PATH ${flatbuffers_SOURCE_DIR}/CMake)
include(BuildFlatBuffers)

# Generate the C++ headers from the FlatBuffers schema
flatbuffers_generate_headers(
    TARGET PolyominoSchemaTarget
    SCHEMAS "${CMAKE_SOURCE_DIR}/schema/polyomino.fbs"
)

target_include_directories(PolyominoSchemaTarget 
    INTERFACE "${CMAKE_CURRENT_BINARY_DIR}"
)

# ---------------- Subdirectories ----------------
# Add Shared Library
add_subdirectory(src/shared)

# Add Scripts Directory for pre-game generation
add_subdirectory(scripts)

# Add Source Directory (defines BlokusClient, BlokusTests)
add_subdirectory(src/client)
add_subdirectory(tests)


# ---- Testing ----
include(CTest)
enable_testing()
