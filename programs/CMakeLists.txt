#-----------------------------------------------------------------------
# Apps
add_subdirectory(flhistogram) 
add_subdirectory(flsimulate)
add_subdirectory(flreconstruct)
add_subdirectory(flpcd2cpcd)
add_subdirectory(fltags)
add_subdirectory(fluddstat) 
add_subdirectory(flgeomap) 
### 2026-06-19 FM: WIP
# add_subdirectory(flompreparedb) 

### 2026-06-19 FM: stalled
# add_subdirectory(flscangeom) 

# - To allow modules to be developed independently, point
# them to the current Bayeux/Falaise
# To be replaced with the "Falaise::Falaise + overidden find_package" trick
# depending on how we finally integrate flvisualize
set(Falaise_DIR "${PROJECT_BUILD_PREFIX}/${CMAKE_INSTALL_CMAKEDIR}/${FALAISE_TAG}")
add_subdirectory(flvisualize)
