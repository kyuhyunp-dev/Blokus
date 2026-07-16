# Fetch GoogleTest if not already available
FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG v1.15.2 # latest as of 2025
    GIT_SHALLOW ON
    EXCLUDE_FROM_ALL
    SYSTEM
)

set(gtest_build_shared_libs OFF CACHE BOOL "" FORCE)
set(gmock_build_shared_libs OFF CACHE BOOL "" FORCE)

set(gtest_force_shared_crt ON CACHE BOOL "" FORCE) # For Windows compatibility

set(BUILD_SHARED_LIBS_BACKUP ${BUILD_SHARED_LIBS})
set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(googletest)

set(BUILD_SHARED_LIBS ${BUILD_SHARED_LIBS_BACKUP} CACHE BOOL "" FORCE)

# Forcefully strip the DLL expectation macro from all GoogleTest interface targets
foreach(tgt gtest gtest_main gmock gmock_main)
    if(TARGET ${tgt})
        get_target_property(defs ${tgt} INTERFACE_COMPILE_DEFINITIONS)
        if(defs)
            list(REMOVE_ITEM defs "GTEST_LINKED_AS_SHARED_LIBRARY=1")
            set_target_properties(${tgt} PROPERTIES INTERFACE_COMPILE_DEFINITIONS "${defs}")
        endif()

        target_compile_definitions(${tgt} INTERFACE GTEST_HAS_DLL=0)
    endif()
endforeach()

# Make GoogleTest commands available to all subdirectories
include(GoogleTest)

# All executables defined in the tests/ directory 
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/tests)

# Route to the workers
add_subdirectory(shared)
add_subdirectory(server)
add_subdirectory(client)


