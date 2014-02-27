# - Build local and imported plugins
#
#-----------------------------------------------------------------------
# Modules are compiled code, so need to be installed under libdir...
#
# - Local setting of output path
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${Falaise_BUILDPRODUCT_DIR}/${CMAKE_INSTALL_LIBDIR}/${Falaise_PLUGINLIBDIR}")

# - To allow modules to be developed independently, point
# them to the current Bayeux/Falaise
set(Bayeux_DIR "${Falaise_BUILDPRODUCT_DIR}/${CMAKE_INSTALL_LIBDIR}/cmake/Bayeux-1.0.0")

# - Process modules here...
# add_subdirectory(things2root)
add_subdirectory(CAT)
#add_subdirectory(TrackFit)
