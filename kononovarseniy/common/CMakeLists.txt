cmake_minimum_required(VERSION 3.19)

project(r7common)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/bin)

include(CTest)

include(cmake/warnings.cmake)

add_library(r7common
    include/common/assert.hpp
    include/common/cast.hpp
    include/common/fixed.hpp
    include/common/hash.hpp
    include/common/log.hpp

    src/log.cpp
)

find_package(fmt REQUIRED)

target_include_directories(r7common PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

target_enable_warnings(r7common)

target_link_libraries(r7common
    PUBLIC
        fmt::fmt
)
