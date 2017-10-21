add_executable(bench_sort sort.cpp)

target_link_libraries(bench_sort m pthread benchmark)
install(TARGETS bench_sort
        RUNTIME DESTINATION ${CMAKE_BINARY_DIR}/bench)

add_executable(bench_multidimarray multidimarray.cpp)

target_link_libraries(bench_multidimarray m pthread benchmark)
install(TARGETS bench_multidimarray
        RUNTIME DESTINATION ${CMAKE_BINARY_DIR}/bench)