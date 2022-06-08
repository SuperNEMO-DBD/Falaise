# - Basic CMake setup
# Check version meets ou requirements
# Declare project, which will configure compiler for us
cmake_minimum_required(VERSION 3.3)
project(FalaiseSimRCPlugin)

# Build a dynamic library from our sources
add_library(Falaise_SimRC SHARED
  snsimrc/event_timestamper.hpp
  snsimrc/event_timestamper.cpp
  snsimrc/tracker_cell_tagger.hpp
  snsimrc/tracker_cell_tagger.cpp
  snsimrc/calorimeter_om_tagger.hpp
  snsimrc/calorimeter_om_tagger.cpp
  snsimrc/simrc_module.hpp
  snsimrc/simrc_module.cpp
  # snsimrc/simrc_plugin.cpp
  snsimrc/simrc_lib.cpp
  snsimrc/simrc_lib.hpp
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

add_custom_target(Falaise_SimRC_Resources ALL
  COMMAND ${CMAKE_COMMAND} -E make_directory "${PROJECT_BUILD_DATAROOTDIR}/${PROJECT_RESOURCE_DIR}/modules/SimRC/config"
  COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_CURRENT_SOURCE_DIR}/config/activity_model_factory.conf" "${PROJECT_BUILD_DATAROOTDIR}/${PROJECT_RESOURCE_DIR}/modules/SimRC/"
  COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_CURRENT_SOURCE_DIR}/config/activity_models.conf" "${PROJECT_BUILD_DATAROOTDIR}/${PROJECT_RESOURCE_DIR}/modules/SimRC/"
  DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/config/activity_model_factory.conf ${CMAKE_CURRENT_SOURCE_DIR}/config/activity_models.conf
  COMMENT "Copying Falaise SimRC plugin's config files into Falaise build directory"
  )

add_subdirectory(programs)

# Test support:
option(FalaiseSimRCPlugin_ENABLE_TESTING "Build unit testing system for FalaiseSimRCPlugin" ON)
if(FalaiseSimRCPlugin_ENABLE_TESTING)
  enable_testing()
  add_subdirectory(testing)
endif()

# - end
