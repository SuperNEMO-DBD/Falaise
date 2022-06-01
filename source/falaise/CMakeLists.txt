# - CMake build script for Falaise library

#-----------------------------------------------------------------------
# Copyright 2012,2013 Ben Morgan <bmorgan.warwick@gmail.com>
# Copyright 2012,2013 University of Warwick
#
# This file is part of Falaise.
#
# Falaise is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# Falaise is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with Falaise.  If not, see <http://www.gnu.org/licenses/>.
#-----------------------------------------------------------------------

#-----------------------------------------------------------------------
# Configure library
#
configure_file(version.h.in version.h @ONLY)
configure_file(version.cc.in version.cc @ONLY)
configure_file(resource.cc.in resource.cc @ONLY)

# - Generate binreloc header/source with mangled names, ensuring first
# character(s) are a valid for a C identifier
string(RANDOM LENGTH 18 MANGLE_BINRELOC_MAIN)
set(MANGLE_BINRELOC "MB${MANGLE_BINRELOC_MAIN}")
configure_file(falaise_binreloc.h.in falaise_binreloc.h @ONLY)


#-----------------------------------------------------------------------
# Declare sources
#
set(FalaiseLibrary_HEADERS
  ${CMAKE_CURRENT_BINARY_DIR}/version.h
  bounded_int.h
  exitcodes.h
  falaise.h
  resource.h
  path.h
  property_set.h
  quantity.h
  tags.h
  user_level.h
  detail/falaise_sys.h
  metadata_utils.h
)

set(FalaiseLibrary_SOURCES
  ${CMAKE_CURRENT_BINARY_DIR}/version.cc
  ${CMAKE_CURRENT_BINARY_DIR}/resource.cc
  ${CMAKE_CURRENT_BINARY_DIR}/falaise_binreloc.h
  falaise_binreloc.c
  path.cpp
  property_set.cpp
  quantity.cpp
  tags.cc
  user_level.cc
  detail/falaise_sys.cc
  metadata_utils.cc
  falaise.cc
  )

set(FalaiseLibrary_TESTS)
set(FalaiseLibrary_TESTS_CATCH)

# - Declare headers, sources and test programs specific
#   to various setups
include(snemo.cmake)

#-----------------------------------------------------------------------
# Build/Link Library
#
add_library(Falaise SHARED ${FalaiseLibrary_HEADERS} ${FalaiseLibrary_SOURCES})
target_compile_features(Falaise PUBLIC ${FALAISE_CXX_COMPILE_FEATURES})
target_compile_definitions(Falaise PRIVATE ENABLE_BINRELOC)
target_include_directories(Falaise PUBLIC
  $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/source>
  $<BUILD_INTERFACE:${PROJECT_BINARY_DIR}/source>
  $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
  $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
  )
target_link_libraries(Falaise PUBLIC Bayeux::Bayeux)
target_clang_format(Falaise)

# - Rpath it
set_target_properties(Falaise PROPERTIES INSTALL_RPATH_USE_LINK_PATH 1)

# - Version it
set_target_properties(Falaise PROPERTIES VERSION ${PROJECT_VERSION} SOVERSION ${PROJECT_VERSION_MAJOR})

if(UNIX AND NOT APPLE)
  set_target_properties(Falaise
    PROPERTIES INSTALL_RPATH "\$ORIGIN/../${CMAKE_INSTALL_LIBDIR}"
    )
endif()

# Interface library that plugins can link to obtain usage requirements
# without having to directly link to Falaise or its dependents. Current
# policy is that modules should obtain needed symbols at runtime from
# Falaise programs. Having an interface target simplifies usage for
# clients as they just link to the FalaiseModule target. At present, this
# is just an interface target, but can easily be updated later to a full
# target or even an alias to Falaise itself without affecting how clients
# should link to it.
add_library(FalaiseModule INTERFACE)
target_include_directories(FalaiseModule INTERFACE
  $<TARGET_PROPERTY:Falaise,INTERFACE_INCLUDE_DIRECTORIES>
  )
target_include_directories(FalaiseModule SYSTEM INTERFACE
  $<TARGET_PROPERTY:Falaise,INTERFACE_SYSTEM_INCLUDE_DIRECTORIES>
  )

target_compile_features(FalaiseModule INTERFACE
  $<TARGET_PROPERTY:Falaise,INTERFACE_COMPILE_FEATURES>
  )
target_link_libraries(FalaiseModule
  INTERFACE "$<$<PLATFORM_ID:Darwin>:-undefined dynamic_lookup>"
  )

# - Install it
# TODO : fixup rpaths as needed
install(TARGETS Falaise FalaiseModule
  EXPORT  FalaiseTargets
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
  )

# - Install public headers (preliminary)
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/ ${CMAKE_CURRENT_BINARY_DIR}/
  DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/falaise
  FILES_MATCHING PATTERN "*.h" PATTERN "*.ipp"
  PATTERN "CMakeFiles" EXCLUDE
  PATTERN "test" EXCLUDE
  )

if(FALAISE_ENABLE_TESTING)
  list(APPEND FalaiseLibrary_TESTS
    test/test_falaise.cxx
    test/test_io_data.cxx
    test/test_falaise_tags.cxx
   )
  list(APPEND FalaiseLibrary_TESTS_CATCH
    test/test_falaise_version.cxx
    test/test_bounded_int.cxx
    test/test_path.cxx
    test/test_property_set.cxx
    test/test_quantity.cxx
    )

  # Testing version is sensitive to compile definitions, so match them
  set_property(SOURCE ${CMAKE_CURRENT_SOURCE_DIR}/test/test_falaise_version.cxx
  APPEND PROPERTY COMPILE_DEFINITIONS
    FALAISE_VERSION_COMMIT=${Falaise_GIT_HASH_SHORT}
    $<$<BOOL:${FALAISE_VERSION_IS_DIRTY}>:FALAISE_VERSION_IS_DIRTY>)
endif()

#-----------------------------------------------------------------------
# Build Tests
#
# - NB assumes each test .cxx file generates a single executable
# - We also output the executables to a dedicated directory
set(_falaise_TEST_ENVIRONMENT
  "FALAISE_RESOURCE_DIR=${PROJECT_SOURCE_DIR}/resources"
  "FALAISE_TESTING_DIR=${CMAKE_CURRENT_SOURCE_DIR}/test"
  "FALAISE_SNEMO_TESTING_DIR=${CMAKE_CURRENT_SOURCE_DIR}/snemo/test"
  )
message( STATUS "_falaise_TEST_ENVIRONMENT='${_falaise_TEST_ENVIRONMENT}'")

if(FALAISE_ENABLE_TESTING)
  # - CATCH TESTS
  foreach(_testsource ${FalaiseLibrary_TESTS_CATCH})
    get_filename_component(_testname "${_testsource}" NAME_WE)
    set(_testname "falaise-${_testname}")
    add_executable(${_testname} ${_testsource})
    target_link_libraries(${_testname}
      PUBLIC Falaise FLCatch
      PRIVATE "$<$<PLATFORM_ID:Darwin>:-undefined dynamic_lookup>"
      )
    target_clang_format(${_testname})
    add_test(NAME ${_testname} COMMAND ${_testname})
    set_property(TEST ${_testname}
      PROPERTY ENVIRONMENT ${_falaise_TEST_ENVIRONMENT}
      )
    # - For now, dump them into the testing output directory
    set_target_properties(${_testname}
      PROPERTIES
      RUNTIME_OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/fltests
      ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/fltests
      )
  endforeach()


  # - OLDSTYLE TESTS
  foreach(_testsource ${FalaiseLibrary_TESTS})
    message( STATUS "OLDSTYLE TEST: ${_testsource}")
    message( STATUS "  _testsource='${_testsource}'")
    get_filename_component(_testname "${_testsource}" NAME_WE)
    set(_testname "falaise-${_testname}")
    message( STATUS "  _testname='${_testname}'")
    message( STATUS "  _falaise_TEST_ENVIRONMENT='${_falaise_TEST_ENVIRONMENT}'")
    add_executable(${_testname} ${_testsource})
    target_link_libraries(${_testname}
      PUBLIC Falaise
      PRIVATE "$<$<PLATFORM_ID:Darwin>:-undefined dynamic_lookup>"
      )
    # target_clang_format(${_testname})
    add_test(NAME ${_testname} COMMAND ${_testname})
    set_property(TEST ${_testname}
      APPEND PROPERTY ENVIRONMENT ${_falaise_TEST_ENVIRONMENT}
      )
    # - For now, dump them into the testing output directory
    set_target_properties(${_testname}
      PROPERTIES
      RUNTIME_OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/fltests
      ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/fltests
      )
  endforeach()
endif()
