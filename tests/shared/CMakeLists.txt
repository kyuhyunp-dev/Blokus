# Library for my header-only Mocks
add_library(BlokusSharedMocks INTERFACE)

target_include_directories(BlokusSharedMocks INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}/..
)

# Build the test executable
add_executable(BlokusSharedTests
    PolyominoUtilsUnittest.cc
    RefereeUnittest.cc
    # add other test files here

)

target_link_libraries(BlokusSharedTests
    PRIVATE
        BlokusSharedLib
        BlokusSharedMocks
        GTest::gtest_main
        GTest::gmock      
        GTest::gmock_main
)

if(MSVC AND WIN32 AND BUILD_SHARED_LIBS)
    add_custom_command(TARGET BlokusSharedTests POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        $<TARGET_RUNTIME_DLLS:BlokusSharedTests>
        $<TARGET_FILE_DIR:BlokusSharedTests>
    COMMAND_EXPAND_LISTS
    )
endif()

# Discover and register tests automatically
gtest_discover_tests(BlokusSharedTests)
