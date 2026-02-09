# Check version meets ou requirements
# Declare project, which will configure compiler for us
cmake_minimum_required(VERSION 3.22)
project(flgeom)

execute_process(COMMAND falaise-config --cmakedir
  OUTPUT_VARIABLE Falaise_DIR
  COMMAND_ERROR_IS_FATAL ANY)

find_package(Falaise 5.1.9 REQUIRED)
message( STATUS "Falaise_VERSION = ${Falaise_VERSION}")
include_directories(Falaise_INCLUDE_DIRS)

########################################################################
# Build an app from our sources
add_executable(flgeom flgeom.cxx)
target_include_directories(flgeom PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${CMAKE_CURRENT_BINARY_DIR}
  )
target_compile_definitions(flgeom PRIVATE ENABLE_BINRELOC)
target_link_libraries(flgeom
  Falaise::Falaise
  # Bayeux::Bayeux
  # Boost::program_options
  )
