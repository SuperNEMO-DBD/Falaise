# - CMake build script for Falaise flhistogram app

#-----------------------------------------------------------------------
# Copyright 2026 François Mauger <mauger@lpccaen.in2p3.fr>
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
find_package(Bayeux ${FALAISE_BAYEUX_MIN_VERSION} REQUIRED)

#-----------------------------------------------------------------------
# Compile/Link App
#
add_executable(flhistogram
  flhistogram.cxx
)
target_include_directories(flhistogram PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${CMAKE_CURRENT_BINARY_DIR}
)
target_compile_definitions(flhistogram PRIVATE ENABLE_BINRELOC)
target_link_libraries(flhistogram
  Falaise
  Bayeux::Bayeux
  Boost::program_options
)
# target_clang_format(fluddstat)

# - Ensure link to internal and external deps
set_target_properties(flhistogram PROPERTIES INSTALL_RPATH_USE_LINK_PATH 1)

if(UNIX AND NOT APPLE)
  set_target_properties(flhistogram
    PROPERTIES INSTALL_RPATH "\$ORIGIN/../${CMAKE_INSTALL_LIBDIR}"
  )
elseif(APPLE)
  # Temporary setting - needs testing
  set_target_properties(flhistogram
    PROPERTIES
    INSTALL_RPATH "@loader_path/../${CMAKE_INSTALL_LIBDIR}"
  )
endif()

# - Install it
install(TARGETS flhistogram
  EXPORT FalaiseTargets
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

# - end
