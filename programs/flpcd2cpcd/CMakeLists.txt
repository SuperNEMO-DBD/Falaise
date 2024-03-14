# - CMake build script for Falaise flpcd2cpcd app

#-----------------------------------------------------------------------
# Copyright 2024 F.Mauger, E.Chauveau
# Copyright 2024 SuperNEMO Collaboration
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
find_package(Boost 1.60 REQUIRED program_options)

#-----------------------------------------------------------------------
# Compile/Link App
#
add_executable(flpcd2cpcd
  flpcd2cpcdmain.cxx
  FLpCDToCpCDAlgo.h
  FLpCDToCpCDAlgo.cc
  FLpCDToCpCDApplication.h
  FLpCDToCpCDApplication.cc
  FLpCDToCpCDPipeline.h
  FLpCDToCpCDPipeline.cc
  FLpCDToCpCDImpl.h
  FLpCDToCpCDImpl.cc
  FLpCDToCpCDParams.h
  FLpCDToCpCDParams.cc
  FLpCDToCpCDCommandLine.h
  FLpCDToCpCDCommandLine.cc
  FLpCDToCpCDErrors.h
  FLpCDToCpCDErrors.cc
)
target_include_directories(flpcd2cpcd PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${CMAKE_CURRENT_BINARY_DIR}
  )
target_compile_definitions(flpcd2cpcd PRIVATE ENABLE_BINRELOC)
target_link_libraries(flpcd2cpcd
  Falaise
  Bayeux::Bayeux
  Boost::program_options
  )
target_clang_format(flpcd2cpcd)

# - Ensure link to internal and external deps
set_target_properties(flpcd2cpcd PROPERTIES INSTALL_RPATH_USE_LINK_PATH 1)

if(UNIX AND NOT APPLE)
  set_target_properties(flpcd2cpcd
    PROPERTIES INSTALL_RPATH "\$ORIGIN/../${CMAKE_INSTALL_LIBDIR}"
    )
elseif(APPLE)
  # Temporary setting - needs testing
  set_target_properties(flpcd2cpcd
    PROPERTIES
      INSTALL_RPATH "@loader_path/../${CMAKE_INSTALL_LIBDIR}"
      )
endif()

# - Install it
install(TARGETS flpcd2cpcd
  EXPORT FalaiseTargets
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
  )

#-----------------------------------------------------------------------
# Build Tests
#
if(FALAISE_ENABLE_TESTING)
  add_subdirectory(tests)
endif()
