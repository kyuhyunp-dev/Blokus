# Define library to be shared with tests
add_library(BlokusClientLib STATIC
Command/Command.cpp
Command/CommandQueue.cpp
Nodes/Arena.cpp
Nodes/BlockNode.cpp
Nodes/BoardNode.cpp
Nodes/PieceNode.cpp
Nodes/SceneNode.cpp
Nodes/TrayNode.cpp
Game.cpp
Path.cpp
Player.cpp 
StatisticsTracker.cpp
)

# Link dependencies
target_link_libraries(BlokusClientLib PUBLIC
    BlokusSharedLib
    project_configs
)

# Include directories
target_include_directories(BlokusClientLib PUBLIC 
    "${CMAKE_SOURCE_DIR}/include/client"
)

# Define client executable
add_executable(BlokusClient main.cpp)

# Link dependencies
target_link_libraries(BlokusClient PRIVATE
    BlokusClientLib
)

# Define PIECES_PATH and ASSETS_PATH
target_compile_definitions(BlokusClient PRIVATE 
)

# Assets copy (Portability)
# This copies the client-specific assets to the build folder automatically
add_custom_command(
    TARGET BlokusClient POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_SOURCE_DIR}/assets"
        "$<TARGET_FILE_DIR:BlokusClient>/assets"
    COMMENT "Deploying all assets to build directory..."
)

# Install
install(TARGETS BlokusClient RUNTIME DESTINATION bin)

