# Generic test that uses conan libs
add_library(CppRO INTERFACE)
target_link_libraries(CppRO INTERFACE project_options project_warnings)

set(INCLUDE_DIR "../include")

target_include_directories(
  CppRO
  INTERFACE "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/${INCLUDE_DIR}>"
            "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>")
