find_package(OpenMP REQUIRED)
find_package(CPLEX REQUIRED Concert IloCplex CP)

add_library(CppRO INTERFACE)
target_link_libraries(CppRO INTERFACE project_options project_warnings)
target_link_system_libraries(CppRO INTERFACE OpenMP::OpenMP_CXX CPLEX::CPLEX
                             CPLEX::Concert CPLEX::IloCplex)
set(INCLUDE_DIR "") # must be relative paths
target_include_directories(
    CppRO
    INTERFACE "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/${INCLUDE_DIR}>"
              "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>")
