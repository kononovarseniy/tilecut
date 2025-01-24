cmake_minimum_required(VERSION 3.23)

project(example)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(ka_tilecut REQUIRED
    COMPONENTS
        exact
        generate_grid
        geometry_types
        tilecut
)

if(TARGET ka::generate_grid)
    set(generated_dir ${CMAKE_CURRENT_BINARY_DIR}/include)
    set(generated_file ${generated_dir}/ka_test/grid.hpp)

    execute_process(
        COMMAND ka_generate_grid
            --name=g_embedded_grid
            --world-cells=0x1p32
            --world-size=inexact:40075016.68
            --min-world-coordinate=inexact:0.005
            --max-world-coordinate=0x1p25
        OUTPUT_VARIABLE GENERATED_EMBEDDED_GRID
        COMMAND_ERROR_IS_FATAL ANY)
    configure_file(grid.hpp.in ${generated_file})
else()
    # Package ka_tilecut is build with option build_generate_grid=False
    set(generated_dir without_generate_grid)
    set(generated_file ${generated_dir}/ka_test/grid.hpp)
endif()

add_executable(example
    main.cpp
    ${generated_file}
)

target_include_directories(example PRIVATE ${generated_dir})

target_link_libraries(example
    PRIVATE
        ka::exact
        ka::geometry_types
        ka::tilecut
)
