# - Basic CMake setup
# Check version meets ou requirements
# Declare project, which will configure compiler for us
cmake_minimum_required(VERSION 3.3)
project(Things2Root)

# hack CMAKE_INSTALL_LIBDIR for dictionnary's .pcm and .rootmap file
set(CMAKE_INSTALL_LIBDIR_OLD "${CMAKE_INSTALL_LIBDIR}")
set(CMAKE_INSTALL_LIBDIR "${CMAKE_INSTALL_LIBDIR_OLD}/Falaise/modules")

# dictionary + its library declaration
root_generate_dictionary(G__Things2Root_dict LINKDEF Things2Root_linkdef.h) # OPTIONS "-noIncludePaths"
add_library(Things2Root_dict SHARED G__Things2Root_dict)
target_link_libraries(Things2Root_dict PUBLIC ROOT::Core)

# unhack CMAKE_INSTALL_LIBDI for dictionnary's .pcm and .rootmap file
set(CMAKE_INSTALL_LIBDIR "${CMAKE_INSTALL_LIBDIR_OLD}")

# Things2Root module declaration
add_library(Things2Root SHARED Things2Root.h Things2Root.cpp G__Things2Root_dict.cxx)
target_link_libraries(Things2Root FalaiseModule)

# install librairies
install(TARGETS Things2Root Things2Root_dict DESTINATION ${CMAKE_INSTALL_PLUGINDIR})
