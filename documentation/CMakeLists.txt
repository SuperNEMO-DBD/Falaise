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
# Configure documentation generation, if required
#
set(FALAISE_DOCEXPORT_DIR "${PROJECT_BINARY_DIR}/documentation")
find_package(Doxygen 1.8 REQUIRED)

# - For man pages, pandoc for now
#find_package(Pandoc 1.10 REQUIRED)
#find_package(<something to compress man?>)
find_program(Pandoc_EXECUTABLE pandoc
  DOC "path to pandoc executable"
  )
set(Pandoc_FOUND ${Pandoc_EXECUTABLE})


#-----------------------------------------------------------------------
# Core Doxygen Configuration
#
set(FALAISE_DOXYFILE_IN  falaise_doxygen.conf.in)
set(FALAISE_DOXYFILE_OUT ${PROJECT_BINARY_DIR}/documentation/falaise_doxygen.conf)
set(FALAISE_DOXYGEN_OUTPUT_DIR  ${FALAISE_BUILD_DATAROOTDIR}/${FALAISE_DOCUMENTATION_DIR}/API)
set(FALAISE_DOXYGEN_OUTPUT_FILE ${FALAISE_DOXYGEN_OUTPUT_DIR}/html/index.html)
set(FALAISE_DOXYGEN_OUTPUT_IMAGE_DIR ${FALAISE_DOXYGEN_OUTPUT_DIR}/html/images)

option(FALAISE_BUILD_LATEX_DOCS "Build LaTeX Manual for Falaise" OFF)
mark_as_advanced(FALAISE_BUILD_LATEX_DOCS)
if(FALAISE_BUILD_LATEX_DOCS)
  set(FALAISE_GENERATE_LATEX "YES")
else()
  set(FALAISE_GENERATE_LATEX "NO")
endif()

# Bayeux tag files
# If we're using an internal Bayeux, use relative paths, otherwise
# absolute
if(Bayeux_DOXYGEN_HTML_DIR)
  # Keep a record of full paths as doxygen may use these!
  set(Bayeux_DOXYGEN_FULL_TAGFILE "${Bayeux_DOXYGEN_TAGFILE}")
  file(RELATIVE_PATH Bayeux_DOXYGEN_HTML_DIR ${FALAISE_DOXYGEN_OUTPUT_DIR}/html ${Bayeux_DOXYGEN_HTML_DIR})
  file(RELATIVE_PATH Bayeux_DOXYGEN_TAGFILE ${PROJECT_BINARY_DIR} ${Bayeux_DOXYGEN_TAGFILE})

  # Only generate the prep file when we crosslink to Bayeux
  configure_file(prepare_release_docs.bash.in ${PROJECT_BINARY_DIR}/release-tools/prepare_release_docs.bash @ONLY)
endif()

set(FALAISE_MARKDOWN_DOCS
  Mainpage.md
  ${PROJECT_SOURCE_DIR}/README.md
  flsimulate/FLSimulate.md
  flsimulate/FLSimulateOutput.md
  flreconstruct/FLReconstruct.md
  flreconstruct/PipelineOutput.md
  flreconstruct/WritingFLReconstructPipelineScripts.md
  flreconstruct/SupportedFLReconstructModules.md
  flreconstruct/UsingMockCalibration.md
  flreconstruct/WritingFLReconstructModules.md
  flreconstruct/WorkingWithEventRecords.md
  flreconstruct/UsingServices.md
  flreconstruct/DocumentingFLReconstructModules.md
  flvisualize/FLVisualize.md
  )

set(FALAISE_ABSOLUTE_MARKDOWN_DOCS)
foreach(_mdDoc ${FALAISE_MARKDOWN_DOCS})
  get_filename_component(_absMdDoc ${_mdDoc} ABSOLUTE)
  set(FALAISE_ABSOLUTE_MARKDOWN_DOCS "${FALAISE_ABSOLUTE_MARKDOWN_DOCS} ${_absMdDoc}")
endforeach()

# - Image paths
set(FALAISE_DOXYGEN_IMAGE_DIRS
  "${CMAKE_CURRENT_SOURCE_DIR}/flsimulate/images ${CMAKE_CURRENT_SOURCE_DIR}/flreconstruct/images ${CMAKE_CURRENT_SOURCE_DIR}/flvisualize/images"
  )

#-----------------------------------------------------------------------
# Build doxygen docs
#
configure_file(${FALAISE_DOXYFILE_IN} ${FALAISE_DOXYFILE_OUT} @ONLY)

add_custom_command(
  OUTPUT  ${FALAISE_DOXYGEN_OUTPUT_FILE}
  COMMAND ${CMAKE_COMMAND} -E make_directory ${FALAISE_DOXYGEN_OUTPUT_DIR}
  COMMAND ${DOXYGEN_EXECUTABLE} ${FALAISE_DOXYFILE_OUT}
  WORKING_DIRECTORY ${PROJECT_BINARY_DIR}
  DEPENDS Falaise ${FALAISE_DOXYFILE_OUT} ${FALAISE_MARKDOWN_DOCS}
  COMMENT "Doxygenating Falaise"
  )

add_custom_target(falaise_docs ALL DEPENDS ${FALAISE_DOXYGEN_OUTPUT_FILE})

configure_file("flsimulate/FLSimulateVariantsDoc-2.1.pdf"
  ${FALAISE_DOXYGEN_OUTPUT_DIR}/html/FLSimulateVariantsDoc-2.1.pdf
  COPYONLY)

configure_file("flsimulate/FLSimulate-3.0.0.conf"
  ${FALAISE_DOXYGEN_OUTPUT_DIR}/html/FLSimulate-3.0.0.conf
  COPYONLY)

# - end
