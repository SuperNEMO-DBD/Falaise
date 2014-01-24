# - Top level CMake script for Falaise
#

#-----------------------------------------------------------------------
# Copyright 2012,2013 Ben Morgan <bmorgan.warwick@gmail.com>
# Copyright 2012,2013 University of Warwick
#
# This file is part of Falaise.
#
# Falaise is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# Falaise is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with Falaise.  If not, see <http://www.gnu.org/licenses/>.
#-----------------------------------------------------------------------

#-----------------------------------------------------------------------
# Enforce an out-of-source build.
#
if(${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_BINARY_DIR})
  message(STATUS "Falaise requires an out-of-source build.")
  message(STATUS "Please remove these files from ${CMAKE_BINARY_DIR} first:")
  message(STATUS "  CMakeCache.txt")
  message(STATUS "  CMakeFiles")
  message(STATUS "Once these files are removed, create a separate directory")
  message(STATUS "and run CMake from there, pointing it to:")
  message(STATUS "  ${CMAKE_SOURCE_DIR}")
  message(FATAL_ERROR "in-source build detected")
endif()

#-----------------------------------------------------------------------
# CMake/project requirements and configuration
#
cmake_minimum_required(VERSION 2.8 FATAL_ERROR)
project(Falaise)

# - Enforce all warnings, but ensure other flags are retained
# Note that the added flags *won't* show up in the cache.
# TODO : better way to do this with the "Make Override" system?
if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  set(Falaise_CXX_FLAGS "-W -Wall")

  # On clang, need larger template depth
  if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    set(Falaise_CXX_FLAGS "${Falaise_CXX_FLAGS} -ftemplate-depth=512")
  endif()
elseif(CMAKE_CXX_COMPILER MATCHES "icpc.*|icc.*")
  # Needs more testing
  set(Falaise_CXX_FLAGS "-w2")
endif()
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${Falaise_CXX_FLAGS}")

# - Versioning
set(Falaise_VERSION_MAJOR 1)
set(Falaise_VERSION_MINOR 0)
set(Falaise_VERSION_PATCH 0)
set(Falaise_VERSION "${Falaise_VERSION_MAJOR}.${Falaise_VERSION_MINOR}.${Falaise_VERSION_PATCH}")

# - SVN Versioning, if available
set(Falaise_VERSION_REVISION 0)

# - Standard UNIX Tool install paths, including relative paths for use
# by applications requiring resources
# Validate that certain paths are relative, otherwise relocation may fail
include(GNUInstallDirs)
foreach(_dir BINDIR LIBDIR INCLUDEDIR DATAROOTDIR)
  if(IS_ABSOLUTE "${CMAKE_INSTALL_${_dir}}")
    message(FATAL_ERROR "Absolute path for CMAKE_INSTALL_${_dir} not allowed")
  endif()
endforeach()

# - Configure output paths for products to give inplace build that roughly
#   matches install layout.
set(Falaise_BUILDPRODUCT_DIR "${PROJECT_BINARY_DIR}/BuildProducts")
set(Falaise_DATAROOTDIR ${Falaise_BUILDPRODUCT_DIR}/${CMAKE_INSTALL_DATAROOTDIR}/Falaise-${Falaise_VERSION})
file(RELATIVE_PATH Falaise_BINDIR_TO_RESOURCEDIR "${CMAKE_INSTALL_FULL_BINDIR}" "${CMAKE_INSTALL_FULL_DATAROOTDIR}/Falaise-${Falaise_VERSION}")

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${Falaise_BUILDPRODUCT_DIR}/${CMAKE_INSTALL_LIBDIR}")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${Falaise_BUILDPRODUCT_DIR}/${CMAKE_INSTALL_BINDIR}")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${Falaise_BUILDPRODUCT_DIR}/${CMAKE_INSTALL_LIBDIR}")

#-----------------------------------------------------------------------
# Configure testing if required
#
option(Falaise_ENABLE_TESTING "Build unit testing system for Falaise" OFF)
if(Falaise_ENABLE_TESTING)
  enable_testing()
endif()

#-----------------------------------------------------------------------
# Configure documentation generation, if required
#
set(Falaise_DOCEXPORT_DIR "${PROJECT_BINARY_DIR}/documentation")
option(Falaise_BUILD_DOCS "Build documentation for Falaise" ON)
if(Falaise_BUILD_DOCS)
  # - For main HTML docs
  find_package(Doxygen 1.8 REQUIRED)
  # - For man pages, pandoc for now
  #find_package(Pandoc 1.10 REQUIRED)
  #find_package(<something to compress man?>)
  find_program(Pandoc_EXECUTABLE pandoc
    DOC "path to pandoc executable"
    )
  set(Pandoc_FOUND ${Pandoc_EXECUTABLE})
endif()

#-----------------------------------------------------------------------
# Configure/Build utilities as needed
#
option(Falaise_USE_SYSTEM_BAYEUX "Use system-installed Bayeux" OFF)
mark_as_advanced(Falaise_USE_SYSTEM_BAYEUX)

if(Falaise_USE_SYSTEM_BAYEUX)
  find_package(Bayeux 1.0.0 REQUIRED)
  if(NOT Bayeux_MCTOOLS_GEANT4_FOUND)
    message(FATAL_ERROR "
  Bayeux found, but it does not supply the MCTools/Geant4 module
  "  )
  endif()
else()
  # - Configure internal build
  set(Bayeux_EMBEDDED 1)
  set(Bayeux_BUILDPRODUCT_DIR ${Falaise_BUILDPRODUCT_DIR})
  set(Bayeux_ENABLE_TESTING ${Falaise_ENABLE_TESTING})
  set(Bayeux_BUILD_DOCS ${Falaise_BUILD_DOCS})
  set(Bayeux_WITH_GEANT4 ON)

  # - Option to build Bayeux's developer tools
  option(Falaise_BUILD_DEVELOPER_TOOLS "Build tools for developing Falaise" OFF)
  mark_as_advanced(Falaise_BUILD_DEVELOPER_TOOLS)
  set(Bayeux_BUILD_TOOLS ${Falaise_BUILD_DEVELOPER_TOOLS})

  # - Build
  add_subdirectory(utilities/flbayeux)

  # - Set up so that Falaise will use the internal build
  # Hacked temporarily so we can use this technique on CMake < 2.8.11
  # that doesn't allow duplicate imported targets.
  set(Bayeux_TARGETS_LOADED 1)
  include(${Falaise_BUILDPRODUCT_DIR}/${CMAKE_INSTALL_LIBDIR}/cmake/Bayeux-1.0.0/BayeuxConfig.cmake)
endif()


#-----------------------------------------------------------------------
# Build the subdirectories as required
#
add_subdirectory(source)

if(Falaise_BUILD_DOCS)
  add_subdirectory(documentation)
endif()

add_subdirectory(modules)

#-----------------------------------------------------------------------
# Resource installation
#
set(_falaise_resources_dir "${PROJECT_SOURCE_DIR}/resources")

# - Load the  list of resource  files to be  published.  We don't  use a
# 'file(GLOB  ...)'  because  it  is  not  recommended  by  the  CMake
# documentation.   The  'resources/_resources.cmake'   file  is   thus
# explicitely  built  by the  'resources/_make_list_of_resource_files'
# shell script:
include(resources/_resources.cmake)

# - Publish resource files using the '_falaise_list_of_resources_files'
# variable defined in the 'resources/_resources.cmake' file:
foreach(_rfin ${_falaise_list_of_resources_files})
  string(REGEX REPLACE "\\.in$" "" _rfout "${_rfin}")
  string(REGEX REPLACE "^${PROJECT_SOURCE_DIR}/resources" "${Falaise_DATAROOTDIR}/resources" _rfout "${_rfout}")
  configure_file(${_rfin} ${_rfout} @ONLY)
endforeach()


# - Install published resources
install(DIRECTORY ${Falaise_DATAROOTDIR}
  DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}
  )

#-----------------------------------------------------------------------
# Package Sources and Binaries
#
#include(FLCPack)
