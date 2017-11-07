cmake_minimum_required(VERSION 2.4)
project(MyC++ LANGUAGES CXX)
# Get include directories 
# get_property(dirs DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY INCLUDE_DIRECTORIES)
# foreach(dir ${dirs})
#   string(CONCAT INCLUDE_COMMAND ${INCLUDE_COMMAND} "-I${dir}")
# endforeach()
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}")
set(CMAKE_FIND_LIBRARY_PREFIXES "lib")
set(CMAKE_FIND_LIBRARY_SUFFIXES "")

# Additional targets to perform clang-format/clang-tidy

include_directories(./include)
include_directories(../include)
include_directories($ENV{MY_CPP_INCLUDE})
include_directories(/usr/local/include/)

# google benchmark
include_directories(SYSTEM $ENV{HOME}/Documents//benchmark/include/)
link_directories($ENV{HOME}/Documents/benchmark/src/)

# cplex stuffs
# include_directories(SYSTEM $ENV{CPLEX_DIR}cplex/include/ $ENV{CPLEX_DIR}concert/include/)
# if(${APPLE})
# 	find_library(CPLEX_LIB HINTS cplex/lib/x86-64_osx/static_pic ENV CPLEX_DIR /static_pic ${CPLEX_DIR})
# 	find_library(CONCERT_LIB HINTS ENV CPLEX_DIR /static_pic ${CPLEX_DIR}concert/lib/x86-64_osx/static_pic)
#     link_directories(CPLEX_LIB)
#     link_directories(CONCERT_LIB)
# elseif(${UNIX})
#     link_directories(${CPLEX_DIR}cplex/lib/x86-64_linux/static_pic ${CPLEX_DIR}concert/lib/x86-64_linux/static_pic)
# endif()
set(CPLEX_ROOT_DIR $ENV{CPLEX_DIR})
find_package(CPLEX)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 -Wall -Wextra -Wshadow -Wnon-virtual-dtor \
    -pedantic -Wno-c++11-compat -DIL_STD -march=native -Weverything \
    -Wno-c++98-compat -Wno-c++98-compat-pedantic -Wno-sign-conversion")
set(CMAKE_CXX_FLAGS_DEBUG "-Og -fsanitize=address")
set(CMAKE_CXX_FLAGS_RELEASE " -Ofast -DNDEBUG -DBOOST_DISABLE_ASSERTS -DBOOST_UBLAS_NDEBUG")
set(CMAKE_CXX_FLAGS_RELWITHDEBINFO " -g -Og")
SET(CMAKE_INCLUDE_SYSTEM_FLAG_CXX "-isystem ")

enable_testing()
add_subdirectory(test)
add_subdirectory(bench)

include(clang-extra-tools)