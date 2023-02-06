"""
File: conanfile.py
Author: Nicolas HUIN
Email: nicolas.huin@imt-atlantique.fr
Github: https://github.com/nhuin
Description: Provides conan configuration
"""

from conan import ConanFile
from conan.tools.cmake import CMakeToolchain, CMake, cmake_layout


class CppROConan(ConanFile):
    name = "CppRO"
    version = "0.1.0"

    # Optional metadata
    license = "Mozilla Public License 2.0"
    author = "Nicolas HUIN"
    url = "https://github.com/nhuin/CppRO"
    description = "C++ library for operational research"
    topics = ("Operational research", "CPLEX", "Column generation")

    # Binary configuration
    settings = "os", "compiler", "build_type", "arch"

    generators = "cmake_find_package_multi"

    cmake = None
    requires = ["catch2/2.13.9", "boost/1.81.0"]

    # Sources are located in the same place as this recipe, copy them to the recipe
    exports_sources = "CMakeLists.txt", "src/*", "test/*", "configured_files/*", "conanfile.txt", "conanfile.py", "cmake/*"

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
        self.cmake = CMake(self)
        self.cmake.configure()
        self.cmake.build()

    def package(self):
        self.cmake.install()

    def package_info(self):
        self.cpp_info.includedirs = ["src"]
