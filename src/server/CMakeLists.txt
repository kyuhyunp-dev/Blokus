# Define library to be shared with tests
add_library(BlokusServerLib STATIC
    GameServer.cpp
    # Add other server-specific source files here as you create them
    # e.g., Network/ClientProxy.cpp, GameLogic/Authority.cpp
)

# Link dependencies
# This gives the server access to your shared packets, enums, and SFML Network
target_link_libraries(BlokusServerLib PUBLIC
    BlokusSharedLib
    project_configs
)

# Include directories
target_include_directories(BlokusServerLib PUBLIC 
    "${CMAKE_SOURCE_DIR}/include/server"
)

# Define server executable
add_executable(BlokusServer Main.cpp)

# Link dependencies
target_link_libraries(BlokusServer PRIVATE
    BlokusServerLib
)

# Install
install(TARGETS BlokusServer RUNTIME DESTINATION bin)