# - CMake build script for Falaise flsimulate app

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
#

#-----------------------------------------------------------------------
# Declare sources
#
set(flsimulate_SOURCES flsimulate.cc)

#-----------------------------------------------------------------------
# Build App
#
add_executable(flsimulate ${flsimulate_SOURCES})
target_link_libraries(flsimulate Falaise)

#-----------------------------------------------------------------------
# Build Tests
#

