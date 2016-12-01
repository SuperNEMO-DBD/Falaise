# CMAKE_COMPILER
# - Basic CMake setup
# Check version meets ou requirements
# Declare project, which will configure compiler for us
cmake_minimum_required(VERSION 3.0)
project(FalaiseAnalogSignalBuilderPlugin)

set(FalaiseASB_VERSION_MAJOR 0)
set(FalaiseASB_VERSION_MINOR 1)
set(FalaiseASB_VERSION_PATCH 0)
set(FalaiseASB_VERSION_REVISION 0)

# # - ROOT
# # find_package(ROOT 5.34.00 REQUIRED Gui Geom GeomPainter Physics)
# set(ROOT_COMPONENTS Gui Gpad Geom GeomPainter Physics Graf Graf3d)
# find_package(ROOT 5.34.00 QUIET COMPONENTS ${ROOT_COMPONENTS} NO_MODULE)
# find_package(ROOT 5.34.00 REQUIRED ${ROOT_COMPONENTS})

# Ensure our code can see the Falaise headers
#include_directories(${Falaise_INCLUDE_DIRS})
include_directories(${CMAKE_CURRENT_BINARY_DIR}/source)
include_directories(${CMAKE_CURRENT_BINARY_DIR}/source/falaise)
include_directories(${Falaise_BUILDPRODUCT_DIR}/include)
include_directories(${Falaise_BUILDPRODUCT_DIR}/include/falaise)
include_directories(${Bayeux_INCLUDE_DIRS})
include_directories(${CMAKE_CURRENT_BINARY_DIR})
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/source)

############################################################################################
# - AnalogSignalBuilder modules:
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/source/falaise)

# - Prepare files from skelettons:
configure_file(source/falaise/snemo/asb/version.h.in
  ${CMAKE_CURRENT_BINARY_DIR}/source/falaise/snemo/asb/version.h
  @ONLY)

# - Headers:
list(APPEND FalaiseAnalogSignalBuilderPlugin_HEADERS
  ${CMAKE_CURRENT_BINARY_DIR}/source/falaise/snemo/asb/version.h
  # source/falaise/snemo/asb/the_serializable.ipp
  # source/falaise/snemo/asb/the_serializable.h
  )

# - Sources:
list(APPEND FalaiseAnalogSignalBuilderPlugin_SOURCES
  source/falaise/snemo/asb/version.cc
  # source/falaise/snemo/asb/the_serializable.cc
  #source/falaise/snemo/asb/sim_signal_module.cc

  )

############################################################################################

# Build a dynamic library from our sources
add_library(Falaise_AnalogSignalBuilder SHARED
  ${FalaiseAnalogSignalBuilderPlugin_HEADERS}
  ${FalaiseAnalogSignalBuilderPlugin_SOURCES})

target_link_libraries(Falaise_AnalogSignalBuilder
  Falaise)

# Apple linker requires dynamic lookup of symbols, so we
# add link flags on this platform
if(APPLE)
  set_target_properties(Falaise_AnalogSignalBuilder
    PROPERTIES LINK_FLAGS "-undefined dynamic_lookup"
    )
endif()

# Install it:
install(TARGETS Falaise_AnalogSignalBuilder DESTINATION ${CMAKE_INSTALL_LIBDIR}/Falaise/modules)

# Test support:
option(FalaiseAnalogSignalBuilderPlugin_ENABLE_TESTING "Build unit testing system for FalaiseAnalogSignalBuilder" ON)
if(FalaiseAnalogSignalBuilderPlugin_ENABLE_TESTING)
  enable_testing()
  add_subdirectory(testing)
endif()
