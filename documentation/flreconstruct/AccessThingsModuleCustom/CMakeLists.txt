# - Basic CMake setup
# Check version meets ou requirements
# Declare project, which will configure compiler for us
cmake_minimum_required(VERSION 3.3)
project(AccessThingsModuleCustom)

# Modules use FalaiseBayeux, so we need to locate this or fail
# Locating Falaise will automatically locate all of its
# dependencies such as Bayeux, ROOT and Boost.
find_package(Falaise 2.1.0 REQUIRED)

# Build a dynamic library from our sources
add_library(AccessThingsModule SHARED
  AccessThingsModule.h
  AccessThingsModule.cpp
  MyDataType.h
  MyDataType.cpp
  MyDataType-serial.ipp
  SerializationSupport.cpp
  )

# Link it to the FalaiseModule library
# This ensures the correct compiler flags, include paths
# and linker flags are applied to our dynamic library.
target_link_libraries(AccessThingsModule PUBLIC FalaiseModule)
