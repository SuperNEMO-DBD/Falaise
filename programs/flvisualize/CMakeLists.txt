# - Top level CMakeLists.txt for EventBrowser project
#-----------------------------------------------------------------------
# Copyright (c) 2011-2014, Xavier Garrido <garrido@lal.in2p3.fr>
#                          François Mauger <mauger@lpccaen.in2p3.fr>
#                          Ben Morgan <Ben.Morgan@warwick.ac.uk>

#-----------------------------------------------------------------------
# Project configuration
#
cmake_minimum_required(VERSION 3.3 FATAL_ERROR)
project(FLVisualize VERSION 3.0.0)

#-----------------------------------------------------------------------
# Find external packages needed to build
#
# - Boost
set(Boost_COMPONENTS thread program_options)
find_package(Boost 1.55 REQUIRED ${Boost_COMPONENTS})

# - ROOT
set(ROOT_COMPONENTS Gui Geom GeomPainter Physics Graf3d)
find_package(ROOT 6.12 REQUIRED ${ROOT_COMPONENTS})
include(${ROOT_DIR}/modules/RootNewMacros.cmake)

#-----------------------------------------------------------------------
# Build the submodules as required
#
option(FLVISUALIZE_ENABLE_OPENGL "Enable support for OpenGL rendering" OFF)
if(FLVISUALIZE_ENABLE_OPENGL)
  set(EVENTBROWSER_USE_OPENGL 1)
endif()

add_subdirectory(EventBrowser)
add_subdirectory(programs)

if(Falaise_ENABLE_TESTING)
  enable_testing()
  add_subdirectory(testing)
endif()

# Copy resources to build directory (assumes at present that we are within falaise)
# Only need to copy dir, Falaise install handles rest.
set(FLVISUALIZE_STYLE_FILES
  styles/snemo_default.sty
  styles/snemo_demonstrator_light.sty
  styles/snemo_shielding.sty
  styles/snemo_demonstrator_default.sty
  styles/snemo_light.sty
  styles/snemo_tracker_commissioning_default.sty
)
foreach(_stylefile ${FLVISUALIZE_STYLE_FILES})
  configure_file(${_stylefile} "${PROJECT_BUILD_DATAROOTDIR}/${PROJECT_RESOURCE_DIR}/EventBrowser/${_stylefile}" COPYONLY)
endforeach()

# - end of top level CMakeLists.txt for EventBrowser project.
