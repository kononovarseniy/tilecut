cmake_minimum_required(VERSION 3.23)

project(example)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(ka_tilecut REQUIRED)

add_executable(example main.cpp)
target_link_libraries(example PRIVATE ka::tilecut)
