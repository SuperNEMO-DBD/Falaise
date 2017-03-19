# - CMake build script for Falaise flreconstruct app

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
# Configure application
# - Bit hacky for now
find_package(Boost 1.60 REQUIRED program_options)

# #-----------------------------------------------------------------------
# # Compile/Link App
# #
# add_executable(flreconstruct_legacy
#   legacy/flreconstructmain_legacy.cc
#   FLReconstructResources.h
#   FLReconstructResources.cc
# )
# target_include_directories(flreconstruct_legacy PRIVATE
#   ${CMAKE_CURRENT_SOURCE_DIR}
#   ${CMAKE_CURRENT_BINARY_DIR}
#   )
# target_compile_definitions(flreconstruct_legacy PRIVATE ENABLE_BINRELOC)
# target_link_libraries(flreconstruct_legacy
#   Falaise
#   Bayeux::Bayeux
#   Boost::program_options
#   )

# # - Ensure link to internal and external deps
# set_target_properties(flreconstruct_legacy PROPERTIES INSTALL_RPATH_USE_LINK_PATH 1)

# if(UNIX AND NOT APPLE)
#   set_target_properties(flreconstruct_legacy
#     PROPERTIES INSTALL_RPATH "\$ORIGIN/../${CMAKE_INSTALL_LIBDIR}"
#     )
# elseif(APPLE)
#   # Temporary setting - needs testing
#   set_target_properties(flreconstruct_legacy
#     PROPERTIES
#       INSTALL_RPATH "@loader_path/../${CMAKE_INSTALL_LIBDIR}"
#       )
# endif()

# # - Install it
# install(TARGETS flreconstruct_legacy
#   EXPORT FalaiseTargets
#   RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
#   )

#-----------------------------------------------------------------------
# Compile/Link App
#
add_executable(flreconstruct
  flreconstructmain.cc
  FLReconstructPipeline.h
  FLReconstructPipeline.cc
  FLReconstructImpl.h
  FLReconstructImpl.cc
  FLReconstructResources.h
  FLReconstructResources.cc
  FLReconstructParams.h
  FLReconstructParams.cc
  FLReconstructCommandLine.h
  FLReconstructCommandLine.cc
  FLReconstructErrors.h
  FLReconstructErrors.cc
  FLReconstructUtils.h
  FLReconstructUtils.cc
)
target_include_directories(flreconstruct PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${CMAKE_CURRENT_BINARY_DIR}
  )
target_compile_definitions(flreconstruct PRIVATE ENABLE_BINRELOC)
target_link_libraries(flreconstruct
  Falaise
  Bayeux::Bayeux
  Boost::program_options
  )

# - Ensure link to internal and external deps
set_target_properties(flreconstruct PROPERTIES INSTALL_RPATH_USE_LINK_PATH 1)

if(UNIX AND NOT APPLE)
  set_target_properties(flreconstruct
    PROPERTIES INSTALL_RPATH "\$ORIGIN/../${CMAKE_INSTALL_LIBDIR}"
    )
elseif(APPLE)
  # Temporary setting - needs testing
  set_target_properties(flreconstruct
    PROPERTIES
      INSTALL_RPATH "@loader_path/../${CMAKE_INSTALL_LIBDIR}"
      )
endif()

# - Install it
install(TARGETS flreconstruct
  EXPORT FalaiseTargets
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
  )

#-----------------------------------------------------------------------
# Build Tests
#
#add_subdirectory(CommissioningIO)
if(FALAISE_ENABLE_TESTING)
  add_executable(testFhiclProperties testFhiclProperties.cc)
  set_target_properties(testFhiclProperties
    PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/fltests
    ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/fltests
    )
  target_link_libraries(testFhiclProperties FLCatch Bayeux::Bayeux Boost::boost)
  add_test(NAME falaise-testFhiclProperties COMMAND testFhiclProperties)
endif()
