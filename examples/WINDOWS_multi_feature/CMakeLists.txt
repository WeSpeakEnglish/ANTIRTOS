cmake_minimum_required(VERSION 3.28)

# Create the Project
project(antirtos_win32_static VERSION 1.0 LANGUAGES CXX)

# Setup source and header files
set (SOURCES
    "WINDOWS_multi_feature.cpp"
)
set (HEADERS
    "WINDOWS_multi_feature.hpp"
    "../../antirtos.h"
)
# setup directories
set (DIRECTORIES
    "./"
    "../../"
)

if(CMAKE_CONFIGURATION_TYPES)
    message(STATUS "Multi-config build! Configurations: ${CMAKE_CONFIGURATION_TYPES}")
    # Final output directory
    set(APP_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/$<CONFIG>")
else()
    message(STATUS "Single-config build!")
    # Final output directory
    set(APP_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
endif()

# Set output directory for everything
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${APP_OUTPUT_DIRECTORY}")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${APP_OUTPUT_DIRECTORY}")
set(CMAKE_PDB_OUTPUT_DIRECTORY "${APP_OUTPUT_DIRECTORY}")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${APP_OUTPUT_DIRECTORY}")

###################################
#       APPLICATION
###################################
# Add a test executable
add_executable(antirtos_win32_static ${SOURCES} ${HEADERS})

# Include directories
target_include_directories(antirtos_win32_static
    PUBLIC
    ${DIRECTORIES}
)

set_target_properties(antirtos_win32_static PROPERTIES
    CXX_STANDARD 11
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF)

