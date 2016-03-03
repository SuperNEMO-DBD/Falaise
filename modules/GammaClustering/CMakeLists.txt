# CMAKE_COMPILER
# - Basic CMake setup
# Check version meets ou requirements
# Declare project, which will configure compiler for us
cmake_minimum_required(VERSION 2.8)
project(FalaiseGammaClusteringPlugin)

# Ensure our code can see the Falaise headers
#include_directories(${Falaise_INCLUDE_DIRS})
include_directories(${FALAISE_BUILDPRODUCT_DIR}/include)
include_directories(${FALAISE_BUILDPRODUCT_DIR}/include/falaise)
include_directories(${Bayeux_INCLUDE_DIRS})
include_directories(${CMAKE_CURRENT_BINARY_DIR})
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/source)

############################################################################################
# - GammaClustering modules:
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/source/falaise)

# - Headers:
list(APPEND FalaiseGammaClusteringPlugin_HEADERS source/falaise/snemo/reconstruction/gamma_clustering_driver.h)
list(APPEND FalaiseGammaClusteringPlugin_HEADERS source/falaise/snemo/reconstruction/gamma_clustering_module.h)

# - Sources:
list(APPEND FalaiseGammaClusteringPlugin_SOURCES source/falaise/snemo/reconstruction/gamma_clustering_driver.cc)
list(APPEND FalaiseGammaClusteringPlugin_SOURCES source/falaise/snemo/reconstruction/gamma_clustering_module.cc)

############################################################################################

# Build a dynamic library from our sources
add_library(Falaise_GammaClustering SHARED
  ${FalaiseGammaClusteringPlugin_HEADERS}
  ${FalaiseGammaClusteringPlugin_SOURCES})

target_link_libraries(Falaise_GammaClustering Falaise)

# Apple linker requires dynamic lookup of symbols, so we
# add link flags on this platform
if(APPLE)
  set_target_properties(Falaise_GammaClustering
    PROPERTIES LINK_FLAGS "-undefined dynamic_lookup"
    )
endif()

# Install it:
install(TARGETS Falaise_GammaClustering DESTINATION ${CMAKE_INSTALL_LIBDIR}/Falaise/modules)

# Test support:
option(FalaiseGammaClusteringPlugin_ENABLE_TESTING "Build unit testing system for FalaiseGammaClustering" ON)
if(FalaiseGammaClusteringPlugin_ENABLE_TESTING)
  enable_testing()
  add_subdirectory(testing)
endif()
