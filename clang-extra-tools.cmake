# Adding clang-tidy target if executable is found
find_program(
  CLANG_TIDY
  NAMES "clang-tidy"
  DOC "Path to clang-tidy executable")

if(CLANG_TIDY)
    message(STATUS "clang-tidy found: ${CLANG_TIDY}")  
    add_custom_target(
        clang-tidy
        $ENV{HOME}/Documents/MyPython/run-clang-tidy.py -header-filter='.*' 
            -extra-arg="-std=c++17"            
    )
else()
    message(STATUS "clang-tidy not found.")
endif()

# additional target to perform clang-format run, requires clang-format

# get all project files
find_program(
  CLANG_FORMAT
  NAMES "clang-format"
  DOC "Path to clang-tidy executable")

if(CLANG_FORMAT)
    message(STATUS "clang-format found: ${CLANG_FORMAT}")  
    file(GLOB
        ALL_CXX_SOURCE_FILES
        ${PROJECT_SOURCE_DIR}/src/*.[chi]pp 
        ${PROJECT_SOURCE_DIR}/include/*.[chi]pp 
        *.[chi]pp
    )
    add_custom_target(
        clang-format
        COMMAND clang-format
        -i
        -style=file
        ${ALL_CXX_SOURCE_FILES}
    )
else()
    message(STATUS "clang-format not found.")
endif()
