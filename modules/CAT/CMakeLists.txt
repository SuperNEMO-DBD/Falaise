# - Basic CMake setup
# Check version meets ou requirements
# Declare project, which will configure compiler for us
cmake_minimum_required(VERSION 2.8)
project(CAT)

# Modules use Falaise, so we need to locate this or fail
find_package(Falaise REQUIRED)

# Ensure our code can see the Falaise headers
include_directories(${Falaise_INCLUDE_DIRS})

# Build a dynamic library from our sources
add_library(Falaise_CAT SHARED ${CAT_HEADERS} ${CAT_SOURCES})

# Apple linker requires dynamic lookup of symbols, so we
# add link flags on this platform
if(APPLE)
  set_target_properties(Falaise_CAT
    PROPERTIES LINK_FLAGS "-undefined dynamic_lookup"
    )
endif()

# Install it
install(TARGETS Falaise_CAT DESTINATION ${CMAKE_INSTALL_LIBDIR}/Falaise/modules)
