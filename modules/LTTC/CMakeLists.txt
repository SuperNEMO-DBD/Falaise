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
  lttc/i_curve2.hh
  lttc/point.hh
  lttc/fitted_point2.hh
  lttc/triplet2.hh
  lttc/line2.hh
  lttc/fitted_line2.hh
  lttc/segment2.hh
  lttc/rectangle.hh
  lttc/circle.hh
  lttc/translated_curve2.hh
  lttc/rotated_curve2.hh
  lttc/kinked_curve2.cc
  lttc/sngeometry.hh
  lttc/tracker_hit.hh
  lttc/track.hh
  lttc/track_simulator.hh
  lttc/hit_simulator.hh
  lttc/legendre_transform_tools.hh
  lttc/dbscan_clusterizer.hh
  lttc/track_path_tools.hh
  lttc/lttc_utils.hh
  lttc/missing_hits_finder.hh
  lttc/tangent_circle_3x1.hh
  # lttc/value_with_error.hh
  lttc/uhistogram.hh
  lttc/lttc_algo.hh
  lttc/lttc_algo_circle.hh
  # lttc/lttc_driver.hh
  # lttc/lttc_tracker_clustering_module.hh
)

# - Sources:
list(APPEND FalaiseLttcPlugin_SOURCES
  lttc/point.cc
  lttc/fitted_point2.cc
  lttc/triplet2.cc
  lttc/line2.cc
  lttc/fitted_line2.cc
  lttc/segment2.cc
  lttc/rectangle.cc
  lttc/circle.cc
  lttc/translated_curve2.cc
  lttc/rotated_curve2.cc
  lttc/kinked_curve2.cc
  lttc/sngeometry.cc
  lttc/tracker_hit.cc
  lttc/track.cc
  lttc/track_simulator.cc
  lttc/hit_simulator.cc
  lttc/legendre_transform_tools.cc
  lttc/dbscan_clusterizer.cc
  lttc/track_path_tools.cc
  lttc/lttc_utils.cc
  lttc/missing_hits_finder.cc
  lttc/tangent_circle_3x1.cc
  # lttc/value_with_error.cc
  lttc/uhistogram.cc
  lttc/lttc_algo.cc
  lttc/lttc_algo_circle.cc
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
