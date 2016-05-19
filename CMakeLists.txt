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
project(Falaise VERSION "2.1.0")

# - Load custom modules
list(INSERT CMAKE_MODULE_PATH 0 ${PROJECT_SOURCE_DIR}/cmake)

#-----------------------------------------------------------------------
# Additional SVN Revision tracking for developers
include(SVNUtilities)
set(Falaise_VERSION_REVISION 0)

Subversion_DIRECTORY_IS_WC(${PROJECT_SOURCE_DIR} Falaise)
if(Falaise_DIRECTORY_IS_WC)
  Subversion_WC_INFO(${PROJECT_SOURCE_DIR} Falaise)
  set(Falaise_VERSION_REVISION ${Falaise_WC_REVISION})
endif()

#-----------------------------------------------------------------------
# - Standard UNIX Tool install paths, including relative paths for use
# by applications requiring resources
# Validate that certain paths are relative, otherwise relocation may fail
include(GNUInstallDirs)
foreach(_dir BINDIR LIBDIR INCLUDEDIR DATAROOTDIR)
  if(IS_ABSOLUTE "${CMAKE_INSTALL_${_dir}}")
    message(FATAL_ERROR "Absolute path for CMAKE_INSTALL_${_dir} not allowed")
  endif()
endforeach()

# Add a path for CMake config files
set(CMAKE_INSTALL_CMAKEDIR ${CMAKE_INSTALL_LIBDIR}/cmake)
set(CMAKE_INSTALL_FULL_CMAKEDIR ${CMAKE_INSTALL_FULL_LIBDIR}/cmake)

# Add a path for Falaise system plugins (will be relative to LIBDIR)
set(FALAISE_PLUGINLIBDIR "Falaise/modules")

# - Configure output paths for products to give inplace build that roughly
#   matches install layout.
set(FALAISE_BUILDPRODUCT_DIR "${PROJECT_BINARY_DIR}/BuildProducts")
set(FALAISE_DATAROOTDIR ${FALAISE_BUILDPRODUCT_DIR}/${CMAKE_INSTALL_DATAROOTDIR}/Falaise-${Falaise_VERSION})
file(RELATIVE_PATH FALAISE_CMAKEDIR_TO_INCLUDEDIR "${CMAKE_INSTALL_FULL_CMAKEDIR}/Falaise-${Falaise_VERSION}"  "${CMAKE_INSTALL_FULL_INCLUDEDIR}")
file(RELATIVE_PATH FALAISE_BINDIR_TO_RESOURCEDIR "${CMAKE_INSTALL_FULL_BINDIR}"  "${CMAKE_INSTALL_FULL_DATAROOTDIR}/Falaise-${Falaise_VERSION}")
file(RELATIVE_PATH FALAISE_BINDIR_TO_PLUGINLIBDIR "${CMAKE_INSTALL_FULL_BINDIR}" "${CMAKE_INSTALL_FULL_LIBDIR}/${FALAISE_PLUGINLIBDIR}")
file(RELATIVE_PATH FALAISE_LIBDIR_TO_RESOURCEDIR "${CMAKE_INSTALL_FULL_LIBDIR}"  "${CMAKE_INSTALL_FULL_DATAROOTDIR}/Falaise-${Falaise_VERSION}")
file(RELATIVE_PATH FALAISE_LIBDIR_TO_PLUGINLIBDIR "${CMAKE_INSTALL_FULL_LIBDIR}" "${CMAKE_INSTALL_FULL_LIBDIR}/${FALAISE_PLUGINLIBDIR}")


set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${FALAISE_BUILDPRODUCT_DIR}/${CMAKE_INSTALL_LIBDIR}")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${FALAISE_BUILDPRODUCT_DIR}/${CMAKE_INSTALL_BINDIR}")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${FALAISE_BUILDPRODUCT_DIR}/${CMAKE_INSTALL_LIBDIR}")

#-----------------------------------------------------------------------
# Compiler configuration
# - Enforce all warnings, but ensure other flags are retained
# Note that the added flags *won't* show up in the cache.
option(FALAISE_COMPILER_ERROR_ON_WARNING "Turn warnings into errors" ON)

if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|(Apple)+Clang")
  set(FALAISE_CXX_FLAGS "-Wall -Wextra -Wshadow -pedantic")

  # Disable specific warnings on GCC (-Wlong-long/pedantic) because Root source code (Rtypes.h)
  # makes use of unsupported type by ISO C++ 98 ([unsigned] long long int) and
  # the 'pragma GCC diagnostic ignored "-Wlong-long"' does not work
  if(CMAKE_CXX_COMPILER_ID MATCHES "GNU")
    set(FALAISE_CXX_FLAGS "${FALAISE_CXX_FLAGS} -Wno-long-long")
  endif()

  # Disable C99 extension warnings on Clang
  if(CMAKE_CXX_COMPILER_ID MATCHES "(Apple)+Clang")
    set(FALAISE_CXX_FLAGS "${FALAISE_CXX_FLAGS} -Wno-c99-extensions")
  endif()

  if(FALAISE_COMPILER_ERROR_ON_WARNING)
    # Use fatal-errors to fall over at first sign of trouble
    set(FALAISE_CXX_FLAGS "${FALAISE_CXX_FLAGS} -Werror -Wfatal-errors")
  endif()

  # On clang, need larger template depth
  if(CMAKE_CXX_COMPILER_ID MATCHES "(Apple)+Clang")
    set(FALAISE_CXX_FLAGS "${FALAISE_CXX_FLAGS} -ftemplate-depth=512")
  endif()

elseif(CMAKE_CXX_COMPILER_ID MATCHES "Intel")
  # Needs more testing
  set(FALAISE_CXX_FLAGS "-w2")
endif()

# - Prepend Falaise's flags to any preexisting ones, allowing user
#   to supply additional ones or override if required
set(CMAKE_CXX_FLAGS "${FALAISE_CXX_FLAGS} ${CMAKE_CXX_FLAGS}")

#-----------------------------------------------------------------------
# C++ Standard support
#
include(BXCMakeUtilities)

# - Do not want extensions
set(CMAKE_CXX_EXTENSIONS OFF)

# - Allow choice of standard
enum_option(FALAISE_CXX_STANDARD
  VALUES "98" "11" "14"
  TYPE   STRING
  DOC    "Choose C++ Standard to compile against"
  )

# - Default is 98, add an arbitrary feature here so that appropriate
#   flags/requirement is added and propagated
set(FALAISE_CXX_COMPILE_FEATURES cxx_template_template_parameters)

# - For C++11, add all features supported in GCC 4.9 and above, except
#   thread_local because that needs a workarounf on OS X
set(FALAISE_CXX11_COMPILE_FEATURES
  cxx_alias_templates
  cxx_alignas
  cxx_alignof
  cxx_attributes
  cxx_auto_type
  cxx_constexpr
  cxx_decltype
  cxx_decltype_incomplete_return_types
  cxx_default_function_template_args
  cxx_defaulted_functions
  cxx_defaulted_move_initializers
  cxx_delegating_constructors
  cxx_deleted_functions
  cxx_enum_forward_declarations
  cxx_explicit_conversions
  cxx_extended_friend_declarations
  cxx_extern_templates
  cxx_final
  cxx_func_identifier
  cxx_generalized_initializers
  cxx_inheriting_constructors
  cxx_inline_namespaces
  cxx_lambdas
  cxx_local_type_template_args
  cxx_long_long_type
  cxx_noexcept
  cxx_nonstatic_member_init
  cxx_nullptr
  cxx_override
  cxx_range_for
  cxx_raw_string_literals
  cxx_reference_qualified_functions
  cxx_right_angle_brackets
  cxx_rvalue_references
  cxx_sizeof_member
  cxx_static_assert
  cxx_strong_enums
  cxx_trailing_return_types
  cxx_unicode_literals
  cxx_uniform_initialization
  cxx_unrestricted_unions
  cxx_user_literals
  cxx_variadic_macros
  cxx_variadic_templates
  )

# - For C++14, add all features supported by current compiler
set(FALAISE_CXX14_COMPILE_FEATURES "${CMAKE_CXX14_COMPILE_FEATURES}")

# - Process compile features based on selected standard
if(FALAISE_CXX_STANDARD EQUAL 11)
  list(APPEND FALAISE_CXX_COMPILE_FEATURES ${FALAISE_CXX11_COMPILE_FEATURES})
endif()

# - If C++14 is requested, add all features supported by current
#   compiler. If no 14 features are supported, fail under assumption
#   compiler does not support it.
if(FALAISE_CXX_STANDARD EQUAL 14)
  if(NOT FALAISE_CXX14_COMPILE_FEATURES)
    message(FATAL_ERROR "C++14 requested, but no support for any C++14 features for compiler:\n"
      "'${CMAKE_CXX_COMPILER_ID}', '${CMAKE_CXX_COMPILER_VERSION}'"
      )
  endif()

  list(APPEND FALAISE_CXX_COMPILE_FEATURES
    ${FALAISE_CXX11_COMPILE_FEATURES}
    ${FALAISE_CXX14_COMPILE_FEATURES}
    )
endif()

#-----------------------------------------------------------------------
# Configure testing if required
#
option(FALAISE_ENABLE_TESTING "Build unit testing system for Falaise" OFF)
if(FALAISE_ENABLE_TESTING)
  enable_testing()
endif()

#-----------------------------------------------------------------------
# Configure documentation generation, if required
#
set(FALAISE_DOCEXPORT_DIR "${PROJECT_BINARY_DIR}/documentation")
option(FALAISE_WITH_DOCS "Build documentation for Falaise" ON)
if(FALAISE_WITH_DOCS)
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
option(FALAISE_WITH_DEVELOPER_TOOLS "Build/install developer tools" ON)
mark_as_advanced(FALAISE_WITH_DEVELOPER_TOOLS)

#-----------------------------------------------------------------------
# Bayeux is the main external dependency, and we know it will additionally
# search for and provide compatible versions of:
#
# - Boost
# - GSL
# - ROOT
# - Geant4
find_package(Bayeux 2.1.0 REQUIRED)
if(NOT Bayeux_MCTOOLS_GEANT4_FOUND)
  message(FATAL_ERROR "Bayeux found, but it does not supply the MCTools/Geant4 module")
endif()

#-----------------------------------------------------------------------
# Build the subdirectories as required
#
add_subdirectory(source)

if(FALAISE_WITH_DOCS)
  add_subdirectory(documentation)
endif()

#-----------------------------------------------------------------------
# Resource installation
#
add_subdirectory(resources)

#-----------------------------------------------------------------------
# - Install published resources and documentation
install(DIRECTORY ${FALAISE_DATAROOTDIR}
  DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}
  )

#-----------------------------------------------------------------------
# - CMake Support files
# - Config (build and install trees *at present*)
configure_file(${PROJECT_SOURCE_DIR}/cmake/FalaiseConfig.cmake.in
  ${FALAISE_BUILDPRODUCT_DIR}/${CMAKE_INSTALL_CMAKEDIR}/Falaise-${Falaise_VERSION}/FalaiseConfig.cmake
  @ONLY
  )

# - Versioning (build and install trees)
configure_file(${PROJECT_SOURCE_DIR}/cmake/FalaiseConfigVersion.cmake.in
  ${FALAISE_BUILDPRODUCT_DIR}/${CMAKE_INSTALL_CMAKEDIR}/Falaise-${Falaise_VERSION}/FalaiseConfigVersion.cmake
  @ONLY
  )

# - Targets (build tree)
set(_fbfile cmake/FalaiseBayeux-system.cmake.in)
configure_file(${_fbfile} ${FALAISE_BUILDPRODUCT_DIR}/${CMAKE_INSTALL_CMAKEDIR}/Falaise-${Falaise_VERSION}/FalaiseBayeux.cmake @ONLY)

export(TARGETS Falaise FalaiseModule ${Falaise_ADDON_TARGETS} FILE ${FALAISE_BUILDPRODUCT_DIR}/${CMAKE_INSTALL_CMAKEDIR}/Falaise-${Falaise_VERSION}/FalaiseTargets.cmake)

# - Targets (install tree)
install(EXPORT FalaiseTargets DESTINATION ${CMAKE_INSTALL_CMAKEDIR}/Falaise-${Falaise_VERSION})
set(_fbinstallfile ${FALAISE_BUILDPRODUCT_DIR}/${CMAKE_INSTALL_CMAKEDIR}/Falaise-${Falaise_VERSION}/FalaiseBayeux.cmake)

# - Installation of, well, install tree files
install(
  FILES
    ${FALAISE_BUILDPRODUCT_DIR}/${CMAKE_INSTALL_CMAKEDIR}/Falaise-${Falaise_VERSION}/FalaiseConfigVersion.cmake
    ${FALAISE_BUILDPRODUCT_DIR}/${CMAKE_INSTALL_CMAKEDIR}/Falaise-${Falaise_VERSION}/FalaiseConfig.cmake
    ${_fbinstallfile}
  DESTINATION
    ${CMAKE_INSTALL_CMAKEDIR}/Falaise-${Falaise_VERSION}
  )

#-----------------------------------------------------------------------
# - Plugins/modules installation - Must come last because they must find
# Falaise
add_subdirectory(modules)

# # - Online software
# option(FALAISE_ONLINE_EXPERIMENT "Built the experiment on-line software" ON)

# if (FALAISE_ONLINE_EXPERIMENT)
#   add_subdirectory(source/flexperiment)
# endif()

#-----------------------------------------------------------------------
# Package Sources and Binaries
#
#include(FLCPack)
