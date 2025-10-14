cmake_minimum_required(VERSION 3.23)

project(tilecut)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/bin)

include(ka_common)

option(BUILD_GENERATE_GRID "Build code generation utility using GNU MPFR" ON)

add_subdirectory(src/exact)
if(BUILD_GENERATE_GRID)
    add_subdirectory(src/generate_grid)
endif()
add_subdirectory(src/geometry_types)
add_subdirectory(src/tilecut)
