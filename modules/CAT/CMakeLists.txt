# - Basic CMake setup
# Check version meets ou requirements
# Declare project, which will configure compiler for us
cmake_minimum_required(VERSION 3.3)
project(FalaiseCATPlugin VERSION "2.1.0")

# Modules use Falaise, so we need to locate this or fail
#find_package(Falaise REQUIRED)

# Ensure our code can see the Falaise headers
#include_directories(${Falaise_INCLUDE_DIRS})
include_directories(${ROOT_INCLUDE_DIRS})
include_directories(${CMAKE_CURRENT_BINARY_DIR})
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/source)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/source/falaise)

############################################################################################
# - Include directives to build the CellularAutomatonTracker source code (svn:externals)
#   from the Channel repository:
include(cmake/CAT_import.cmake)

############################################################################################
# - CAT & SULTAN modules:

# - Headers:
list(APPEND FalaiseCATPlugin_HEADERS
  source/falaise/snemo/reconstruction/cat_driver.h
  source/falaise/snemo/reconstruction/sultan_driver.h
  source/falaise/snemo/reconstruction/sultan_then_cat_driver.h
  source/falaise/snemo/reconstruction/cat_tracker_clustering_module.h
  source/falaise/snemo/reconstruction/sultan_tracker_clustering_module.h
)

# - Sources:
list(APPEND FalaiseCATPlugin_SOURCES
  source/falaise/snemo/reconstruction/cat_driver.cc
  source/falaise/snemo/reconstruction/sultan_driver.cc
  source/falaise/snemo/reconstruction/sultan_then_cat_driver.cc
  source/falaise/snemo/reconstruction/cat_tracker_clustering_module.cc
  source/falaise/snemo/reconstruction/sultan_tracker_clustering_module.cc
)

############################################################################################
#-----------------------------------------------------------------------
# CAT is shit tier quality, so disable shadowing warnings to avoid mental
# breakdown
if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-shadow")
endif()


# Build a dynamic library from our sources
add_library(Falaise_CAT SHARED ${FalaiseCATPlugin_HEADERS} ${FalaiseCATPlugin_SOURCES})
target_link_libraries(Falaise_CAT Falaise)

# Apple linker requires dynamic lookup of symbols, so we
# add link flags on this platform
if(APPLE)
  set_target_properties(Falaise_CAT
    PROPERTIES LINK_FLAGS "-undefined dynamic_lookup"
    )
endif()

# Install it:
install(TARGETS Falaise_CAT DESTINATION ${CMAKE_INSTALL_LIBDIR}/${FALAISE_PLUGINLIBDIR})

# Test support:
option(FalaiseCATPlugin_ENABLE_TESTING "Build unit testing system for FalaiseCATPlugin" ON)
if(FalaiseCATPlugin_ENABLE_TESTING)
  enable_testing()
  add_subdirectory(testing)
endif()
