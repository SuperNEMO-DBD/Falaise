# CMAKE_COMPILER
# - Basic CMake setup
# Check version meets ou requirements
# Declare project, which will configure compiler for us
cmake_minimum_required(VERSION 2.8)
project(FalaiseGammaTrackingPlugin)

# Ensure our code can see the Falaise headers
#include_directories(${Falaise_INCLUDE_DIRS})
include_directories(${Falaise_BUILDPRODUCT_DIR}/include)
include_directories(${Falaise_BUILDPRODUCT_DIR}/include/falaise)
include_directories(${Bayeux_INCLUDE_DIRS})
include_directories(${CMAKE_CURRENT_BINARY_DIR})
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/source)

############################################################################################
# - GammaTracking library:

list(APPEND GammaTracking_HEADERS
  source/GammaTracking/event.h
  source/GammaTracking/tof_computing.h
  source/GammaTracking/gamma_tracking.h
)

list(APPEND GammaTracking_SOURCES
  source/GammaTracking/event.cc
  source/GammaTracking/tof_computing.cc
  source/GammaTracking/gamma_tracking.cc
)

# Build a dynamic library from our sources
add_library(GammaTracking SHARED ${GammaTracking_HEADERS} ${GammaTracking_SOURCES})

target_link_libraries(GammaTracking ${GSL_LIBRARIES} ${Bayeux_LIBRARIES})

# Apple linker requires dynamic lookup of symbols, so we
# add link flags on this platform
if(APPLE)
  set_target_properties(GammaTracking
    PROPERTIES LINK_FLAGS "-undefined dynamic_lookup"
    )
endif()

# Install it:
install(TARGETS GammaTracking DESTINATION ${CMAKE_INSTALL_LIBDIR}/${Falaise_PLUGINLIBDIR})

############################################################################################
# - GammaTracking modules:
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/source/falaise)

# - Headers:
list(APPEND FalaiseGammaTrackingPlugin_HEADERS source/falaise/snemo/reconstruction/gamma_tracking_driver.h)
list(APPEND FalaiseGammaTrackingPlugin_HEADERS source/falaise/snemo/reconstruction/gamma_tracking_module.h)

# - Sources:
list(APPEND FalaiseGammaTrackingPlugin_SOURCES source/falaise/snemo/reconstruction/gamma_tracking_driver.cc)
list(APPEND FalaiseGammaTrackingPlugin_SOURCES source/falaise/snemo/reconstruction/gamma_tracking_module.cc)

############################################################################################

# Build a dynamic library from our sources
add_library(Falaise_GammaTracking SHARED
  ${FalaiseGammaTrackingPlugin_HEADERS}
  ${FalaiseGammaTrackingPlugin_SOURCES})

target_link_libraries(Falaise_GammaTracking GammaTracking Falaise)

# Apple linker requires dynamic lookup of symbols, so we
# add link flags on this platform
if(APPLE)
  set_target_properties(Falaise_GammaTracking
    PROPERTIES LINK_FLAGS "-undefined dynamic_lookup"
    )
endif()

# Install it:
install(TARGETS Falaise_GammaTracking DESTINATION ${CMAKE_INSTALL_LIBDIR}/Falaise/modules)

# Test support:
option(FalaiseGammaTrackingPlugin_ENABLE_TESTING "Build unit testing system for FalaiseGammaTracking" ON)
if(FalaiseGammaTrackingPlugin_ENABLE_TESTING)
  enable_testing()
  add_subdirectory(testing)
endif()
