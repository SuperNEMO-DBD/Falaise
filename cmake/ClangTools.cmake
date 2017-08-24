#.rst:
# ClangTools
# ----------
#
# Functions for working with the `clang-format` and `clang-tidy` tools for
# source code formatting and static analysis of CMake targets.
#

#-----------------------------------------------------------------------
# Copyright (c) 2017, Ben Morgan <Ben.Morgan@warwick.ac.uk>
# Copyright (c) 2017, University of Warwick
# All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met:
#
# * Redistributions of source code must retain the above copyright notice,
#  this list of conditions and the following disclaimer.
#
# * Redistributions in binary form must reproduce the above copyright notice,
#   this list of conditions and the following disclaimer in the documentation
#   and/or other materials provided with the distribution.
#
# Neither the name of Ben Morgan, the University of Warwick, nor the names of its
# contributors may be used to endorse or promote products derived from this
# software without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
# AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
# WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
# IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
# INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
# BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
# DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
# OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
# NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
# EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.


#-----------------------------------------------------------------------
# "Header guard"
if(__CLANGTOOLS_INCLUDED)
  return()
else()
  set(__CLANGTOOLS_INCLUDED 1)
endif()


#[[.rst:
.. cmake:command:: target_clang_format

  .. code-block:: cmake

    target_clangformat(<target>)

  :cmake:command:`Function <cmake:command:function>` that creates a new target named
  ``<target>_clang_format`` that runs the ``clang-format`` tool over the source code files
  of ``<target>``. The target is only created if the ``clang-format`` tool is present,
  and ``<target>`` must be a valid CMake target. If ``<target>_clang_format`` is created,
  it is added as a dependency to a global ``clang_format`` target which can be invoked
  to format all targets configured by ``target_clang_format``.

  The run of ``clang-format`` uses the ``file`` style to use any ``.clang-format`` file
  found in the source tree. If no file is found, it falls back to the Google style.
  Files are formatted in-place, so invoking the target will result in the source
  code be changed but should not result in any functionality change. Nevertheless,
  a rebuild and retest of the project after a formatting change is recommended.

  Any files in the target ending in ``.in`` are ignored. These are assumed to be
  templates for generated sources, and thus may have placeholder text that
  ``clang-format`` cannot parse.

  This function is intended to help less experienced developers with applying
  a coding style, but use of ``clang-format`` in editors and IDEs is strongly
  preferred.

  The implementation of ``target_clangformat`` is based on that documented here:
  https://www.linkedin.com/pulse/simple-elegant-wrong-how-integrate-clang-format-friends-brendan-drew
#]]

find_program(clangformat_EXECUTABLE
  NAMES clang-format
  DOC "Path to the clang-format executable"
  )

function(target_clang_format _targetname)
  if(clangformat_EXECUTABLE)
    if(NOT TARGET ${_targetname})
      message(FATAL_ERROR "target_clang_format should only be called on targets (got " ${_targetname} ")")
    endif ()

    # figure out which sources this should be applied to
    get_target_property(_clang_sources ${_targetname} SOURCES)
    get_target_property(_builddir ${_targetname} BINARY_DIR)

    # [TEMP] Filter out any "*.in" files because these are templates
    # and may have characters/strings that are incorrectly formatted.
    # It's TEMP, because strictly speaking the template file shouldn't
    # appear in the source list, only the generated file should.
    string(REGEX REPLACE ";?[^;]*\\.in;?" ";" _clang_sources "${_clang_sources}")
    string(REGEX REPLACE "^;|;$" "" _clang_sources "${_clang_sources}")

    set(_sources "")
    foreach(_source ${_clang_sources})
      if(NOT TARGET ${_source})
        get_filename_component(_source_file ${_source} NAME)
        get_source_file_property(_clang_loc "${_source}" LOCATION)

        set(_format_file ${_targetname}_${_source_file}.format)

        add_custom_command(OUTPUT ${_format_file}
          DEPENDS ${_source}
          COMMENT "Clang-Format ${_source}"
          COMMAND ${clangformat_EXECUTABLE} -style=file -fallback-style=Google -i ${_clang_loc}
          COMMAND ${CMAKE_COMMAND} -E touch ${_format_file}
          )

        list(APPEND _sources ${_format_file})
      endif()
    endforeach()

    if(_sources)
      add_custom_target(${_targetname}_clang_format
        SOURCES ${_sources}
        COMMENT "Clang-Format for target ${_target}"
        )

      # Rather than make target depend on <targetname>_clangformat
      # target, create global always-out-of-date target "clangformat"
      # Add each <targetname>_clangformat as a dependency of this.
      # Allows all formatting to be done in one build, but will only
      # rerun formatting on modified files!
      if(NOT TARGET clang_format)
        add_custom_target(clang_format COMMENT "Clang-Format ${PROJECT_NAME}")
      endif()
      add_dependencies(clang_format ${_targetname}_clang_format)
    endif ()
  endif()
endfunction()


# - Clang tidy directly supported by CMake > 3.6, but must
#   still find and configure it.
# See http://www.mariobadr.com/using-clang-tidy-with-cmake-36.html
# for a quick overview
find_program(clangtidy_EXECUTABLE
  NAMES "clang-tidy"
  DOC "Path to the clang-tidy executable"
 )

