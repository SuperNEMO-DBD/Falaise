# - CMake build script for Falaise flsimrc_timestamper app
# message(STATUS "==============> flsimrc_timestamper")

#-----------------------------------------------------------------------
# Copyright (c) 2022 by François Mauger <mauger@lpccaen.in2p3.fr>
# Copyright (c) 2022 by Université de Caen Normandie
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

#-----------------------------------------------------------------------
# Compile/Link App
#
add_executable(flsimrc-timestamper
  app_params.hpp
  app_params.cpp
  app_command_line.hpp
  app_command_line.cpp
  flsimrc_timestampermain.cxx
  )
target_include_directories(flsimrc-timestamper PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${CMAKE_CURRENT_BINARY_DIR}
  )
# target_compile_definitions(flsimrc-timestamper PRIVATE ENABLE_BINRELOC)
target_link_libraries(flsimrc-timestamper
  Falaise_SimRC
  Falaise
  Bayeux::Bayeux
  Boost::program_options
  )
target_clang_format(flsimrc-timestamper)

# - Ensure link to internal and external deps
set_target_properties(flsimrc-timestamper PROPERTIES INSTALL_RPATH_USE_LINK_PATH 1)

if(UNIX AND NOT APPLE)
  set_target_properties(flsimrc-timestamper
    PROPERTIES INSTALL_RPATH "\$ORIGIN/../${CMAKE_INSTALL_LIBDIR};\$ORIGIN/../${CMAKE_INSTALL_PLUGINDIR}"
    )
elseif(APPLE)
  # Temporary setting - needs testing
  set_target_properties(flsimrc-timestamper
    PROPERTIES
    INSTALL_RPATH "@loader_path/../${CMAKE_INSTALL_LIBDIR};@loader_path/../${CMAKE_INSTALL_PLUGINDIR}"
    )
endif()

# - Install it
install(TARGETS flsimrc-timestamper
  EXPORT FalaiseTargets
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
  )

if(FALAISE_ENABLE_TESTING)
  set(_SimRCTimestamperTestConfFiles
    "activity_model_factory.conf"
    "activity_models.conf"
    )
  
  foreach(_file ${_SimRCTimestamperTestConfFiles})
    configure_file("${CMAKE_CURRENT_SOURCE_DIR}/${_file}" "${CMAKE_CURRENT_BINARY_DIR}/${_file}" COPYONLY)
  endforeach()

  add_test(NAME testSimRC_timestamper_regular
    COMMAND flsimrc-timestamper
    -V debug
    -n 1000
    -o timestamps-regular.lis
    -g "regular"
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    ) 

  add_test(NAME testSimRC_timestamper_random
    COMMAND flsimrc-timestamper
    -V debug
    -n 1000
    -o timestamps-random.lis
    -g "random"
    -s 314159
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    ) 

  add_test(NAME testSimRC_timestamper_random_exp
    COMMAND flsimrc-timestamper
    -V debug
    -n 10000
    -f "activity_model_factory.conf"
    -a "decaying-from-2023-01-01"
    -o "timestamps-random-exp.lis"
    -q "times-random-exp.lis"
    -g "random"
    -s 314159
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    ) 

  # add_test(NAME testSimRC_timestamper_random_exp_post
  #   COMMAND bash ${CMAKE_CURRENT_SOURCE_DIR}/test.bash
  #   WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
  #   ) 

endif(FALAISE_ENABLE_TESTING)

# - end
