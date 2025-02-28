import os

from conan import ConanFile
from conan.tools.cmake import CMakeToolchain, CMake, cmake_layout, CMakeDeps
from conan.tools.build import check_min_cppstd
from conan.tools.files import copy


def _make_component_name(name):
    return f"ka_{name}"


def _make_target_name(name):
    return f"ka::{name}"


class TilecutRecipe(ConanFile):
    name = "ka_tilecut"
    version = "0.2.0"

    license = "MIT"
    author = "kononovarseniy@gmail.com"
    url = "https://github.com/kononovarseniy/tilecut"
    description = "Algorithms for cutting cartographic data into tiles"

    settings = "os", "compiler", "build_type", "arch"

    options = {
        "build_tests": [True, False],
        "build_generate_grid": [True, False],
    }

    default_options = {
        "build_tests": True,
        "build_generate_grid": True,
    }

    def export_sources(self):
        copy(self, "CMakeLists.txt", self.recipe_folder, self.export_sources_folder)
        copy(self, "kononovarseniy/*", self.recipe_folder, self.export_sources_folder)
        copy(self, "src/*", self.recipe_folder, self.export_sources_folder)

    def requirements(self):
        self.requires("fmt/9.1.0")

        if self.options.build_tests:
            self.test_requires("gtest/1.15.0")
        if self.options.build_generate_grid:
            self.requires("boost/1.86.0")
        if self.options.build_tests or self.options.build_generate_grid:
            self.requires("mpfr/4.2.1")

    def validate(self):
        check_min_cppstd(self, "20")

    def layout(self):
        cmake_layout(self)

        def component_layout(name, path=None):
            component_name = _make_component_name(name)
            if path is None:
                path = ["src", name]
            # Configuration for editatable package.
            self.cpp.source.components[component_name].includedirs = [os.path.join(*path, "include")]
            self.cpp.build.components[component_name].libdirs = [os.path.join(*path)]

        component_layout("common", ["kononovarseniy", "common"])
        component_layout("geometry_types")
        component_layout("exact")
        component_layout("tilecut")

    def generate(self):
        deps = CMakeDeps(self)
        deps.generate()
        toolchain = CMakeToolchain(self)
        toolchain.variables["BUILD_TESTS"] = bool(self.options.build_tests)
        toolchain.variables["BUILD_GENERATE_GRID"] = bool(self.options.build_generate_grid)
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
        def component_info(name, kind, requires=[]):
            component = self.cpp_info.components[_make_component_name(name)]
            component.requires = list(map(_make_component_name, requires))
            component.set_property("cmake_target_name", _make_target_name(name))

            if kind == "header":
                component.bindirs = []
                component.libdirs = []
            elif kind == "lib":
                component.libs = [_make_component_name(name)]
            elif kind == "bin":
                pass
            else:
                assert False

        component_info("common", kind="lib")
        component_info("geometry_types", kind="header", requires=["common"])
        component_info("exact", kind="lib", requires=["common", "geometry_types"])
        component_info("tilecut", kind="lib", requires=["common", "geometry_types"])
        if self.options.build_generate_grid:
            component_info("generate_grid", kind="bin")
