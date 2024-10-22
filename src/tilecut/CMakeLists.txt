add_library(TilecutTilecut
    include/tilecut/collect_tiles.hpp
    include/tilecut/filter_segments.hpp
    include/tilecut/find_cuts.hpp

    src/collect_tiles.cpp
    src/filter_segments.cpp
    src/find_cuts.cpp
    src/orient.hpp
)

target_include_directories(TilecutTilecut PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

target_enable_warnings(TilecutTilecut)

target_link_libraries(TilecutTilecut
    PUBLIC
        TilecutGeometryTypes

        r7common
)
