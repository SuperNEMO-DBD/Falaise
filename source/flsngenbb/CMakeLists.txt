# - CMake build script for Falaise sngenbb submodule

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

# Get header files: ls -1 source/flsngenbb/sngenbb/*.h | cut -d/ -f3,4
set(FalaiseLibrary_sngenbb
sngenbb/cosmic_muon_generator.h
)

# Get source files: ls -1 source/flsngenbb/sngenbb/*.cc | cut -d/ -f4
set(FalaiseLibrary_sngenbb_SOURCES
sngenbb/cosmic_muon_generator.cc
)

#-----------------------------------------------------------------------
# Build Library
#
add_library(Falaise_sngenbb SHARED
  ${FalaiseLibrary_sngenbb_HEADERS}
  ${FalaiseLibrary_sngenbb_SOURCES}
  )

target_link_libraries(Falaise_sngenbb
  ${Bayeux_LIBRARIES}
  )

#-----------------------------------------------------------------------
# Build Tests
#
