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

set(FalaiseLibrary_HEADERS
  exitcodes.h
  falaise.h
  resource.h
  plugin.h
  version.h.in
)

#-----------------------------------------------------------------------
# Declare sources
#
configure_file(falaise.cc.in  falaise.cc @ONLY)
configure_file(resource.cc.in resource.cc @ONLY)
configure_file(plugin.cc.in   plugin.cc @ONLY)

set(FalaiseLibrary_SOURCES
  version.cc
  ${CMAKE_CURRENT_BINARY_DIR}/falaise.cc
  ${CMAKE_CURRENT_BINARY_DIR}/resource.cc
  ${CMAKE_CURRENT_BINARY_DIR}/plugin.cc
  )

set(FalaiseLibrary_TESTS )

# - Declare headers, sources and test programs specific
#   to various setups (snemo, bipo3...)
include(snemo.cmake)
include(bipo3.cmake)
include(TrackerPreClustering.cmake)

# - Publish headers
foreach(_hdrin ${FalaiseLibrary_HEADERS})
  string(REGEX REPLACE "\\.in$" "" _hdrout "${_hdrin}")
  configure_file(${_hdrin} ${Falaise_BUILDPRODUCT_DIR}/include/falaise/${_hdrout} @ONLY)
endforeach()


#-----------------------------------------------------------------------
# Build/Link Library
#
add_library(Falaise SHARED ${FalaiseLibrary_HEADERS} ${FalaiseLibrary_SOURCES})
target_link_libraries(Falaise ${Bayeux_LIBRARIES})

# - Rpath it
set_target_properties(Falaise PROPERTIES INSTALL_RPATH_USE_LINK_PATH 1)

if(UNIX AND NOT APPLE)
  set_target_properties(Falaise
    PROPERTIES INSTALL_RPATH "\$ORIGIN/../${CMAKE_INSTALL_LIBDIR}"
    )
elseif(APPLE)
  # Temporary setting - needs testing
  set_target_properties(Falaise
    PROPERTIES
      INSTALL_NAME_DIR "@rpath"
      #LINK_FLAGS "-Wl,-rpath,@loader_path"
    )
endif()

# - Install it
# TODO : fixup rpaths as needed
install(TARGETS Falaise
  EXPORT  FalaiseTargets
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
  )

# - Install public headers (preliminary)
install(DIRECTORY ${Falaise_BUILDPRODUCT_DIR}/include/
  DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
  )

if(Falaise_ENABLE_TESTING)
  list(APPEND FalaiseLibrary_TESTS
    testing/test_falaise.cxx
    )
endif()

#-----------------------------------------------------------------------
# Build Tests
#
# - NB assumes each test .cxx file generates a single executable
# - We also output the executables to a dedicated directory
set(_falaise_TEST_ENVIRONMENT "FALAISE_RESOURCE_DIR=${Falaise_DATAROOTDIR}/resources")
if(Falaise_ENABLE_TESTING)
  foreach(_testsource ${FalaiseLibrary_TESTS})
    get_filename_component(_testname "${_testsource}" NAME_WE)
    set(_testname "falaise-${_testname}")
    add_executable(${_testname} ${_testsource})
    target_link_libraries(${_testname} Falaise)
    # - On Apple, ensure dynamic_lookup of undefined symbols
    if(APPLE)
      set_target_properties(${_testname} PROPERTIES LINK_FLAGS "-undefined dynamic_lookup")
    endif()
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
