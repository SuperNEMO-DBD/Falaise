# - Top level CMakeLists.txt for WcTools

cmake_minimum_required(VERSION 3.3 FATAL_ERROR)
project(wctools LANGUAGES CXX)

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

# - Versioning
set(WcTools_VERSION_MAJOR 0)
set(WcTools_VERSION_MINOR 1)
set(WcTools_VERSION_PATCH 0)
set(WcTools_VERSION
  "${WcTools_VERSION_MAJOR}.${WcTools_VERSION_MINOR}.${WcTools_VERSION_PATCH}")

#-----------------------------------------------------------------------
# - Standard UNIX Tool install paths, including relative paths for use
# by applications requiring resources
include(GNUInstallDirs)

# - Validate that certain paths are relative, otherwise relocation may fail
foreach(_dir BINDIR LIBDIR INCLUDEDIR DATAROOTDIR)
  if(IS_ABSOLUTE "${CMAKE_INSTALL_${_dir}}")
    message(FATAL_ERROR "Absolute path for CMAKE_INSTALL_${_dir} not allowed")
  endif()
endforeach()

# Add a path for CMake config files
set(CMAKE_INSTALL_CMAKEDIR ${CMAKE_INSTALL_LIBDIR}/cmake)
set(CMAKE_INSTALL_FULL_CMAKEDIR ${CMAKE_INSTALL_FULL_LIBDIR}/cmake)

# Resources directories based on name-version subdirs
set(WCTOOLS_TAG "WcTools-${WcTools_VERSION}")
file(RELATIVE_PATH WCTOOLS_BINDIR_TO_RESOURCEDIR "${CMAKE_INSTALL_FULL_BINDIR}" "${CMAKE_INSTALL_FULL_DATAROOTDIR}/${WCTOOLS_TAG}")
file(RELATIVE_PATH WCTOOLS_CMAKEDIR_TO_INCLUDEDIR "${CMAKE_INSTALL_FULL_CMAKEDIR}/${WCTOOLS_TAG}"  "${CMAKE_INSTALL_FULL_INCLUDEDIR}")

message(STATUS "Adding directory 'src'...")
add_subdirectory(src)

message(STATUS "The end.")

# - end