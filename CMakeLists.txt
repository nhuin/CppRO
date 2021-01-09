cmake_minimum_required(VERSION 3.5)
project(CppRo VERSION 1.2.0 LANGUAGES CXX)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${PROJECT_SOURCE_DIR}/cmake)

include(cmake/clang-extra-tools.cmake)

option(CppRO_ENABLE_TEST "Enable test" 0)
option(CppRO_ENABLE_CPLEX "Enable CPLEX support" 0)

find_package(OpenMP REQUIRED)
##############################################
# Create target and set properties
add_library(CppRo
	INTERFACE)
target_compile_features(CppRo INTERFACE cxx_std_17)

#Add an alias so that library can be used inside the build tree, e.g. when testing
add_library(CppRo::CppRo ALIAS CppRo)

target_link_libraries(CppRo INTERFACE OpenMP::OpenMP_CXX)

#Set target properties
target_include_directories(CppRo
	INTERFACE 
	$<INSTALL_INTERFACE:include>
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>)

target_compile_features(CppRo INTERFACE cxx_std_17)

if(CppRO_ENABLE_CPLEX)
    find_package(CPLEX REQUIRED)
    target_link_libraries(CppRo INTERFACE cplex-library cplex-concert ilocplex)
endif()

##############################################
# Installation instructions

include(GNUInstallDirs)
set(INSTALL_CONFIGDIR ${CMAKE_INSTALL_LIBDIR}/cmake/CppRo)

install(TARGETS CppRo
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

if(CppRO_ENABLE_TEST)
    enable_testing()
    add_subdirectory(test)
endif()
