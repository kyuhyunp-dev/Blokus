# All executables defined in the scripts/ directory 
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/scripts)

# Set PIECES_PATH
set(PIECES_PATH "${CMAKE_SOURCE_DIR}/assets/shared/pieces.bin")

# Define the executable 
add_executable(generate_polyominoes PolyominoGeneration.cpp)

# Link the libraries
target_link_libraries(generate_polyominoes PRIVATE 
    BlokusSharedLib 
)

# Passing where to save binary as argument
add_custom_command(
    OUTPUT "${PIECES_PATH}"
    COMMAND ${CMAKE_COMMAND} -E make_directory "${CMAKE_SOURCE_DIR}/assets/shared"
    COMMAND $<TARGET_FILE:generate_polyominoes> "${PIECES_PATH}"
    DEPENDS generate_polyominoes "${CMAKE_SOURCE_DIR}/schema/polyomino.fbs"
    COMMENT "Generating pieces.bin because source or schema changed..."
    VERBATIM
)

# Create a target so you can actually trigger this recipe
add_custom_target(generate_assets ALL DEPENDS "${PIECES_PATH}")