# - Basic CMake setup
# Check version meets ou requirements
# Declare project, which will configure compiler for us
cmake_minimum_required(VERSION 3.3)
project(Things2Root)

add_library(Things2Root SHARED Things2Root.h Things2Root.cpp)
target_link_libraries(Things2Root FalaiseModule)

# Dictionary declaration
ROOT_GENERATE_DICTIONARY(Things2Root_dict LINKDEF Things2Root_linkdef.h)
add_dependencies(Things2Root Things2Root_dict)

# Install it
install(TARGETS Things2Root DESTINATION ${CMAKE_INSTALL_PLUGINDIR})
