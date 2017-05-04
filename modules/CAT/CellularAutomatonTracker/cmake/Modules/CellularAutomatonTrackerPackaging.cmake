# - HerewardPackaging
#
# This module provides CPack configuration for Hereward
#
#----------------------------------------------------------------------------
# Copyright 2011 Ben Morgan
# Copyright 2011 University of Warwick
#
#----------------------------------------------------------------------------
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.
#----------------------------------------------------------------------------

#----------------------------------------------------------------------------
# Package up needed system libraries - seems to only be needed on Windows
#
include(InstallRequiredSystemLibraries)

#----------------------------------------------------------------------------
# General packaging setup - variables relevant to all package formats
#
set(CPACK_PACKAGE_VERSION ${${PROJECT_NAME}_VERSION})
set(CPACK_PACKAGE_VERSION_MAJOR ${${PROJECT_NAME}_VERSION_MAJOR})
set(CPACK_PACKAGE_VERSION_MINOR ${${PROJECT_NAME}_VERSION_MINOR})
set(CPACK_PACKAGE_VERSION_PATCH ${${PROJECT_NAME}_VERSION_PATCH})

set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Hereward Software for SuperNEMO")
set(CPACK_PACKAGE_DESCRIPTION_FILE "${PROJECT_SOURCE_DIR}/README.txt")
set(CPACK_PACKAGE_VENDOR "University of Warwick")
set(CPACK_RESOURCE_FILE_README "${PROJECT_SOURCE_DIR}/README.txt")

# We don't put the license in yet because the GPL text has quotes which make
# sh barf. We need to fix that, but at present I don't want to edit the GPL!
#set(CPACK_RESOURCE_FILE_LICENSE "${PROJECT_SOURCE_DIR}/LICENSE.txt")

#----------------------------------------------------------------------------
# Source package settings
# Exclude VCS and standard temporary files from the source package.
# This is not perfected yet!
set(CPACK_SOURCE_IGNORE_FILES 
    ${CMAKE_BINARY_DIR}
    "~$"
    "/CVS/"
    "/.svn/"
    "/\\\\\\\\.svn/"
    "/.git/"
    "/\\\\\\\\.git/"
    "\\\\\\\\.swp$"
    "\\\\\\\\.swp$"
    "\\\\.swp"
    "\\\\\\\\.#"
    "/#"
)

# - Ensure all standard source packages are built
set(CPACK_SOURCE_GENERATOR "TGZ;TBZ2;ZIP")


#----------------------------------------------------------------------------
# Set name of CPACK_SYSTEM_NAME based on platform and architecture where 
# needed. We do this because we may want to produce packages for, say,
# 32 and 64 bit arches on linux.
#
# N.B. This is all UNTESTED for the cross-compile case!!!
#
if(NOT DEFINED CPACK_SYSTEM_NAME)
  # Cygwin is always Cygwin...
  if("${CMAKE_SYSTEM_NAME}" STREQUAL "CYGWIN")
    set(CPACK_SYSTEM_NAME Cygwin)
  else()
    if(UNIX AND NOT APPLE)
      # On Linux, try and find lsb_release
      find_program(LSB_RELEASE_PROGRAM lsb_release DOC "The Linux Standard Base system query tool")

      if(LSB_RELEASE_PROGRAM)
        # We have linux, so incorporate Vendor info into package name
        # - Distributor ID
        exec_program(${LSB_RELEASE_PROGRAM} ARGS -s -i OUTPUT_VARIABLE LSB_VENDOR)
        string(REGEX REPLACE " " "-" LSB_VENDOR ${LSB_VENDOR})
        string(TOLOWER ${LSB_VENDOR} LSB_VENDOR)

        # - Distributor release
        exec_program(${LSB_RELEASE_PROGRAM} ARGS -s -r OUTPUT_VARIABLE LSB_RELEASE)
        string(TOLOWER ${LSB_RELEASE} LSB_RELEASE)

        # Cache the vendor tag, because users might want to edit it
        set(LSB_VENDOR_TAG ${LSB_VENDOR}-${LSB_RELEASE} 
          CACHE STRING "LSB vendor tag for use in packaging")

        set(CPACK_SYSTEM_NAME
          ${CMAKE_SYSTEM_NAME}-${LSB_VENDOR_TAG}-${CMAKE_SYSTEM_PROCESSOR})
        mark_as_advanced(LSB_RELEASE_PROGRAM LSB_VENDOR_TAG)
      else()
        # Fallback to using NAME-ARCH on other UNICES other than Apple
        set(CPACK_SYSTEM_NAME ${CMAKE_SYSTEM_NAME}-${CMAKE_SYSTEM_PROCESSOR})
      endif()
    else()
      # On Mac, we use NAME-ARCH, but ARCH is 'Universal' if more than
      # one arch is built for. Note that falling back to use
      # CMAKE_SYSTEM_PROCESSOR may *not* be 100% reliable.
      list(LENGTH CMAKE_OSX_ARCHITECTURES _number_of_arches)

      if(NOT _number_of_arches)
        # - Default
        set(CPACK_SYSTEM_NAME ${CMAKE_SYSTEM_NAME}-${CMAKE_SYSTEM_PROCESSOR})
      elseif(_number_of_arches GREATER 1)
        # - Universal
        set(CPACK_SYSTEM_NAME ${CMAKE_SYSTEM_NAME}-Universal)
      else()
        # - Use what the user specified
        set(CPACK_SYSTEM_NAME ${CMAKE_SYSTEM_NAME}-${CMAKE_OSX_ARCHITECTURES})
      endif()
    endif()
  endif()
endif()

# On Windows, distinguish x64 and x86 - not sure this is really needed yet!!!
if("${CPACK_SYSTEM_NAME}" MATCHES Windows)
  if(CMAKE_CL_64)
    set(CPACK_SYSTEM_NAME win64-x64)
  else()
    set(CPACK_SYSTEM_NAME win32-x86)
  endif()
endif()

#----------------------------------------------------------------------------
# Finally, include the base CPack configuration
#
include(CPack)

