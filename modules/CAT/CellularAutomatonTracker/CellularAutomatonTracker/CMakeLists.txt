# - Add components of Hereward to main build
#

add_subdirectory(mybhep)
add_subdirectory(CATAlgorithm)
 
if (CAT_WITH_DEV_UTILS)
  add_subdirectory(CATUtils) 
endif (CAT_WITH_DEV_UTILS)

#

