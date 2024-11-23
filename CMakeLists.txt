cmake_minimum_required(VERSION 3.19)

project(ka_common)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/bin)

include(CTest)

include(cmake/warnings.cmake)

add_library(ka_common
    include/ka/common/assert.hpp
    include/ka/common/cast.hpp
    include/ka/common/fixed.hpp
    include/ka/common/hash.hpp
    include/ka/common/log.hpp

    src/log.cpp
)

find_package(fmt REQUIRED)

target_include_directories(ka_common PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

target_enable_warnings(ka_common)

target_link_libraries(ka_common
    PUBLIC
        fmt::fmt
)

add_library(ka::common ALIAS ka_common)
