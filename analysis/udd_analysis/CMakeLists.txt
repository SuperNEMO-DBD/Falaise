# Check version meets ou requirements
# Declare project, which will configure compiler for us
cmake_minimum_required(VERSION 3.3)
project(udd_analysis)

# Modules use Falaise/Bayeux, so we need to locate this or fail
#find_package(Falaise REQUIRED)

# Build a dynamic library from our sources
add_library(UddAnalysis SHARED UddAnalysis.hpp UddAnalysis.cpp)
target_link_libraries(UddAnalysis PUBLIC FalaiseModule)

# Install it
install(TARGETS UddAnalysis DESTINATION ${CMAKE_INSTALL_PLUGINDIR})

########################################################################
# Build an app from our sources
add_executable(fluddscan fluddscan.cxx)
target_include_directories(fluddscan PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${CMAKE_CURRENT_BINARY_DIR}
  )
target_compile_definitions(fluddscan PRIVATE ENABLE_BINRELOC)
target_link_libraries(fluddscan
  Falaise
  Bayeux::Bayeux
  Boost::program_options
  )

# - Ensure link to internal and external deps
set_target_properties(fluddscan PROPERTIES INSTALL_RPATH_USE_LINK_PATH 1)

if(UNIX AND NOT APPLE)
  set_target_properties(fluddscan
    PROPERTIES INSTALL_RPATH "\$ORIGIN/../${CMAKE_INSTALL_LIBDIR}"
    )
elseif(APPLE)
  # Temporary setting - needs testing
  set_target_properties(fluddscan
    PROPERTIES
      INSTALL_RPATH "@loader_path/../${CMAKE_INSTALL_LIBDIR}"
      )
endif()

# - Install it
install(TARGETS fluddscan
  EXPORT FalaiseTargets
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
  )

# - end
