find_package(CPLEX)

add_library(CppRO INTERFACE)
target_link_libraries(CppRO 
    INTERFACE
    CppRO_options
    CppRO_warnings

    CPLEX::CPLEX
    CPLEX::Concert
    CPLEX::IloCplex
    CPLEX::CP
)

