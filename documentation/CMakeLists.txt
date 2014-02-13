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

#-----------------------------------------------------------------------
# Core Doxygen Configuration
#
set(Falaise_DOXYFILE_IN  falaise_doxygen.conf.in)
set(Falaise_DOXYFILE_OUT ${PROJECT_BINARY_DIR}/documentation/falaise_doxygen.conf)
set(Falaise_DOXYGEN_OUTPUT_DIR  ${Falaise_DATAROOTDIR}/Documentation/API)
set(Falaise_DOXYGEN_OUTPUT_FILE ${Falaise_DOXYGEN_OUTPUT_DIR}/html/index.html)

option(Falaise_BUILD_LATEX_DOCS "Build LaTeX Manual for Falaise" OFF)
mark_as_advanced(Falaise_BUILD_LATEX_DOCS)
if(Falaise_BUILD_LATEX_DOCS)
  set(FALAISE_GENERATE_LATEX "YES")
else()
  set(FALAISE_GENERATE_LATEX "NO")
endif()

# Bayeux tag files
# If we're using an internal Bayeux, use relative paths, otherwise
# absolute
if(NOT Falaise_USE_SYSTEM_BAYEUX AND Bayeux_DOXYGEN_HTML_DIR)
  file(RELATIVE_PATH Bayeux_DOXYGEN_HTML_DIR ${Falaise_DOXYGEN_OUTPUT_DIR}/html ${Bayeux_DOXYGEN_HTML_DIR})
  file(RELATIVE_PATH Bayeux_DOXYGEN_TAGFILE ${PROJECT_BINARY_DIR} ${Bayeux_DOXYGEN_TAGFILE})
endif()

#-----------------------------------------------------------------------
# Individual Documents
#
set(Falaise_MARKDOWN_DOCS
  Mainpage.md
  ${PROJECT_SOURCE_DIR}/source/Falaise.1.md
  InstallationGuide.md
  flsimulate/FLSimulate.md
  flreconstruct/FLReconstruct.md
  flreconstruct/WritingFLReconstructModules.md
  flreconstruct/WorkingWithEventRecords.md
  flreconstruct/UsingServices.md
  flreconstruct/DocumentingFLReconstructModules.md
  development/DevelopingFalaise.md
  development/CodingStandards.md
  )

set(Falaise_ABSOLUTE_MARKDOWN_DOCS)
foreach(_mdDoc ${Falaise_MARKDOWN_DOCS})
  get_filename_component(_absMdDoc ${_mdDoc} ABSOLUTE)
  set(Falaise_ABSOLUTE_MARKDOWN_DOCS "${Falaise_ABSOLUTE_MARKDOWN_DOCS} ${_absMdDoc}")
endforeach()

#-----------------------------------------------------------------------
# Build doxygen docs
#
configure_file(${Falaise_DOXYFILE_IN} ${Falaise_DOXYFILE_OUT} @ONLY)

add_custom_command(
  OUTPUT  ${Falaise_DOXYGEN_OUTPUT_FILE}
  COMMAND ${CMAKE_COMMAND} -E make_directory ${Falaise_DOXYGEN_OUTPUT_DIR}
  COMMAND ${DOXYGEN_EXECUTABLE} ${Falaise_DOXYFILE_OUT}
  WORKING_DIRECTORY ${PROJECT_BINARY_DIR}
  DEPENDS Falaise ${Falaise_DOXYFILE_OUT} ${Falaise_MARKDOWN_DOCS}
  COMMENT "Doxygenating Falaise"
  )

add_custom_target(falaise_docs ALL DEPENDS ${Falaise_DOXYGEN_OUTPUT_FILE})
