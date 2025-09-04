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
add_executable(BlokusTests
    Tracker/StatisticsTracker_UnitTest.cc
    Utility/StringHelpers_UnitTest.cc

    # add other test files here
)


target_link_libraries(BlokusTests
    PRIVATE
        BlokusLibrary
        GTest::gtest_main
        spdlog::spdlog
)

if(MSVC AND BUILD_SHARED_LIBS)
    add_custom_command(TARGET BlokusTests POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        $<TARGET_RUNTIME_DLLS:BlokusTests>
        $<TARGET_FILE_DIR:BlokusTests>
    COMMAND_EXPAND_LISTS
    )
endif()

# Discover and register tests automatically
include(GoogleTest)
gtest_discover_tests(BlokusTests
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)
