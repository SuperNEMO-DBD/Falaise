# - Add components of Hereward to main build
#

add_subdirectory(mybhep)
add_subdirectory(sultan)
add_subdirectory(CATAlgorithm)
 
if (CAT_WITH_DEVEL_UTILS)
  message ( STATUS "${PROJECT_NAME}: Building CATutils" )
  add_subdirectory(CATUtils) 
endif (CAT_WITH_DEVEL_UTILS)

#

