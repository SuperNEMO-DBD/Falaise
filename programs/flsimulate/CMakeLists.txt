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
find_package(Boost 1.60 REQUIRED program_options filesystem system)
find_package(Geant4 REQUIRED)

#-----------------------------------------------------------------------
# Build command line app
# - Compile/Link
add_executable(flsimulate_legacy
  flsimulatemain_legacy.cc
  FLSimulateResources.h
  FLSimulateResources.cc
  )
target_include_directories(flsimulate_legacy PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${CMAKE_CURRENT_BINARY_DIR}
  )
target_link_libraries(flsimulate_legacy
  Falaise
  Bayeux::Bayeux
  Bayeux::Bayeux_mctools_geant4
  ${Boost_LIBRARIES}
  ${Geant4_LIBRARIES}
  )
target_clang_format(flsimulate_legacy)

# - Ensure link to internal and external deps
set_target_properties(flsimulate_legacy PROPERTIES INSTALL_RPATH_USE_LINK_PATH 1)

if(UNIX AND NOT APPLE)
  set_target_properties(flsimulate_legacy
    PROPERTIES INSTALL_RPATH "\$ORIGIN/../${CMAKE_INSTALL_LIBDIR}"
    )
elseif(APPLE)
  # Temporary setting - needs testing
  set_target_properties(flsimulate_legacy
    PROPERTIES
      INSTALL_RPATH "@loader_path/../${CMAKE_INSTALL_LIBDIR}"
    )
endif()

# - Install
# TODO : fixup rpaths as needed
install(TARGETS flsimulate_legacy
  EXPORT FalaiseTargets
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
  )

#-----------------------------------------------------------------------
# Build
add_executable(flsimulate
  flsimulatemain.cc
  FLSimulateArgs.h
  FLSimulateArgs.cc
  FLSimulateCommandLine.h
  FLSimulateCommandLine.cc
  FLSimulateErrors.h
  FLSimulateErrors.cc
  FLSimulateUtils.h
  FLSimulateUtils.cc
  )
target_include_directories(flsimulate PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${CMAKE_CURRENT_BINARY_DIR}
  )
target_link_libraries(flsimulate
  Falaise
  Bayeux::Bayeux
  Bayeux::Bayeux_mctools_geant4
  ${Boost_LIBRARIES}
  ${Geant4_LIBRARIES}
  )
target_clang_format(flsimulate)

# - Ensure link to internal and external deps
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
    )
endif()

# - Install
# TODO : fixup rpaths as needed
install(TARGETS flsimulate
  EXPORT FalaiseTargets
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
  )

#-----------------------------------------------------------------------
# Build
add_executable(flsimulate-configure
  flsimulatecfgmain.cc
  FLSimulateCommandLine.h
  FLSimulateCommandLine.cc
  FLSimulateErrors.h
  FLSimulateErrors.cc
  FLSimulateUtils.h
  FLSimulateUtils.cc
  )
target_include_directories(flsimulate-configure PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${CMAKE_CURRENT_BINARY_DIR}
  )
target_link_libraries(flsimulate-configure
  Falaise
  Bayeux::Bayeux
  ${Boost_LIBRARIES}
  )
target_clang_format(flsimulate-configure)

# - Ensure link to internal and external deps
set_target_properties(flsimulate-configure PROPERTIES INSTALL_RPATH_USE_LINK_PATH 1)

if(UNIX AND NOT APPLE)
  set_target_properties(flsimulate-configure
    PROPERTIES INSTALL_RPATH "\$ORIGIN/../${CMAKE_INSTALL_LIBDIR}"
    )
elseif(APPLE)
  # Temporary setting - needs testing
  set_target_properties(flsimulate-configure
    PROPERTIES
      INSTALL_RPATH "@loader_path/../${CMAKE_INSTALL_LIBDIR}"
    )
endif()

# - Install
# TODO : fixup rpaths as needed
install(TARGETS flsimulate-configure
  EXPORT FalaiseTargets
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
  )

# Testing, if enabled
if(FALAISE_ENABLE_TESTING)
  add_subdirectory(tests)
endif()

# - end
