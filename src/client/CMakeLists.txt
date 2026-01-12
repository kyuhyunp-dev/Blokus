# Define library to be shared with tests
add_library(BlokusClientLib STATIC 
game.cpp 
statistics_tracker.cpp)

# Link dependencies
target_link_libraries(BlokusClientLib PUBLIC
    SFML::Graphics 
    SFML::Window
    SFML::System
    spdlog::spdlog
    project_configs
)

# Include directories
target_include_directories(BlokusClientLib PUBLIC 
    "${CMAKE_SOURCE_DIR}/include/client"
)

# Define ASSETS_PATH
target_compile_definitions(BlokusClientLib PRIVATE 
    ASSETS_PATH=\"${ASSETS_DIR}\")

# Define client executable
add_executable(BlokusClient main.cpp)

# Link dependencies
target_link_libraries(BlokusClient PRIVATE
    BlokusClientLib
)

# Assets copy (Portability)
# This copies the client-specific assets to the build folder automatically
add_custom_command(
    TARGET BlokusClient POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_SOURCE_DIR}/assets/client"
        "$<TARGET_FILE_DIR:BlokusClient>/assets/client"
    COMMENT "Deploying client assets to build directory..."
)

# Install
install(TARGETS BlokusClient RUNTIME DESTINATION bin)

