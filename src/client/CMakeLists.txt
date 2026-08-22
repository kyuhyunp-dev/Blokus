# Define library to be shared with tests
add_library(BlokusClientLib STATIC
Command/Command.cpp
Command/CommandQueue.cpp
GUI/Button.cpp
GUI/Component.cpp
GUI/Container.cpp
GUI/Label.cpp
GUI/TextBox.cpp
Network/NetworkClient.cpp
Nodes/Arena.cpp
Nodes/BlockNode.cpp  
Nodes/BoardNode.cpp
Nodes/PieceNode.cpp
Nodes/SceneNode.cpp
Nodes/TrayNode.cpp
States/LobbyState.cpp
States/NetworkGameState.cpp
States/State.cpp
States/StateStack.cpp
States/TitleState.cpp
States/UsernameState.cpp
ClientUtility.cpp
Application.cpp
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
add_executable(BlokusClient Main.cpp)

# Link dependencies
target_link_libraries(BlokusClient PRIVATE
    BlokusClientLib
)

# Install
install(TARGETS BlokusClient RUNTIME DESTINATION bin)

