# - Basic CMake setup
# Check version meets ou requirements
# Declare project, which will configure compiler for us
cmake_minimum_required(VERSION 3.3)
project(FalaiseLttcPlugin VERSION "0.1.0")

# Modules use Falaise, so we need to locate this or fail
# find_package(Falaise REQUIRED)

# Ensure our code can see the Falaise headers
include_directories(${CMAKE_CURRENT_BINARY_DIR})
include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${PROJECT_BINARY_DIR})

############################################################################################
# - LTTC module:

configure_file(lttc/lttc_config.hh.in ${PROJECT_BINARY_DIR}/lttc/lttc_config.hh @ONLY)

# - Headers:
list(APPEND FalaiseLttcPlugin_HEADERS
  ${PROJECT_BINARY_DIR}/lttc/lttc_config.hh
  lttc/i_curve.hh
  lttc/point.hh
  lttc/triplet.hh
  lttc/line.hh
  lttc/segment.hh
  lttc/rectangle.hh
  lttc/circle.hh
  lttc/translated_curve.hh
  lttc/rotated_curve.hh
  lttc/kinked_curve.cc
  lttc/sngeometry.hh
  lttc/tracker_hit.hh
  lttc/track.hh
  lttc/track_simulator.hh
  lttc/hit_simulator.hh
  lttc/legendre_transform_tools.hh
  lttc/dbscan_clusterizer.hh
  lttc/lttc_algo.hh
  # lttc/lttc_driver.hh
  # lttc/lttc_tracker_clustering_module.hh
)

# - Sources:
list(APPEND FalaiseLttcPlugin_SOURCES
  lttc/point.cc
  lttc/triplet.cc
  lttc/line.cc
  lttc/segment.cc
  lttc/rectangle.cc
  lttc/circle.cc
  lttc/translated_curve.cc
  lttc/rotated_curve.cc
  lttc/kinked_curve.cc
  lttc/sngeometry.cc
  lttc/tracker_hit.cc
  lttc/track.cc
  lttc/track_simulator.cc
  lttc/hit_simulator.cc
  lttc/legendre_transform_tools.cc
  lttc/dbscan_clusterizer.cc
  lttc/lttc_algo.cc
  # lttc/lttc_driver.cc
  # lttc/lttc_tracker_clustering_module.cc
  )

############################################################################################

# # Build a dynamic library from our sources
add_library(Falaise_Lttc SHARED
  ${FalaiseLttcPlugin_HEADERS}
  ${FalaiseLttcPlugin_SOURCES})

target_link_libraries(Falaise_Lttc
  Falaise
  )

# Apple linker requires dynamic lookup of symbols, so we
# add link flags on this platform
if(APPLE)
  set_target_properties(Falaise_Lttc
    PROPERTIES LINK_FLAGS "-undefined dynamic_lookup"
    )
endif()

# Install it:
install(TARGETS Falaise_Lttc DESTINATION ${CMAKE_INSTALL_LIBDIR}/Falaise/modules)

# Test support:
# Test support:
option(FalaiseLttcPlugin_ENABLE_TESTING "Build unit testing system for LTTC" ON)
if(FalaiseLttcPlugin_ENABLE_TESTING)
  enable_testing()
  add_subdirectory(lttc/test)
endif()
