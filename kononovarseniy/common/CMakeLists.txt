cmake_minimum_required(VERSION 3.23)

project(ka_common)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/bin)

include(CTest)

include(cmake/warnings.cmake)

set(module_name common)

add_library(ka_${module_name})
add_library(ka::${module_name} ALIAS ka_${module_name})

target_sources(ka_${module_name}
    PUBLIC
    FILE_SET HEADERS
    BASE_DIRS include
    FILES
        include/ka/common/assert.hpp
        include/ka/common/cast.hpp
        include/ka/common/fixed.hpp
        include/ka/common/hash.hpp
        include/ka/common/log.hpp

    PRIVATE
        src/log.cpp
)

find_package(fmt REQUIRED)

target_link_libraries(ka_${module_name}
    PRIVATE
        fmt::fmt
)

target_enable_warnings(ka_${module_name})

install(TARGETS ka_${module_name}
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin
    FILE_SET HEADERS DESTINATION include
)
