# - CMake build script for Falaise library

#-----------------------------------------------------------------------
# Copyright 2013 François Mauger <mauger@lpccaen.in2p3.fr>
# Copyright 2013 Université de Caen Basse-Normandie
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

message(STATUS "CMAKE_CURRENT_SOURCE_DIR=${CMAKE_CURRENT_SOURCE_DIR}")
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

include_directories(${Bayeux_INCLUDE_DIRS})

#-----------------------------------------------------------------------
# Declare sources
#

# Get header files: ls -1 source/flsngeometry/sngeometry/*.h | cut -d/ -f3,4
set(FalaiseLibrary_sngeometry_HEADERS
sngeometry/calo_locator.h
sngeometry/calo_scin_box_model.h
sngeometry/calo_tapered_scin_box_model.h
sngeometry/gg_locator.h
sngeometry/gveto_locator.h
sngeometry/locator_plugin.h
sngeometry/ocd_support.h
sngeometry/utils.h
sngeometry/xcalo_locator.h
)

# Get source files: ls -1 source/flsngeometry/sngeometry/*.cc | cut -d/ -f4
set(FalaiseLibrary_sngeometry_SOURCES
sngeometry/calo_locator.cc
sngeometry/calo_scin_box_model.cc
sngeometry/calo_tapered_scin_box_model.cc
sngeometry/gg_locator.cc
sngeometry/gveto_locator.cc
sngeometry/locator_plugin.cc
sngeometry/ocd_support.cc
sngeometry/utils.cc
sngeometry/xcalo_locator.cc
)

#-----------------------------------------------------------------------
# Build Library
#
add_library(Falaise_sngeometry SHARED
  ${FalaiseLibrary_sngeometry_HEADERS}
  ${FalaiseLibrary_sngeometry_SOURCES}
  )

target_link_libraries(Falaise_sngeometry
  ${Bayeux_LIBRARIES}
  )

#-----------------------------------------------------------------------
# Build Tests
#
