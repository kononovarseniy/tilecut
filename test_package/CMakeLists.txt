cmake_minimum_required(VERSION 3.23)

project(example)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(ka_tilecut REQUIRED COMPONENTS exact geometry_types tilecut)

add_executable(example main.cpp embedded_grid.hpp)
target_link_libraries(example
    PRIVATE
        ka::exact
        ka::geometry_types
        ka::tilecut
)
