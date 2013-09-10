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
find_package(Boost REQUIRED program_options)

include_directories(${Boost_INCLUDE_DIRS})

#-----------------------------------------------------------------------
# Declare sources
#
set(flreconstruct_SOURCES flreconstruct.cc)

#-----------------------------------------------------------------------
# Build App
#
add_executable(flreconstruct ${flreconstruct_SOURCES})
target_link_libraries(flreconstruct 
  Falaise
  Bayeux 
  ${Boost_program_options_LIBRARY}
  )

#-----------------------------------------------------------------------
# Build Tests
#

