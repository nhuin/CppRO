# Adding clang-tidy target if executable is found
find_program(CLANG_TIDY "clang-tidy")
if(CLANG_TIDY)
    add_custom_target(
        clang-tidy
        $ENV{HOME}/Documents/MyPython/run-clang-tidy.py -header-filter='.*' 
            -extra-arg="-std=c++17"            
    )
    add_custom_target(
        clang-tidy-file
        COMMAND clang-tidy "$(TIDY-FILE)" -header-filter=.* -config='' 
            -- -std=c++14 -Wfatal-errors -Iinclude -I../include 
                -I${MY_CPP_INCLUDE} -isystem${CPLEX_DIR}cplex/include/ 
                -isystem${CPLEX_DIR}concert/include/ -DIL_STD
    )
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
    clangformat
    COMMAND clang-format
    -i
    -style=file
    ${ALL_CXX_SOURCE_FILES}
)
endif()