# Build the test executable
add_executable(BlokusClientTests
    StatisticsTrackerUnittest.cc
    Resource/ResourceHolderUnittest.cc
    Nodes/SceneNodeUnittest.cc
    Nodes/PieceNodeUnittest.cc
    Nodes/ArenaUnittest.cc
    Nodes/TrayNodeUnittest.cc
    Nodes/BoardNodeUnittest.cc     
    Command/CommandQueueUnittest.cc
    PlayerUnittest.cc
    # add other test files here
)

if(BUILD_SHARED_LIBS)
    target_compile_definitions(BlokusClientTests
        PRIVATE
            GTEST_LINKED_AS_SHARED_LIBRARY=1
    )
endif() 

target_link_libraries(BlokusClientTests
    PRIVATE
        BlokusClientLib
        GTest::gmock      
        GTest::gmock_main        
        spdlog::spdlog
)

# Include directories
target_include_directories(BlokusClientTests PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR}                # Your client source code
    ${CMAKE_CURRENT_SOURCE_DIR}/Mock # The Mock folder
)

set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

if(MSVC AND WIN32 AND BUILD_SHARED_LIBS)
    add_custom_command(TARGET BlokusClientTests POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        $<TARGET_RUNTIME_DLLS:BlokusClientTests>
        $<TARGET_FILE_DIR:BlokusClientTests>
    COMMAND_EXPAND_LISTS
    )
endif()

# Discover and register tests automatically
gtest_discover_tests(BlokusClientTests
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)