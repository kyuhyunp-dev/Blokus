
# Define library to be shared with tests
add_library(BlokusSharedLib STATIC 
    polyomino_util.cpp
    polyomino_loader.cpp
)

# Link them so anyone using BlokusSharedLib also gets the headers
target_link_libraries(BlokusSharedLib PUBLIC
    PolyominoSchemaTarget
    flatbuffers::flatbuffers
    project_configs 
)

target_include_directories(BlokusSharedLib PUBLIC 
    "${CMAKE_SOURCE_DIR}/include"
    "${CMAKE_BINARY_DIR}/PolyominoSchemaTarget" 
    "$<TARGET_PROPERTY:PolyominoSchemaTarget,BINARY_DIR>"
)

