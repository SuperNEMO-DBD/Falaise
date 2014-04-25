# - Basic CMake setup
# Check version meets ou requirements
# Declare project, which will configure compiler for us
cmake_minimum_required(VERSION 2.8)
project(FalaiseVisuToyPlugin)

# Modules use Falaise, so we need to locate this or fail
# find_package(Falaise REQUIRED)

# Ensure our code can see the Falaise headers
#include_directories(${Falaise_INCLUDE_DIRS})
include_directories(${Falaise_BUILDPRODUCT_DIR}/include)
include_directories(${Falaise_BUILDPRODUCT_DIR}/include/falaise)
include_directories(${Bayeux_INCLUDE_DIRS})
include_directories(${CMAKE_CURRENT_BINARY_DIR})
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/source)


############################################################################################
# - VisuToy module:
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/source/falaise)

# - Headers:
list(APPEND FalaiseVisuToyPlugin_HEADERS source/falaise/snemo/visualization/toy_display_driver.h)
list(APPEND FalaiseVisuToyPlugin_HEADERS source/falaise/snemo/visualization/visu_toy_module.h)

# - Sources:
list(APPEND FalaiseVisuToyPlugin_SOURCES source/falaise/snemo/visualization/toy_display_driver.cc)
list(APPEND FalaiseVisuToyPlugin_SOURCES source/falaise/snemo/visualization/visu_toy_module.cc)

############################################################################################

# Build a dynamic library from our sources
add_library(Falaise_VisuToy SHARED
  ${FalaiseVisuToyPlugin_HEADERS}
  ${FalaiseVisuToyPlugin_SOURCES})

target_link_libraries(Falaise_VisuToy Falaise)

# Apple linker requires dynamic lookup of symbols, so we
# add link flags on this platform
if(APPLE)
  set_target_properties(Falaise_VisuToy
    PROPERTIES LINK_FLAGS "-undefined dynamic_lookup"
    )
endif()

# Install it:
install(TARGETS Falaise_VisuToy DESTINATION ${CMAKE_INSTALL_LIBDIR}/Falaise/modules)

# Test support:
option(FalaiseVisuToyPlugin_ENABLE_TESTING "Build unit testing system for FalaiseVisuToyPlugin" ON)
if(FalaiseVisuToyPlugin_ENABLE_TESTING)
  enable_testing()
  add_subdirectory(testing)
endif()
