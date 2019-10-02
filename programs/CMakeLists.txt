#-----------------------------------------------------------------------
# Apps
# - Simulation
add_subdirectory(flsimulate)

# - Reconstruction
add_subdirectory(flreconstruct)

# - Tags
add_subdirectory(fltags)


# - To allow modules to be developed independently, point
# them to the current Bayeux/Falaise
# To be replaced with the "Falaise::Falaise + overidden find_package" trick
# depending on how we finally integrate flvisualize
set(Falaise_DIR "${PROJECT_BUILD_PREFIX}/${CMAKE_INSTALL_CMAKEDIR}/${FALAISE_TAG}")

add_subdirectory(flvisualize)
