# 1. Define the exact path where the header WILL be
set(GEN_HEADER "${CMAKE_BINARY_DIR}/PolyominoSchemaTarget/polyomino_generated.h")

# 2. Tell CMake this is a generated file
set_source_files_properties("${GEN_HEADER}" PROPERTIES GENERATED TRUE)

# Set SHARED_SOURCES that depend on GEN_HEADER
set(SHARED_SOURCES
    PolyominoLoader.cpp
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
    PolyominoSchemaTarget
    flatbuffers::flatbuffers
    project_configs 
)

target_include_directories(BlokusSharedLib PUBLIC 
    "${CMAKE_SOURCE_DIR}/include"
    "${CMAKE_BINARY_DIR}/PolyominoSchemaTarget" 
    "$<TARGET_PROPERTY:PolyominoSchemaTarget,BINARY_DIR>"
)

