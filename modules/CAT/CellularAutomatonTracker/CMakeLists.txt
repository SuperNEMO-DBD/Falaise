# - Top level CMakeLists.txt for CellularAutomatonTracker, the SuperNEMO offline toolchain
#
#----------------------------------------------------------------------------
# Enforce an out-of-source build.
# Should be the first action in the CMakeLists.txt
#
if(${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_BINARY_DIR})
  message(STATUS "CellularAutomatonTracker requires an out-of-source build.")
  message(STATUS "Please remove these files from ${CMAKE_BINARY_DIR} first:")
  message(STATUS "    CMakeCache.txt")
  message(STATUS "    CMakeFiles")
  message(STATUS "Once these files are removed, create a separate directory")
  message(STATUS "and run CMake from there, pointing it to:")
  message(STATUS "    ${CMAKE_SOURCE_DIR}")
  message(FATAL_ERROR "in-source build detected")
endif()

#----------------------------------------------------------------------------
# Define the project
#
cmake_minimum_required(VERSION 2.8)
project(CellularAutomatonTracker)

#----------------------------------------------------------------------------
# Prepend this project's custom module path(s) to CMAKE_MODULE_PATH
#
file(TO_CMAKE_PATH "$ENV{CMAKE_MODULE_PATH}" __env_cmake_module_path)

set(CMAKE_MODULE_PATH 
  ${PROJECT_SOURCE_DIR}/cmake/Modules 
  ${CMAKE_MODULE_PATH}
  )

if (DEFINED ADDITIONAL_CMAKE_MODULE_PATH AND NOT "x${ADDITIONAL_CMAKE_MODULE_PATH}" STREQUAL "x" )
  message ( STATUS "${PROJECT_NAME}: Using additional CMAKE_MODULE_PATH : '${ADDITIONAL_CMAKE_MODULE_PATH}'" )
  set(CMAKE_MODULE_PATH
    ${ADDITIONAL_CMAKE_MODULE_PATH}
    ${CMAKE_MODULE_PATH}
    )
endif ()

#----------------------------------------------------------------------------
# Set versioning for this project
#
include(ProjectVersion)
set_project_version(${PROJECT_NAME} 0 3 1)

#----------------------------------------------------------------------------
# Include any modules providing top level functionality
#
# - Standard install locations
include(ProjectInstallDirs)

set ( _ld_library_path_env "LD_LIBRARY_PATH" )
if ( CMAKE_SYSTEM_NAME MATCHES Darwin )
  set ( _ld_library_path_env "DYLD_LIBRARY_PATH" )
endif ( CMAKE_SYSTEM_NAME MATCHES Darwin )

# - Packaging with CPack
include(CellularAutomatonTrackerPackaging)

# - Enable testing - always on by default!!
option(ENABLE_TESTING "Enable building of unit tests for CellularAutomatonTracker" ON)
if(ENABLE_TESTING)
  enable_testing()
  include(TestingFramework)
  enable_cpplint_testing()

  # Add global testing target to which subtests can add themselves
  add_custom_target(test-hereward
    COMMAND ${CMAKE_CTEST_COMMAND}
    WORKING_DIRECTORY ${PROJECT_BINARY_DIR}
    COMMENT "Running all tests for CellularAutomatonTracker"
    )
endif()

#----------------------------------------------------------------------------
# Find needed and optional external packages
#

# Minimal build means we do not build the CATutils DLL nor special debug/development/display material
option ( CAT_MINIMAL_BUILD  "Minimal build " ON)
message( STATUS "${PROJECT_NAME}: CAT_MINIMAL_BUILD=${CAT_MINIMAL_BUILD}." )
 
set ( CAT_WITH_DEVEL_UTILS      0 )
set ( CAT_WITH_DEVEL_DISPLAY    0 )
set ( CAT_WITH_DEVEL_HISTOGRAMS 0 )
if (NOT CAT_MINIMAL_BUILD)
  set ( CAT_WITH_DEVEL_UTILS      1 )
  set ( CAT_WITH_DEVEL_DISPLAY    1 )
  set ( CAT_WITH_DEVEL_HISTOGRAMS 1 )
endif ()

# Optional ROOT dependency:
set (_cat_needs_root 0)
if (CAT_WITH_DEVEL_DISPLAY OR CAT_WITH_DEVEL_HISTOGRAMS OR CAT_WITH_DEVEL_UTILS)
  set (CAT_WITH_DEVEL_ROOT 1)
  set (_cat_needs_root 1)
endif ()

message ( STATUS "DEVEL: _cat_needs_root == ${_cat_needs_root}")
if (_cat_needs_root EQUAL 1)
  if ( NOT "$ENV{ROOT_ROOT_DIR}" STREQUAL "")  
    message ( STATUS "Environment ROOT_ROOT_DIR exists : '$ENV{ROOT_ROOT_DIR}' ")
    set ( ROOT_ROOT_DIR $ENV{ROOT_ROOT_DIR} )
  else ()
    message ( WARNING  "Environment ROOT_ROOT_DIR is not set !" )
    if ( NOT "$ENV{ROOTSYS}" STREQUAL "")  
      message ( STATUS "Environment ROOTSYS exists : '$ENV{ROOTSYS}' ")
      set ( ROOT_ROOT_DIR $ENV{ROOTSYS} )
    else ()
      message ( WARNING  "Environment ROOTSYS is not set !" )
    endif ()
  endif ()
  find_package(ROOT 5.18.00 REQUIRED)
endif ()

# Always need CLHEP :
find_package(CLHEP 2.1.0.1 REQUIRED)

# Always need Boost :
if ( NOT "$ENV{BOOST_ROOT}" STREQUAL "" )  
  message ( STATUS "Environment BOOST_ROOT exists : '$ENV{BOOST_ROOT}' ")
  set ( BOOST_ROOT $ENV{BOOST_ROOT} )
else ()
  message ( STATUS  "Environment BOOST_ROOT is not set" )
endif ()
if ( DEFINED BOOST_ROOT )  
  message ( STATUS  "BOOST_ROOT='${BOOST_ROOT}'" )
endif ()

set ( _boost_requested_components math_c99 ) 
#filesystem serialization iostreams math_c99 math_tr1 system program_options python )
set ( Boost_ADDITIONAL_VERSIONS 1.47 1.47.0 )
set ( Boost_NO_SYSTEM_PATHS ON )
find_package(Boost 1.47.0 REQUIRED COMPONENTS ${_boost_requested_components} )
if (NOT Boost_MATH_C99_FOUND)
  message ( FATAL_ERROR "Cannot find the Boost/Math library !" )
endif ()

#----------------------------------------------------------------------------
# Use of ROOT means we should look up undefined symbols dynamically on Apple
if(APPLE AND UNIX)
  set(CMAKE_SHARED_LINKER_FLAGS 
    "${CMAKE_SHARED_LINKER_FLAGS} -undefined dynamic_lookup"
    )
endif()

#----------------------------------------------------------------------------
# Add include directories needed by all
#
include_directories(CellularAutomatonTracker)

#----------------------------------------------------------------------------
# Add the subdirectories
#
# - Build the libraries first
add_subdirectory(CellularAutomatonTracker)

#----------------------------------------------------------------------------
# Configure files needed to use CellularAutomatonTracker
#
#----------------------------------------------------------------------------
# Usage from Build Tree
#
set(CellularAutomatonTracker_HEADERS_SETUP "
# Header path for build tree is always absolute
set(CellularAutomatonTracker_INCLUDE_DIR
  \"${PROJECT_SOURCE_DIR}\"
  \"${PROJECT_SOURCE_DIR}/CellularAutomatonTracker\"
  \)
")

set(CellularAutomatonTracker_THIRD_PARTY_INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS} ${BOOST_INCLUDE_DIRS})
if (_cat_needs_root EQUAL 1)
  list(APPEND CellularAutomatonTracker_THIRD_PARTY_INCLUDE_DIRS ${ROOT_INCLUDES})
endif ()
# - Export by hand for now

set(CellularAutomatonTracker_THIRD_PARTY_LIBRARIES ${CLHEP_LIBRARIES} ${Boost_MATH_C99_LIBRARY})
if (_cat_needs_root EQUAL 1)
  list(APPEND CellularAutomatonTracker_THIRD_PARTY_INCLUDE_DIRS ${ROOT_LIBRARIES})
endif ()

export(TARGETS 
  mybhep
  FILE ${PROJECT_BINARY_DIR}/CellularAutomatonTrackerLibraryDepends.cmake
  )

if(CAT_WITH_DEVEL_UTILS)
  export(TARGETS 
    CATUtils APPEND
    FILE ${PROJECT_BINARY_DIR}/CellularAutomatonTrackerLibraryDepends.cmake
    )
endif ()

export(TARGETS 
  CATAlgorithm APPEND
  FILE ${PROJECT_BINARY_DIR}/CellularAutomatonTrackerLibraryDepends.cmake
  )

configure_file(${PROJECT_SOURCE_DIR}/cmake/Templates/CellularAutomatonTrackerConfig.cmake.in
  ${PROJECT_BINARY_DIR}/CellularAutomatonTrackerConfig.cmake
  @ONLY
  )

configure_file(${PROJECT_SOURCE_DIR}/cmake/Templates/CellularAutomatonTrackerConfigVersion.cmake.in
  ${PROJECT_BINARY_DIR}/CellularAutomatonTrackerConfigVersion.cmake
  @ONLY
  )

#----------------------------------------------------------------------------
# Usage from Install Tree
#
# Set needed variables for the install tree
set(CellularAutomatonTracker_CMAKE_DIR ${CMAKE_INSTALL_LIBDIR}/cmake/${PACKAGE} )

###-${${PACKAGE}_VERSION})

# Header path for install tree is dependent on whether we have a relocatable
# install
if(CMAKE_INSTALL_IS_NONRELOCATABLE)
  # USE ABSOLUTE PATHS...
  set(CellularAutomatonTracker_HEADERS_SETUP "
# CellularAutomatonTracker configured for the install tree with absolute paths, so must use these
# NB We have hacked in the use of third_party
set(CellularAutomatonTracker_INCLUDE_DIR \"${CMAKE_INSTALL_FULL_INCLUDEDIR}\" \"${CMAKE_INSTALL_FULL_INCLUDEDIR}/CellularAutomatonTracker\")
  ")

else()
  # USE RELATIVE PATHS...
  file(RELATIVE_PATH CellularAutomatonTracker_RELATIVE_HEADER_PATH
    ${CMAKE_INSTALL_PREFIX}/${CellularAutomatonTracker_CMAKE_DIR}
    ${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_INCLUDEDIR}
    )

  set(CellularAutomatonTracker_HEADERS_SETUP "
# CellularAutomatonTracker configured for the install tree with relative paths
get_filename_component(_cellullarautomatontracker_incbase \"\${_thisdir}/${CellularAutomatonTracker_RELATIVE_HEADER_PATH}\" ABSOLUTE)
set(CellularAutomatonTracker_INCLUDE_DIR \"\${_cellullarautomatontracker_incbase}\" \"\${_cellullarautomatontracker_incbase}/CellularAutomatonTracker\")
  ")
endif()

# Target export from the install tree - we just need to install the exports file
install(EXPORT CellularAutomatonTrackerLibraryDepends
  DESTINATION ${CellularAutomatonTracker_CMAKE_DIR}
  COMPONENT Development)

# Configure the install tree config file...
configure_file(${PROJECT_SOURCE_DIR}/cmake/Templates/CellularAutomatonTrackerConfig.cmake.in
  ${PROJECT_BINARY_DIR}/InstallTreeFiles/CellularAutomatonTrackerConfig.cmake
  @ONLY
  )

# Configure the install tree versioning file
configure_file(${PROJECT_SOURCE_DIR}/cmake/Templates/CellularAutomatonTrackerConfigVersion.cmake.in
  ${PROJECT_BINARY_DIR}/InstallTreeFiles/CellularAutomatonTrackerConfigVersion.cmake
  @ONLY
  )

# Install the config files into the install tree
install(FILES
  ${PROJECT_BINARY_DIR}/InstallTreeFiles/CellularAutomatonTrackerConfig.cmake    
  ${PROJECT_BINARY_DIR}/InstallTreeFiles/CellularAutomatonTrackerConfigVersion.cmake
  DESTINATION ${CellularAutomatonTracker_CMAKE_DIR}
  COMPONENT Development
  )

# ---------------------------------------------------------------------------------------

# - Bash setup script :
configure_file ( "${PROJECT_SOURCE_DIR}/cmake/Templates/cellularautomatontracker_setup.sh.in"
  "${PROJECT_BINARY_DIR}/cmake/cellularautomatontracker_setup.sh" @ONLY )

install ( FILES ${PROJECT_BINARY_DIR}/cmake/cellularautomatontracker_setup.sh
  DESTINATION ${CMAKE_INSTALL_ETCDIR}
  PERMISSIONS OWNER_READ GROUP_READ WORLD_READ
  )


configure_file ( "${PROJECT_SOURCE_DIR}/cmake/Templates/cellularautomatontracker-config.in"
  "${PROJECT_BINARY_DIR}/cmake/Templates/cellularautomatontracker-config" @ONLY )

install ( FILES ${PROJECT_BINARY_DIR}/cmake/Templates/cellularautomatontracker-config
  DESTINATION ${CMAKE_INSTALL_BINDIR}
  PERMISSIONS OWNER_READ OWNER_EXECUTE 
  GROUP_READ GROUP_EXECUTE 
  WORLD_READ WORLD_EXECUTE
  )


# And we're done...

