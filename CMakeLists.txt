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
project(Falaise VERSION "4.0.1")

# - Load custom modules
list(INSERT CMAKE_MODULE_PATH 0 ${PROJECT_SOURCE_DIR}/cmake)

#-----------------------------------------------------------------------
# Needed Modules
#
#- Common LPC utilities
include(LPCCMakeSettings)

# - Clang Format/Tidy
include(ClangTools)

# - Add Git info to versioning
#   1. Add "-g<HEADHASH:0-8>" when in Git working copy
#   2. If there are uncommitted local changes, add "-dirty"
include(GetGitRevisionDescription)
get_git_head_revision(Falaise_GIT_REFSPEC Falaise_GIT_HASH)
git_local_changes(Falaise_GIT_STATE)
if(Falaise_GIT_HASH)
  string(SUBSTRING "${Falaise_GIT_HASH}" 0 8 Falaise_GIT_HASH_SHORT)
  set(Falaise_VERSION "${Falaise_VERSION}-g${Falaise_GIT_HASH_SHORT}")
  if(Falaise_GIT_STATE STREQUAL "DIRTY")
    set(Falaise_VERSION "${Falaise_VERSION}-dirty")
    set(FALAISE_VERSION_IS_DIRTY YES)
  endif()
endif()

#-----------------------------------------------------------------------
# Configure testing if required
#
option(FALAISE_ENABLE_TESTING "Build unit testing system for Falaise" OFF)
if(FALAISE_ENABLE_TESTING)
  enable_testing()
endif()

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
set(FALAISE_BAYEUX_MIN_VERSION 3.4.1)
find_package(Bayeux ${FALAISE_BAYEUX_MIN_VERSION} REQUIRED geant4)

#-----------------------------------------------------------------------
# Build the subdirectories as required
#
add_subdirectory(utilities)
add_subdirectory(source)

if(FALAISE_WITH_DOCS)
  add_subdirectory(documentation)
endif()

#-----------------------------------------------------------------------
# Copy/Install resources
#
file(COPY resources/ DESTINATION "${PROJECT_BUILD_DATAROOTDIR}/${PROJECT_RESOURCE_DIR}")
install(DIRECTORY resources/
  DESTINATION "${CMAKE_INSTALL_DATAROOTDIR}/${PROJECT_RESOURCE_DIR}"
  )

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
# Also programs whilst migrating FLVisualize
add_subdirectory(programs)
add_subdirectory(modules)

#-----------------------------------------------------------------------
# - Install published resources and documentation
install(DIRECTORY ${PROJECT_BUILD_DATAROOTDIR}/
  DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}
  )

# - end
