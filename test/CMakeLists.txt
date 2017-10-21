add_executable(tester test_multidimarray.cpp)
add_test(Tester tester)

install(TARGETS tester
        RUNTIME DESTINATION ${CMAKE_BINARY_DIR})