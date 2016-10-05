# - Basic CMake setup
# Check version meets ou requirements
# Declare project, which will configure compiler for us
cmake_minimum_required(VERSION 3.3)
project(Things2Root)

# Modules use Falaise/Bayeux, so we need to locate this or fail
#find_package(Falaise REQUIRED)

# Build a dynamic library from our sources
add_library(Things2Root SHARED Things2Root.h Things2Root.cpp)
target_link_libraries(Things2Root FalaiseModule)

# Install it
install(TARGETS Things2Root DESTINATION ${CMAKE_INSTALL_PLUGINDIR})
