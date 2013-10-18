# - CMake script for CommissioningIO data
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
# Build Tests
#
set(falaise_io_SOURCES
    DriftModel.cc
    GeigerEvent.cc
    RawGeigerHit.cc
    rawkeys.cc
    Unpack.cc
    UnpackingErrors.cc
    )

add_library(falaise_io STATIC ${falaise_io_SOURCES})
add_executable(testCommissioningIO testCommissioningIO.cc)
target_link_libraries(testCommissioningIO falaise_io)

