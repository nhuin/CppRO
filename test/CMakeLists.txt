add_executable(multidimarray_test test_multidimarray.cpp)
add_test(multidimarray_test multidimarray_test)

install(TARGETS multidimarray_test
        RUNTIME DESTINATION ${CMAKE_BINARY_DIR}/test)

add_executable(graph_test graph.cpp)
add_test(graph graph_test)

install(TARGETS graph_test
        RUNTIME DESTINATION ${CMAKE_BINARY_DIR}/test)
