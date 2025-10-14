ka_target(current_target INTERFACE_LIBRARY)

target_sources(${current_target}
    PUBLIC
    FILE_SET HEADERS
    BASE_DIRS include
    FILES
        include/ka/geometry_types/Segment2.hpp
        include/ka/geometry_types/Vec2.hpp
)

find_package(ka_common CONFIG REQUIRED)

target_link_libraries(${current_target}
    INTERFACE
        ka::common
)
