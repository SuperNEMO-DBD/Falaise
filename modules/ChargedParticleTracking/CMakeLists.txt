# - Basic CMake setup
# Check version meets ou requirements
# Declare project, which will configure compiler for us
cmake_minimum_required(VERSION 2.8)
project(FalaiseChargedParticleTrackingPlugin)

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
# - ChargedParticleTracking modules:
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/source/falaise)

# - Headers:
list(APPEND FalaiseChargedParticleTrackingPlugin_HEADERS
  source/falaise/snemo/reconstruction/charge_computation_driver.h
  source/falaise/snemo/reconstruction/vertex_extrapolation_driver.h
  source/falaise/snemo/reconstruction/calorimeter_association_driver.h
  source/falaise/snemo/reconstruction/charged_particle_tracking_module.h)

# - Sources:
list(APPEND FalaiseChargedParticleTrackingPlugin_SOURCES
  source/falaise/snemo/reconstruction/charge_computation_driver.cc
  source/falaise/snemo/reconstruction/vertex_extrapolation_driver.cc
  source/falaise/snemo/reconstruction/calorimeter_association_driver.cc
  source/falaise/snemo/reconstruction/charged_particle_tracking_module.cc)

############################################################################################

# # Build a dynamic library from our sources
add_library(Falaise_ChargedParticleTracking SHARED
  ${FalaiseChargedParticleTrackingPlugin_HEADERS}
  ${FalaiseChargedParticleTrackingPlugin_SOURCES})

target_link_libraries(Falaise_ChargedParticleTracking Falaise)

# Apple linker requires dynamic lookup of symbols, so we
# add link flags on this platform
if(APPLE)
  set_target_properties(Falaise_ChargedParticleTracking
    PROPERTIES LINK_FLAGS "-undefined dynamic_lookup"
    )
endif()

# Install it:
install(TARGETS Falaise_ChargedParticleTracking DESTINATION ${CMAKE_INSTALL_LIBDIR}/Falaise/modules)

# Test support:
option(FalaiseChargedParticleTrackingPlugin_ENABLE_TESTING "Build unit testing system for FalaiseChargedParticleTrackingPlugin" ON)
if(FalaiseChargedParticleTrackingPlugin_ENABLE_TESTING)
  enable_testing()
  add_subdirectory(testing)
endif()
