cmake_minimum_required(VERSION 3.28)
project(Blokus LANGUAGES CXX)

# specify the C++ standard
add_library(tutorial_compiler_flags INTERFACE)
target_compile_features(tutorial_compiler_flags INTERFACE cxx_std_17)

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
FetchContent_Declare(
    spdlog
    GIT_REPOSITORY https://github.com/gabime/spdlog.git
    GIT_TAG v1.14.1   # latest as of 2025
    GIT_SHALLOW ON
    SYSTEM)
FetchContent_MakeAvailable(spdlog)

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

# ---- Assets ----
set(ASSETS_DIR "${CMAKE_SOURCE_DIR}/Assets")

# Add Source Directory (defines BlokusLibrary, BlokusTests)
add_subdirectory(Source)
add_subdirectory(Test)

# ---- Main executable ----
add_executable(Blokus Source/Main.cpp)
target_link_libraries(Blokus PRIVATE BlokusLibrary)


# Copy Assets after build (so running from build works)
add_custom_command(
    TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${ASSETS_DIR}
            $<TARGET_FILE_DIR:${PROJECT_NAME}>/Assets
)

add_custom_command(TARGET Blokus POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        $<TARGET_FILE:BlokusLibrary>
        $<TARGET_FILE_DIR:Blokus>)

# Install
set(CMAKE_INSTALL_PREFIX "${CMAKE_SOURCE_DIR}/install")

# Also install Assets (so running from install works)
install(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION .)
install(DIRECTORY ${ASSETS_DIR}/ DESTINATION Assets)

# ---- Testing ----
include(CTest)
enable_testing()
