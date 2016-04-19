# - CMake build script for Falaise flsimulate app

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
# Configure application
# - Bit hacky for now
find_package(Boost 1.55 REQUIRED program_options filesystem system)
include_directories(SYSTEM ${Boost_INCLUDE_DIRS})

find_package(Geant4 REQUIRED)

include_directories(BEFORE ${CMAKE_CURRENT_SOURCE_DIR})
include_directories(BEFORE ${Bayeux_INCLUDE_DIRS})
include_directories(BEFORE ${CMAKE_CURRENT_BINARY_DIR})

#-----------------------------------------------------------------------
# Build command line app
#
# - Generate application configuration file
configure_file(FLSimulateConfig.h.in FLSimulateConfig.h @ONLY)

# - Generate binreloc header/source with mangled names, ensuring first
# character(s) are a valid for a C identifier
string(RANDOM LENGTH 18 MANGLE_BINRELOC_MAIN)
set(MANGLE_BINRELOC "MB${MANGLE_BINRELOC_MAIN}")
configure_file(FLSimulateBinReloc.h.in FLSimulateBinReloc.h @ONLY)

set(FLSIMULATE_SOURCES
  flsimulatemain.cc
  FLSimulateConfig.h
  FLSimulateResources.h
  FLSimulateResources.cc
  FLSimulateBinReloc.h
  FLSimulateBinReloc.c
  )

# - Compile/Link
add_executable(flsimulate ${FLSIMULATE_SOURCES})
set_target_properties(flsimulate PROPERTIES COMPILE_DEFINITIONS ENABLE_BINRELOC)

target_link_libraries(flsimulate
  Falaise
  ${Bayeux_MCTOOLS_GEANT4_LIBRARY}
  ${Bayeux_LIBRARIES}
  ${Boost_LIBRARIES}
  ${Geant4_LIBRARIES}
  )

# - RPath it
set_target_properties(flsimulate PROPERTIES INSTALL_RPATH_USE_LINK_PATH 1)

if(UNIX AND NOT APPLE)
  set_target_properties(flsimulate
    PROPERTIES INSTALL_RPATH "\$ORIGIN/../${CMAKE_INSTALL_LIBDIR}"
    )
elseif(APPLE)
  # Temporary setting - needs testing
  set_target_properties(flsimulate
    PROPERTIES
      INSTALL_RPATH "@loader_path/../${CMAKE_INSTALL_LIBDIR}"
      LINK_FLAGS "-Wl,-rpath,@loader_path/../${CMAKE_INSTALL_LIBDIR}"
    )
endif()

# - Install
# TODO : fixup rpaths as needed
install(TARGETS flsimulate
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
  )

#-----------------------------------------------------------------------
# Build Tests
#
