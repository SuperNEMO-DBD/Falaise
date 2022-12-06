# - Basic CMake setup
# Check version meets ou requirements
# Declare project, which will configure compiler for us
cmake_minimum_required(VERSION 3.3)
project(Things2Root)

# Modules use Falaise/Bayeux, so we need to locate this or fail
#find_package(Bayeux REQUIRED)
#find_package(Falaise REQUIRED)

set(Things2RootPlugin_PUBLIC_HEADERS
  Things2Root_linkdef.h
  Things2Root.h
)

# - Generate ROOT headers
set(Things2RootPlugin_DICT_HEADERS
  Things2Root.h
)

#-----------------------------------------------------------------------
# SOURCE FILES
# Pretty basic!
set(Things2RootPlugin_SOURCES
  ${_things2root_root_dict}
  Things2Root.cpp
)

# Build a dynamic library from our sources
add_library(Things2Root SHARED
  ${Things2RootPlugin_PUBLIC_HEADERS}
  ${Things2RootPlugin_SOURCES}
)

target_include_directories(Things2Root PUBLIC
  ${ROOT_INCLUDE_DIRS}
  ${BAYEUX_INCLUDE_DIRS}
  ${PROJECT_BINARY_DIR}
  ${PROJECT_SOURCE_DIR}
)

target_link_libraries(Things2Root
  ${ROOT_LIBRARIES}
  Falaise
  FalaiseModule
)

set(__THINGS2ROOT_MODULE_ARG MODULE Things2Root)

# Have to haxor CMAKE_INSTALL_LIBDIR and Output Dir because we can't disable
# install of pcm/rootmap files...)
set(CMAKE_INSTALL_LIBDIR_OLD "${CMAKE_INSTALL_LIBDIR}")
set(CMAKE_INSTALL_LIBDIR "${CMAKE_INSTALL_LIBDIR_OLD}/Falaise/modules")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/Falaise/modules")

message(STATUS "PROJECT_SOURCE_DIR=${PROJECT_SOURCE_DIR}")
message(STATUS "CMAKE_INSTALL_LIBDIR=${CMAKE_INSTALL_LIBDIR}")
message(STATUS "CMAKE_INSTALL_LIBDIR_OLD=${CMAKE_INSTALL_LIBDIR_OLD}")
message(STATUS "CMAKE_LIBRARY_OUTPUT_DIRECTORY=${CMAKE_LIBRARY_OUTPUT_DIRECTORY}")
message(STATUS "CMAKE_CURRENT_BINARY_DIR=${CMAKE_CURRENT_BINARY_DIR}")
message(STATUS "CMAKE_INSTALL_PLUGINDIR=${CMAKE_INSTALL_PLUGINDIR}")


# Have to explicitly include the header path
include_directories("${PROJECT_SOURCE_DIR}")
ROOT_GENERATE_DICTIONARY(things2root_dict
  ${__THINGS2ROOT_MODULE_ARG}
  LINKDEF "${PROJECT_SOURCE_DIR}/Things2Root_linkdef.h"
  OPTIONS "-noIncludePaths"
)
set(_things2root_root_dict ${CMAKE_CURRENT_BINARY_DIR}/things2root_dict.cxx)

# Publish the dictionary headers next to the module - needed for
# cling parsing in R6. NB: this is connected with the setting of
# ROOT_INCLUDE_PATH in the main flvisualize application.
foreach(_dictheader ${Things2RootPlugin_DICT_HEADERS})
  get_filename_component(_dictheader_file "${_dictheader}" NAME)
  configure_file(${_dictheader} ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/things2root/${_dictheader_file})
endforeach()

# pop the true libinstalldir
set(CMAKE_INSTALL_LIBDIR "${CMAKE_INSTALL_LIBDIR_OLD}")

# ROOT dictionaries will refine accessor keywords for reflection purposes
# This is expected, but Clang will warn on ROOT5, so disable it.
# Done as a compile flag because we cant edit the code directly.
if(CMAKE_CXX_COMPILER_ID MATCHES "(Apple)+Clang")
  set_property(SOURCE ${_things2root_root_dict}
    PROPERTY COMPILE_FLAGS "-Wno-keyword-macro"
  )
endif()


#Because it's loaded, make sure its RPATH can find dependencies
set_target_properties(Things2Root PROPERTIES INSTALL_RPATH_USE_LINK_PATH 1)

# Install it
install(TARGETS Things2Root DESTINATION ${CMAKE_INSTALL_PLUGINDIR})

# Also have to install the headers for the dictionary so that Cling can use them
# at Runtime.
install(FILES ${Things2RootPlugin_DICT_HEADERS}
  DESTINATION ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/Falaise/
)
