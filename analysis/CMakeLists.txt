set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${PROJECT_BUILD_PREFIX}/${CMAKE_INSTALL_PLUGINDIR}")
# - To allow modules to be developed independently, point
# them to the current Bayeux/Falaise
set(Falaise_DIR "${PROJECT_BUILD_PREFIX}/${CMAKE_INSTALL_CMAKEDIR}/${FALAISE_TAG}")

add_subdirectory(udd_analysis)
