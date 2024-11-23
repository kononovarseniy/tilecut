from conan import ConanFile
from conan.tools.cmake import CMakeToolchain, CMake, cmake_layout, CMakeDeps
from conan.tools.build import check_min_cppstd


class TilecutRecipe(ConanFile):
    name = "ka_tilecut"
    version = "0.1"

    license = "MIT"
    author = "kononovarseniy@gmail.com"
    url = "https://github.com/kononovarseniy/tilecut"
    description = "Algorithms for cutting cartographic data into tiles"

    settings = "os", "compiler", "build_type", "arch"

    exports_sources = (
        "CMakeLists.txt",
        "kononovarseniy/*",
        "src/*",
    )

    def validate(self):
        check_min_cppstd(self, "20")

    def requirements(self):
        pass

    def build_requirements(self):
        self.test_requires("gtest/1.15.0")
        self.requires("boost/1.86.0")
        self.requires("mpfr/4.2.1")

    def layout(self):
        cmake_layout(self)

    def generate(self):
        deps = CMakeDeps(self)
        deps.generate()
        toolchain = CMakeToolchain(self)
        toolchain.generate()

    def build(self):
        cmake = CMake(self)
        cmake.configure()
        cmake.build()

    def package(self):
        cmake = CMake(self)
        cmake.install()

    def package_info(self):
        self.cpp_info.libs = ["ka_tilecut"]
        self.cpp_info.set_property("cmake_target_name", "ka::tilecut")
