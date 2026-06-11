# Define library to be shared with tests
add_library(BlokusSharedLib STATIC 
    PolyominoUtil.cpp
    Referee.cpp
)

# Link them so anyone using BlokusSharedLib also gets the headers
target_link_libraries(BlokusSharedLib PUBLIC
    SFML::Graphics 
    SFML::Window
    SFML::System
    SFML::Network
    spdlog::spdlog
    project_configs 
)

target_include_directories(BlokusSharedLib PUBLIC 
    "${CMAKE_SOURCE_DIR}/include"
)

