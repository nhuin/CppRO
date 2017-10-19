cmake_minimum_required(VERSION 2.4)

# Get include directories 
get_property(dirs DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY INCLUDE_DIRECTORIES)
foreach(dir ${dirs})
  string(CONCAT INCLUDE_COMMAND ${INCLUDE_COMMAND} "-I${dir}")
endforeach()

# Additional targets to perform clang-format/clang-tidy
# Get all project files
file(GLOB
    ALL_CXX_SOURCE_FILES
    src/*.[chi]pp 
    include/*.[chi]pp 
    *.[chi]pp
)

include(clang-extra-tools.cmake)

set(CMAKE_BINARY_DIR ${CMAKE_SOURCE_DIR}/bin)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR})
set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR})

include_directories(include)
include_directories(../include)
include_directories($ENV{MY_CPP_INCLUDE})

# google benchmark
include_directories(SYSTEM $ENV{HOME}/Documents//benchmark/include/)
link_directories($ENV{HOME}/Documents/benchmark/src/)

# cplex stuffs
include_directories(SYSTEM $ENV{CPLEX_DIR}cplex/include/ $ENV{CPLEX_DIR}concert/include/)
if(${APPLE})
    link_directories(${CPLEX_DIR}cplex/lib/x86-64_osx/static_pic ${CPLEX_DIR}concert/lib/x86-64_osx/static_pic)
elseif(${UNIX})
    link_directories(${CPLEX_DIR}cplex/lib/x86-64_linux/static_pic ${CPLEX_DIR}concert/lib/x86-64_linux/static_pic)
endif()

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 -Wall -Wextra -Wshadow -Wnon-virtual-dtor \
    -pedantic -Wno-c++11-compat -DIL_STD -march=native -Weverything \
    -Wno-c++98-compat -Wno-c++98-compat-pedantic -Wno-sign-conversion")
set(CMAKE_CXX_FLAGS_DEBUG "-g -Og")
set(CMAKE_CXX_FLAGS_RELEASE " -Ofast -DNDEBUG -DBOOST_UBLAS_NDEBUG")
set(CMAKE_CXX_FLAGS_RELWITHDEBINFO " -g -Og")
SET(CMAKE_INCLUDE_SYSTEM_FLAG_CXX "-isystem ")

project (bench_sort)

add_executable(bench_sort ${PROJECT_SOURCE_DIR}/bench.cpp)
# target_link_libraries(bench_sort m pthread)
# target_link_libraries(bench_sort ilocplex concert cplex)