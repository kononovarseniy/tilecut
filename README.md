# kononovarseniy/common

The repository contains some functions and types that are frequently used in my C++ projects.

# Building the library

Execute these commands before the first build and rerun them after adding new dependencies.

`conan install . --build=missing --output-folder=./build/Release -s build_type=Release`
`conan install . --build=missing --output-folder=./build/Debug -s build_type=Debug`

Now you can use CMake presets `conan-release` and `conan-debug`.
