ka_target(current_target EXECUTABLE)

target_sources(${current_target}
    PRIVATE
        src/main.cpp
        src/MPFRDouble.hpp
)

find_package(fmt REQUIRED)
find_package(Boost REQUIRED)
find_package(mpfr REQUIRED)
find_package(ka_common CONFIG REQUIRED)

target_link_libraries(${current_target}
    PRIVATE
        fmt::fmt
        Boost::program_options
        mpfr::mpfr
        ka::common
)
