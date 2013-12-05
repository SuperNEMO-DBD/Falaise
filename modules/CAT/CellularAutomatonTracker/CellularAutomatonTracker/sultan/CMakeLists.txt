# - CMakeLists.txt script for sultan
#

#----------------------------------------------------------------------------
# Glob the main headers and sources
# NB: note the warning in the README about globbing...
#
file(GLOB SULTAN_HEADERS *.h)
file(GLOB SULTAN_SOURCES *.cpp)

include_directories(${CLHEP_INCLUDE_DIRS})

add_definitions (-Wno-deprecated)

include_directories ( ${PROJECT_BINARY_DIR}/CellularAutomatonTracker )

configure_file ( ${PROJECT_SOURCE_DIR}/cmake/Templates/SULTAN_config.h.in
  ${PROJECT_BINARY_DIR}/CellularAutomatonTracker/sultan/SULTAN_config.h @ONLY )

install ( FILES ${PROJECT_BINARY_DIR}/CellularAutomatonTracker/sultan/SULTAN_config.h
  DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/${PROJECT_NAME}/sultan
  PERMISSIONS OWNER_READ GROUP_READ WORLD_READ
  )

list (APPEND CATALGORITHM_HEADERS ${PROJECT_BINARY_DIR}/CellularAutomatonTracker/sultan/SULTAN_config.h )

configure_file ( ${PROJECT_SOURCE_DIR}/cmake/Templates/SULTAN_config.h.in
  ${PROJECT_BINARY_DIR}/CellularAutomatonTracker/sultan/SULTAN_config.h @ONLY )

install ( FILES ${PROJECT_BINARY_DIR}/CellularAutomatonTracker/sultan/SULTAN_config.h
  DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/${PROJECT_NAME}/sultan
  PERMISSIONS OWNER_READ GROUP_READ WORLD_READ
  )

list (APPEND CATALGORITHM_HEADERS ${PROJECT_BINARY_DIR}/CellularAutomatonTracker/sultan/SULTAN_config.h )


#----------------------------------------------------------------------------
# Add the library targets, force them to be shared
#
add_library(sultan SHARED ${SULTAN_SOURCES} ${SULTAN_HEADERS})

# Link them as needed to ROOT libraries
# Hmm, this isn't really right...

find_package (ZLIB REQUIRED)
if (NOT ZLIB_FOUND )
  message ( FATAL_ERROR "Cannot find the ZLIB library !" )
else ()
  message ( STATUS "NOTICE: found the ZLIB library : '${ZLIB_LIBRARIES}'" )
endif ()
target_link_libraries( sultan ${ZLIB_LIBRARIES} )

#----------------------------------------------------------------------------
# Install the library and headers
# Note that we export the library for use by other projects
#
install(TARGETS sultan
  EXPORT CellularAutomatonTrackerLibraryDepends    
  DESTINATION ${CMAKE_INSTALL_LIBDIR}
  )

install(FILES ${SULTAN_HEADERS} 
  DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/${PROJECT_NAME}/sultan
  )





