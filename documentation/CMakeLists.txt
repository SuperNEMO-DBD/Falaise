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
  file(RELATIVE_PATH Bayeux_DOXYGEN_HTML_DIR ${FALAISE_DOXYGEN_OUTPUT_DIR}/html ${Bayeux_DOXYGEN_HTML_DIR})
  file(RELATIVE_PATH Bayeux_DOXYGEN_TAGFILE ${PROJECT_BINARY_DIR} ${Bayeux_DOXYGEN_TAGFILE})
endif()

# execute_process(COMMAND ${CMAKE_COMMAND} -E make_directory ${FALAISE_DOXYGEN_OUTPUT_DIR}
#   WORKING_DIRECTORY ${PROJECT_BINARY_DIR}
#   )

# execute_process(COMMAND rst2pdf
#   ${CMAKE_CURRENT_BINARY_DIR}/FLSimulateVariantsDoc.rst
#   --output ${CMAKE_CURRENT_BINARY_DIR}/FLSimulateVariantsDoc.pdf
#   RESULT_VARIABLE _errcode
#   WORKING_DIRECTORY ${PROJECT_BUILD_PREFIX}
#   ERROR_FILE ${CMAKE_CURRENT_BINARY_DIR}/_rst2html.err
#   )
# message(STATUS "****** _errcode = [${_errcode}] ")

# #--title="FLSimulate variant  p arameters description (version 2.1)"

# execute_process(COMMAND rst2html
#   --no-generator
#   --no-source-link
#   --toc-entry-backlinks
#   --toc-top-backlinks
#   --field-name-limit=0
#   --stylesheet=tabs.css
#   --link-stylesheet
#   ${CMAKE_CURRENT_BINARY_DIR}/FLSimulateVariantsDoc.rst
#   RESULT_VARIABLE _errcode
#   WORKING_DIRECTORY ${PROJECT_BUILD_PREFIX}
#   OUTPUT_FILE ${CMAKE_CURRENT_BINARY_DIR}/FLSimulateVariantsDoc.html
#   ERROR_FILE ${CMAKE_CURRENT_BINARY_DIR}/_rst2html.err
#   )
# message(STATUS "****** _errcode = [${_errcode}] ")

# configure_file(${CMAKE_CURRENT_BINARY_DIR}/FLSimulateVariantsDoc.html
#   ${FALAISE_DOXYGEN_OUTPUT_DIR}/html
#   COPYONLY)

# configure_file(${CMAKE_CURRENT_BINARY_DIR}/FLSimulateVariantsDoc.pdf
#   ${FALAISE_DOXYGEN_OUTPUT_DIR}/html
#   COPYONLY)

#-----------------------------------------------------------------------
# Individual Documents
#
set(Cadfael_VERSION "1.1.0")
configure_file(InstallationGuide.md.in InstallationGuide.md @ONLY)

# execute_process(COMMAND bxvariant_inspector
#   --datatools::resource-path=falaise@${PROJECT_SOURCE_DIR}/resources
#   --variant-config=@falaise:config/snemo/demonstrator/simulation/geant4_control/2.1/variants/repository.conf
#   --action doc
#   RESULT_VARIABLE _errcode
#   WORKING_DIRECTORY ${PROJECT_BUILD_PREFIX}
#   OUTPUT_FILE ${CMAKE_CURRENT_BINARY_DIR}/FLSimulateVariantsDoc.rst
#   ERROR_FILE ${CMAKE_CURRENT_BINARY_DIR}/_bxvariant_inspector.err
#   )
# message(STATUS "****** _errcode = [${_errcode}]")
# ${CMAKE_CURRENT_BINARY_DIR}/FLSimulateVariantsDoc.rst

set(FALAISE_MARKDOWN_DOCS
  Mainpage.md
  ${PROJECT_SOURCE_DIR}/source/Falaise.1.md
  ${CMAKE_CURRENT_BINARY_DIR}/InstallationGuide.md
  flsimulate/FLSimulate.md
  flsimulate/FLSimulateOutput.md
  flreconstruct/FLReconstruct.md
  flreconstruct/PipelineOutput.md
  flreconstruct/FLPTD2Root.md
  flreconstruct/WritingFLReconstructPipelineScripts.md
  flreconstruct/SupportedFLReconstructModules.md
  flreconstruct/UsingMockCalibration.md
  flreconstruct/WritingFLReconstructModules.md
  flreconstruct/WorkingWithEventRecords.md
  flreconstruct/UsingServices.md
  flreconstruct/DocumentingFLReconstructModules.md
  flvisualize/FLVisualize.md
  development/DevelopingFalaise.md
  development/UnitTestingWithCatch.md
  development/CodingStandards.md
  )

set(FALAISE_ABSOLUTE_MARKDOWN_DOCS)
foreach(_mdDoc ${FALAISE_MARKDOWN_DOCS})
  get_filename_component(_absMdDoc ${_mdDoc} ABSOLUTE)
  set(FALAISE_ABSOLUTE_MARKDOWN_DOCS "${FALAISE_ABSOLUTE_MARKDOWN_DOCS} ${_absMdDoc}")
endforeach()

# - Image paths
set(FALAISE_DOXYGEN_IMAGE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}/flsimulate/images")

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
  ${FALAISE_DOXYGEN_OUTPUT_DIR}/html
  COPYONLY)

# - end