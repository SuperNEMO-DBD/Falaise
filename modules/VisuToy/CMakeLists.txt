# - Basic CMake setup
# Check version meets ou requirements
# Declare project, which will configure compiler for us
cmake_minimum_required(VERSION 3.3)
project(FalaiseVisuToyPlugin VERSION "2.1.0")

# Modules use Falaise, so we need to locate this or fail
# find_package(Falaise REQUIRED)

# Ensure our code can see the Falaise headers
#include_directories(${Falaise_INCLUDE_DIRS})
include_directories(${FALAISE_BUILDPRODUCT_DIR}/include)
include_directories(${FALAISE_BUILDPRODUCT_DIR}/include/falaise)
include_directories(${Bayeux_INCLUDE_DIRS})
include_directories(${CMAKE_CURRENT_BINARY_DIR})
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

list(INSERT CMAKE_MODULE_PATH 0 ${PROJECT_SOURCE_DIR}/cmake)

# - Readline:
find_package(Readline QUIET)
if (Readline_FOUND)
  set(FALAISEVISUTOYPLUGIN_WITH_READLINE 1)
endif()

############################################################################################
# - VisuToy module:
configure_file(VisuToy/visu_toy_config.h.in
  ${CMAKE_CURRENT_BINARY_DIR}/VisuToy/visu_toy_config.h
  @ONLY)


# - Headers:
list(APPEND FalaiseVisuToyPlugin_HEADERS VisuToy/toy_display_driver.h)
list(APPEND FalaiseVisuToyPlugin_HEADERS VisuToy/visu_toy_module.h
            ${CMAKE_CURRENT_BINARY_DIR}/VisuToy/visu_toy_config.h)

# - Sources:
list(APPEND FalaiseVisuToyPlugin_SOURCES VisuToy/toy_display_driver.cc)
list(APPEND FalaiseVisuToyPlugin_SOURCES VisuToy/visu_toy_module.cc)

############################################################################################

# Build a dynamic library from our sources
add_library(Falaise_VisuToy SHARED
  ${FalaiseVisuToyPlugin_HEADERS}
  ${FalaiseVisuToyPlugin_SOURCES})

target_link_libraries(Falaise_VisuToy Falaise)

if(FALAISEVISUTOYPLUGIN_WITH_READLINE)
  include_directories(${Readline_INCLUDE_DIR})
  target_link_libraries(Falaise_VisuToy ${Readline_LIBRARIES})
endif()

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
  add_subdirectory(VisuToy/test)
endif()
