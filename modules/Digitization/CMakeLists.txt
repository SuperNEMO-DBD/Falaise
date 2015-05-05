# CMAKE_COMPILER
# - Basic CMake setup
# Check version meets ou requirements
# Declare project, which will configure compiler for us
cmake_minimum_required(VERSION 2.8)
project(FalaiseDigitizationPlugin)

# Ensure our code can see the Falaise headers
#include_directories(${Falaise_INCLUDE_DIRS})
include_directories(${Falaise_BUILDPRODUCT_DIR}/include)
include_directories(${Falaise_BUILDPRODUCT_DIR}/include/falaise)
include_directories(${Bayeux_INCLUDE_DIRS})
include_directories(${CMAKE_CURRENT_BINARY_DIR})
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/source)

############################################################################################
# - Digitization modules:
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/source/falaise)

# - Headers:

list(APPEND FalaiseDigitizationPlugin_HEADERS
  
  source/falaise/snemo/digitization/electronic_mapping.h
  source/falaise/snemo/digitization/clock_utils.h
  source/falaise/snemo/digitization/ID_convertor.h
  source/falaise/snemo/digitization/mapping.h

  source/falaise/snemo/digitization/calo_signal.h
  source/falaise/snemo/digitization/calo_feb.h
  source/falaise/snemo/digitization/calo_tp.h
  source/falaise/snemo/digitization/calo_tp_data.h
  source/falaise/snemo/digitization/calo_ctw.h
  source/falaise/snemo/digitization/calo_ctw_data.h
  source/falaise/snemo/digitization/calo_tp_to_ctw_algo.h
  source/falaise/snemo/digitization/calo_tp.ipp
  source/falaise/snemo/digitization/calo_tp_data.ipp
  source/falaise/snemo/digitization/calo_ctw.ipp
  source/falaise/snemo/digitization/calo_ctw_data.ipp

  source/falaise/snemo/digitization/geiger_signal.h
  source/falaise/snemo/digitization/geiger_tp.h 
  source/falaise/snemo/digitization/geiger_tp_data.h 
  source/falaise/snemo/digitization/geiger_ctw.h
  source/falaise/snemo/digitization/geiger_ctw_data.h
  source/falaise/snemo/digitization/geiger_tp_to_ctw_algo.h
  source/falaise/snemo/digitization/geiger_tp.ipp
  source/falaise/snemo/digitization/geiger_tp_data.ipp
  source/falaise/snemo/digitization/geiger_ctw.ipp
  source/falaise/snemo/digitization/geiger_ctw_data.ipp

  source/falaise/snemo/digitization/signal_data.h

  source/falaise/snemo/digitization/sd_to_calo_signal_algo.h
  source/falaise/snemo/digitization/sd_to_geiger_signal_algo.h
  source/falaise/snemo/digitization/signal_to_geiger_tp_algo.h
  source/falaise/snemo/digitization/signal_to_calo_tp_algo.h
  source/falaise/snemo/digitization/tracker_trigger_algorithm.h
  source/falaise/snemo/digitization/memory.h
  source/falaise/snemo/digitization/memory-inl.h

  source/falaise/snemo/digitization/the_serializable.ipp
  source/falaise/snemo/digitization/the_serializable.h
  )

# - Sources:
list(APPEND FalaiseDigitizationPlugin_SOURCES 

  source/falaise/snemo/digitization/tracker_trigger_algorithm.cc
  source/falaise/snemo/digitization/signal_to_calo_tp_algo.cc
  source/falaise/snemo/digitization/signal_to_geiger_tp_algo.cc
  source/falaise/snemo/digitization/sd_to_geiger_signal_algo.cc
  source/falaise/snemo/digitization/sd_to_calo_signal_algo.cc

  source/falaise/snemo/digitization/signal_data.cc

  source/falaise/snemo/digitization/calo_tp_to_ctw_algo.cc
  source/falaise/snemo/digitization/calo_ctw_data.cc
  source/falaise/snemo/digitization/calo_ctw.cc
  source/falaise/snemo/digitization/calo_tp_data.cc
  source/falaise/snemo/digitization/calo_tp.cc
  source/falaise/snemo/digitization/calo_feb.cc
  source/falaise/snemo/digitization/calo_signal.cc

  source/falaise/snemo/digitization/geiger_tp_to_ctw_algo.cc
  source/falaise/snemo/digitization/geiger_ctw_data.cc
  source/falaise/snemo/digitization/geiger_ctw.cc
  source/falaise/snemo/digitization/geiger_tp_data.cc
  source/falaise/snemo/digitization/geiger_tp.cc 
  source/falaise/snemo/digitization/geiger_signal.cc

  source/falaise/snemo/digitization/electronic_mapping.cc
  source/falaise/snemo/digitization/clock_utils.cc
  source/falaise/snemo/digitization/ID_convertor.cc
  source/falaise/snemo/digitization/mapping.cc
  source/falaise/snemo/digitization/the_serializable.cc
  )

############################################################################################

# Build a dynamic library from our sources
add_library(Falaise_Digitization SHARED
  ${FalaiseDigitizationPlugin_HEADERS}
  ${FalaiseDigitizationPlugin_SOURCES})

target_link_libraries(Falaise_Digitization Falaise)

# Apple linker requires dynamic lookup of symbols, so we
# add link flags on this platform
if(APPLE)
  set_target_properties(Falaise_Digitization
    PROPERTIES LINK_FLAGS "-undefined dynamic_lookup"
    )
endif()

# Install it:
install(TARGETS Falaise_Digitization DESTINATION ${CMAKE_INSTALL_LIBDIR}/Falaise/modules)

# Test support:
option(FalaiseDigitizationPlugin_ENABLE_TESTING "Build unit testing system for FalaiseDigitization" ON)
if(FalaiseDigitizationPlugin_ENABLE_TESTING)
  enable_testing()
  add_subdirectory(testing)
endif()
