# Check version meets ou requirements
# Declare project, which will configure compiler for us
cmake_minimum_required(VERSION 3.3)
project(udd_analysis)

# Modules use Falaise/Bayeux, so we need to locate this or fail
#find_package(Falaise REQUIRED)

# Build a dynamic library from our sources
add_library(UddAnalysis SHARED UddAnalysis.hpp UddAnalysis.cpp)
target_link_libraries(UddAnalysis FalaiseModule)

# Install it
install(TARGETS UddAnalysis DESTINATION ${CMAKE_INSTALL_PLUGINDIR})
