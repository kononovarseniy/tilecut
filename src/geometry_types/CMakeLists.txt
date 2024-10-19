add_library(TilecutGeometryTypes INTERFACE
    include/geometry_types/Vec2.hpp
)

target_include_directories(TilecutGeometryTypes INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include)

# target_enable_warnings(TilecutGeometryTypes)

target_link_libraries(TilecutGeometryTypes
    INTERFACE
        r7common
)
