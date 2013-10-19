# -*- mode: cmake; -*-
# - Top level CMakeLists.txt for sngeometry_ex01 project

cmake_minimum_required(VERSION 2.6.2)
project(sngeometry_ex01)

# ----------------------------------------------------------------------------
# Enforce an out-of-source build.
# Should be the first action in the CMakeLists.txt
#
if ( ${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_BINARY_DIR} )
  message ( STATUS "${PROJECT_NAME}: sngeometry/ex01 requires an out-of-source build." )
  message ( STATUS "${PROJECT_NAME}: Please remove these files from ${CMAKE_BINARY_DIR} first:" )
  message ( STATUS "${PROJECT_NAME}:   CMakeCache.txt" )
  message ( STATUS "${PROJECT_NAME}:   CMakeFiles" )
  message ( STATUS "${PROJECT_NAME}: Once these files are removed, create a separate directory" )
  message ( STATUS "${PROJECT_NAME}: and run CMake from there, pointing it to:" )
  message ( STATUS "${PROJECT_NAME}:   ${CMAKE_SOURCE_DIR}" )
  message ( FATAL_ERROR "${PROJECT_NAME}: in-source build detected" )
endif ()

#----------------------------------------------------------------------------
# Prepend this project's custom module path(s) to CMAKE_MODULE_PATH
#

set(CMAKE_MODULE_PATH
  ${PROJECT_SOURCE_DIR}/cmake/Modules
  ${CMAKE_MODULE_PATH}
  )

find_package(sngeometry 3.0.0 REQUIRED)

include_directories(${PROJECT_SOURCE_DIR} ${sngeometry_INCLUDE_DIRS})

add_executable ( standalone_geo standalone_geo.cxx )
target_link_libraries ( standalone_geo ${sngeometry_LIBRARIES} )
install ( FILES standalone_geo
          DESTINATION ${CMAKE_INSTALL_PREFIX}/bin
          PERMISSIONS
          OWNER_READ OWNER_WRITE OWNER_EXECUTE
          GROUP_READ GROUP_WRITE GROUP_EXECUTE
          WORLD_READ WORLD_WRITE WORLD_EXECUTE
        )

# end
