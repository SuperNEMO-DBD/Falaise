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
project(Falaise VERSION "5.1.0")

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

set(_falaise_TEST_RESOURCE_DIR "${PROJECT_BUILD_DATAROOTDIR}/${PROJECT_RESOURCE_DIR}")
# Common testing environment
set(_falaise_TEST_ENVIRONMENT
  "FALAISE_RESOURCE_DIR=${_falaise_TEST_RESOURCE_DIR}" 
  "PATH=${PROJECT_BUILD_BINDIR}:$ENV{PATH}"
  )
message(STATUS "_falaise_TEST_ENVIRONMENT = ${_falaise_TEST_ENVIRONMENT}")

macro(set_falaise_test_environment _tname)
  set_property(TEST ${_tname}
    APPEND PROPERTY ENVIRONMENT ${_falaise_TEST_ENVIRONMENT}
  )
endmacro()

if(FALAISE_ENABLE_TESTING)
  enable_testing()
endif()

#-----------------------------------------------------------------------
# Optional build of documentation
#
option(FALAISE_WITH_DOCS "Build documentation for Falaise" ON)
set(Falaise_WITH_DOCS 0)
if (FALAISE_WITH_DOCS)
  set(Falaise_WITH_DOCS 1)
endif(FALAISE_WITH_DOCS)

#-----------------------------------------------------------------------
# Bayeux is the main external dependency, and we know it will additionally
# search for and provide compatible versions of:
# - CLHEP
# - CAMP
# - Boost
# - GSL
# - ROOT
# - Geant4
set(FALAISE_BAYEUX_MIN_VERSION 3.5.4)
find_package(Bayeux ${FALAISE_BAYEUX_MIN_VERSION} REQUIRED geant4)

# Depends on SNRS (https://github.com/SuperNEMO-DBD/SNRS)
set(FALAISE_SNRS_MIN_VERSION 1.1)
find_program(FalaiseSnrsConfigExe snrs-config DOC "Path to the snrs-config utility" REQUIRED)
if (NOT FalaiseSnrsConfigExe)
  message(FATAL_ERROR "The SNRS package was not located on this system")
endif()
execute_process(COMMAND ${FalaiseSnrsConfigExe} --libdir
  OUTPUT_VARIABLE FalaiseSnrsLibPath
  ERROR_VARIABLE FalaiseSnrsConfigError
  OUTPUT_STRIP_TRAILING_WHITESPACE)
if (FalaiseSnrsConfigError)
  message(FATAL_ERROR "Failed to determine SNRS library path: ${FalaiseSnrsConfigError}")
endif()
execute_process(COMMAND ${FalaiseSnrsConfigExe} --incdir
  OUTPUT_VARIABLE FalaiseSnrsIncPath
  ERROR_VARIABLE FalaiseSnrsConfigError
  OUTPUT_STRIP_TRAILING_WHITESPACE)
if (FalaiseSnrsConfigError)
  message(FATAL_ERROR "Failed to determine SNRS include path: ${FalaiseSnrsConfigError}")
endif()
execute_process(COMMAND ${FalaiseSnrsConfigExe} --resourcedir
  OUTPUT_VARIABLE FalaiseSnrsResourcePath
  ERROR_VARIABLE FalaiseSnrsConfigError
  OUTPUT_STRIP_TRAILING_WHITESPACE)
if (FalaiseSnrsConfigError)
  message(FATAL_ERROR "Failed to determine SNRS resource files path: ${FalaiseSnrsConfigError}")
endif()
execute_process(COMMAND ${FalaiseSnrsConfigExe} --version
  OUTPUT_VARIABLE FalaiseSnrsVersion
  ERROR_VARIABLE FalaiseSnrsConfigError
  OUTPUT_STRIP_TRAILING_WHITESPACE)
if (FalaiseSnrsConfigError)
  message(FATAL_ERROR "Failed to determine SNRS version: ${FalaiseSnrsConfigError}")
endif()
if (FalaiseSnrsVersion VERSION_LESS ${FALAISE_SNRS_MIN_VERSION})
  message(FATAL_ERROR "SNRS version ${FalaiseSnrsVersion} is not supported")
endif()
message( STATUS "SNRS version : ${FalaiseSnrsVersion}")
message( STATUS "SNRS library is loadable from '${FalaiseSnrsLibPath}'")
message( STATUS "SNRS' header from '${FalaiseSnrsIncPath}'")
message( STATUS "SNRS resource files are loadable from '${FalaiseSnrsResourcePath}'")
set(Falaise_SNRS_INC_PATH ${FalaiseSnrsIncPath})
set(Falaise_SNRS_LIB_PATH ${FalaiseSnrsLibPath})
set(Falaise_SNRS_RESOURCE_PATH ${FalaiseSnrsResourcePath})
set(Falaise_SNRS_VERSION ${FalaiseSnrsVersion})

# Depends on Python3 for some tests
find_program(Python3Exe python3 DOC "Path to the python3 program" REQUIRED)
if (NOT Python3Exe)
  message(FATAL_ERROR "The python3 executable was not located on this system")
endif()

# # Hack for Boost 1.69:
# set(PROJECT_CXX_FLAGS "${PROJECT_CXX_FLAGS} -Wno-error=pedantic")

#-----------------------------------------------------------------------
# Build the subdirectories as required
#
add_subdirectory(utilities)

# This version of Falaise uses defines configuration paths 
# for various modelling topics:
set(FALAISE_DEFAULT_GEOMETRY_TAG "urn:snemo:demonstrator:geometry:5.0")
set(FALAISE_DEFAULT_SETUP_TAG "urn:snemo:demonstrator:setup:2.0")
set(FALAISE_DEFAULT_SIMULATION_TAG "urn:snemo:demonstrator:simulation:2.4")
set(FALAISE_DEFAULT_RECONSTRUCTION_TAG "urn:snemo:demonstrator:reconstruction:3.0")

add_subdirectory(source)

if(FALAISE_WITH_DOCS)
  add_subdirectory(documentation)
endif()

#-----------------------------------------------------------------------
# Install resources
#
# add_subdirectory(resources)
# Build tree
# This allows the programs to be run directly without requiring FALAISE_RESOURCE_DIR
# to be set.
# add_custom_command(OUTPUT ${PROJECT_BUILD_DATAROOTDIR}/${PROJECT_RESOURCE_DIR}
#   COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/resources ${PROJECT_BUILD_DATAROOTDIR}/${PROJECT_RESOURCE_DIR}
#   COMMENT "Copying Falaise resources directory into build directory"
#   )
add_custom_target(FalaiseResourcesCopyResourceDir ALL
  COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/resources ${PROJECT_BUILD_DATAROOTDIR}/${PROJECT_RESOURCE_DIR}
  COMMENT "Copying Falaise resources directory into build directory"
  )
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/resources/urn/db/falaise.conf.in
  ${PROJECT_BUILD_DATAROOTDIR}/${PROJECT_RESOURCE_DIR}/urn/db/falaise.conf @ONLY)

# Clean template config files:
add_custom_target(FalaiseResourcesCleanUrnDbFalaiseConf ALL
  COMMAND ${CMAKE_COMMAND} -E rm -f ${PROJECT_BUILD_DATAROOTDIR}/${PROJECT_RESOURCE_DIR}/urn/db/falaise.conf.in
  DEPENDS FalaiseResourcesCopyResourceDir) 

add_custom_target(FalaiseResourcesPublishResourceDir ALL
  DEPENDS ${PROJECT_BUILD_DATAROOTDIR}/${PROJECT_RESOURCE_DIR} )

# Install tree
install(DIRECTORY ${PROJECT_BUILD_DATAROOTDIR}/
  DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}
  # Exclude resources symlink
  # PATTERN "${PROJECT_RESOURCE_DIR}" EXCLUDE
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
add_subdirectory(analysis)

configure_file(
  cmake/falaise-config.in
  ${PROJECT_BINARY_DIR}/falaise-config
  @ONLY)

install(PROGRAMS ${PROJECT_BINARY_DIR}/falaise-config
  DESTINATION ${CMAKE_INSTALL_BINDIR}
  )

# - end
