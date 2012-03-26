# - Functions and Macros for implemeting common testing tasks
#
#----------------------------------------------------------------------------
# Copyright 2011 Ben Morgan <Ben.Morgan@warwick.ac.uk>
# Copyright 2011 University of Warwick
# All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met:
#     * Redistributions of source code must retain the above copyright
#       notice, this list of conditions and the following disclaimer.
#     * Redistributions in binary form must reproduce the above copyright
#       notice, this list of conditions and the following disclaimer in the
#       documentation and/or other materials provided with the distribution.
#    *  Neither the name of the University of Warwick nor the
#       names of its contributors may be used to endorse or promote products
#       derived from this software without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
# ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
# WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
# DISCLAIMED. IN NO EVENT SHALL THE UNIVERSITY OF WARWICK BE LIABLE FOR ANY
# DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
# (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
# LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
# ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
# (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
# SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#----------------------------------------------------------------------------

#----------------------------------------------------------------------------
# Locate ourselves??
#
get_filename_component(__TestingFrameworkModuleDir 
  ${CMAKE_CURRENT_LIST_FILE}
  PATH
  )


#----------------------------------------------------------------------------
# Setup custom command for cpplint based tests
#
macro(enable_cpplint_testing)
  # cpplint needs Python
  find_package(PythonInterp QUIET)
  if(PYTHONINTERP_FOUND)
    # Then we set up a command for running cpplint...
    # Locate cpplint relative to our parent module.
    set(_cpplint_script ${__TestingFrameworkModuleDir}/cpplint.py)

    if(EXISTS ${_cpplint_script})
      set(CPPLINT_FOUND 1)
      set(CPPLINT_COMMAND ${PYTHON_EXECUTABLE} ${_cpplint_script})
    else()
      message(FATAL_ERROR "Could not locate cpplint.py.
It *should* be at 
${_cpplint_script}
but it does not appear to exist there."
        )
    endif()
  else()
    message(FATAL_ERROR "cpplint cannot be used because Python was not found")
  endif()
endmacro()











