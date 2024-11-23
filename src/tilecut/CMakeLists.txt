set(module_name tilecut)

add_library(ka_${module_name})
add_library(ka::${module_name} ALIAS ka_${module_name})

target_sources(ka_${module_name}
    PUBLIC
    FILE_SET HEADERS
    BASE_DIRS include
    FILES
        include/ka/tilecut/collect_tiles.hpp
        include/ka/tilecut/filter_segments.hpp
        include/ka/tilecut/find_cuts.hpp

    PRIVATE
        src/collect_tiles.cpp
        src/filter_segments.cpp
        src/find_cuts.cpp
        src/orient.hpp
)

target_enable_warnings(ka_${module_name})

target_link_libraries(ka_${module_name}
    PUBLIC
        ka::common
        ka::geometry_types
)

install(TARGETS ka_${module_name}
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin
    FILE_SET HEADERS DESTINATION include
)
