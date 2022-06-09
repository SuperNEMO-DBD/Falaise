# - CMake build script for Falaise flsimrc_mkcellstatuslist app
# message(STATUS "==============> flsimrc_mkcellstatuslist")

#-----------------------------------------------------------------------
# Copyright (c) 2022 by François Mauger <mauger@lpccaen.in2p3.fr>
# Copyright (c) 2022 by Université de Caen Normandie
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
add_executable(flsimrc-mkcellstatuslist
  make_cell_status_lists.cxx
  )
target_include_directories(flsimrc-mkcellstatuslist PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${CMAKE_CURRENT_BINARY_DIR}
  )
target_link_libraries(flsimrc-mkcellstatuslist
  Falaise
  Bayeux::Bayeux
  Boost::program_options
  )
target_clang_format(flsimrc-mkcellstatuslist)

# - Ensure link to internal and external deps
set_target_properties(flsimrc-mkcellstatuslist PROPERTIES INSTALL_RPATH_USE_LINK_PATH 1)

if(UNIX AND NOT APPLE)
  set_target_properties(flsimrc-mkcellstatuslist
    PROPERTIES INSTALL_RPATH "\$ORIGIN/../${CMAKE_INSTALL_LIBDIR};\$ORIGIN/../${CMAKE_INSTALL_PLUGINDIR}"
    )
elseif(APPLE)
  # Temporary setting - needs testing
  set_target_properties(flsimrc-mkcellstatuslist
    PROPERTIES
    INSTALL_RPATH "@loader_path/../${CMAKE_INSTALL_LIBDIR};@loader_path/../${CMAKE_INSTALL_PLUGINDIR}"
    )
endif()

# - Install it
install(TARGETS flsimrc-mkcellstatuslist
  EXPORT FalaiseTargets
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
  )

if(FALAISE_ENABLE_TESTING)

  add_test(NAME testSimRC_mkcellstatuslist_1
    COMMAND flsimrc-mkcellstatuslist
    -V "debug"
    -d "0.1.1"
    -o "1.3.5"
    -o "*.*.15"
    -o "*.*.16"
    -l "traker_cell_status.lis"
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    ) 

endif(FALAISE_ENABLE_TESTING)

# - end
