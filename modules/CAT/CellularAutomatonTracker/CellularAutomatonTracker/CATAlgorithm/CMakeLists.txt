# - CMakeLists.txt script for SuperNemoProcessing Module 
#

#----------------------------------------------------------------------------
# Glob the main headers and sources
# NB: note the warning in the README about globbing...
#
file(GLOB CATALGORITHM_HEADERS *.h)
file(GLOB CATALGORITHM_SOURCES *.cpp)

include_directories(${Boost_INCLUDE_DIR})
include_directories(${CLHEP_INCLUDE_DIRS})
if (_cat_needs_root)
  include_directories(${ROOT_INCLUDES})
endif ()

include_directories ( ${PROJECT_BINARY_DIR}/CellularAutomatonTracker )

configure_file ( ${PROJECT_SOURCE_DIR}/cmake/Templates/CAT_config.h.in
  ${PROJECT_BINARY_DIR}/CellularAutomatonTracker/CATAlgorithm/CAT_config.h @ONLY )

install ( FILES ${PROJECT_BINARY_DIR}/CellularAutomatonTracker/CATAlgorithm/CAT_config.h
  DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/${PROJECT_NAME}/CATAlgorithm
  PERMISSIONS OWNER_READ GROUP_READ WORLD_READ
  )

list (APPEND CATALGORITHM_HEADERS ${PROJECT_BINARY_DIR}/CellularAutomatonTracker/CATAlgorithm/CAT_config.h )

#----------------------------------------------------------------------------
# Add the library targets, force them to be shared
#
message("${Boost_INCLUDE_DIR} ${Boost_LIBRARIES}")
add_library(CATAlgorithm SHARED ${Boost_INCLUDE_DIR} ${CATALGORITHM_SOURCES} ${CATALGORITHM_HEADERS})

if (_cat_needs_root)
  # Link them to ROOT libraries in special build modes
  target_link_libraries(CATAlgorithm ${ROOT_LIBRARIES})
endif ()

target_link_libraries(CATAlgorithm mybhep ${Boost_MATH_C99_LIBRARY} ${CLHEP_LIBRARIES})

#----------------------------------------------------------------------------
# Install the library and headers
# Note that we export the library for use by other projects
#
install(TARGETS CATAlgorithm
  EXPORT CellularAutomatonTrackerLibraryDepends    
  DESTINATION ${CMAKE_INSTALL_LIBDIR}
  )

install(FILES ${CATALGORITHM_HEADERS} 
  DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/${PROJECT_NAME}/CATAlgorithm
  )

#----------------------------------------------------------------------------
# Add the programs
#




