from conan import ConanFile
from conan.tools.cmake import CMakeToolchain, CMake, cmake_layout


class CppROConan(ConanFile):
    name = "CppRO"
    version = "0.0.1"

    # Optional metadata
    license = "Mozilla Public License 2.0"
    author = "Nicolas HUIN"
    url = "https://github.com/nhuin/CppRO"
    description = "C++ library for operational research"
    topics = ("Operational research", "CPLEX", "Column generation")

    # Binary configuration
    settings = "os", "compiler", "build_type", "arch"

    # Sources are located in the same place as this recipe, copy them to the recipe
    exports_sources =  "CMakeLists.txt", "src/*", "include/*", "conanfile.txt", "test/*", "configured_files/*"
    #no_copy_source = True

    requires = [("catch2/3.1.0@")]

    def config_options(self):
        if self.settings.os == "Windows":
            del self.options.fPIC

    def layout(self):
        cmake_layout(self)

    def generate(self):
        tc = CMakeToolchain(self)
        tc.cache_variables['ENABLE_DEVELOPER_MODE'] = False
        tc.generate()

    def build(self):
        cmake = CMake(self)
        cmake.configure()
        cmake.build()

    def package(self):
        cmake = CMake(self)
        cmake.install()