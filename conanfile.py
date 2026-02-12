from conan import ConanFile
from conan.tools.build import check_min_cppstd
from conan.tools.cmake import CMake, CMakeDeps, CMakeToolchain, cmake_layout
from conan.tools.files import copy


class KaTilecutRecipe(ConanFile):
    name = "ka_tilecut"
    version = "0.3.2"
    package_type = "library"

    license = "MIT"
    author = "kononovarseniy@gmail.com"
    url = "https://github.com/kononovarseniy/tilecut"
    description = "Algorithms for cutting cartographic data into tiles"

    settings = "os", "compiler", "build_type", "arch"
    options = {
        "shared": [True, False],
        "fPIC": [True, False],
        "build_generate_grid": [True, False],
    }
    default_options = {
        "shared": False,
        "fPIC": True,
        "build_generate_grid": True,
    }

    def config_options(self):
        if self.settings.os == "Windows":
            self.options.rm_safe("fPIC")

    def configure(self):
        if self.options.shared:
            self.options.rm_safe("fPIC")

    def build_testing(self):
        return not self.conf.get("tools.build:skip_test", default=False)

    def requirements(self):
        self.requires("ka_common/0.1.0", transitive_headers=True)
        self.requires("fmt/9.1.0")

        if self.build_testing():
            self.test_requires("gtest/1.17.0")
        if self.options.build_generate_grid:
            self.requires("boost/1.86.0")
        if self.build_testing() or self.options.build_generate_grid:
            self.requires("mpfr/4.2.1", visible=False)

    def export_sources(self):
        copy(self, "CMakeLists.txt", self.recipe_folder, self.export_sources_folder)
        copy(self, "src/*", self.recipe_folder, self.export_sources_folder)

    def validate(self):
        check_min_cppstd(self, "20")

    def layout(self):
        cmake_layout(self)

        self.cpp.source.components["exact"].includedirs = ["src/exact/include"]
        self.cpp.build.components["exact"].libdirs = ["src/exact"]

        if self.options.build_generate_grid:
            self.cpp.source.components["generate_grid"].includedirs = ["src/generate_grid/include"]
            self.cpp.build.components["generate_grid"].libdirs = ["src/generate_grid"]

        self.cpp.source.components["geometry_types"].includedirs = ["src/geometry_types/include"]
        self.cpp.build.components["geometry_types"].libdirs = ["src/geometry_types"]

        self.cpp.source.components["tilecut"].includedirs = ["src/tilecut/include"]
        self.cpp.build.components["tilecut"].libdirs = ["src/tilecut"]

    def generate(self):
        deps = CMakeDeps(self)
        deps.generate()
        tc = CMakeToolchain(self)
        tc.variables["BUILD_GENERATE_GRID"] = bool(self.options.build_generate_grid)
        tc.generate()

    def build(self):
        cmake = CMake(self)
        cmake.configure()
        cmake.build()
        if self.build_testing():
            cmake.ctest(cli_args=["--output-on-failure"])

    def package(self):
        cmake = CMake(self)
        cmake.install()

    def package_info(self):
        self.cpp_info.components["exact"].set_property("cmake_target_name", "ka::exact")
        self.cpp_info.components["exact"].requires = ["fmt::fmt", "ka_common::ka_common", "geometry_types"]
        self.cpp_info.components["exact"].libs = ["ka_exact"]

        if self.options.build_generate_grid:
            self.cpp_info.components["generate_grid"].set_property("cmake_target_name", "ka::generate_grid")
            self.cpp_info.components["generate_grid"].requires = ["boost::boost", "ka_common::ka_common"]

        self.cpp_info.components["geometry_types"].set_property("cmake_target_name", "ka::geometry_types")
        self.cpp_info.components["geometry_types"].requires = ["fmt::fmt", "ka_common::ka_common"]
        self.cpp_info.components["geometry_types"].bindirs = []
        self.cpp_info.components["geometry_types"].libdirs = []

        self.cpp_info.components["tilecut"].set_property("cmake_target_name", "ka::tilecut")
        self.cpp_info.components["tilecut"].requires = ["fmt::fmt", "ka_common::ka_common", "geometry_types", "exact"]
        self.cpp_info.components["tilecut"].libs = ["ka_tilecut"]
