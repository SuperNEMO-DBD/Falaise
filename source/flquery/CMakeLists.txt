# - CMake build script for Falaise flquery utility

#-----------------------------------------------------------------------
# Copyright 2014-2015 François Mauger <mauger@lpccaen.in2p3.fr>
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

if (Falaise_BUILD_DEVELOPER_TOOLS)
  # - Generate application configuration file
  configure_file(flquery.in ${Falaise_BUILDPRODUCT_DIR}/${CMAKE_INSTALL_BINDIR}/flquery @ONLY)

  install(FILES ${Falaise_BUILDPRODUCT_DIR}/${CMAKE_INSTALL_BINDIR}/flquery
    DESTINATION ${CMAKE_INSTALL_BINDIR}
    PERMISSIONS
    OWNER_READ OWNER_EXECUTE
    GROUP_READ GROUP_EXECUTE
    WORLD_READ WORLD_EXECUTE
    COMPONENT Development
    )
endif()

#-----------------------------------------------------------------------
