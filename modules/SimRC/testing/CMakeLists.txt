
# Configure example pipeline script for use from the build dir
# configure_file("SimRCExample.conf.in" "SimRCModuleExample.conf" @ONLY)

# Add a basic test of reading a brio file output by the standard pipeline

set(_SimRCTestConfFiles
  "simu.profile"
  "dead_cell_map.csv"
  "dead_om_map.csv"
  "off_om_map.csv"
  "tracker_cell_status_service.conf"
  "calorimeter_om_status_service.conf"
  "services.conf"
  "activity_model_factory.conf"
  "activity_models.conf"
  "runs.lis"
  "mc_event_timestamps.lis"
  )

foreach(_file ${_SimRCTestConfFiles})
  configure_file("${CMAKE_CURRENT_SOURCE_DIR}/${_file}" "${CMAKE_CURRENT_BINARY_DIR}/${_file}" COPYONLY)
endforeach()

# - Simulate 100 events
add_test(NAME testSimRC_simulate
  COMMAND flsimulate
  -c ${FalaiseSimRCPlugin_SOURCE_DIR}/testing/simu.conf
  -o testSimRC-sd.brio
  -m testSimRC-simulate-meta.xml
  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
  )

# - Simulate 1000 events
add_test(NAME testSimRC_simulate2
  COMMAND flsimulate
  -c ${FalaiseSimRCPlugin_SOURCE_DIR}/testing/simu2.conf
  -o testSimRC-sd2.brio
  -m testSimRC-simulate-meta2.xml
  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
  )
 
# - Apply RC then reconstruct
add_test(NAME testSimRC_simrc
  COMMAND flreconstruct
  -p ${FalaiseSimRCPlugin_SOURCE_DIR}/testing/simrc.conf
  -i testSimRC-sd.brio
  -o testSimRC-rec-rc.brio
  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
  ) 
set_tests_properties(testSimRC_simrc PROPERTIES DEPENDS testSimRC_simulate)

# - Apply RC then reconstruct
add_test(NAME testSimRC_simrc2
  COMMAND flreconstruct
  -V debug
  -p ${FalaiseSimRCPlugin_SOURCE_DIR}/testing/simrc2.conf
  -i testSimRC-sd2.brio
  -o testSimRC-rec-rc2.brio
  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
  ) 
set_tests_properties(testSimRC_simrc2 PROPERTIES DEPENDS testSimRC_simulate2)

add_test(NAME testSimRC_simrc3
  COMMAND flreconstruct
  -V debug
  -p ${FalaiseSimRCPlugin_SOURCE_DIR}/testing/simrc3.conf
  -i testSimRC-sd.brio
  -o testSimRC-rec-rc3.brio
  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
  ) 
set_tests_properties(testSimRC_simrc3 PROPERTIES DEPENDS testSimRC_simulate)

add_test(NAME testSimRC_simrc4
  COMMAND flreconstruct
  -V debug
  -p ${FalaiseSimRCPlugin_SOURCE_DIR}/testing/simrc4.conf
  -i testSimRC-sd.brio
  -o testSimRC-rec-rc4.brio
  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
  ) 
set_tests_properties(testSimRC_simrc4 PROPERTIES DEPENDS testSimRC_simulate)

#############################################################
# 2022-06-01 FM :
# For developers only : uncomment the following tests to activate the visualization
# 
# # - Apply RC then reconstruct
# add_test(NAME testSimRC_nosimrc
#   COMMAND flreconstruct
#   -p ${FalaiseSimRCPlugin_SOURCE_DIR}/testing/nosimrc.conf
#   -i testSimRC-simulate.brio
#   -o testSimRC-simulate-norc.brio
#   WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
#   ) 

# # - Apply RC then reconstruct
# add_test(NAME testSimRC_visurc
#   COMMAND flvisualize
#   -i testSimRC-simulate-rc.brio
#   -f simu.profile
#   WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
#   )

# # - Apply RC then reconstruct
# add_test(NAME testSimRC_visunorc
#   COMMAND flvisualize
#   -i testSimRC-simulate-norc.brio
#   -f simu.profile
#   WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
# )

# end