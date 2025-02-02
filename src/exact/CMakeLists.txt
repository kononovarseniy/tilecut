set(module_name exact)

add_library(ka_${module_name})
add_library(ka::${module_name} ALIAS ka_${module_name})

target_sources(ka_${module_name}
    PUBLIC
    FILE_SET HEADERS
    BASE_DIRS include
    FILES
        include/ka/exact/grid.hpp
        include/ka/exact/GridParameters.hpp
        include/ka/exact/GridRounding.hpp

    PRIVATE
        src/grid/border_between_coordinates.cpp
        src/grid/border_intersection.cpp
        src/grid/cell_containg_position.cpp
        src/grid/common.hpp
        src/grid/line_intersects_cell.cpp

        src/expansion.hpp
)

target_enable_warnings(ka_${module_name})

target_link_libraries(ka_${module_name}
    PUBLIC
        ka::common
        ka::geometry_types
)

install(TARGETS ka_${module_name}
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin
    FILE_SET HEADERS DESTINATION include
)

if(BUILD_TESTS)
    find_package(fmt REQUIRED)
    find_package(mpfr REQUIRED)

    add_executable(ka_${module_name}_test
        test/check_column_border_intersection.cpp
        test/check_column_border_intersection.hpp
        test/mock_grid_parameters.hpp
        test/test_check_column_border_intersection.cpp
        test/test_column_border_intersection.cpp
    )
    target_enable_warnings(ka_${module_name}_test)
    target_link_libraries(ka_${module_name}_test
        PRIVATE
            ka::common
            ka::exact
            ka::geometry_types
            fmt::fmt
            gtest::gtest
            mpfr::mpfr
    )

    gtest_discover_tests(ka_${module_name}_test)
endif()
