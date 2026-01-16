# Build the test executable
add_executable(BlokusSharedTests
    polyomino_loader_unittest.cc
    # add other test files here
)


target_link_libraries(BlokusSharedTests
    PRIVATE
        BlokusSharedLib
        PolyominoSchemaTarget
        GTest::gtest_main
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
