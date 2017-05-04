cmake_minimum_required(VERSION 2.8)
project(CATClient)

if(${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_BINARY_DIR})
  message(STATUS "CATClient requires an out-of-source build.")
  message(STATUS "Please remove these files from ${CMAKE_BINARY_DIR} first:")
  message(STATUS "    CMakeCache.txt")
  message(STATUS "    CMakeFiles")
  message(STATUS "Once these files are removed, create a separate directory")
  message(STATUS "and run CMake from there, pointing it to:")
  message(STATUS "    ${CMAKE_SOURCE_DIR}")
  message(FATAL_ERROR "in-source build detected")
endif()
cmake_minimum_required(VERSION 2.8)
project(CATClient)

find_package(Gnuplot 4.0 REQUIRED)
message(STATUS " Gnuplot executable =   ${GNUPLOT_EXECUTABLE}")


set (CAT_DIR $ENV{CAT_DIR})
set (CellularAutomatonTracker_DIR ${CAT_DIR})  
find_package(CellularAutomatonTracker 0.1.0 REQUIRED NO_MODULE)


message(STATUS " inc  =   ${CellularAutomatonTracker_INCLUDE_DIRS}")
message(STATUS " libs =   ${CellularAutomatonTracker_LIBRARIES}")

include_directories (${CellularAutomatonTracker_INCLUDE_DIRS})
add_executable (client_example client.cxx)
target_link_libraries (client_example CATAlgorithm)
install(FILES ${PROJECT_BINARY_DIR}/client_example 
  DESTINATION ${CMAKE_INSTALL_PREFIX}/bin 
  PERMISSIONS 
      OWNER_READ OWNER_EXECUTE 
      GROUP_READ GROUP_EXECUTE  
      WORLD_READ WORLD_EXECUTE   
  )

#
