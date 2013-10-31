# - Top level CMakeLists.txt for Falaise data model example 1 program

cmake_minimum_required(VERSION 2.8 FATAL_ERROR)
project(foo)

if(${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_BINARY_DIR})
  message(STATUS "${PROJECT_NAME} requires an out-of-source build.")
  message(STATUS "Please remove these files from ${CMAKE_BINARY_DIR} first:")
  message(STATUS "  CMakeCache.txt")
  message(STATUS "  CMakeFiles")
  message(STATUS "Once these files are removed, create a separate directory")
  message(STATUS "and run CMake from there, pointing it to:")
  message(STATUS "  ${CMAKE_SOURCE_DIR}")
  message(FATAL_ERROR "in-source build detected")
endif()

find_package(Bayeux 1.0.0 REQUIRED)
set(_incs ${Bayeux_INCLUDE_DIRS})
set(_libs ${Bayeux_LIBRARIES})

include_directories(${PROJECT_SOURCE_DIR} ${_incs})

add_library(foo_module SHARED
            foo_module.h
	    foo_module.cc
	    foo_module-ocd_impl.cc
	    )

target_link_libraries(foo_module ${_libs} )

install(FILES
  ${PROJECT_BINARY_DIR}/libfoo_module${CMAKE_SHARED_LIBRARY_SUFFIX}
  DESTINATION ${CMAKE_INSTALL_PREFIX}/lib
  PERMISSIONS OWNER_READ OWNER_EXECUTE
  GROUP_READ GROUP_EXECUTE
  WORLD_READ WORLD_EXECUTE
  )

add_executable(prog_foo_module prog_foo_module.cxx )

target_link_libraries(prog_foo_module
  foo_module
  )

install(FILES
  ${PROJECT_BINARY_DIR}/prog_foo_module
  DESTINATION ${CMAKE_INSTALL_PREFIX}/bin
  PERMISSIONS OWNER_READ OWNER_EXECUTE
  GROUP_READ GROUP_EXECUTE
  WORLD_READ WORLD_EXECUTE
  )

# end