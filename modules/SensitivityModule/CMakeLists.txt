# - Basic CMake setup
# Check version meets ou requirements
# Declare project, which will configure compiler for us
cmake_minimum_required(VERSION 3.3)
project(SensitivityModule)

# Build a dynamic library from our sources
add_library(SensitivityModule SHARED SensitivityModule.h SensitivityModule.cpp TrackDetails.h trackDetails.cpp)
# Back compat for data model API, now always new
target_compile_definitions(SensitivityModule PRIVATE NEW_DATAMODEL_API)

# Link it to the FalaiseModule library
# This ensures the correct compiler flags, include paths
# and linker flags are applied to our dynamic library.
# - Explicit link to ROOT Physics library which is not linked
#   by flreconstruct
target_link_libraries(SensitivityModule
  PUBLIC
    FalaiseModule
    ${ROOT_Physics_LIBRARY}
    )

# Install Targets
install(TARGETS SensitivityModule DESTINATION ${CMAKE_INSTALL_PLUGINDIR})

# Configure example pipeline script for use from the build dir
configure_file("SensitivityModuleExample.conf.in" "SensitivityModuleExample.conf" @ONLY)

# Add a basic test of reading a brio file output by the
# standard pipeline
enable_testing()
# - Simulate
add_test(NAME testSensitivityModule_simulate
  COMMAND flsimulate -o test-simulate.brio
  WORKING_DIRECTORY ${PROJECT_BINARY_DIR}
  )
# - Reconstruct
add_test(NAME testSensitivityModule_reconstruct
  COMMAND flreconstruct -i test-simulate.brio -p urn:snemo:demonstrator:reconstruction:3.0:config:default -o test-reconstruct.brio
  WORKING_DIRECTORY ${PROJECT_BINARY_DIR}
  )
set_tests_properties(testSensitivityModule_reconstruct
  PROPERTIES DEPENDS testSensitivityModule_simulate
  )
# - Run Module
add_test(NAME testSensitivityModule_sensitivity
  COMMAND flreconstruct -i test-reconstruct.brio -p SensitivityModuleExample.conf
  WORKING_DIRECTORY ${PROJECT_BINARY_DIR}
  )
set_tests_properties(testSensitivityModule_sensitivity
  PROPERTIES DEPENDS testSensitivityModule_reconstruct
  )