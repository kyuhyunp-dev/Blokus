add_library(BlokusLibrary
    Game/Game.cpp
    Tracker/StatisticsTracker.cpp
)

# For Assets 
target_compile_definitions(BlokusLibrary 
    PUBLIC 
    ASSETS_DIR=\"${ASSETS_DIR}\")

# state that anybody linking to us 
# needs to include the current source dir
target_include_directories(BlokusLibrary
    PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR}
)

# Dependencies
target_link_libraries(BlokusLibrary 
    PUBLIC 
    SFML::Graphics 
    SFML::System
    spdlog::spdlog
    tutorial_compiler_flags)

