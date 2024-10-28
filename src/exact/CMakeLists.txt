add_subdirectory(gen)

add_library(TilecutExact
    include/exact/grid.hpp
    include/exact/GridRounding.hpp
    include/exact/HotPixelCollector.hpp
    include/exact/HotPixelIndex.hpp
    include/exact/HotPixelOrder.hpp
    include/exact/snap_round.hpp

    src/grid/border_between_coordinates.cpp
    src/grid/border_intersection.cpp
    src/grid/cell_containg_position.cpp
    src/grid/common.hpp
    src/grid/line_intersects_cell.cpp

    src/expansion.hpp
    src/HotPixelCollector.cpp
)

target_include_directories(TilecutExact PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

target_enable_warnings(TilecutExact)

target_link_libraries(TilecutExact
    PUBLIC
        TilecutExact_Generated
        
        r7common
        TilecutGeometryTypes
)


find_package(mpfr REQUIRED)

add_executable(TilecutExact_Test
    test/check_column_border_intersection.cpp
    test/check_column_border_intersection.hpp
    test/test_check_column_border_intersection.cpp
    test/test_column_border_intersection.cpp
    test/test_snap_rounding.cpp
)
target_enable_warnings(TilecutExact_Test)
target_link_libraries(TilecutExact_Test
    PRIVATE
        r7common
        TilecutExact
        TilecutGeometryTypes
        gtest::gtest
        mpfr::mpfr
)

gtest_discover_tests(TilecutExact_Test)
