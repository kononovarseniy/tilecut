add_executable(Tilecut
    include/tilecut/orient.h
    include/tilecut/tilecut.h
    include/tilecut/uvec16.h

    src/tilecut.cpp
)

target_include_directories(Tilecut PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

target_enable_warnings(Tilecut)

target_link_libraries(Tilecut
    PUBLIC
        r7common
)
