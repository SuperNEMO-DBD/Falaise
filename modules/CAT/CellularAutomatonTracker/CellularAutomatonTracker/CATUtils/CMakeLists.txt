# - CMakeLists.txt script for CATUtils
#

#----------------------------------------------------------------------------
# Glob the main headers and sources
# NB: note the warning in the README about globbing...
#
file(GLOB CATUTILS_HEADERS *.h)
file(GLOB CATUTILS_SOURCES *.cpp)

include_directories(${Boost_INCLUDE_DIR})
include_directories(${CLHEP_INCLUDE_DIRS})
if (_cat_needs_root)
  include_directories(${ROOT_INCLUDES})
endif ()

include_directories ( ${PROJECT_BINARY_DIR}/CellularAutomatonTracker )

#----------------------------------------------------------------------------
# Add the library targets, force them to be shared
#
add_library(CATUtils SHARED ${CATUTILS_SOURCES} ${CATUTILS_HEADERS})

# Link them as needed to ROOT libraries
# Hmm, this isn't really right...
target_link_libraries(CATUtils CATAlgorithm)

add_executable(test_experimental_classes test/test_experimental_classes.cpp)
target_link_libraries(test_experimental_classes CATUtils)

add_executable(test_sequentiator test/test_sequentiator.cpp)
target_link_libraries(test_sequentiator CATUtils)

add_executable(local_tracking test/local_tracking.cpp)
target_link_libraries(local_tracking CATUtils)

#----------------------------------------------------------------------------
# Install the library and headers
# Note that we export the library for use by other projects
#
install(TARGETS CATUtils
  EXPORT CellularAutomatonTrackerLibraryDepends    
  DESTINATION ${CMAKE_INSTALL_LIBDIR}
  )

install(FILES ${CATUTILS_HEADERS} 
  DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/${PROJECT_NAME}/CATUtils
  )

install(TARGETS test_experimental_classes DESTINATION ${CMAKE_INSTALL_BINDIR})
install(TARGETS test_sequentiator DESTINATION ${CMAKE_INSTALL_BINDIR})
install(TARGETS local_tracking DESTINATION ${CMAKE_INSTALL_BINDIR})






