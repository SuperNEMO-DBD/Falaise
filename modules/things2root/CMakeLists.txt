# - Basic CMake setup
# Check version meets ou requirements
# Declare project, which will configure compiler for us
cmake_minimum_required(VERSION 2.8)
project(RConvert)

# Modules use Bayeux, so we need to locate this or fail
find_package(Bayeux REQUIRED)

# Ensure our code can see the Bayeux headers
include_directories(${Bayeux_INCLUDE_DIRS})

# Build a dynamic library from our sources
add_library(Things2Root SHARED Things2Root.h Things2Root.cpp)

# Apple linker requires dynamic lookup of symbols, so we
# add link flags on this platform
if(APPLE)
  set_target_properties(Things2Root
    PROPERTIES LINK_FLAGS "-undefined dynamic_lookup"
    )
endif()

# Install it
install(TARGETS Things2Root DESTINATION ${CMAKE_INSTALL_LIBDIR}/Falaise/modules)

