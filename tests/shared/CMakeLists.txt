# Build the test executable
add_executable(BlokusSharedTests
    PolyominoLoaderUnittest.cc
    PolyominoUtilsUnittest.cc
    RefereeUnittest.cc
    # add other test files here

)

# Define the path clearly
set(PIECES_PATH "${CMAKE_SOURCE_DIR}/assets/shared/pieces.bin")

# Add the definition to your test target
target_compile_definitions(BlokusSharedTests PRIVATE 
    PIECES_BIN_PATH="${PIECES_PATH}"
)

# Ensure the assets are built before the tests run
add_dependencies(BlokusSharedTests generate_assets)

target_link_libraries(BlokusSharedTests
    PRIVATE
        BlokusSharedLib
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
