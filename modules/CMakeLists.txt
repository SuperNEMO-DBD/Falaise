# - Build local and imported plugins
#
#-----------------------------------------------------------------------
# Modules are compiled code, so need to be installed under libdir...
#
# - Local setting of output path
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${PROJECT_BUILD_PREFIX}/${CMAKE_INSTALL_PLUGINDIR}")

# - To allow modules to be developed independently, point
# them to the current Bayeux/Falaise
set(Falaise_DIR "${PROJECT_BUILD_PREFIX}/${CMAKE_INSTALL_CMAKEDIR}/${FALAISE_TAG}")

# - List of plugins:
set(_falaise_PLUGINS
  CAT
  TrackFit
  ChargedParticleTracking
  GammaTracking
  GammaClustering
  EventBrowser
  things2root
)

# - Process modules here:
foreach(_flplugin ${_falaise_PLUGINS})
  add_subdirectory(${_flplugin})
  # LEGACY ONLY, for EventBrowser style files.
  # Publish resource files for this plugin:
  if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/${_flplugin}/resources/_resources.cmake)
    include(${CMAKE_CURRENT_SOURCE_DIR}/${_flplugin}/resources/_resources.cmake)
    foreach(_rfin ${_falaise_${_flplugin}_RESOURCES_FILES})
      string(REGEX REPLACE "^${PROJECT_SOURCE_DIR}/modules/${_flplugin}/resources/" "${PROJECT_BUILD_DATAROOTDIR}/${FALAISE_RESOURCE_DIR}/modules/${_flplugin}/" _rfout "${_rfin}")
      configure_file(${_rfin} ${_rfout} COPYONLY)
    endforeach()
  endif()
endforeach()
