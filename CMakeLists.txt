# - Top level CMake script for Falaise

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
cmake_minimum_required(VERSION 3.3 FATAL_ERROR)
project(Falaise VERSION "3.1.0")

# - Load custom modules
list(INSERT CMAKE_MODULE_PATH 0 ${PROJECT_SOURCE_DIR}/cmake)

#-----------------------------------------------------------------------
# Common LPC utilities
#
include(LPCCMakeSettings)
include(ClangTools)

# - SVN revision number for dev builds
include(LPCSVNUtilities)
set(Falaise_VERSION_REVISION 0)

Subversion_DIRECTORY_IS_WC(${PROJECT_SOURCE_DIR} Falaise)
if(Falaise_DIRECTORY_IS_WC)
  Subversion_WC_INFO(${PROJECT_SOURCE_DIR} Falaise)
  set(Falaise_VERSION_REVISION ${Falaise_WC_REVISION})
endif()

#-----------------------------------------------------------------------
# Configure testing if required
#
option(FALAISE_ENABLE_TESTING "Build unit testing system for Falaise" OFF)
if(FALAISE_ENABLE_TESTING)
  enable_testing()
endif()

#-----------------------------------------------------------------------
# Configure/Build utilities as needed
#
option(FALAISE_WITH_DEVELOPER_TOOLS "Build/install developer tools" ON)
mark_as_advanced(FALAISE_WITH_DEVELOPER_TOOLS)

#-----------------------------------------------------------------------
# Optional build of documentation
#
option(FALAISE_WITH_DOCS "Build documentation for Falaise" ON)

#-----------------------------------------------------------------------
# Bayeux is the main external dependency, and we know it will additionally
# search for and provide compatible versions of:
#
# - Boost
# - GSL
# - ROOT
# - Geant4
set(FALAISE_BAYEUX_MIN_VERSION 3.0.0)
message( STATUS "Searching Bayeux ${FALAISE_BAYEUX_MIN_VERSION} ...")
find_package(Bayeux ${FALAISE_BAYEUX_MIN_VERSION} REQUIRED geant4)
# message( STATUS "Bayeux_VERSION          = '${Bayeux_VERSION}'")
# message( STATUS "Bayeux_CMAKE_CONFIG_DIR = '${Bayeux_CMAKE_CONFIG_DIR}'")

#-----------------------------------------------------------------------
# Build the subdirectories as required
#
add_subdirectory(utilities)
add_subdirectory(source)
add_subdirectory(resources)

if(FALAISE_WITH_DOCS)
  add_subdirectory(documentation)
endif()

#-----------------------------------------------------------------------
# - CMake Support files
include(CMakePackageConfigHelpers)

# - Versioning file is the same for the build and install trees
write_basic_package_version_file(
  ${FALAISE_BUILD_CMAKEDIR}/${FALAISE_TAG}/FalaiseConfigVersion.cmake
  COMPATIBILITY SameMajorVersion
  )

# - Config file is also the same in build/install trees as we use same layout
configure_package_config_file(
  ${PROJECT_SOURCE_DIR}/cmake/FalaiseConfig.cmake.in
  ${FALAISE_BUILD_CMAKEDIR}/${FALAISE_TAG}/FalaiseConfig.cmake
  INSTALL_DESTINATION ${CMAKE_INSTALL_CMAKEDIR}/${PROJECT_TAG}
  PATH_VARS
    CMAKE_INSTALL_BINDIR
    CMAKE_INSTALL_LIBDIR
    CMAKE_INSTALL_INCLUDEDIR
    CMAKE_INSTALL_DATAROOTDIR
    )

# - Targets (build tree)
export(EXPORT FalaiseTargets
  NAMESPACE Falaise::
  FILE ${FALAISE_BUILD_CMAKEDIR}/${FALAISE_TAG}/FalaiseTargets.cmake
  )

# - Targets (install tree)
install(EXPORT FalaiseTargets
  NAMESPACE Falaise::
  DESTINATION ${CMAKE_INSTALL_CMAKEDIR}/${FALAISE_TAG}
  )

# - Installation of, well, install tree files
install(
  FILES
    ${PROJECT_BUILD_CMAKEDIR}/${FALAISE_TAG}/FalaiseConfigVersion.cmake
    ${PROJECT_BUILD_CMAKEDIR}/${FALAISE_TAG}/FalaiseConfig.cmake
  DESTINATION
    ${CMAKE_INSTALL_CMAKEDIR}/${FALAISE_TAG}
  )

#-----------------------------------------------------------------------
# - Plugins/modules installation - Must come last because they must find
# Falaise, so require the config/targets file to have been generated
add_subdirectory(modules)

#-----------------------------------------------------------------------
# Configure/Build companions as needed
#
option(FALAISE_WITH_COMPANIONS "Build/install companions tools" OFF)
mark_as_advanced(FALAISE_WITH_COMPANIONS)
if (FALAISE_WITH_COMPANIONS)
  add_subdirectory(companions)
endif()

#-----------------------------------------------------------------------
# - Install published resources and documentation
install(DIRECTORY ${PROJECT_BUILD_DATAROOTDIR}/
  DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}
  )

# - end
