find_package(Catch2 REQUIRED)

enable_testing()

add_executable(multidimarray_test src/test_multidimarray.cpp)
target_compile_features(multidimarray_test PRIVATE cxx_std_17)
target_link_libraries(multidimarray_test Catch2::Catch CppRo)
add_test(multidimarray_test multidimarray_test)

add_executable(graph_test src/graph.cpp)
target_compile_features(graph_test PRIVATE cxx_std_17)
target_link_libraries(graph_test Catch2::Catch CppRo)
add_test(graph graph_test)

add_executable(cplex_utility_test src/test_cplex_utility.cpp)
target_compile_features(cplex_utility_test PRIVATE cxx_std_17)
target_link_libraries(cplex_utility_test Catch2::Catch CppRo)
add_test(cplex_utility_test cplex_utility_test)
