# Check cmake version meets our requirements
cmake_minimum_required(VERSION 3.3)

# Declare project, which will configure compiler for us
project(FalaiseTKReconstructPlugin)

# #Finds ROOT and Falaise packages and defines cmake variables
# find_package(Falaise REQUIRED)
# find_package(ROOT    REQUIRED)

# include_directories(include ${Bayeux_INCLUDE_DIRS})
include_directories(${CMAKE_CURRENT_BINARY_DIR}/source)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/source)

list(APPEND TKReconstruct_HEADERS
  source/tkrec/TKcluster.h
  source/tkrec/TKEvent.h
  source/tkrec/TKOMhit.h
  source/tkrec/TKpoint.h
  source/tkrec/TKReconstruct.h
  source/tkrec/TKtrack.h
  source/tkrec/TKtrajectory.h
  source/tkrec/TKtrhit.h
  )

list(APPEND TKReconstruct_SOURCES
  source/tkrec/TKcluster.cpp
  source/tkrec/TKEvent.cpp
  source/tkrec/TKOMhit.cpp
  source/tkrec/TKpoint.cpp
  source/tkrec/TKReconstruct.cpp
  source/tkrec/TKtrack.cpp
  source/tkrec/TKtracking_tools.cpp
  source/tkrec/TKtrajectory.cpp
  source/tkrec/TKtrhit.cpp
  )

add_library(Falaise_TKReconstruct SHARED
  ${TKReconstruct_SOURCES}
  ${TKReconstruct_HEADERS} )

target_link_libraries(Falaise_TKReconstruct FalaiseModule ${ROOT_LIBRARIES} ROOT::Geom)
# PUBLIC Falaise::Falaise ${ROOT_LIBRARIES} ROOT::Geom)  

# Apple linker requires dynamic lookup of symbols, so we
# add link flags on this platform
if(APPLE)
  set_target_properties(Falaise_TKReconstruct
    PROPERTIES
      LINK_FLAGS "-undefined dynamic_lookup"
      INSTALL_RPATH "@loader_path"
    )
endif()

# Install Targets
install(TARGETS Falaise_TKReconstruct DESTINATION ${CMAKE_INSTALL_PLUGINDIR})


# Test support:
option(FalaiseTKReconstructPlugin_ENABLE_TESTING "Build unit testing system for FalaiseTKReconstructPlugin" ON)
if(FalaiseTKReconstructPlugin_ENABLE_TESTING)
  enable_testing()
  ### add_subdirectory(testing)
endif()

# configure_file("TKReconstructPipeline.conf.in" "TKReconstructPipeline.conf" @ONLY)
