find_package(Boost REQUIRED)
find_package(mpfr REQUIRED)

add_executable(TilecutExact_Gen
    generate.cpp
)

target_enable_warnings(TilecutExact_Gen)

target_link_libraries(TilecutExact_Gen
    PRIVATE
        r7common
        Boost::program_options
        mpfr::mpfr
)

set(tilecut_exact_include_dir ${CMAKE_CURRENT_BINARY_DIR}/include)
set(tilecut_exact_generated_dir ${tilecut_exact_include_dir}/exact/generated)
file(MAKE_DIRECTORY ${tilecut_exact_generated_dir})
set(tilecut_exact_generated_constants ${tilecut_exact_generated_dir}/constants.hpp)

add_custom_command(
    OUTPUT ${tilecut_exact_generated_constants}
    COMMAND TilecutExact_Gen ARGS --output ${tilecut_exact_generated_constants}
    DEPENDS TilecutExact_Gen
)

add_library(TilecutExact_Generated
    INTERFACE ${tilecut_exact_generated_constants}
)

target_include_directories(TilecutExact_Generated INTERFACE ${tilecut_exact_include_dir})
