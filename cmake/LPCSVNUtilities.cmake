#.rst:
# LPCSVNUtilities
# ---------------
#
# Nothing more than a simple wrapper around CMake's FindSubversion
#

#-----------------------------------------------------------------------
# Copyright 2012 Ben Morgan <bmorgan.warwick@gmail.com>
# Copyright 2012 University of Warwick
#
# Distributed under the OSI-approved BSD 3-Clause License (the "License");
# see accompanying file BSD3_LICENSE.txt for details.
#
# This software is distributed WITHOUT ANY WARRANTY; without even the
# implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
# See the License for more information.
#-----------------------------------------------------------------------

# - Include Guard
if(__svn_utilities_isloaded)
  return()
endif()

# - Basic Subversion check
find_package(Subversion QUIET)

#=======================================================================
# SVNUtilities API
#=======================================================================
#-----------------------------------------------------------------------
# function
function(Subversion_DIRECTORY_IS_WC _dir _prefix)
  if(NOT SUBVERSION_FOUND)
    # We almost certainly don't have a Working Copy!!
    set(${_prefix}_DIRECTORY_IS_WC NO PARENT_SCOPE)
    return()
  endif()

  # - Otherwise, use info command in supplied directory
  # Execute in C locale to prevent translation issues
  set(_Subversion_SAVED_LC_ALL "$ENV{LC_ALL}")
  set(ENV{LC_ALL} C)

  execute_process(COMMAND ${Subversion_SVN_EXECUTABLE} info ${_dir}
    OUTPUT_VARIABLE _svn_dir_is_wc
    ERROR_VARIABLE _svn_dir_is_wc_error
    RESULT_VARIABLE _svn_dir_is_wc_result
    OUTPUT_STRIP_TRAILING_WHITESPACE)

  # restore the previous LC_ALL
  set(ENV{LC_ALL} ${_Subversion_SAVED_LC_ALL})

  if("${_svn_dir_is_wc_result}" STREQUAL "1")
    set(${_prefix}_DIRECTORY_IS_WC NO PARENT_SCOPE)
    return()
  endif()

  set(${_prefix}_DIRECTORY_IS_WC YES PARENT_SCOPE)
endfunction()

# Mark that we've loaded o.k.
set(__svn_utilities_isloaded YES)
