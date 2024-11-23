set(module_name geometry_types)

add_library(ka_${module_name} INTERFACE)
add_library(ka::${module_name} ALIAS ka_${module_name})

target_sources(ka_${module_name}
    INTERFACE
    FILE_SET HEADERS
    BASE_DIRS include
    FILES
        include/ka/geometry_types/Segment2.hpp
        include/ka/geometry_types/Vec2.hpp
)

target_link_libraries(ka_${module_name}
    INTERFACE
        ka::common
)

install(TARGETS ka_${module_name}
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin
    FILE_SET HEADERS DESTINATION include
)
