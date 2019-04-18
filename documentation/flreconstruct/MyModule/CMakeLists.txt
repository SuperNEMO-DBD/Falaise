# Check cmake version meets our requirements
cmake_minimum_required(VERSION 3.9)

# Declare project, which will configure compiler for us
project(MyModule)

# Modules use Falaise, so we need to locate this or fail
# Locating Falaise will automatically locate all of its
# dependencies such as Bayeux, ROOT and Boost.
find_package(Falaise REQUIRED)

# Build a shared/dynamic library from our source
add_library(MyModule SHARED MyModule.cpp)

# Link this library to the FalaiseModule library
# This ensures the correct compiler flags, include paths
# and linker flags are used to compile our library.
target_link_libraries(MyModule Falaise::FalaiseModule)

