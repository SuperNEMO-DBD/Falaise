# - Basic CMake setup
# Check version meets ou requirements
# Declare project, which will configure compiler for us
cmake_minimum_required(VERSION 3.3)
project(FalaiseChargedParticleTrackingPlugin VERSION "2.1.0")

# Modules use Falaise, so we need to locate this or fail
# find_package(Falaise REQUIRED)

# Ensure our code can see the Falaise headers
include_directories(${CMAKE_CURRENT_BINARY_DIR})
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

############################################################################################
# - ChargedParticleTracking modules:

# - Headers:
list(APPEND FalaiseChargedParticleTrackingPlugin_HEADERS
  ChargedParticleTracking/charge_computation_driver.h
  ChargedParticleTracking/vertex_extrapolation_driver.h
  ChargedParticleTracking/calorimeter_association_driver.h
  ChargedParticleTracking/alpha_finder_driver.h
  ChargedParticleTracking/charged_particle_tracking_module.h)

# - Sources:
list(APPEND FalaiseChargedParticleTrackingPlugin_SOURCES
  ChargedParticleTracking/charge_computation_driver.cc
  ChargedParticleTracking/vertex_extrapolation_driver.cc
  ChargedParticleTracking/calorimeter_association_driver.cc
  ChargedParticleTracking/alpha_finder_driver.cc
  ChargedParticleTracking/charged_particle_tracking_module.cc)

############################################################################################

# # Build a dynamic library from our sources
add_library(Falaise_ChargedParticleTracking SHARED
  ${FalaiseChargedParticleTrackingPlugin_HEADERS}
  ${FalaiseChargedParticleTrackingPlugin_SOURCES})

target_link_libraries(Falaise_ChargedParticleTracking
  Falaise
  )

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
# There would be if there were any tests...
