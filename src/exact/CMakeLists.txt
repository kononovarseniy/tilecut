add_subdirectory(gen)

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
        include/ka/exact/HotPixelCollector.hpp
        include/ka/exact/HotPixelIndex.hpp
        include/ka/exact/HotPixelOrder.hpp
        include/ka/exact/snap_round.hpp

    PRIVATE
        src/grid/border_between_coordinates.cpp
        src/grid/border_intersection.cpp
        src/grid/cell_containg_position.cpp
        src/grid/common.hpp
        src/grid/line_intersects_cell.cpp

        src/expansion.hpp
        src/HotPixelCollector.cpp
)

target_enable_warnings(ka_${module_name})

target_link_libraries(ka_${module_name}
    PUBLIC
        ka::common
        ka::exact_generated
        ka::geometry_types
)

install(TARGETS ka_${module_name}
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin
    FILE_SET HEADERS DESTINATION include
)

if(BUILD_TESTS)
    find_package(mpfr REQUIRED)

    add_executable(ka_${module_name}_test
        test/check_column_border_intersection.cpp
        test/check_column_border_intersection.hpp
        test/test_check_column_border_intersection.cpp
        test/test_column_border_intersection.cpp
        test/test_snap_rounding.cpp
    )
    target_enable_warnings(ka_${module_name}_test)
    target_link_libraries(ka_${module_name}_test
        PRIVATE
            ka::common
            ka::exact
            ka::geometry_types
            gtest::gtest
            mpfr::mpfr
    )

    gtest_discover_tests(ka_${module_name}_test)
endif()
