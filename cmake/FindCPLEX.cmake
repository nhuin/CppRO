# Try to find the CPLEX, Concert, IloCplex and CP Optimizer libraries.
#
# Once done this will add the following imported targets:
#
#  CPLEX::CPLEX - the Concert library
#  CPLEX::Concert - the Concert library
#  CPLEX::IloCplex - the IloCplex library
#  CPLEX::CP - the CP Optimizer library

include(FindPackageHandleStandardArgs)

if (UNIX)
  set(CPLEX_ILOG_DIRS /opt/ibm/ILOG /opt/IBM/ILOG)
  if (CMAKE_SIZEOF_VOID_P EQUAL 8)
    set(CPLEX_ARCH x86-64)
  else ()
    set(CPLEX_ARCH x86)
  endif ()
  message(STATUS "Set architecture to ${CPLEX_ARCH}")

  if (APPLE)
    set(CPLEX_ILOG_DIRS $ENV{HOME}/Applications/IBM/ILOG ${CPLEX_ILOG_DIRS})
    foreach (suffix "osx" "darwin9_gcc4.0")
      set(CPLEX_LIB_PATH_SUFFIXES
          ${CPLEX_LIB_PATH_SUFFIXES} lib/${CPLEX_ARCH}_${suffix}/static_pic)
    endforeach ()
  else ()
    set(CPLEX_LIB_PATH_SUFFIXES
      lib/${CPLEX_ARCH}_sles10_4.1/static_pic lib/${CPLEX_ARCH}_linux/static_pic)
  endif ()
else ()
  set(CPLEX_ILOG_DIRS "C:/Program Files/IBM/ILOG")
  if (CMAKE_SIZEOF_VOID_P EQUAL 8)
    set(CPLEX_ARCH x64)
  else ()
    set(CPLEX_ARCH x86)
    set(CPLEX_ILOG_DIRS "C:/Program Files (x86)/IBM/ILOG" ${CPLEX_ILOG_DIRS})
  endif ()
  if (MSVC10)
    set(CPLEX_LIB_PATH_SUFFIXES
      lib/${CPLEX_ARCH}_windows_vs2010/stat_mda)
    set(CPLEX_LIB_PATH_SUFFIXES_DEBUG
      lib/${CPLEX_ARCH}_windows_vs2010/stat_mdd)
  elseif (MSVC9)
    set(CPLEX_LIB_PATH_SUFFIXES
      lib/${CPLEX_ARCH}_windows_vs2008/stat_mda)
    set(CPLEX_LIB_PATH_SUFFIXES_DEBUG
      lib/${CPLEX_ARCH}_windows_vs2008/stat_mdd)
  endif ()
endif ()
message(STATUS "Set lib path suffix to ${CPLEX_LIB_PATH_SUFFIXES}")

#
# Try to deduce CPLEX Studio folder 
#
if (NOT CPLEX_STUDIO_DIR)
  foreach (dir ${CPLEX_ILOG_DIRS})
    file(GLOB CPLEX_STUDIO_DIRS "${dir}/CPLEX_Studio*")
    list(SORT CPLEX_STUDIO_DIRS)
    list(REVERSE CPLEX_STUDIO_DIRS)
    if (CPLEX_STUDIO_DIRS)
      list(GET CPLEX_STUDIO_DIRS 0 CPLEX_STUDIO_DIR_)
      message(STATUS "Found CPLEX Studio: ${CPLEX_STUDIO_DIR_}")
      break ()
    endif ()
  endforeach ()
  if (NOT CPLEX_STUDIO_DIR_)
    set(CPLEX_STUDIO_DIR_ CPLEX_STUDIO_DIR-NOTFOUND)
  endif ()
  set(CPLEX_STUDIO_DIR ${CPLEX_STUDIO_DIR_} CACHE PATH
    "Path to the CPLEX Studio directory")
endif ()
message(STATUS "CPLEX_STUDIO_DIR set to ${CPLEX_STUDIO_DIR}")

find_package(Threads)

# ----------------------------------------------------------------------------
# CPLEX

set(CPLEX_DIR ${CPLEX_STUDIO_DIR}/cplex)

# Find the CPLEX include directory.
find_path(CPLEX_INCLUDE_DIR ilcplex/cplex.h PATHS ${CPLEX_DIR}/include)

macro(find_win_cplex_library var path_suffixes)
  foreach (s ${path_suffixes})
    file(GLOB CPLEX_LIBRARY_CANDIDATES "${CPLEX_DIR}/${s}/cplex*.lib")
    if (CPLEX_LIBRARY_CANDIDATES)
      list(GET CPLEX_LIBRARY_CANDIDATES 0 ${var})
      break ()
    endif ()
  endforeach ()
  if (NOT ${var})
    set(${var} NOTFOUND)
  endif ()
endmacro()

# Find the CPLEX library.
if (UNIX)
  find_library(CPLEX_LIBRARY NAMES cplex
    PATHS ${CPLEX_DIR} PATH_SUFFIXES ${CPLEX_LIB_PATH_SUFFIXES})
    message(STATUS "Found ${CPLEX_LIBRARY}")
  set(CPLEX_LIBRARY_DEBUG ${CPLEX_LIBRARY})
elseif (NOT CPLEX_LIBRARY)
  # On Windows the version is appended to the library name which cannot be
  # handled by find_library, so search manually.
  find_win_cplex_library(CPLEX_LIB "${CPLEX_LIB_PATH_SUFFIXES}")
  set(CPLEX_LIBRARY ${CPLEX_LIB} CACHE FILEPATH "Path to the CPLEX library")
  find_win_cplex_library(CPLEX_LIB "${CPLEX_LIB_PATH_SUFFIXES_DEBUG}")
  set(CPLEX_LIBRARY_DEBUG ${CPLEX_LIB} CACHE
    FILEPATH "Path to the debug CPLEX library")
  if (CPLEX_LIBRARY MATCHES ".*/(cplex.*)\\.lib")
    file(GLOB CPLEX_DLL_ "${CPLEX_DIR}/bin/*/${CMAKE_MATCH_1}.dll")
    set(CPLEX_DLL ${CPLEX_DLL_} CACHE PATH "Path to the CPLEX DLL.")
  endif ()
endif ()


# ----------------------------------------------------------------------------
# Concert

macro(find_cplex_library var name paths)
  find_library(${var} NAMES ${name}
    PATHS ${paths} PATH_SUFFIXES ${CPLEX_LIB_PATH_SUFFIXES})
  if (UNIX)
    set(${var}_DEBUG ${${var}})
  else ()
    find_library(${var}_DEBUG NAMES ${name}
      PATHS ${paths} PATH_SUFFIXES ${CPLEX_LIB_PATH_SUFFIXES_DEBUG})
  endif ()
endmacro()
set(CPLEX_Concert_DIR ${CPLEX_STUDIO_DIR}/concert)
# Find the Concert include directory.
find_path(CPLEX_Concert_INCLUDE_DIR ilconcert/ilosys.h
    PATHS ${CPLEX_Concert_DIR}/include)
# Find the Concert library.
find_cplex_library(CPLEX_Concert_LIBRARY concert ${CPLEX_Concert_DIR})
if (CPLEX_Concert_INCLUDE_DIR AND CPLEX_Concert_LIBRARY)
    message(STATUS "Find Concert in ${CPLEX_Concert_LIBRARY}")
    mark_as_advanced(CPLEX_Concert_LIBRARY CPLEX_Concert_LIBRARY_DEBUG
        CPLEX_Concert_INCLUDE_DIR)
    set(CPLEX_Concert_FOUND TRUE)
endif()


# ----------------------------------------------------------------------------
# IloCplex - depends on CPLEX and Concert

include(CheckCXXCompilerFlag)
check_cxx_compiler_flag(-Wno-long-long HAVE_WNO_LONG_LONG_FLAG)
if (HAVE_WNO_LONG_LONG_FLAG)
  # Required if -pedantic is used.
  set(CPLEX_IloCplex_DEFINITIONS -Wno-long-long)
endif ()
# Find the IloCplex include directory - normally the same as the one for CPLEX
# but check if ilocplex.h is there anyway.
find_path(CPLEX_IloCplex_INCLUDE_DIR ilcplex/ilocplex.h
  PATHS ${CPLEX_INCLUDE_DIR})
# Find the IloCplex library.
find_library(CPLEX_IloCplex_LIBRARY ilocplex PATHS ${CPLEX_DIR} PATH_SUFFIXES ${CPLEX_LIB_PATH_SUFFIXES})
if(CPLEX_IloCplex_INCLUDE_DIR AND CPLEX_IloCplex_LIBRARY)
    message(STATUS "Found IloCplex in ${CPLEX_IloCplex_LIBRARY}")
    set(CPLEX_IloCplex_FOUND TRUE)
else()
    message(STATUS "Couldn't find IloCplex in ${CPLEX_DIR}: (${CPLEX_IloCplex_INCLUDE_DIR} and ${CPLEX_IloCplex_LIBRARY})")
endif()

# ----------------------------------------------------------------------------
# ---------------------CP Optimizer - depends on Concert----------------------
# ----------------------------------------------------------------------------

set(CPLEX_CP_DIR ${CPLEX_STUDIO_DIR}/cpoptimizer)
# Find the CP Optimizer include directory.
find_path(CPLEX_CP_INCLUDE_DIR ilcp/cp.h PATHS ${CPLEX_CP_DIR}/include)
# Find the CP Optimizer library.
find_cplex_library(CPLEX_CP_LIBRARY cp ${CPLEX_CP_DIR})
if (WIN32)
  set(CPLEX_CP_EXTRA_LIBRARIES Ws2_32.lib)
endif ()
if (CPLEX_CP_INCLUDE_DIR AND CPLEX_CP_LIBRARY)
    message(STATUS "Found CplexCP in ${CPLEX_CP_LIBRARY}")
    set(CPLEX_CP_FOUND TRUE)
    mark_as_advanced(CPLEX_CP_LIBRARY CPLEX_CP_LIBRARY_DEBUG CPLEX_CP_INCLUDE_DIR)
endif()

find_package_handle_standard_args(CPLEX
    REQUIRED_VARS CPLEX_INCLUDE_DIR CPLEX_LIBRARY
    HANDLE_COMPONENTS
)

if (CPLEX_FOUND)
    message(STATUS "Found Cplex Library")
    mark_as_advanced(CPLEX_LIBRARY CPLEX_INCLUDE_DIR)
endif()


if (CPLEX_FOUND AND NOT TARGET CPLEX::CPLEX)
  set(CPLEX_LINK_LIBRARIES ${CMAKE_THREAD_LIBS_INIT})
  check_library_exists(m floor "" HAVE_LIBM)
  if (HAVE_LIBM)
    set(CPLEX_LINK_LIBRARIES ${CPLEX_LINK_LIBRARIES} m)
  endif ()
  add_library(CPLEX::CPLEX STATIC IMPORTED GLOBAL)
  set_target_properties(CPLEX::CPLEX PROPERTIES
    IMPORTED_LOCATION "${CPLEX_LIBRARY}"
    #IMPORTED_LOCATION_DEBUG "${CPLEX_LIBRARY_DEBUG}"
    INTERFACE_INCLUDE_DIRECTORIES "${CPLEX_INCLUDE_DIR}"
    INTERFACE_LINK_LIBRARIES "${CPLEX_LINK_LIBRARIES}")
endif ()

if (CPLEX_CP_FOUND AND NOT TARGET CPLEX::CP)
  add_library(CPLEX::CP STATIC IMPORTED GLOBAL)
  set_target_properties(CPLEX::CP PROPERTIES
    IMPORTED_LOCATION "${CPLEX_CP_LIBRARY}"
    #IMPORTED_LOCATION_DEBUG "${CPLEX_CP_LIBRARY_DEBUG}"
    INTERFACE_INCLUDE_DIRECTORIES "${CPLEX_CP_INCLUDE_DIR}"
    INTERFACE_LINK_LIBRARIES "CPLEX::Concert;${CPLEX_CP_EXTRA_LIBRARIES}")
endif ()

if (CPLEX_Concert_FOUND AND NOT TARGET CPLEX::Concert) 
  add_library(CPLEX::Concert STATIC IMPORTED GLOBAL)
  set_target_properties(CPLEX::Concert PROPERTIES
      IMPORTED_LOCATION "${CPLEX_Concert_LIBRARY}"
      #IMPORTED_LOCATION_DEBUG "${CPLEX_Concert_LIBRARY_DEBUG}"
    INTERFACE_COMPILE_DEFINITIONS IL_STD # Require standard compliance.
    INTERFACE_INCLUDE_DIRECTORIES "${CPLEX_Concert_INCLUDE_DIR}"
    INTERFACE_LINK_LIBRARIES "${CMAKE_THREAD_LIBS_INIT}")
endif ()

if (CPLEX_IloCplex_FOUND AND NOT TARGET CPLEX::IloCplex)
    mark_as_advanced(CPLEX_IloCplex_LIBRARY CPLEX_IloCplex_LIBRARY_DEBUG CPLEX_IloCplex_INCLUDE_DIR)
    message(STATUS "Found IloCplex")
    add_library(CPLEX::IloCplex STATIC IMPORTED GLOBAL)
    set_target_properties(CPLEX::IloCplex PROPERTIES
        IMPORTED_LOCATION "${CPLEX_IloCplex_LIBRARY}"
        #IMPORTED_LOCATION_DEBUG "${CPLEX_IloCplex_LIBRARY_DEBUG}"
        INTERFACE_INCLUDE_DIRECTORIES "${CPLEX_IloCplex_INCLUDE_DIR}"
        INTERFACE_LINK_LIBRARIES "CPLEX::Concert;CPLEX::CPLEX")
endif ()

