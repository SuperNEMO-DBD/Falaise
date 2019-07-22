# - CMakeLists.txt script for mybhep
#

#----------------------------------------------------------------------------
# Glob the main headers and sources
# NB: note the warning in the README about globbing...
#
file(GLOB MYBHEP_HEADERS *.h)
file(GLOB MYBHEP_SOURCES *.cpp)

include_directories(${CLHEP_INCLUDE_DIRS})

add_definitions (-Wno-deprecated)

#----------------------------------------------------------------------------
# Add the library targets, force them to be shared
#
add_library(mybhep SHARED ${MYBHEP_SOURCES} ${MYBHEP_HEADERS})

# Link them as needed to ROOT libraries
# Hmm, this isn't really right...

find_package (ZLIB REQUIRED)
if (NOT ZLIB_FOUND )
  message ( FATAL_ERROR "Cannot find the ZLIB library !" )
else ()
  message ( STATUS "NOTICE: found the ZLIB library : '${ZLIB_LIBRARIES}'" )
endif ()
target_link_libraries( mybhep ${ZLIB_LIBRARIES} )

#----------------------------------------------------------------------------
# Install the library and headers
# Note that we export the library for use by other projects
#
install(TARGETS mybhep
  EXPORT CellularAutomatonTrackerLibraryDepends    
  DESTINATION ${CMAKE_INSTALL_LIBDIR}
  )

install(FILES ${MYBHEP_HEADERS} 
  DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/${PROJECT_NAME}/mybhep
  )





