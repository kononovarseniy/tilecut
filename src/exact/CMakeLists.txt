add_library(TilecutExact
    include/exact/line_intersects_cell.hpp

    src/expansion.hpp
    src/line_intersects_cell.cpp
)

target_include_directories(TilecutExact PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

target_enable_warnings(TilecutExact)

target_link_libraries(TilecutExact
    PUBLIC
        r7common
)
