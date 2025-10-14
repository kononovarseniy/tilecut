ka_target(current_target LIBRARY)

target_sources(${current_target}
    PUBLIC
    FILE_SET HEADERS
    BASE_DIRS include
    FILES
        include/ka/exact/grid.hpp
        include/ka/exact/GridParameters.hpp
        include/ka/exact/GridRounding.hpp
        include/ka/exact/orientation.hpp

    PRIVATE
        src/grid/border_between_coordinates.cpp
        src/grid/border_intersection.cpp
        src/grid/cell_containg_position.cpp
        src/grid/line_intersects_cell.cpp

        src/common.hpp
        src/expansion.hpp
        src/orientation.cpp
)

find_package(ka_common CONFIG REQUIRED)

target_link_libraries(${current_target}
    PUBLIC
        ka::common
        ka::geometry_types
)

if(BUILD_TESTING)
    ka_gtest_target(${current_target}_test
        SOURCES
            test/check_column_border_intersection.cpp
            test/check_column_border_intersection.hpp
            test/mock_grid_parameters.hpp
            test/test_check_column_border_intersection.cpp
            test/test_column_border_intersection.cpp
            test/test_orientation.cpp
    )

    find_package(fmt REQUIRED)
    find_package(mpfr REQUIRED)

    target_link_libraries(${current_target}_test
        PRIVATE
            fmt::fmt
            mpfr::mpfr
            ka::common
            ka::exact
            ka::geometry_types
    )
endif()
