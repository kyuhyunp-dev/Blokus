# Build the test executable
add_executable(BlokusServerTests
    GameServerUnittest.cc
)

# Handle shared library definitions for GTest
if(BUILD_SHARED_LIBS)
    target_compile_definitions(BlokusServerTests
        PRIVATE
            GTEST_LINKED_AS_SHARED_LIBRARY=1
    )
endif() 

# Link dependencies
target_link_libraries(BlokusServerTests
    PRIVATE
        BlokusServerLib     
        # BlokusSharedMocks # Uncomment if you have shared mock objects used by the server
        GTest::gmock      
        GTest::gmock_main        
        spdlog::spdlog
)

# Include directories
target_include_directories(BlokusServerTests PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR}                
)

# GTest specific setting
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

# Handle copying DLLs for Windows/MSVC environments
if(MSVC AND WIN32 AND BUILD_SHARED_LIBS)
    add_custom_command(TARGET BlokusServerTests POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        $<TARGET_RUNTIME_DLLS:BlokusServerTests>
        $<TARGET_FILE_DIR:BlokusServerTests>
    COMMAND_EXPAND_LISTS
    )
endif()

# Discover and register tests automatically with CTest
gtest_discover_tests(BlokusServerTests
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)