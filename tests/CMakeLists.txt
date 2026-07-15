# Fetch GoogleTest if not already available
FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG v1.15.2 # latest as of 2025
    GIT_SHALLOW ON
    EXCLUDE_FROM_ALL
    SYSTEM
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE) # For Windows compatibility

set(gtest_build_shared_libs OFF CACHE BOOL "" FORCE)
set(gmock_build_shared_libs OFF CACHE BOOL "" FORCE)

set(TEMP_BUILD_SHARED_LIBS ${BUILD_SHARED_LIBS})
set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(googletest)

set(BUILD_SHARED_LIBS ${TEMP_BUILD_SHARED_LIBS} CACHE BOOL "" FORCE)

# Make GoogleTest commands available to all subdirectories
include(GoogleTest)

# All executables defined in the tests/ directory 
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/tests)

# Route to the workers
add_subdirectory(shared)
add_subdirectory(server)
add_subdirectory(client)


