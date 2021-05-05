#.rst:
# LPCCMakeSettings
# ---------------
#
# Common and useful CMake settings for LPC C/C++ projects.
#
# Variable Naming Convention
# ^^^^^^^^^^^^^^^^^^^^^^^^^^
#
# Variables that the user of a project may set or otherwise interact with
# are named using the pattern ``<PROJECT_NAME Upper Cased>_<VARNAME>``.
# This is for consistency with standard CMake client interfaces.
#
# Variables that are internal to the project, i.e. for use only by authors
# of the build scripts are named ``PROJECT_VARNAME``. A copy of this value
# is also stored in the variable ``<PROJECT_NAME_UC>_VARNAME``.
#
# Variables exported from the project for use by clients, e.g. in CMake
# -config files, are, and should be, named ``<PROJECT_NAME>_<VARNAME>``.
# This provides a consistent convention for users of the project when
# finding it via ``find_package`` and others.
#
# At present, subprojects are not supported as this module can only
# be included once in any CMake directory hierarchy. Variables thus
# only refer to the highest level project. This is o.k. for all
# current LPC projects.
#
# Output directories and relative paths also only support single mode
# generators such as Make and Ninja.
#
# Global CMake behaviour
# ^^^^^^^^^^^^^^^^^^^^^^
#
# The following CMake variables are set when including this module
#
# - ``CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION`` : ON
#
#   - Disallow absolute paths in install destinations, other than
#     ``CMAKE_INSTALl_PREFIX``. This helps in making packages fully
#     relocatable
#
# - ``CMAKE_EXPORT_NO_PACKAGE_REGISTRY`` : ON
# - ``CMAKE_FIND_PACKAGE_NO_PACKAGE_REGISTRY`` : ON
# - ``CMAKE_FIND_PACKAGE_NO_SYSTEM_PACKAGE_REGISTRY`` : ON
#
#   - These settings ensure CMake will not export to or look in the
#     system package repository for dependencies.
#
# - ``CMAKE_INCLUDE_DIRECTORIES_PROJECT_BEFORE`` : ON
#
#   - Force project directories to appear first in any list of include paths.
#     This applies to both full paths and those created by generator expressions.
#
# - ``CMAKE_LINK_DEPENDS_NO_SHARED`` : ON
#
#   - Do not relink a target to any shared library dependencies when
#     only the shared library implementation has changed.
#
#
# - ``CMAKE_INSTALL_MESSAGE`` : ``LAZY``
#
#   - Only report new or updated files installed by the ``install`` target.
#
# Build/Install directory hierarchies
# ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
#
# By default the builting ``GNUInstallDirs`` module is included to provide
# a base set of standard install paths. This module adds the following
# variables to this set
#
# - ``CMAKE_INSTALL_CMAKEDIR``
#
#   - Base directory for CMake config files (defaults to ``CMAKE_INSTALL_LIBDIR/cmake``)
#
# - ``CMAKE_INSTALL_PLUGINDIR``
#
#   - Base directory for plugin (shared library) extensions (defaults to
#     ``CMAKE_INSTALL_LIBDIR/<PROJECT_NAME>/modules``)
#
# To help projects build in a consistent, relocatable manner, this module
# sets a sequence of variables for placing build products into a directory
# hierarchy matching that of the final install.
#
# - ``PROJECT_TAG`` : (PROJECT_NAME-PROJECT_VERSION``)
#
#   - Standard Name-Version tag for a project
#
# - ``PROJECT_RESOURCE_DIR`` : (``PROJECT_TAG/resources``)
#
#   - LPC standard relative path for resources
#
# - ``PROJECT_BUILD_PREFIX`` (``PROJECT_BINARY_DIR/BuildProducts``)
#
#   - Base directory for products, build time equivalent to ``CMAKE_PREFIX_PATH``
#
# - ``PROJECT_BUILD_DATAROOTDIR`` (``PROJECT_BUILDPRODUCT_DIR/share/PROJECT_TAG``)
#
#   - Base directory for shared architecture independent files
#
# - ``CMAKE_ARCHIVE_OUTPUT_DIRECTORY`` : (``PROJECT_BUILD_PREFIX/CMAKE_INSTALL_LIBDIR``)
#
#   - Default output directory for build products created with ``add_library(... STATIC ...)``
#
# - ``CMAKE_RUNTIME_OUTPUT_DIRECTORY`` : (``PROJECT_BUILD_PREFIX/CMAKE_INSTALL_BINDIR``)
#
#   - Default output directory for build products created with ``add_executable(...)``
#
# - ``CMAKE_LIBRARY_OUTPUT_DIRECTORY`` : (``PROJECT_BUILD_PREFIX/CMAKE_INSTALL_LIBDIR``)
#
#   - Default output directory for build products created with ``add_library(... SHARED ...)``
#
# To help with relocatability, a sequence of variables is provided listing relative
# paths from one build/install location to another. These may be encoded into binaries
# to, e.g., help a program locate a needed resource file at runtime without a hardcoded
# absolute path or requirement to set an environment variable.
#
# - ``PROJECT_BINDIR_TO_RESOURCEDIR``
# - ``PROJECT_BINDIR_TO_PLUGINLIBDIR``
# - ``PROJECT_LIBDIR_TO_PREFIX``
# - ``PROJECT_LIBDIR_TO_BINDIR``
# - ``PROJECT_LIBDIR_TO_RESOURCEDIR``
# - ``PROJECT_LIBDIR_TO_DATAROOTDIR``
#
# Core Compiler Flags
# ^^^^^^^^^^^^^^^^^^^
#
# LPC projects support GNU and Clang compilers directly, with the following set of
# default flags:
#
# - GNU/Clang : ``-Wall -Wextra -Wshadow -pedantic``
# - Clang : ``-Wno-c99-extensions -ftemplate-depth=512``
#
# The user may select the escalation of warnings into errors using the option
#
# - ``<PROJECT_NAME_UC>_COMPILER_ERROR_ON_WARNING`` : (default ``ON``)
#
#   - Escalates compiler warnings to errors and attemptes to fail at first error.
#
#
# C++ Standard Selection
# ^^^^^^^^^^^^^^^^^^^^^^
#
# LPC projects require support for C++11 at minimum, but users may optionally
# compile against newer standards for compatibility. Including this module
# adds a CMake option to select the standard:
#
# - ``<PROJECT_NAME_UC>_CXX_STANDARD`` : (default : 11)
#
#   - Compile projects against this C++ Standard. Allowed values are
#     ``11`` (default), ``14``, ``17`` (CMake > 3.7) , and ``20`` (CMake > 3.11).
#

#-----------------------------------------------------------------------
# "Header guard"
if(__LPCCCMAKESETTINGS_INCLUDED)
  return()
else()
  set(__LPCCCMAKESETTINGS_INCLUDED 1)
endif()

#-----------------------------------------------------------------------
# Include needed modules
#
include(LPCCMakeUtilities)

# Define Uppercased project name for use in
string(TOUPPER "${PROJECT_NAME}" PROJECT_NAME_UC)
set(PROJECT_NAME_UC "${PROJECT_NAME_UC}" CACHE INTERNAL "Internal uppercased project name")

#-----------------------------------------------------------------------
# Global CMake/target properties
#
# - Though we check for some absolute paths, ensure there are no others
set(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION ON)

# - Never export to or seach in user/system package registry
set(CMAKE_EXPORT_NO_PACKAGE_REGISTRY ON)
set(CMAKE_FIND_PACKAGE_NO_PACKAGE_REGISTRY ON)
set(CMAKE_FIND_PACKAGE_NO_SYSTEM_PACKAGE_REGISTRY ON)

# - Force project directories to appear first in any list of includes
set(CMAKE_INCLUDE_DIRECTORIES_PROJECT_BEFORE ON)

# - Only relink shared libs when interface changes
set(CMAKE_LINK_DEPENDS_NO_SHARED ON)

# - Only report newly installed files
set(CMAKE_INSTALL_MESSAGE LAZY)

#-----------------------------------------------------------------------
# - Standard UNIX Tool install paths, including relative paths for use
# by applications requiring resources
include(GNUInstallDirs)

# - Validate that certain paths are relative, otherwise relocation may fail
foreach(_dir BINDIR LIBDIR INCLUDEDIR DATAROOTDIR)
  if(IS_ABSOLUTE "${CMAKE_INSTALL_${_dir}}")
    message(FATAL_ERROR "Absolute path for CMAKE_INSTALL_${_dir} not allowed")
  endif()
endforeach()

# Add a path for CMake config files (immutable)
set(CMAKE_INSTALL_CMAKEDIR "${CMAKE_INSTALL_LIBDIR}/cmake")
set(CMAKE_INSTALL_FULL_CMAKEDIR "${CMAKE_INSTALL_FULL_LIBDIR}/cmake")

# Add a path for plugins (immutable)
set(CMAKE_INSTALL_PLUGINDIR "${CMAKE_INSTALL_LIBDIR}/${PROJECT_NAME}/modules")
set(CMAKE_INSTALL_FULL_PLUGINDIR "${CMAKE_INSTALL_FULL_LIBDIR}/${PROJECT_NAME}/modules")

#-----------------------------------------------------------------------
# - Configure output paths for products to give inplace build that roughly
#   matches install layout.
# - <PROJECT> Set
set(PROJECT_BUILD_PREFIX "${PROJECT_BINARY_DIR}/BuildProducts")
set(PROJECT_TAG "${PROJECT_NAME}-${PROJECT_VERSION}")
foreach(_path ITEMS BINDIR LIBDIR INCLUDEDIR DATAROOTDIR CMAKEDIR PLUGINDIR)
  set(PROJECT_BUILD_${_path} "${PROJECT_BUILD_PREFIX}/${CMAKE_INSTALL_${_path}}")
endforeach()

# - Resources subdirectories based on name-version subdirs
set(PROJECT_RESOURCE_DIR      "${PROJECT_TAG}/resources")
set(PROJECT_EXAMPLE_DIR       "${PROJECT_TAG}/examples")
set(PROJECT_DOCUMENTATION_DIR "${PROJECT_TAG}/Documentation")

# - Note that at present, this layout only supports single mode
# generators like make/ninja...
file(RELATIVE_PATH PROJECT_BINDIR_TO_DATADIR     "${CMAKE_INSTALL_FULL_BINDIR}" "${CMAKE_INSTALL_FULL_DATAROOTDIR}")
file(RELATIVE_PATH PROJECT_BINDIR_TO_RESOURCEDIR "${CMAKE_INSTALL_FULL_BINDIR}" "${CMAKE_INSTALL_FULL_DATAROOTDIR}/${PROJECT_RESOURCE_DIR}")
file(RELATIVE_PATH PROJECT_BINDIR_TO_PLUGINDIR   "${CMAKE_INSTALL_FULL_BINDIR}" "${CMAKE_INSTALL_FULL_PLUGINDIR}")

file(RELATIVE_PATH PROJECT_LIBDIR_TO_PREFIX      "${CMAKE_INSTALL_FULL_LIBDIR}" "${CMAKE_INSTALL_PREFIX}")
file(RELATIVE_PATH PROJECT_LIBDIR_TO_BINDIR      "${CMAKE_INSTALL_FULL_LIBDIR}" "${CMAKE_INSTALL_FULL_BINDIR}")
file(RELATIVE_PATH PROJECT_LIBDIR_TO_PLUGINDIR   "${CMAKE_INSTALL_FULL_LIBDIR}" "${CMAKE_INSTALL_FULL_PLUGINDIR}")
file(RELATIVE_PATH PROJECT_LIBDIR_TO_DATADIR     "${CMAKE_INSTALL_FULL_LIBDIR}" "${CMAKE_INSTALL_FULL_DATAROOTDIR}")
file(RELATIVE_PATH PROJECT_LIBDIR_TO_DATAROOTDIR "${CMAKE_INSTALL_FULL_LIBDIR}" "${CMAKE_INSTALL_FULL_DATAROOTDIR}/${PROJECT_TAG}")
file(RELATIVE_PATH PROJECT_LIBDIR_TO_RESOURCEDIR "${CMAKE_INSTALL_FULL_LIBDIR}" "${CMAKE_INSTALL_FULL_DATAROOTDIR}/${PROJECT_RESOURCE_DIR}")
file(RELATIVE_PATH PROJECT_CMAKEDIR_TO_INCLUDEDIR "${CMAKE_INSTALL_FULL_CMAKEDIR}/${PROJECT_TAG}"  "${CMAKE_INSTALL_FULL_INCLUDEDIR}")

# <PROJECT_NAME_UC> Set
foreach(_varname ITEMS
    BUILD_PREFIX
    BUILD_BINDIR
    BUILD_LIBDIR
    BUILD_INCLUDEDIR
    BUILD_DATAROOTDIR
    BUILD_CMAKEDIR
    BUILD_PLUGINDIR
    TAG
    RESOURCE_DIR
    EXAMPLE_DIR
    DOCUMENTATION_DIR
    BINDIR_TO_RESOURCEDIR
    BINDIR_TO_PLUGINDIR
    LIBDIR_TO_PREFIX
    LIBDIR_TO_BINDIR
    LIBDIR_TO_PLUGINDIR
    LIBDIR_TO_RESOURCEDIR
    LIBDIR_TO_DATAROOTDIR
    CMAKEDIR_TO_INCLUDEDIR
    )
  set(${PROJECT_NAME_UC}_${_varname} "${PROJECT_${_varname}}")
endforeach()

#-----------------------------------------------------------------------
# Set the default output directories for archive, runtime and library targets
#
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${PROJECT_BUILD_LIBDIR}")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BUILD_BINDIR}")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${PROJECT_BUILD_LIBDIR}")

#-----------------------------------------------------------------------
# If the user specifies -DCMAKE_BUILD_TYPE on the command line, take their definition
# and dump it in the cache along with proper documentation, otherwise set CMAKE_BUILD_TYPE
# to RelWithDebInfo
#
if(NOT CMAKE_CONFIGURATION_TYPES)
  if(NOT CMAKE_BUILD_TYPE)
    # Default to a RelWithDebInfo build if nothing else...
    set(CMAKE_BUILD_TYPE RelWithDebInfo
      CACHE STRING "Choose the type of build, options are: None Release MinSizeRel Debug RelWithDebInfo MinSizeRel."
      FORCE
      )
  else()
    # Force to the cache, but use existing value.
    set(CMAKE_BUILD_TYPE "${CMAKE_BUILD_TYPE}"
      CACHE STRING "Choose the type of build, options are: None Release MinSizeRel Debug RelWithDebInfo MinSizeRel."
      FORCE
      )
  endif()
endif()

#-----------------------------------------------------------------------
# Compiler/Linker configuration
# - Enforce all warnings, but ensure other flags are retained
# Note that the added flags *won't* show up in the cache.
# TODO : better way to do this with the "Make Override" system?
option(${PROJECT_NAME_UC}_COMPILER_ERROR_ON_WARNING "Turn warnings into errors" ON)
mark_as_advanced(${PROJECT_NAME_UC}_COMPILER_ERROR_ON_WARNING)

if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|(Apple)+Clang")
  set(PROJECT_CXX_FLAGS "-Wall -Wextra -Wshadow -pedantic -Wimplicit-fallthrough=0")

  # Disable specific warnings on GCC (-Wlong-long/pedantic) because Root source code (Rtypes.h)
  # makes use of unsupported type by ISO C++ 98 ([unsigned] long long int) and
  # the 'pragma GCC diagnostic ignored "-Wlong-long"' does not work
  if(CMAKE_CXX_COMPILER_ID MATCHES "GNU")
    set(PROJECT_CXX_FLAGS "${PROJECT_CXX_FLAGS} -Wno-long-long")
  endif()

  # Disable C99 extension warnings on Clang
  if(CMAKE_CXX_COMPILER_ID MATCHES "(Apple)+Clang")
    set(PROJECT_CXX_FLAGS "${PROJECT_CXX_FLAGS} -Wno-c99-extensions")
  endif()

  if(${PROJECT_NAME_UC}_COMPILER_ERROR_ON_WARNING)
    # Use fatal-errors to fall over at first sign of trouble
    set(PROJECT_CXX_FLAGS "${PROJECT_CXX_FLAGS} -Werror -Wfatal-errors")
  endif()

  # On clang, need larger template depth
  if(CMAKE_CXX_COMPILER_ID MATCHES "(Apple)+Clang")
    set(PROJECT_CXX_FLAGS "${PROJECT_CXX_FLAGS} -ftemplate-depth=512")
  endif()

elseif(CMAKE_CXX_COMPILER_ID MATCHES "Intel")
  # Needs more testing
  set(PROJECT_CXX_FLAGS "-w2")
endif()

# - Prepend package's flags to any preexisting ones, allowing user
#   to supply additional ones or override if required
set(CMAKE_CXX_FLAGS "${PROJECT_CXX_FLAGS} ${CMAKE_CXX_FLAGS}")

# - On OS X, default exe/shared linking to dynamic lookup to match GNU
#   linker behaviour
if(APPLE)
  set(CMAKE_SHARED_LINKER_FLAGS "-undefined dynamic_lookup ${CMAKE_SHARED_LINKER_FLAGS}")
  set(CMAKE_EXE_LINKER_FLAGS "-undefined dynamic_lookup ${CMAKE_EXE_LINKER_FLAGS}")
endif()

# - Could set RPATHs here, but difficult to pass genexps in variables,
#   and we need that to handle exe/lib differences

#-----------------------------------------------------------------------
# C++ Standard support
# - Do not want extensions
set(CMAKE_CXX_EXTENSIONS OFF)

# - Allow choice of standard
set(BX_CXXSTD_VALUES "11" "14")
if(CMAKE_VERSION VERSION_GREATER 3.7)
  list(APPEND BX_CXXSTD_VALUES "17")
endif()
if(CMAKE_VERSION VERSION_GREATER 3.11)
  list(APPEND BX_CXXSTD_VALUES "20")
endif()

enum_option(${PROJECT_NAME_UC}_CXX_STANDARD
  VALUES ${BX_CXXSTD_VALUES}
  TYPE   STRING
  DOC    "Choose C++ Standard to compile against"
  )

# - Default is 98, add an arbitrary feature here so that appropriate
#   flags/requirement is added and propagated
set(${PROJECT_NAME_UC}_CXX_COMPILE_FEATURES cxx_template_template_parameters)

# - For C++11, add all features supported in GCC 4.9 and above, except
#   thread_local because that needs a workaround on OS X
set(${PROJECT_NAME_UC}_CXX11_COMPILE_FEATURES
  cxx_alias_templates
  cxx_alignas
  cxx_alignof
  cxx_attributes
  cxx_auto_type
  cxx_constexpr
  cxx_decltype
  cxx_decltype_incomplete_return_types
  cxx_default_function_template_args
  cxx_defaulted_functions
  cxx_defaulted_move_initializers
  cxx_delegating_constructors
  cxx_deleted_functions
  cxx_enum_forward_declarations
  cxx_explicit_conversions
  cxx_extended_friend_declarations
  cxx_extern_templates
  cxx_final
  cxx_func_identifier
  cxx_generalized_initializers
  cxx_inheriting_constructors
  cxx_inline_namespaces
  cxx_lambdas
  cxx_local_type_template_args
  cxx_long_long_type
  cxx_noexcept
  cxx_nonstatic_member_init
  cxx_nullptr
  cxx_override
  cxx_range_for
  cxx_raw_string_literals
  cxx_reference_qualified_functions
  cxx_right_angle_brackets
  cxx_rvalue_references
  cxx_sizeof_member
  cxx_static_assert
  cxx_strong_enums
  cxx_trailing_return_types
  cxx_unicode_literals
  cxx_uniform_initialization
  cxx_unrestricted_unions
  cxx_user_literals
  cxx_variadic_macros
  cxx_variadic_templates
  )

# - For C++14,17,20 add all features supported by current compiler
set(${PROJECT_NAME_UC}_CXX14_COMPILE_FEATURES "${CMAKE_CXX14_COMPILE_FEATURES}")
set(${PROJECT_NAME_UC}_CXX17_COMPILE_FEATURES "${CMAKE_CXX17_COMPILE_FEATURES}")
set(${PROJECT_NAME_UC}_CXX20_COMPILE_FEATURES "${CMAKE_CXX20_COMPILE_FEATURES}")

# - Process compile features based on selected standard
if(${PROJECT_NAME_UC}_CXX_STANDARD EQUAL 11)
  list(APPEND ${PROJECT_NAME_UC}_CXX_COMPILE_FEATURES ${${PROJECT_NAME_UC}_CXX11_COMPILE_FEATURES})
endif()

# - If C++14/17/20 is requested, add all features supported by current
#   compiler. If no 14/17/20 features are supported, fail under assumption
#   compiler/cmake does not support it.
if(${PROJECT_NAME_UC}_CXX_STANDARD EQUAL 14)
  if(NOT ${PROJECT_NAME_UC}_CXX14_COMPILE_FEATURES)
    message(FATAL_ERROR "C++14 requested, but no support for any C++14 features for compiler:\n"
      "'${CMAKE_CXX_COMPILER_ID}', '${CMAKE_CXX_COMPILER_VERSION}'"
      )
  endif()

  list(APPEND ${PROJECT_NAME_UC}_CXX_COMPILE_FEATURES
    ${${PROJECT_NAME_UC}_CXX11_COMPILE_FEATURES}
    ${${PROJECT_NAME_UC}_CXX14_COMPILE_FEATURES}
    )
endif()

if(${PROJECT_NAME_UC}_CXX_STANDARD EQUAL 17)
  if(NOT ${PROJECT_NAME_UC}_CXX17_COMPILE_FEATURES)
    message(FATAL_ERROR "C++17 requested, but no support for any C++17 features for compiler:\n"
      "'${CMAKE_CXX_COMPILER_ID}', '${CMAKE_CXX_COMPILER_VERSION}'"
      )
  endif()

  list(APPEND ${PROJECT_NAME_UC}_CXX_COMPILE_FEATURES
    ${${PROJECT_NAME_UC}_CXX11_COMPILE_FEATURES}
    ${${PROJECT_NAME_UC}_CXX14_COMPILE_FEATURES}
    ${${PROJECT_NAME_UC}_CXX17_COMPILE_FEATURES}
    )
endif()

if(${PROJECT_NAME_UC}_CXX_STANDARD EQUAL 20)
  if(NOT ${PROJECT_NAME_UC}_CXX20_COMPILE_FEATURES)
    message(FATAL_ERROR "C++20 requested, but no support for any C++20 features for compiler:\n"
      "'${CMAKE_CXX_COMPILER_ID}', '${CMAKE_CXX_COMPILER_VERSION}'"
      )
  endif()

  list(APPEND ${PROJECT_NAME_UC}_CXX_COMPILE_FEATURES
    ${${PROJECT_NAME_UC}_CXX11_COMPILE_FEATURES}
    ${${PROJECT_NAME_UC}_CXX14_COMPILE_FEATURES}
    ${${PROJECT_NAME_UC}_CXX17_COMPILE_FEATURES}
    ${${PROJECT_NAME_UC}_CXX20_COMPILE_FEATURES}
    )
endif()

