add_library(TilecutExact
    include/exact/grid.hpp

    src/expansion.hpp
    src/grid.cpp
)

target_include_directories(TilecutExact PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

target_enable_warnings(TilecutExact)

target_link_libraries(TilecutExact
    PUBLIC
        r7common
)
