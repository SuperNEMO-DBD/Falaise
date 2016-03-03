# - Basic CMake setup
# Check version meets ou requirements
# Declare project, which will configure compiler for us
cmake_minimum_required(VERSION 2.8)
project(FalaiseMockTrackerClusterizerPlugin)

# Modules use Falaise, so we need to locate this or fail
# find_package(Falaise REQUIRED)

# Ensure our code can see the Falaise headers
#include_directories(${Falaise_INCLUDE_DIRS})
include_directories(${FALAISE_BUILDPRODUCT_DIR}/include)
include_directories(${FALAISE_BUILDPRODUCT_DIR}/include/falaise)
include_directories(${Bayeux_INCLUDE_DIRS})
include_directories(${CMAKE_CURRENT_BINARY_DIR})
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/source)


############################################################################################
# - MockTrackerClusterizer modules:
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/source/falaise)

# - Headers:
list(APPEND FalaiseMockTrackerClusterizerPlugin_HEADERS source/falaise/snemo/reconstruction/mock_tracker_clustering_driver.h)
list(APPEND FalaiseMockTrackerClusterizerPlugin_HEADERS source/falaise/snemo/reconstruction/mock_tracker_clustering_module.h)

# - Sources:
list(APPEND FalaiseMockTrackerClusterizerPlugin_SOURCES source/falaise/snemo/reconstruction/mock_tracker_clustering_driver.cc)
list(APPEND FalaiseMockTrackerClusterizerPlugin_SOURCES source/falaise/snemo/reconstruction/mock_tracker_clustering_module.cc)

############################################################################################

# # Build a dynamic library from our sources
add_library(Falaise_MockTrackerClusterizer SHARED
  ${FalaiseMockTrackerClusterizerPlugin_HEADERS}
  ${FalaiseMockTrackerClusterizerPlugin_SOURCES})

target_link_libraries(Falaise_MockTrackerClusterizer Falaise)

# Apple linker requires dynamic lookup of symbols, so we
# add link flags on this platform
if(APPLE)
  set_target_properties(Falaise_MockTrackerClusterizer
    PROPERTIES LINK_FLAGS "-undefined dynamic_lookup"
    )
endif()

# Install it:
install(TARGETS Falaise_MockTrackerClusterizer DESTINATION ${CMAKE_INSTALL_LIBDIR}/Falaise/modules)

# Test support:
option(FalaiseMockTrackerClusterizerPlugin_ENABLE_TESTING "Build unit testing system for FalaiseMockTrackerClusterizerPlugin" ON)
if(FalaiseMockTrackerClusterizerPlugin_ENABLE_TESTING)
  enable_testing()
  add_subdirectory(testing)
endif()
