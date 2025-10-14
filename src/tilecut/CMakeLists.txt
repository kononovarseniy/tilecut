ka_target(current_target LIBRARY)

target_sources(${current_target}
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
        include/ka/tilecut/polygon_orientation.hpp
        include/ka/tilecut/snap_round.hpp
        include/ka/tilecut/sort_hot_pixels_along_segment.hpp
        include/ka/tilecut/TileCellGrid.hpp
        include/ka/tilecut/TileGrid.hpp

    PRIVATE
        src/collect_tiles.cpp
        src/filter_segments.cpp
        src/find_cuts.cpp
)

find_package(ka_common CONFIG REQUIRED)

target_link_libraries(${current_target}
    PUBLIC
        ka::common
        ka::exact
        ka::geometry_types
)

if(BUILD_TESTING)
    ka_gtest_target(${current_target}_test
        SOURCES
            test/debug_output.hpp
            test/mock_grid_parameters.hpp
            test/test_cut_polyline.cpp
            test/test_find_cuts.cpp
            test/test_lerp_along_segment.cpp
            test/test_line_snapper.cpp
            test/test_snap_rounding.cpp
            test/test_orient.cpp
            test/test_polygon_orientation.cpp
            test/test_sort_hot_pixels_along_segment.cpp
            test/test_tile_cell_grid.cpp
            test/test_tile_grid.cpp
    )

    find_package(fmt CONFIG REQUIRED)

    target_link_libraries(${current_target}_test
        PRIVATE
            fmt::fmt
            ka::common
            ka::geometry_types
            ka::tilecut
    )
endif()
