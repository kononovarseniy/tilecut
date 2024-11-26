cmake_minimum_required(VERSION 3.23)

project(tilecut)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/bin)

option(GENERATE_GRID "Build using GNU MPFR for code generation" ON)
option(BUILD_TESTS "Build tests" ON)

if(BUILD_TESTS)
    find_package(GTest REQUIRED)
    include(GoogleTest)
    enable_testing()
endif()

function(target_enable_warnings TARGET_NAME)
    target_compile_options(${TARGET_NAME} PRIVATE
        $<$<CXX_COMPILER_ID:MSVC>:/W4 /WX>
        $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall -Wextra -Wpedantic -Werror>
    )
endfunction()

add_subdirectory(kononovarseniy/common)
add_subdirectory(src/exact)
add_subdirectory(src/geometry_types)
add_subdirectory(src/tilecut)
