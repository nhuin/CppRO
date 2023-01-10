cmake_minimum_required(VERSION 3.15...3.23)

project(CmakeConfigPackageTests LANGUAGES CXX)

# ---- Test as standalone project the exported config package ----

if(PROJECT_IS_TOP_LEVEL OR TEST_INSTALLED_VERSION)
    enable_testing()

    find_package(CppRO CONFIG REQUIRED) # for LP, project_options, ...

    if(NOT TARGET CppRO::project_options)
        message(FATAL_ERROR "Requiered config package not found!")
        return() # be strictly paranoid for Template Janitor github action! CK
    endif()
endif()

# ---- Dependencies ----

find_package(Catch2 CONFIG REQUIRED)

include(Catch)

add_library(catch_main OBJECT catch_main.cpp)
target_link_libraries(catch_main PUBLIC Catch2::Catch2)
target_link_libraries(catch_main PRIVATE CppRO CppRO::project_options)

add_executable(tests_cplex_utility src/test_cplex_utility.cpp)
target_link_libraries(tests_cplex_utility PRIVATE CppRO CppRO::project_warnings
    CppRO::project_options catch_main)
catch_discover_tests(
    tests_cplex_utility
    TEST_PREFIX
    "cplex_utility."
    REPORTER
    xml
    OUTPUT_DIR
    .
    OUTPUT_PREFIX
    "cplex_utility."
    OUTPUT_SUFFIX
    .xml)

add_executable(tests_AllShortestPathBF src/test_AllShortestPathBF.cpp)
target_link_libraries(tests_AllShortestPathBF PRIVATE CppRO CppRO::project_warnings
    CppRO::project_options catch_main)
catch_discover_tests(
    tests_AllShortestPathBF
    TEST_PREFIX
    "AllShortestPathBF."
    REPORTER
    xml
    OUTPUT_DIR
    .
    OUTPUT_PREFIX
    "AllShortestPathBF."
    OUTPUT_SUFFIX
    .xml)

add_executable(tests_ColumnGeneration src/test_ColumnGeneration.cpp)
target_link_libraries(tests_ColumnGeneration PRIVATE CppRO CppRO::project_warnings
    CppRO::project_options catch_main)
catch_discover_tests(
    tests_ColumnGeneration
    TEST_PREFIX
    "ColumnGeneration"
    REPORTER
    xml
    OUTPUT_DIR
    .
    OUTPUT_PREFIX
    "ColumnGeneration"
    OUTPUT_SUFFIX
    .xml)
