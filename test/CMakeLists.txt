find_package(Catch2 REQUIRED)

enable_testing()

add_executable(multidimarray_test src/test_multidimarray.cpp)
target_compile_features(multidimarray_test PRIVATE cxx_std_17)
target_link_libraries(multidimarray_test 
	PRIVATE
	Catch2::Catch CppRo)
add_test(multidimarray_test multidimarray_test)

install(TARGETS multidimarray_test
        RUNTIME DESTINATION ${CMAKE_BINARY_DIR}/test)

add_executable(graph_test src/graph.cpp)
target_compile_features(graph_test PRIVATE cxx_std_17)
target_link_libraries(graph_test 
	PRIVATE
	Catch2::Catch CppRo)
add_test(graph graph_test)
install(TARGETS graph_test
        RUNTIME DESTINATION ${CMAKE_BINARY_DIR}/test)
