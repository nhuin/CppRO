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

# Adding clang-format target if executable is found
find_program(CLANG_FORMAT "clang-format")
if(CLANG_FORMAT)
  add_custom_target(
    clang-format
    COMMAND clang-format
    -i
    -style=file
    ${ALL_CXX_SOURCE_FILES}
  )
endif()

# Adding clang-tidy target if executable is found
find_program(CLANG_TIDY "clang-tidy")
if(CLANG_TIDY)
    # add_custom_target(
    #     clang-tidy
    #     COMMAND echo 
    #     ${ALL_CXX_SOURCE_FILES}
    #     | 
    #     xargs -d ' ' -L 1 -I {} -t
    #     clang-tidy {} -config='' -- -std=c++14 -Wfatal-errors -Iinclude -I../include -I$ENV{MY_CPP_INCLUDE} -I$ENV{CPLEX_DIR}cplex/include/  -I$ENV{CPLEX_DIR}concert/include/ -DIL_STD
    # )
    add_custom_target(
        clang-tidy
        $ENV{HOME}/Documents/MyPython/run-clang-tidy.py -header-filter='.*' 
            -extra-arg="-std=c++17"
            # -Wfatal-errors -Iinclude -I../include -I${MY_CPP_INCLUDE} -isystem${CPLEX_DIR}cplex/include/ -isystem${CPLEX_DIR}concert/include/ -DIL_STD"
    )
    add_custom_target(
        clang-tidy-file
        COMMAND clang-tidy "$(TIDY-FILE)" -header-filter=.* -config='' -- -std=c++14 -Wfatal-errors -Iinclude -I../include -I$ENV{MY_CPP_INCLUDE} -isystem$ENV{CPLEX_DIR}cplex/include/  -isystem$ENV{CPLEX_DIR}concert/include/ -DIL_STD
    )
endif()

set(CMAKE_BINARY_DIR ${CMAKE_SOURCE_DIR}/bin)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR})
set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR})

include_directories(include)
include_directories(../include)
include_directories($ENV{MY_CPP_INCLUDE})

# google benchmark
include_directories(SYSTEM $ENV{HOME}/benchmark/include/)
link_directories($ENV{HOME}/benchmark/src/)

# cplex stuffs
include_directories(SYSTEM $ENV{CPLEX_DIR}cplex/include/ $ENV{CPLEX_DIR}concert/include/)
link_directories($ENV{CPLEX_DIR}cplex/lib/x86-64_linux/static_pic $ENV{CPLEX_DIR}concert/lib/x86-64_linux/static_pic)

set(CMAKE_CXX_FLAGS "-Wextra -Wall -std=c++1z -Weffc++ -Wfatal-errors -Wno-unused-parameter -DIL_STD -march=native")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS} -g -Og")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS} -Ofast -DNDEBUG -DBOOST_UBLAS_NDEBUG")
set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS} -g -Og")
SET(CMAKE_INCLUDE_SYSTEM_FLAG_CXX "-isystem ")
