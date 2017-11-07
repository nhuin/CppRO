# CPLEX
include_directories(SYSTEM ${CPLEX_DIR}cplex/include/ ${CPLEX_DIR}concert/include/)
if(${APPLE})
    link_directories(${CPLEX_DIR}cplex/lib/x86-64_osx/static_pic ${CPLEX_DIR}concert/lib/x86-64_osx/static_pic)
elseif(${UNIX})
    link_directories(${CPLEX_DIR}cplex/lib/x86-64_linux/static_pic ${CPLEX_DIR}concert/lib/x86-64_linux/static_pic)
endif()

add_executable(sort sort.cpp)
target_link_libraries(sort benchmark)
install(TARGETS sort
        RUNTIME DESTINATION ${CMAKE_BINARY_DIR}/bench)

add_executable(multidimarray_bench multidimarray.cpp)
target_link_libraries(multidimarray_bench benchmark)
install(TARGETS multidimarray_bench
        RUNTIME DESTINATION ${CMAKE_BINARY_DIR}/bench)

# add_executable(cplex_bench cplex.cpp)
# target_link_libraries(cplex_bench benchmark ilocplex concert cplex)
# install(TARGETS cplex_bench
#         RUNTIME DESTINATION ${CMAKE_BINARY_DIR}/bench)