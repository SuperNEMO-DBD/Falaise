# - CMake script for Falaise's Documentation
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


set(Falaise_DOXYFILE_IN  falaise_doxygen.conf.in)
set(Falaise_DOXYFILE_OUT ${PROJECT_BINARY_DIR}/doc/falaise_doxygen.conf)
set(Falaise_DOXYGEN_OUTPUT_DIR  ${Falaise_BUILDPRODUCT_DIR}/share/Falaise-${Falaise_VERSION}/Documentation/API)
set(Falaise_DOXYGEN_OUTPUT_FILE ${Falaise_DOXYGEN_OUTPUT_DIR}/html/index.html)

#-----------------------------------------------------------------------
# Build doxygen docs
#
configure_file(${Falaise_DOXYFILE_IN} ${Falaise_DOXYFILE_OUT} @ONLY)

add_custom_command(
  OUTPUT  ${Falaise_DOXYGEN_OUTPUT_FILE}
  COMMAND ${CMAKE_COMMAND} -E make_directory ${Falaise_DOXYGEN_OUTPUT_DIR}
  COMMAND ${DOXYGEN_EXECUTABLE} ${Falaise_DOXYFILE_OUT}
  WORKING_DIRECTORY ${PROJECT_BINARY_DIR}
  DEPENDS Falaise ${Falaise_DOXYFILE_OUT}
  COMMENT "Doxygenating Falaise"
  )

add_custom_target(falaise_docs ALL DEPENDS ${Falaise_DOXYGEN_OUTPUT_FILE})

