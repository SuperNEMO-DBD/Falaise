# - Build local and imported plugins
#
#-----------------------------------------------------------------------
# Modules are compiled code, so need to be installed under libdir...
#
# - Local setting of output path
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${FALAISE_BUILDPRODUCT_DIR}/${CMAKE_INSTALL_LIBDIR}/${FALAISE_PLUGINLIBDIR}")

# - To allow modules to be developed independently, point
# them to the current Bayeux/Falaise
set(Falaise_DIR "${FALAISE_BUILDPRODUCT_DIR}/${CMAKE_INSTALL_CMAKEDIR}/Falaise-${Falaise_VERSION}")
if(NOT FALAISE_USE_SYSTEM_BAYEUX)
  set(Bayeux_DIR "${FALAISE_BUILDPRODUCT_DIR}/${CMAKE_INSTALL_CMAKEDIR}/Bayeux-2.0.0")
endif()

# - List of plugins:
set(_falaise_PLUGINS
  things2root
  # Digitization
  CAT
  MockTrackerClusterizer
  TrackFit
  ChargedParticleTracking
  GammaTracking
  GammaClustering
  VisuToy
  EventBrowser
)

# - Process modules here:
foreach(_flplugin ${_falaise_PLUGINS})
  if(EXISTS ${PROJECT_SOURCE_DIR}/modules/${_flplugin})
    add_subdirectory(${PROJECT_SOURCE_DIR}/modules/${_flplugin})
    # Publish resource files for this plugin:
    if(EXISTS ${PROJECT_SOURCE_DIR}/modules/${_flplugin}/resources/_resources.cmake)
      include(${PROJECT_SOURCE_DIR}/modules/${_flplugin}/resources/_resources.cmake)
      foreach(_rfin ${_falaise_${_flplugin}_RESOURCES_FILES})
	string(REGEX REPLACE "\\.in$" "" _rfout "${_rfin}")
	string(REGEX REPLACE "^${PROJECT_SOURCE_DIR}/modules/${_flplugin}/resources/" "${FALAISE_DATAROOTDIR}/resources/modules/${_flplugin}/" _rfout "${_rfout}")
	get_filename_component (_rfin_ext ${_rfin} EXT)
	if (_rfin_ext STREQUAL ".in")
	  configure_file(${_rfin} ${_rfout} @ONLY)
	else()
	  configure_file(${_rfin} ${_rfout} COPYONLY)
	endif()
      endforeach()
    endif()
  endif()
endforeach()
