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
        include/ka/tilecut/HotPixelCollector.hpp
        include/ka/tilecut/HotPixelIndex.hpp
        include/ka/tilecut/HotPixelOrder.hpp
        include/ka/tilecut/snap_round.hpp

    PRIVATE
        src/collect_tiles.cpp
        src/filter_segments.cpp
        src/find_cuts.cpp
        src/HotPixelCollector.cpp
        src/orient.hpp
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
    find_package(mpfr REQUIRED)

    add_executable(ka_${module_name}_test
        test/mock_grid_parameters.hpp
        test/test_snap_rounding.cpp
    )
    target_enable_warnings(ka_${module_name}_test)
    target_link_libraries(ka_${module_name}_test
        PRIVATE
            ka::common
            ka::geometry_types
            ka::tilecut
            gtest::gtest
    )

    gtest_discover_tests(ka_${module_name}_test)
endif()
