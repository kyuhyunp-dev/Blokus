# Fetch GoogleTest if not already available
include(FetchContent)
FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG v1.15.2 # latest as of 2025
    GIT_SHALLOW ON
    SYSTEM
)
FetchContent_MakeAvailable(googletest)

# Build the test executable
add_executable(BlokusClientTests
    statistics_tracker_unittest.cc

    # add other test files here
)


target_link_libraries(BlokusClientTests
    PRIVATE
        BlokusClientLib
        GTest::gtest_main
        spdlog::spdlog
)

if(MSVC AND WIN32 AND BUILD_SHARED_LIBS)
    add_custom_command(TARGET BlokusClientTests POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        $<TARGET_RUNTIME_DLLS:BlokusClientTests>
        $<TARGET_FILE_DIR:BlokusClientTests>
    COMMAND_EXPAND_LISTS
    )
endif()

# Discover and register tests automatically
include(GoogleTest)
gtest_discover_tests(BlokusClientTests
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)
