set(module_name tilecut)

add_library(ka_${module_name})
add_library(ka::${module_name} ALIAS ka_${module_name})

target_sources(ka_${module_name}
    PUBLIC
    FILE_SET HEADERS
    BASE_DIRS include
    FILES
        include/ka/tilecut/collect_tiles.hpp
        include/ka/tilecut/cut_polyline.hpp
        include/ka/tilecut/filter_segments.hpp
        include/ka/tilecut/find_cuts.hpp
        include/ka/tilecut/hot_pixel_less.hpp
        include/ka/tilecut/HotPixelCollector.hpp
        include/ka/tilecut/HotPixelIndex.hpp
        include/ka/tilecut/HotPixelOrder.hpp
        include/ka/tilecut/lerp_along_segment.hpp
        include/ka/tilecut/LineSnapper.hpp
        include/ka/tilecut/LineSnapperCoordinateHandler.hpp
        include/ka/tilecut/orient.hpp
        include/ka/tilecut/snap_round.hpp
        include/ka/tilecut/sort_hot_pixels_along_segment.hpp
        include/ka/tilecut/TileCellGrid.hpp
        include/ka/tilecut/TileGrid.hpp

    PRIVATE
        src/collect_tiles.cpp
        src/filter_segments.cpp
        src/find_cuts.cpp
)

target_enable_warnings(ka_${module_name})

target_link_libraries(ka_${module_name}
    PUBLIC
        ka::common
        ka::exact
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
        test/debug_output.hpp
        test/mock_grid_parameters.hpp
        test/test_cut_polyline.cpp
        test/test_find_cuts.cpp
        test/test_lerp_along_segment.cpp
        test/test_line_snapper.cpp
        test/test_snap_rounding.cpp
        test/test_orient.cpp
        test/test_sort_hot_pixels_along_segment.cpp
        test/test_tile_cell_grid.cpp
        test/test_tile_grid.cpp
    )
    target_enable_warnings(ka_${module_name}_test)
    target_link_libraries(ka_${module_name}_test
        PRIVATE
            ka::common
            ka::geometry_types
            ka::tilecut
            fmt::fmt
            gtest::gtest
    )

    gtest_discover_tests(ka_${module_name}_test)
endif()
