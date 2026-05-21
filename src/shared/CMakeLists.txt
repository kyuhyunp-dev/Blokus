# Set SHARED_SOURCES that depend on GEN_HEADER
set(SHARED_SOURCES
    PolyominoUtil.cpp
    Referee.cpp
)

set_source_files_properties(${SHARED_SOURCES} PROPERTIES 
    OBJECT_DEPENDS "${GEN_HEADER}"
)
    
# Define library to be shared with tests
add_library(BlokusSharedLib STATIC 
    ${GEN_HEADER}
    ${SHARED_SOURCES}
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

