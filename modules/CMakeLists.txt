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

# Modules
add_subdirectory(CAT)
# add_subdirectory(LTTC)
add_subdirectory(TrackFit)
add_subdirectory(ChargedParticleTracking)
add_subdirectory(GammaTracking)
add_subdirectory(GammaClustering)
add_subdirectory(things2root)
add_subdirectory(SensitivityModule)
add_subdirectory(SimRC)

# - end
