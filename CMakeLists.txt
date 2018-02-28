cmake_minimum_required(VERSION 3.5)
project(CppRo VERSION 1.0.0 LANGUAGES CXX)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${PROJECT_SOURCE_DIR}/cmake)

find_package(CPLEX REQUIRED MODULE)
include(cmake/clang-extra-tools.cmake)

##############################################
# Create target and set properties
add_library(CppRo
	INTERFACE)

#Add an alias so that library can be used inside the build tree, e.g. when testing
add_library(CppRo::CppRo ALIAS CppRo)

#Set target properties
target_include_directories(CppRo
	INTERFACE 
	$<INSTALL_INTERFACE:include>
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>)

target_compile_features(CppRo INTERFACE cxx_std_17)
target_compile_options(CppRo
    INTERFACE
    -Werror
    -Wall
    -fpermissive
    -Wextra
    -Wshadow
    -Wnon-virtual-dtor
    -pedantic
    -Wno-c++11-compat
    -DIL_STD
    -march=native
    -Wno-c++98-compat
    -Wno-sign-compare
    -Weverything
    -Wno-sign-compare
    -Wno-sign-conversion
    -Wno-c++98-compat-pedantic
    -Wno-error=padded
    -Wno-error=unused-variable
    -Wno-shorten-64-to-32
    -Wno-documentation-unknown-command
    -Wno-padded)

target_link_libraries(CppRo INTERFACE cplex-library cplex-concert ilocplex)

# Create target and set properties
add_library(TCLAP
    INTERFACE)

#Add an alias so that library can be used inside the build tree, e.g. when testing
add_library(CppRo::TCLAP ALIAS TCLAP)

#Set target properties
target_include_directories(TCLAP
    INTERFACE 
    $<INSTALL_INTERFACE:include/tclap>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}include/tclap>)

target_compile_features(TCLAP INTERFACE cxx_std_17)
target_compile_options(TCLAP
    INTERFACE
    -Werror
    -Wall
    -fpermissive
    -Wextra
    -Wshadow
    -Wnon-virtual-dtor
    -pedantic
    -Wno-c++11-compat
    -DIL_STD
    -march=native
    -Wno-c++98-compat
    -Wno-sign-compare
    -Weverything
    -Wno-sign-compare
    -Wno-sign-conversion
    -Wno-c++98-compat-pedantic
    -Wno-error=padded
    -Wno-error=unused-variable
    -Wno-shorten-64-to-32
    -Wno-documentation-unknown-command)

target_link_libraries(TCLAP INTERFACE cplex-library cplex-concert ilocplex)

##############################################
# Installation instructions

include(GNUInstallDirs)
set(INSTALL_CONFIGDIR ${CMAKE_INSTALL_LIBDIR}/cmake/CppRo)

install(TARGETS CppRo
    EXPORT CppRo-targets
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

install(TARGETS TCLAP
    EXPORT CppRo-targets
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

install(DIRECTORY include/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
install(DIRECTORY tclap/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/tclap)

#Export the targets to a script
install(EXPORT CppRo-targets
  FILE
    CppRoTargets.cmake
  NAMESPACE
    CppRo::
  DESTINATION
    ${INSTALL_CONFIGDIR}
)

#Create a ConfigVersion.cmake file
include(CMakePackageConfigHelpers)
write_basic_package_version_file(
    ${CMAKE_CURRENT_BINARY_DIR}/CppRoConfigVersion.cmake
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY AnyNewerVersion
)

configure_package_config_file(${CMAKE_CURRENT_LIST_DIR}/cmake/CppRoConfig.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/CppRoConfig.cmake
    INSTALL_DESTINATION ${INSTALL_CONFIGDIR}
)

#Install the config, configversion and custom find modules
install(FILES
	${CMAKE_CURRENT_LIST_DIR}/cmake/FindCPLEX.cmake
	${CMAKE_CURRENT_LIST_DIR}/cmake/clang-extra-tools.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/CppRoConfig.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/CppRoConfigVersion.cmake
    DESTINATION ${INSTALL_CONFIGDIR}
)

##############################################
## Exporting from the build tree

export(EXPORT CppRo-targets FILE ${CMAKE_CURRENT_BINARY_DIR}/CppRoTargets.cmake NAMESPACE CppRo::)

#Register package in user's package registry
export(PACKAGE CppRo)

enable_testing()
add_subdirectory(test)
add_subdirectory(bench)