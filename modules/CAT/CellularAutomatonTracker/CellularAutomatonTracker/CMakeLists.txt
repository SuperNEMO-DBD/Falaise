# - Add components of Hereward to main build
#

add_subdirectory(mybhep)
add_subdirectory(CATAlgorithm)
 
if (CAT_WITH_DEVEL_UTILS)
  message ( STATUS "WARNING: Building CATutils" )
  add_subdirectory(CATUtils) 
endif (CAT_WITH_DEVEL_UTILS)

#

