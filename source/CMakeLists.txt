# - CMake script for Falaise Library and Apps
#

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
# Core configuration
#
#include_directories(${FALAISE_BUILDPRODUCT_DIR}/include)
#include_directories(${FALAISE_BUILDPRODUCT_DIR}/include/falaise)
#include_directories(${Bayeux_INCLUDE_DIRS})

#-----------------------------------------------------------------------
# Library
#
add_subdirectory(falaise)


#-----------------------------------------------------------------------
# Manual
#
if(Pandoc_FOUND)
  set(FALAISE_MANPAGE_IN  ${CMAKE_CURRENT_SOURCE_DIR}/Falaise.1.md)
  set(FALAISE_MANPAGE_OUT ${FALAISE_BUILDPRODUCT_DIR}/share/man/man1/Falaise.1)

  add_custom_command(OUTPUT ${FALAISE_MANPAGE_OUT}
    COMMAND ${CMAKE_COMMAND} -E make_directory ${FALAISE_BUILDPRODUCT_DIR}/share/man/man1
    COMMAND ${Pandoc_EXECUTABLE} -s -w man ${FALAISE_MANPAGE_IN} -o ${FALAISE_MANPAGE_OUT}
    COMMENT "Generating Falaise.1 man page"
    DEPENDS ${FALAISE_MANPAGE_IN}
    )
  add_custom_target(falaise_man ALL DEPENDS ${FALAISE_MANPAGE_OUT})
endif()
