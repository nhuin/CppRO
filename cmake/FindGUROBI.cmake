find_path(GUROBI_INCLUDE_DIRS
    NAMES gurobi_c.h
    HINTS ${GUROBI_DIR} $ENV{GUROBI_HOME}
    PATH_SUFFIXES include)

    message(STATUS "Found Gurobi include directory in ${GUROBI_INCLUDE_DIRS}")

find_library(GUROBI_LIBRARY
    NAMES gurobi gurobi95
    HINTS ${GUROBI_DIR} $ENV{GUROBI_HOME}
    PATH_SUFFIXES lib)

if(MSVC)
    # determine Visual Studio year
    if(MSVC_TOOLSET_VERSION EQUAL 142)
        set(MSVC_YEAR "2019")
    elseif(MSVC_TOOLSET_VERSION EQUAL 141)
        set(MSVC_YEAR "2017")
    endif()

    if(MT)
        set(M_FLAG "mt")
    else()
        set(M_FLAG "md")
    endif()
    
    find_library(GUROBI_CXX_LIBRARY
        NAMES gurobi_c++${M_FLAG}${MSVC_YEAR}
        HINTS ${GUROBI_DIR} $ENV{GUROBI_HOME}
        PATH_SUFFIXES lib)
    find_library(GUROBI_CXX_DEBUG_LIBRARY
        NAMES gurobi_c++${M_FLAG}d${MSVC_YEAR}
        HINTS ${GUROBI_DIR} $ENV{GUROBI_HOME}
        PATH_SUFFIXES lib)
else()
    find_library(GUROBI_CXX_LIBRARY
        NAMES gurobi_c++
        HINTS ${GUROBI_DIR} $ENV{GUROBI_HOME}
        PATH_SUFFIXES lib)
    set(GUROBI_CXX_DEBUG_LIBRARY ${GUROBI_CXX_LIBRARY})
endif()
add_library(Gurobi::Base UNKNOWN IMPORTED)
set_target_properties(Gurobi::Base PROPERTIES
    IMPORTED_LOCATION ${GUROBI_LIBRARY}
)
add_library(Gurobi::Gurobi UNKNOWN IMPORTED)
set_target_properties(Gurobi::Gurobi PROPERTIES
    INTERFACE_INCLUDE_DIRECTORIES ${GUROBI_INCLUDE_DIRS}
)
target_link_libraries(Gurobi::Gurobi INTERFACE
    Gurobi::Base)
set_property(TARGET Gurobi::Gurobi PROPERTY
    IMPORTED_LOCATION ${GUROBI_CXX_LIBRARY}
    )

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(GUROBI DEFAULT_MSG GUROBI_LIBRARY)
