import os

from conan import ConanFile
from conan.tools.cmake import CMakeToolchain, CMake, cmake_layout, CMakeDeps
from conan.tools.build import check_min_cppstd
from conan.tools.files import copy


class TilecutRecipe(ConanFile):
    name = "ka_tilecut"
    version = "0.1"

    license = "MIT"
    author = "kononovarseniy@gmail.com"
    url = "https://github.com/kononovarseniy/tilecut"
    description = "Algorithms for cutting cartographic data into tiles"

    settings = "os", "compiler", "build_type", "arch"

    options = {
        "build_tests": [True, False],
        "generate_grid": [True, False],
    }

    default_options = {
        "build_tests": True,
        "generate_grid": True,
    }

    def export_sources(self):
        copy(self, "CMakeLists.txt", self.recipe_folder, self.export_sources_folder)
        copy(self, "kononovarseniy/*", self.recipe_folder, self.export_sources_folder)
        copy(self, "src/*", self.recipe_folder, self.export_sources_folder)
        copy(
            self,
            "embedded_grid.hpp",
            self.recipe_folder,
            os.path.join(self.export_sources_folder, "src", "exact", "include", "ka", "exact", "generated"),
        )

    def validate(self):
        check_min_cppstd(self, "20")

    def requirements(self):
        pass

    def build_requirements(self):
        if self.options.build_tests:
            self.test_requires("gtest/1.15.0")
        if self.options.generate_grid:
            self.requires("boost/1.86.0")
        if self.options.build_tests or self.options.generate_grid:
            self.requires("mpfr/4.2.1")

    def layout(self):
        cmake_layout(self)

    def generate(self):
        deps = CMakeDeps(self)
        deps.generate()
        toolchain = CMakeToolchain(self)
        toolchain.variables["BUILD_TESTS"] = bool(self.options.build_tests)
        toolchain.variables["GENERATE_GRID"] = bool(self.options.generate_grid)
        toolchain.generate()

    def build(self):
        cmake = CMake(self)
        cmake.configure()
        cmake.build()
        if self.options.build_tests:
            cmake.test()

    def package(self):
        cmake = CMake(self)
        cmake.install()

    def package_info(self):
        self.cpp_info.components["ka_exact"].libs = ["ka_exact"]
        self.cpp_info.components["ka_exact"].set_property("cmake_target_name", "ka::exact")

        self.cpp_info.components["ka_geometry_types"].bindirs = []
        self.cpp_info.components["ka_geometry_types"].libdirs = []
        self.cpp_info.components["ka_geometry_types"].set_property("cmake_target_name", "ka::geometry_types")

        self.cpp_info.components["ka_tilecut"].libs = ["ka_tilecut"]
        self.cpp_info.components["ka_tilecut"].set_property("cmake_target_name", "ka::tilecut")
