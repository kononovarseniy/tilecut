set(module_name generate_grid)

find_package(Boost REQUIRED)
find_package(mpfr REQUIRED)

add_executable(ka_${module_name})
add_executable(ka::${module_name} ALIAS ka_${module_name})

target_sources(ka_${module_name}
    PRIVATE
        src/main.cpp
)

target_enable_warnings(ka_${module_name})

target_link_libraries(ka_${module_name}
    PRIVATE
        ka::common
        Boost::program_options
        mpfr::mpfr
)

install(TARGETS ka_${module_name}
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin
    FILE_SET HEADERS DESTINATION include
)
