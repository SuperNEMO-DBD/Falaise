# - Basic CMake setup
# Check version meets ou requirements
# Declare project, which will configure compiler for us
cmake_minimum_required(VERSION 3.3)
project(FalaiseSimRCPlugin)

# Build a dynamic library from our sources
add_library(Falaise_SimRC SHARED
  snsimrc/simrc_module.hpp
  snsimrc/simrc_module.cpp
  )

include_directories(${CMAKE_CURRENT_BINARY_DIR})
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

# Back compat for data model API, now always new
# target_compile_definitions(Falaise_SimRC PRIVATE NEW_DATAMODEL_API)

# Link it to the FalaiseModule library
# This ensures the correct compiler flags, include paths
# and linker flags are applied to our dynamic library.
# - Explicit link to ROOT Physics library which is not linked
#   by flreconstruct
target_link_libraries(Falaise_SimRC
  PUBLIC
    FalaiseModule
    )

# Apple linker requires dynamic lookup of symbols, so we
# add link flags on this platform
if(APPLE)
  set_target_properties(Falaise_SimRC
    PROPERTIES
      LINK_FLAGS "-undefined dynamic_lookup"
      INSTALL_RPATH "@loader_path"
    )
endif()

# Install Targets
install(TARGETS Falaise_SimRC DESTINATION ${CMAKE_INSTALL_PLUGINDIR})



# # Test support:
# option(FalaiseSimRCPlugin_ENABLE_TESTING "Build unit testing system for FalaiseSimRCPlugin" ON)
# if(FalaiseSimRCPluginPlugin_ENABLE_TESTING)
#   enable_testing()
#   add_subdirectory(testing)
# endif()


# Configure example pipeline script for use from the build dir
# configure_file("SimRCExample.conf.in" "SimRCModuleExample.conf" @ONLY)

# Add a basic test of reading a brio file output by the
# standard pipeline
# enable_testing()
# - Simulate
# add_test(NAME testSimRC_simulate
#   COMMAND flsimulate -o test-simulate.brio
#   WORKING_DIRECTORY ${PROJECT_BINARY_DIR}
#   )


# - end
