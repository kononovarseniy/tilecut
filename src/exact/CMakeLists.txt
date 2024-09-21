add_subdirectory(gen)

add_library(TilecutExact
    include/exact/grid.hpp

    src/expansion.hpp
    src/grid.cpp
)

target_include_directories(TilecutExact PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

target_enable_warnings(TilecutExact)

target_link_libraries(TilecutExact
    PUBLIC
        TilecutExact_Generated
    PRIVATE
        r7common
)


find_package(mpfr REQUIRED)

add_executable(TilecutExact_Test
    test/check_column_border_intersection.cpp
    test/check_column_border_intersection.hpp
    test/test_check_column_border_intersection.cpp
    test/test_column_border_intersection.cpp
)
target_enable_warnings(TilecutExact_Test)
target_link_libraries(TilecutExact_Test
    PRIVATE
        r7common
        TilecutExact
        gtest::gtest
        mpfr::mpfr
)

gtest_discover_tests(TilecutExact_Test)
