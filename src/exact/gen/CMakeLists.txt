set(module_name exact_generated)

set(embedded_grid_file ka/exact/generated/embedded_grid.hpp)

if (GENERATE_GRID)
    find_package(Boost REQUIRED)
    find_package(mpfr REQUIRED)

    add_executable(ka_${module_name}_generator)
    add_executable(ka::${module_name}_generator ALIAS ka_${module_name}_generator)

    target_sources(ka_${module_name}_generator
        PRIVATE
            generate.cpp
    )

    target_enable_warnings(ka_${module_name}_generator)

    target_link_libraries(ka_${module_name}_generator
        PRIVATE
            ka::common
            Boost::program_options
            mpfr::mpfr
    )

    set(generated_headers_base_dir ${CMAKE_CURRENT_BINARY_DIR}/include)
    set(generated_constants_file ${generated_headers_base_dir}/${embedded_grid_file})

    add_custom_command(
        OUTPUT ${generated_constants_file}
        COMMAND ${CMAKE_COMMAND} -E make_directory $<PATH:GET_PARENT_PATH,${generated_constants_file}>
        COMMAND ka::${module_name}_generator --output ${generated_constants_file}
        DEPENDS ka::${module_name}_generator
    )
else()
    set(generated_headers_base_dir ${CMAKE_CURRENT_SOURCE_DIR}/../include)
    set(generated_constants_file ${generated_headers_base_dir}/${embedded_grid_file})
endif()

add_library(ka_${module_name} INTERFACE
    # Why?!
    ${generated_constants_file}
)
add_library(ka::${module_name} ALIAS ka_${module_name})

target_sources(ka_${module_name}
    INTERFACE
    FILE_SET HEADERS
    BASE_DIRS ${generated_headers_base_dir}
    FILES
        ${generated_constants_file}
)

install(TARGETS ka_${module_name}
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin
    FILE_SET HEADERS DESTINATION include
)
