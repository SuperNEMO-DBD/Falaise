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
  calo_locator.h
  calo_scin_box_model.h
  calo_tapered_scin_box_model.h
  cosmic_muon_generator.h
  exitcodes.h
  falaise.h
  gg_locator.h
  gg_step_hit_processor.h
  gg_track_utils.h
  gveto_locator.h
  locator_plugin.h
  ocd_support.h
  resource.h
  utils.h
  xcalo_locator.h
  version.h.in
  )

# - Publish headers
foreach(_hdrin ${FalaiseLibrary_HEADERS})
  string(REGEX REPLACE "\\.in$" "" _hdrout "${_hdrin}")
  configure_file(${_hdrin} ${Falaise_BUILDPRODUCT_DIR}/include/falaise/${_hdrout} @ONLY)
endforeach()

#-----------------------------------------------------------------------
# Declare sources
#
configure_file(falaise.cc.in falaise.cc @ONLY)
configure_file(resource.cc.in resource.cc @ONLY)

set(FalaiseLibrary_SOURCES
  calo_locator.cc
  calo_scin_box_model.cc
  calo_tapered_scin_box_model.cc
  cosmic_muon_generator.cc
  gg_locator.cc
  gg_step_hit_processor.cc
  gg_track_utils.cc
  gveto_locator.cc
  locator_plugin.cc
  ocd_support.cc
  utils.cc
  version.cc
  xcalo_locator.cc
  ${CMAKE_CURRENT_BINARY_DIR}/falaise.cc
  ${CMAKE_CURRENT_BINARY_DIR}/resource.cc
  )

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
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
  )

# - Install public headers (preliminary)
install(DIRECTORY ${Falaise_BUILDPRODUCT_DIR}/include/
  DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
  )

#-----------------------------------------------------------------------
# Build Tests
#
