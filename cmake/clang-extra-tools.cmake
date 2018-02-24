# Adding clang-tidy target if executable is found
find_program(CLANG_TIDY "clang-tidy")
find_program(CLANG_TIDY_PYTHON "run-clang-tidy.py")
if(CLANG_TIDY)
    add_custom_target(
        clang-tidy-file
        COMMAND clang-tidy "$(TIDY-FILE)" -header-filter=.* -config='' 
            -- -std=c++14 -Wfatal-errors -Iinclude -I../include 
                -I${MY_CPP_INCLUDE} -isystem${CPLEX_DIR}cplex/include/ 
                -isystem${CPLEX_DIR}concert/include/ -DIL_STD
    )
    if(CLANG_TIDY_PYTHON)
    add_custom_target(
        clang-tidy
        ${CLANG_TIDY_PYTHON} -header-filter='.*' 
        -extra-arg="-std=c++17"
    )
    else()
        message(STATUS "run-clang-tidy.py was not found")    
    endif()
else()
    message(STATUS "clang-tidy was not found")
endif()

# additional target to perform clang-format run, requires clang-format

# get all project files
file(GLOB
    ALL_CXX_SOURCE_FILES
    ${PROJECT_SOURCE_DIR}/src/*.[chi]pp 
    ${PROJECT_SOURCE_DIR}/include/*.[chi]pp 
    *.[chi]pp
)

find_program(CLANG_FORMAT "clang-format")
if(CLANG_FORMAT)
    add_custom_target(
        clang-format
        COMMAND clang-format
        -i
        -style=file
        ${ALL_CXX_SOURCE_FILES}
    )
else()
    message(STATUS "clang-format was not found")
endif()