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

file(RELATIVE_PATH Falaise_BINDIR_TO_RESOURCEDIR "${CMAKE_INSTALL_FULL_BINDIR}" "${CMAKE_INSTALL_FULL_DATAROOTDIR}/${PROJECT_NAME}-${${PROJECT_NAME}_VERSION}")

# - Configure output paths for products to give inplace build that roughly
#   matches install layout.
set(Falaise_BUILDPRODUCT_DIR "${PROJECT_BINARY_DIR}/BuildProducts")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${Falaise_BUILDPRODUCT_DIR}/${CMAKE_INSTALL_LIBDIR}")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY  "${Falaise_BUILDPRODUCT_DIR}/${CMAKE_INSTALL_BINDIR}")
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
option(Falaise_BUILD_DOCS "Build documentation for Falaise" OFF)
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
else()
  # - Configure internal build
  set(Bayeux_EMBEDDED 1)
  set(Bayeux_BUILDPRODUCT_DIR ${Falaise_BUILDPRODUCT_DIR})
  if(Falaise_ENABLE_TESTING)
    set(Bayeux_ENABLE_TESTING ON)
  endif()
  set(Bayeux_WITH_GEANT4 ON)

  # - Build
  add_subdirectory(utilities/flbayeux)

  # - Set up so that Falaise will use the internal build
  set(Bayeux_INCLUDE_DIRS "${Falaise_BUILDPRODUCT_DIR}/include" "${Falaise_BUILDPRODUCT_DIR}/include/bayeux")
  set(Bayeux_LIBRARIES Bayeux)
  set(Bayeux_MCTOOLS_GEANT4_LIBRARY Bayeux_mctools_geant4)
  set(Bayeux_DOXYGEN_TAGFILE)
endif()

set(Falaise_DATAROOTDIR ${Falaise_BUILDPRODUCT_DIR}/share/Falaise-${Falaise_VERSION})

#-----------------------------------------------------------------------
# Build the subdirectories as required
#
add_subdirectory(source)
if(Falaise_BUILD_DOCS)
  add_subdirectory(documentation)
endif()

#-----------------------------------------------------------------------
# Resource installation
#
set(_falaise_resources_dir "${CMAKE_SOURCE_DIR}/resources")

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
  string(REGEX REPLACE "^${CMAKE_SOURCE_DIR}/resources" "${Falaise_DATAROOTDIR}/resources" _rfout "${_rfout}")
  configure_file(${_rfin} ${_rfout} @ONLY)
endforeach()

#-----------------------------------------------------------------------
# Package Sources and Binaries
#
#include(FLCPack)
