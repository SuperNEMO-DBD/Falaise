# - Top level CMakeLists.txt for EventBrowser project
#-----------------------------------------------------------------------
# Copyright (c) 2011-2014, Xavier Garrido <garrido@lal.in2p3.fr>
#                          François Mauger <mauger@lpccaen.in2p3.fr>
#                          Ben Morgan <Ben.Morgan@warwick.ac.uk>

#-----------------------------------------------------------------------
# Project configuration
#
cmake_minimum_required(VERSION 3.3 FATAL_ERROR)
project(FalaiseEventBrowserPlugin)

# - Versioning
set(FalaiseEventBrowserPlugin_VERSION_MAJOR 3)
set(FalaiseEventBrowserPlugin_VERSION_MINOR 0)
set(FalaiseEventBrowserPlugin_VERSION_PATCH 0)

#-----------------------------------------------------------------------
# Find external packages needed to build
#
# - Boost
set(Boost_COMPONENTS thread program_options)
find_package(Boost 1.55 QUIET COMPONENTS ${Boost_COMPONENTS} NO_MODULE)
find_package(Boost 1.55 REQUIRED ${Boost_COMPONENTS})

# - ROOT
if(NOT FALAISE_USE_LEGACY_ROOT)
  set(CMAKE_MODULE_PATH_PREROOT ${CMAKE_MODULE_PATH})
endif()
set(ROOT_COMPONENTS Gui Geom GeomPainter Physics Graf3d)
find_package(ROOT 5.34.00 QUIET COMPONENTS ${ROOT_COMPONENTS} NO_MODULE)
find_package(ROOT 5.34.00 REQUIRED ${ROOT_COMPONENTS})
if(NOT FALAISE_USE_LEGACY_ROOT)
  include(RootNewMacros)
  set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH_PREROOT})
endif()

#-----------------------------------------------------------------------
# Include directories for all subdirectories
#
include_directories(
  ${PROJECT_BINARY_DIR}/include
  ${PROJECT_SOURCE_DIR}/include
  ${ROOT_INCLUDE_DIRS}
  )

#-----------------------------------------------------------------------
# Build the submodules as required
#
option(FalaiseEventBrowserPlugin_ENABLE_OPENGL "Enable support for OpenGL rendering" OFF)
if(FalaiseEventBrowserPlugin_ENABLE_OPENGL)
  set(EVENTBROWSER_USE_OPENGL 1)
endif()
add_subdirectory(source)
add_subdirectory(programs)

if(Falaise_ENABLE_TESTING)
  enable_testing()
  add_subdirectory(testing)
endif()

# - end of top level CMakeLists.txt for EventBrowser project.
