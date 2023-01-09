find_package(OpenMP REQUIRED)
# Generic test that uses conan libs

add_library(CppRO INTERFACE)
target_link_libraries(CppRO INTERFACE project_options project_warnings)
target_link_system_libraries(CppRO INTERFACE OpenMP::OpenMP_CXX)

# Includes
set(CppRO_INCLUDE_DIR
    "${CMAKE_CURRENT_SOURCE_DIR}/include"
    CACHE STRING "")
target_include_directories(
    CppRO INTERFACE "$<BUILD_INTERFACE:${CppRO_INCLUDE_DIR}>"
                    "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>")
