# - CMake build script for Falaise fltags app

#-----------------------------------------------------------------------
# Copyright 2018 François Mauger <mauger@lpccaen.in2p3.fr>
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
find_package(Boost 1.60 REQUIRED program_options filesystem system)

#-----------------------------------------------------------------------
# Build
add_executable(fltags
  fltagsmain.cc
  FLTagsArgs.h
  FLTagsArgs.cc
  FLTagsCommandLine.h
  FLTagsCommandLine.cc
  FLTagsErrors.h
  FLTagsErrors.cc
  FLTagsUtils.h
  FLTagsUtils.cc
  FLTags.h
  FLTags.cc
  )
target_include_directories(fltags PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${CMAKE_CURRENT_BINARY_DIR}
  )
target_link_libraries(fltags
  Falaise
  Bayeux::Bayeux
  ${Boost_LIBRARIES}
  )
target_clang_format(fltags)

# - Ensure link to internal and external deps
set_target_properties(fltags PROPERTIES INSTALL_RPATH_USE_LINK_PATH 1)

if(UNIX AND NOT APPLE)
  set_target_properties(fltags
    PROPERTIES INSTALL_RPATH "\$ORIGIN/../${CMAKE_INSTALL_LIBDIR}"
    )
elseif(APPLE)
  # Temporary setting - needs testing
  set_target_properties(fltags
    PROPERTIES
      INSTALL_RPATH "@loader_path/../${CMAKE_INSTALL_LIBDIR}"
    )
endif()

# - Install
# TODO : fixup rpaths as needed
install(TARGETS fltags
  EXPORT FalaiseTargets
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
  )

# # Testing, if enabled
# if(FALAISE_ENABLE_TESTING)
#   add_subdirectory(tests)
# endif()

# - end
