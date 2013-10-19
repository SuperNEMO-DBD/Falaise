# - Top level CMake script for Falaise
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
# Enforce an out-of-source build.
#
if(${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_BINARY_DIR})
  message(STATUS "Falaise requires an out-of-source build.")
  message(STATUS "Please remove these files from ${CMAKE_BINARY_DIR} first:")
  message(STATUS "  CMakeCache.txt")
  message(STATUS "  CMakeFiles")
  message(STATUS "Once these files are removed, create a separate directory")
  message(STATUS "and run CMake from there, pointing it to:")
  message(STATUS "  ${CMAKE_SOURCE_DIR}")
  message(FATAL_ERROR "in-source build detected")
endif()

#-----------------------------------------------------------------------
# CMake/project requirements and configuration
#
cmake_minimum_required(VERSION 2.8 FATAL_ERROR)
project(Falaise)

# - Versioning
set(Falaise_VERSION_MAJOR 1)
set(Falaise_VERSION_MINOR 0)
set(Falaise_VERSION_PATCH 0)
set(Falaise_VERSION "${Falaise_VERSION_MAJOR}.${Falaise_VERSION_MINOR}.${Falaise_VERSION_PATCH}")

# - SVN Versioning, if available
set(Falaise_VERSION_REVISION 0)

# - Standard UNIX Tool install paths, including relative paths for use
# by applications requiring resources
# Validate that certain paths are relative, otherwise relocation may fail
include(GNUInstallDirs)
foreach(_dir BINDIR LIBDIR INCLUDEDIR DATAROOTDIR)
  if(IS_ABSOLUTE "${CMAKE_INSTALL_${_dir}}")
    message(FATAL_ERROR "Absolute path for CMAKE_INSTALL_${_dir} not allowed")
  endif()
endforeach()

file(RELATIVE_PATH Falaise_BINDIR_TO_RESOURCEDIR "${CMAKE_INSTALL_FULL_BINDIR}" "${CMAKE_INSTALL_FULL_DATAROOTDIR}/${PROJECT_NAME}-${${PROJECT_NAME}_VERSION}")

# - Configure output paths for products to give inplace build that roughly
#   matches install layout.
set(Falaise_BUILDPRODUCT_DIR "${PROJECT_BINARY_DIR}/BuildProducts")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${Falaise_BUILDPRODUCT_DIR}/${CMAKE_INSTALL_LIBDIR}")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY  "${Falaise_BUILDPRODUCT_DIR}/${CMAKE_INSTALL_BINDIR}")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${Falaise_BUILDPRODUCT_DIR}/${CMAKE_INSTALL_LIBDIR}")

#-----------------------------------------------------------------------
# Configure testing if required
#
option(Falaise_ENABLE_TESTING "Build unit testing system for Falaise" OFF)
if(Falaise_ENABLE_TESTING)
  enable_testing()
endif()

#-----------------------------------------------------------------------
# Configure documentation generation, if required
#
set(Falaise_DOCEXPORT_DIR "${PROJECT_BINARY_DIR}/documentation")
option(Falaise_BUILD_DOCS "Build documentation for Falaise" OFF)
if(Falaise_BUILD_DOCS)
  # - For main HTML docs
  find_package(Doxygen 1.8 REQUIRED)
  # - For man pages, pandoc for now
  #find_package(Pandoc 1.10 REQUIRED)
  #find_package(<something to compress man?>)
  find_program(Pandoc_EXECUTABLE pandoc
    DOC "path to pandoc executable"
    )
  set(Pandoc_FOUND ${Pandoc_EXECUTABLE})
endif()

#-----------------------------------------------------------------------
# Configure/Build utilities as needed
#
option(Falaise_USE_SYSTEM_BAYEUX "Use system-installed Bayeux" OFF)
mark_as_advanced(Falaise_USE_SYSTEM_BAYEUX)

if(Falaise_USE_SYSTEM_BAYEUX)
  find_package(Bayeux 1.0.0 REQUIRED)
else()
  # - Configure internal build
  set(Bayeux_EMBEDDED 1)
  set(Bayeux_BUILDPRODUCT_DIR ${Falaise_BUILDPRODUCT_DIR})
  if(Falaise_ENABLE_TESTING)
    set(Bayeux_ENABLE_TESTING ON)
  endif()
  set(Bayeux_WITH_GEANT4 ON)

  # - Build
  add_subdirectory(utilities/flbayeux)

  # - Set up so that Falaise will use the internal build
  set(Bayeux_INCLUDE_DIRS "${Falaise_BUILDPRODUCT_DIR}/include" "${Falaise_BUILDPRODUCT_DIR}/include/bayeux")
  set(Bayeux_LIBRARIES Bayeux)
  set(Bayeux_MCTOOLS_GEANT4_LIBRARY Bayeux_mctools_geant4)
  set(Bayeux_DOXYGEN_TAGFILE)
endif()

set(Falaise_DATAROOTDIR ${Falaise_BUILDPRODUCT_DIR}/share/Falaise-${Falaise_VERSION})

#-----------------------------------------------------------------------
# Build the subdirectories as required
#
add_subdirectory(source)
if(Falaise_BUILD_DOCS)
  add_subdirectory(documentation)
endif()


#-----------------------------------------------------------------------
# Resource installation
#
set(_falaise_resources_dir "${CMAKE_SOURCE_DIR}/resources")


# Extract the list of resource files with:
# find resources/ -type f | grep -v "/_" | grep -v "~$" | grep -v /.svn/ | sed -e 's@^resources/@${CMAKE_SOURCE_DIR}/resources/@g'
# A GLOB_RECURSE could be useful here or some shell command.

set(_falaise_resources
${CMAKE_SOURCE_DIR}/resources/config/geometry/snemo/tracker_commissioning/1.0/categories.lis
${CMAKE_SOURCE_DIR}/resources/config/geometry/snemo/tracker_commissioning/1.0/manager.conf
${CMAKE_SOURCE_DIR}/resources/config/geometry/snemo/tracker_commissioning/1.0/README.rst
${CMAKE_SOURCE_DIR}/resources/config/geometry/snemo/tracker_commissioning/1.0/materials_plugin.conf
${CMAKE_SOURCE_DIR}/resources/config/geometry/snemo/tracker_commissioning/1.0/C0_module.geom
${CMAKE_SOURCE_DIR}/resources/config/geometry/snemo/tracker_commissioning/1.0/C0_shape_layers.geom
${CMAKE_SOURCE_DIR}/resources/config/geometry/snemo/tracker_commissioning/1.0/setup.geom
${CMAKE_SOURCE_DIR}/resources/config/geometry/snemo/tracker_commissioning/1.0/muon_trigger.geom
${CMAKE_SOURCE_DIR}/resources/config/geometry/snemo/demonstrator/3.0/calorimeter_modules/calorimeter_submodules.geom
${CMAKE_SOURCE_DIR}/resources/config/geometry/snemo/demonstrator/3.0/calorimeter_modules/calorimeter_walls.geom
${CMAKE_SOURCE_DIR}/resources/config/geometry/snemo/demonstrator/3.0/calorimeter_modules/README.rst
${CMAKE_SOURCE_DIR}/resources/config/geometry/snemo/demonstrator/3.0/calorimeter_modules/calorimeter_module.geom
${CMAKE_SOURCE_DIR}/resources/config/geometry/snemo/demonstrator/3.0/calorimeter_modules/pmt_8inch.geom
${CMAKE_SOURCE_DIR}/resources/config/geometry/snemo/demonstrator/3.0/categories.lis
${CMAKE_SOURCE_DIR}/resources/config/geometry/snemo/demonstrator/3.0/locators_plugin.conf
${CMAKE_SOURCE_DIR}/resources/config/geometry/snemo/demonstrator/3.0/manager.conf
${CMAKE_SOURCE_DIR}/resources/config/geometry/snemo/demonstrator/3.0/source_module/pictures/foil_roller.gp
${CMAKE_SOURCE_DIR}/resources/config/geometry/snemo/demonstrator/3.0/source_module/pictures/source_submodule_yz.jpeg
${CMAKE_SOURCE_DIR}/resources/config/geometry/snemo/demonstrator/3.0/source_module/pictures/source_foil_layout_0.fig
${CMAKE_SOURCE_DIR}/resources/config/geometry/snemo/demonstrator/3.0/source_module/pictures/source_foil_layout_0.pdf
${CMAKE_SOURCE_DIR}/resources/config/geometry/snemo/demonstrator/3.0/source_module/pictures/source_submodule_yz_detail.jpeg
${CMAKE_SOURCE_DIR}/resources/config/geometry/snemo/demonstrator/3.0/source_module/source_module.geom
${CMAKE_SOURCE_DIR}/resources/config/geometry/snemo/demonstrator/3.0/source_module/source_frame_horizontal_beams.geom
${CMAKE_SOURCE_DIR}/resources/config/geometry/snemo/demonstrator/3.0/source_module/README.rst
${CMAKE_SOURCE_DIR}/resources/config/geometry/snemo/demonstrator/3.0/source_module/source_frame_vertical_beams.geom
${CMAKE_SOURCE_DIR}/resources/config/geometry/snemo/demonstrator/3.0/source_module/source_calibration.geom
${CMAKE_SOURCE_DIR}/resources/config/geometry/snemo/demonstrator/3.0/source_module/source_foil_support.geom
${CMAKE_SOURCE_DIR}/resources/config/geometry/snemo/demonstrator/3.0/source_module/source_strips.geom
${CMAKE_SOURCE_DIR}/resources/config/geometry/snemo/demonstrator/3.0/source_module/source_foil_support.geom.pool
${CMAKE_SOURCE_DIR}/resources/config/geometry/snemo/demonstrator/3.0/README.rst
${CMAKE_SOURCE_DIR}/resources/config/geometry/snemo/demonstrator/3.0/materials_plugin.conf
${CMAKE_SOURCE_DIR}/resources/config/geometry/snemo/demonstrator/3.0/setup.geom
${CMAKE_SOURCE_DIR}/resources/config/geometry/snemo/demonstrator/3.0/module_0.geom
${CMAKE_SOURCE_DIR}/resources/config/geometry/snemo/demonstrator/3.0/tracker_modules/tracker_volumes.geom
${CMAKE_SOURCE_DIR}/resources/config/geometry/snemo/demonstrator/3.0/tracker_modules/pictures/gveto_module.model.view_yz_0.jpeg
${CMAKE_SOURCE_DIR}/resources/config/geometry/snemo/demonstrator/3.0/tracker_modules/pictures/tracker_horizontal_beam_front_bottom.model.view_xz_0.jpeg
${CMAKE_SOURCE_DIR}/resources/config/geometry/snemo/demonstrator/3.0/tracker_modules/pictures/xwall_front_right.model.view_xy_0.jpeg
${CMAKE_SOURCE_DIR}/resources/config/geometry/snemo/demonstrator/3.0/tracker_modules/pictures/drift_cell_node_13.model.view_xy_0.jpeg
${CMAKE_SOURCE_DIR}/resources/config/geometry/snemo/demonstrator/3.0/tracker_modules/pictures/cell_design.fig
${CMAKE_SOURCE_DIR}/resources/config/geometry/snemo/demonstrator/3.0/tracker_modules/pictures/drift_cell_core.model.view_3d_0.jpeg
${CMAKE_SOURCE_DIR}/resources/config/geometry/snemo/demonstrator/3.0/tracker_modules/pictures/xwall_module_left.model.view_xz_0.jpeg
${CMAKE_SOURCE_DIR}/resources/config/geometry/snemo/demonstrator/3.0/tracker_modules/pictures/xwall_front_right.model.view_yz_1.jpeg
${CMAKE_SOURCE_DIR}/resources/config/geometry/snemo/demonstrator/3.0/tracker_modules/pictures/tracker_front_volume.model.view_yz_1.jpeg
${CMAKE_SOURCE_DIR}/resources/config/geometry/snemo/demonstrator/3.0/tracker_modules/pictures/gveto_module.model.view_xy_0.jpeg
${CMAKE_SOURCE_DIR}/resources/config/geometry/snemo/demonstrator/3.0/tracker_modules/pictures/gveto_module_inner.model.view_3d_0.jpeg
${CMAKE_SOURCE_DIR}/resources/config/geometry/snemo/demonstrator/3.0/tracker_modules/pictures/xwall_front_right.model.view_xz_0.jpeg
${CMAKE_SOURCE_DIR}/resources/config/geometry/snemo/demonstrator/3.0/tracker_modules/pictures/tracker_horizontal_beam_back_top.model.view_xz_0.jpeg
${CMAKE_SOURCE_DIR}/resources/config/geometry/snemo/demonstrator/3.0/tracker_modules/pictures/tracker_horizontal_beam_outer.model.view_3d_0.jpeg
${CMAKE_SOURCE_DIR}/resources/config/geometry/snemo/demonstrator/3.0/tracker_modules/pictures/tracker_front_submodule.model.view_yz_0.jpeg
${CMAKE_SOURCE_DIR}/resources/config/geometry/snemo/demonstrator/3.0/tracker_modules/pictures/cell_dimensions.fig
${CMAKE_SOURCE_DIR}/resources/config/geometry/snemo/demonstrator/3.0/tracker_modules/pictures/tracker_frame_corner.model.view_3d_0.jpeg
${CMAKE_SOURCE_DIR}/resources/config/geometry/snemo/demonstrator/3.0/tracker_modules/pictures/tracker_horizontal_beam_front_top.model.view_xz_0.jpeg
${CMAKE_SOURCE_DIR}/resources/config/geometry/snemo/demonstrator/3.0/tracker_modules/pictures/tracker_front_volume.model.view_xy_1.jpeg
${CMAKE_SOURCE_DIR}/resources/config/geometry/snemo/demonstrator/3.0/tracker_modules/pictures/tracker_front_submodule.model.view_xy_0.jpeg
${CMAKE_SOURCE_DIR}/resources/config/geometry/snemo/demonstrator/3.0/tracker_modules/pictures/gveto_line_bottom.model.view_yz_2.jpeg
${CMAKE_SOURCE_DIR}/resources/config/geometry/snemo/demonstrator/3.0/tracker_modules/pictures/tracker_front_volume.model.view_xy_0.jpeg
${CMAKE_SOURCE_DIR}/resources/config/geometry/snemo/demonstrator/3.0/tracker_modules/pictures/tracker_front_submodule.model.view_xz_0.jpeg
${CMAKE_SOURCE_DIR}/resources/config/geometry/snemo/demonstrator/3.0/tracker_modules/pictures/xwall_front_right.model.view_yz_0.jpeg
${CMAKE_SOURCE_DIR}/resources/config/geometry/snemo/demonstrator/3.0/tracker_modules/pictures/cell_dimensions.pdf
${CMAKE_SOURCE_DIR}/resources/config/geometry/snemo/demonstrator/3.0/tracker_modules/pictures/gveto_line_left.model.view_yz_0.jpeg
${CMAKE_SOURCE_DIR}/resources/config/geometry/snemo/demonstrator/3.0/tracker_modules/pictures/tracker_horizontal_beam_back_bottom.model.view_xz_0.jpeg
${CMAKE_SOURCE_DIR}/resources/config/geometry/snemo/demonstrator/3.0/tracker_modules/pictures/xwall_inner_module.pdf
${CMAKE_SOURCE_DIR}/resources/config/geometry/snemo/demonstrator/3.0/tracker_modules/pictures/cell_design.gp
${CMAKE_SOURCE_DIR}/resources/config/geometry/snemo/demonstrator/3.0/tracker_modules/pictures/tracker_layer_base.model.view_xy_0.jpeg
${CMAKE_SOURCE_DIR}/resources/config/geometry/snemo/demonstrator/3.0/tracker_modules/pictures/gveto_line_bottom.model.view_yz_0.jpeg
${CMAKE_SOURCE_DIR}/resources/config/geometry/snemo/demonstrator/3.0/tracker_modules/pictures/field_wires_set.model.view_3d_0.jpeg
${CMAKE_SOURCE_DIR}/resources/config/geometry/snemo/demonstrator/3.0/tracker_modules/pictures/cell_design.pdf
${CMAKE_SOURCE_DIR}/resources/config/geometry/snemo/demonstrator/3.0/tracker_modules/pictures/drift_cell_core.model.view_yz_1.jpeg
${CMAKE_SOURCE_DIR}/resources/config/geometry/snemo/demonstrator/3.0/tracker_modules/pictures/tracker_front_submodule.model.view_yz_1.jpeg
${CMAKE_SOURCE_DIR}/resources/config/geometry/snemo/demonstrator/3.0/tracker_modules/pictures/gveto_module_outer.model.view_yz_0.jpeg
${CMAKE_SOURCE_DIR}/resources/config/geometry/snemo/demonstrator/3.0/tracker_modules/pictures/gveto_line_bottom.model.view_yz_1.jpeg
${CMAKE_SOURCE_DIR}/resources/config/geometry/snemo/demonstrator/3.0/tracker_modules/pictures/tracker_horizontal_beam_back_bottom.model.view_yz_0.jpeg
${CMAKE_SOURCE_DIR}/resources/config/geometry/snemo/demonstrator/3.0/tracker_modules/pictures/drift_cell_node_0123.model.view_xy_0.jpeg
${CMAKE_SOURCE_DIR}/resources/config/geometry/snemo/demonstrator/3.0/tracker_modules/pictures/drift_cell_node_123.model.view_xy_0.jpeg
${CMAKE_SOURCE_DIR}/resources/config/geometry/snemo/demonstrator/3.0/tracker_modules/pictures/xwall_module_left.rotated.model.view_3d_0.jpeg
${CMAKE_SOURCE_DIR}/resources/config/geometry/snemo/demonstrator/3.0/tracker_modules/pictures/drift_cell_node_013.model.view_xy_0.jpeg
${CMAKE_SOURCE_DIR}/resources/config/geometry/snemo/demonstrator/3.0/tracker_modules/pictures/xwall_module_left.model.view_3d_0.jpeg
${CMAKE_SOURCE_DIR}/resources/config/geometry/snemo/demonstrator/3.0/tracker_modules/pictures/tracker_horizontal_beam_inner.model.view_3d_0.jpeg
${CMAKE_SOURCE_DIR}/resources/config/geometry/snemo/demonstrator/3.0/tracker_modules/pictures/xwall_inner_module.fig
${CMAKE_SOURCE_DIR}/resources/config/geometry/snemo/demonstrator/3.0/tracker_modules/pictures/drift_cell_core.model.view_yz_0.jpeg
${CMAKE_SOURCE_DIR}/resources/config/geometry/snemo/demonstrator/3.0/tracker_modules/pictures/tracker_front_volume.model.view_yz_0.jpeg
${CMAKE_SOURCE_DIR}/resources/config/geometry/snemo/demonstrator/3.0/tracker_modules/pictures/gveto_module.model.view_xz_0.jpeg
${CMAKE_SOURCE_DIR}/resources/config/geometry/snemo/demonstrator/3.0/tracker_modules/pmt_5inch.geom
${CMAKE_SOURCE_DIR}/resources/config/geometry/snemo/demonstrator/3.0/tracker_modules/tracker_drift_cell_nodes.geom
${CMAKE_SOURCE_DIR}/resources/config/geometry/snemo/demonstrator/3.0/tracker_modules/tracker_drift_cell_base.geom
${CMAKE_SOURCE_DIR}/resources/config/geometry/snemo/demonstrator/3.0/tracker_modules/xwall.geom
${CMAKE_SOURCE_DIR}/resources/config/geometry/snemo/demonstrator/3.0/tracker_modules/tracker_submodules.geom
${CMAKE_SOURCE_DIR}/resources/config/geometry/snemo/demonstrator/3.0/tracker_modules/gveto.geom
${CMAKE_SOURCE_DIR}/resources/config/geometry/snemo/demonstrator/3.0/tracker_modules/README.rst
${CMAKE_SOURCE_DIR}/resources/config/geometry/snemo/demonstrator/3.0/tracker_modules/gveto_module.geom
${CMAKE_SOURCE_DIR}/resources/config/geometry/snemo/demonstrator/3.0/tracker_modules/tracker_layers.geom
${CMAKE_SOURCE_DIR}/resources/config/geometry/snemo/demonstrator/3.0/tracker_modules/xwall_module.geom
${CMAKE_SOURCE_DIR}/resources/config/geometry/snemo/common/materials/1.0/README.rst
${CMAKE_SOURCE_DIR}/resources/config/geometry/snemo/common/3.0/README.rst
${CMAKE_SOURCE_DIR}/resources/config/geometry/snemo/common/3.0/tracker_drift_cell_core.geom
${CMAKE_SOURCE_DIR}/resources/config/geometry/snemo/common/3.0/tracker_drift_cell.geom
${CMAKE_SOURCE_DIR}/resources/config/geometry/bipo3/2.0/tb_module.geom
${CMAKE_SOURCE_DIR}/resources/config/geometry/bipo3/2.0/shielding_5sce_top.geom
${CMAKE_SOURCE_DIR}/resources/config/geometry/bipo3/2.0/shielding_5sce_side_top.geom
${CMAKE_SOURCE_DIR}/resources/config/geometry/bipo3/2.0/source.geom
${CMAKE_SOURCE_DIR}/resources/config/geometry/bipo3/2.0/bipo3.geom
${CMAKE_SOURCE_DIR}/resources/config/geometry/bipo3/2.0/calibration_source.geom
${CMAKE_SOURCE_DIR}/resources/config/geometry/bipo3/2.0/shielding_5sce_side.geom
${CMAKE_SOURCE_DIR}/resources/config/geometry/bipo3/2.0/README.rst
${CMAKE_SOURCE_DIR}/resources/config/geometry/bipo3/2.0/shielding_5sce_3neg_2pos_side.geom
${CMAKE_SOURCE_DIR}/resources/config/geometry/bipo3/2.0/categories.lis
${CMAKE_SOURCE_DIR}/resources/config/geometry/bipo3/2.0/pmt_5inch.geom
${CMAKE_SOURCE_DIR}/resources/config/geometry/bipo3/2.0/manager.conf
${CMAKE_SOURCE_DIR}/resources/config/geometry/bipo3/2.0/optical_module.geom
${CMAKE_SOURCE_DIR}/resources/config/geometry/bipo3/2.0/README.rst
${CMAKE_SOURCE_DIR}/resources/config/geometry/bipo3/2.0/shielding.geom
${CMAKE_SOURCE_DIR}/resources/config/geometry/bipo3/2.0/setup.geom
${CMAKE_SOURCE_DIR}/resources/config/geometry/bipo3/2.0/materials.def
${CMAKE_SOURCE_DIR}/resources/config/geometry/common/pmt/README.rst
${CMAKE_SOURCE_DIR}/resources/config/geometry/common/pmt/hamamatsu_R6594/hamamatsu_R6594_shape.data
${CMAKE_SOURCE_DIR}/resources/config/geometry/common/pmt/hamamatsu_R6594/hamamatsu_R6594.jpg
${CMAKE_SOURCE_DIR}/resources/config/geometry/common/pmt/hamamatsu_R5912MOD/hamamatsu_R5912MOD_shape.data
${CMAKE_SOURCE_DIR}/resources/config/geometry/common/pmt/hamamatsu_R5912MOD/hamamatsu_R5912MOD.jpg
${CMAKE_SOURCE_DIR}/resources/config/geometry/common/materials/1.0/elements.def
${CMAKE_SOURCE_DIR}/resources/config/geometry/common/materials/1.0/README.rst
${CMAKE_SOURCE_DIR}/resources/config/geometry/common/materials/1.0/materials_aliases.def
${CMAKE_SOURCE_DIR}/resources/config/geometry/common/materials/1.0/materials.def
${CMAKE_SOURCE_DIR}/resources/config/simulation/snemo/tracker_commissioning/primary_events/1.0/manager.conf
${CMAKE_SOURCE_DIR}/resources/config/simulation/snemo/tracker_commissioning/vertexes/1.0/generators.lis
${CMAKE_SOURCE_DIR}/resources/config/simulation/snemo/tracker_commissioning/vertexes/1.0/manager.conf
${CMAKE_SOURCE_DIR}/resources/config/simulation/snemo/demonstrator/primary_events/1.0/manager.conf
${CMAKE_SOURCE_DIR}/resources/config/simulation/snemo/demonstrator/vertexes/3.0/scin_vg.lis
${CMAKE_SOURCE_DIR}/resources/config/simulation/snemo/demonstrator/vertexes/3.0/manager.conf
${CMAKE_SOURCE_DIR}/resources/config/simulation/snemo/demonstrator/vertexes/3.0/tracker_vg.lis
${CMAKE_SOURCE_DIR}/resources/config/simulation/snemo/demonstrator/vertexes/3.0/shield_vg.lis
${CMAKE_SOURCE_DIR}/resources/config/simulation/snemo/demonstrator/vertexes/3.0/hall_vg.lis
${CMAKE_SOURCE_DIR}/resources/config/simulation/snemo/demonstrator/vertexes/3.0/source_vg.lis
${CMAKE_SOURCE_DIR}/resources/config/simulation/snemo/demonstrator/vertexes/2.0/scin_vg.lis
${CMAKE_SOURCE_DIR}/resources/config/simulation/snemo/demonstrator/vertexes/2.0/manager.conf
${CMAKE_SOURCE_DIR}/resources/config/simulation/snemo/demonstrator/vertexes/2.0/tracker_vg.lis
${CMAKE_SOURCE_DIR}/resources/config/simulation/snemo/demonstrator/vertexes/2.0/shield_vg.lis
${CMAKE_SOURCE_DIR}/resources/config/simulation/snemo/demonstrator/vertexes/2.0/hall_vg.lis
${CMAKE_SOURCE_DIR}/resources/config/simulation/snemo/demonstrator/vertexes/2.0/source_vg.lis
${CMAKE_SOURCE_DIR}/resources/config/simulation/snemo/demonstrator/vertexes/1.0/manager_2.conf
${CMAKE_SOURCE_DIR}/resources/config/simulation/snemo/demonstrator/vertexes/1.0/scin_vg.lis
${CMAKE_SOURCE_DIR}/resources/config/simulation/snemo/demonstrator/vertexes/1.0/manager.conf
${CMAKE_SOURCE_DIR}/resources/config/simulation/snemo/demonstrator/vertexes/1.0/tracker_vg.lis
${CMAKE_SOURCE_DIR}/resources/config/simulation/snemo/demonstrator/vertexes/1.0/shield_vg.lis
${CMAKE_SOURCE_DIR}/resources/config/simulation/snemo/demonstrator/vertexes/1.0/hall_vg.lis
${CMAKE_SOURCE_DIR}/resources/config/simulation/snemo/demonstrator/vertexes/1.0/source_vg.lis
${CMAKE_SOURCE_DIR}/resources/config/simulation/bipo3/vertexes/1.0/generators.lis
${CMAKE_SOURCE_DIR}/resources/config/simulation/bipo3/vertexes/1.0/manager.conf
${CMAKE_SOURCE_DIR}/resources/README.rst

)

# - Publish resource files
foreach(_rfin ${_falaise_resources})
  string(REGEX REPLACE "\\.in$" "" _rfout "${_rfin}")
  string(REGEX REPLACE "^${CMAKE_SOURCE_DIR}/resources" "${Falaise_DATAROOTDIR}/resources" _rfout "${_rfout}")
  configure_file(${_rfin} ${_rfout} @ONLY)
endforeach()

#-----------------------------------------------------------------------
# Package Sources and Binaries
#
#include(FLCPack)
