find_package(benchmark REQUIRED)
find_package(Boost REQUIRED)

add_executable(sort sort.cpp)
target_link_libraries(sort benchmark Boost::boost CppRo::CppRo)

add_executable(multidimarray_bench multidimarray.cpp)
target_compile_options(multidimarray_bench PRIVATE -O3 -Wno-global-constructors)
target_link_libraries(multidimarray_bench benchmark Boost::boost CppRo::CppRo)

# add_executable(cplex_bench cplex.cpp)
# target_link_libraries(cplex_bench benchmark ilocplex concert cplex)
# install(TARGETS cplex_bench
#         RUNTIME DESTINATION ${CMAKE_BINARY_DIR}/bench)