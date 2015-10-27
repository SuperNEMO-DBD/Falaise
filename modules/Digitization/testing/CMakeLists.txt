# - List of test programs:
set(FalaiseDigitizationPlugin_TESTS
  test_calo_tp.cxx
  test_calo_tp_data.cxx
  test_calo_ctw.cxx
  test_calo_ctw_data.cxx
  test_calo_tp_to_ctw_algo.cxx

  test_geiger_tp.cxx
  test_geiger_tp_data.cxx
  test_geiger_ctw.cxx
  test_geiger_ctw_data.cxx
  test_geiger_tp_to_ctw_algo.cxx

  test_sd_to_geiger_signal_algo.cxx
  test_sd_to_calo_signal_algo.cxx
  test_sd_to_signal_process.cxx
  test_sd_to_tp_process.cxx
  test_sd_to_ctw_process.cxx

  test_signal_to_geiger_tp_algo.cxx
  test_tracker_trigger_algorithm.cxx
  test_calo_trigger_algorithm.cxx
  test_trigger_algorithm.cxx

  test_trigger_analysis_gen_histo.cxx

  test_simulated_data_reading.cxx
  test_ID_convertor.cxx
  test_bimap.cxx
  test_memory.cxx
  )

# Use C++11
#set(CMAKE_CXX_FLAGS "-std=c++11")

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
set(_FalaiseDigitizationPlugin_TEST_ENVIRONMENT
  "FALAISE_DIGITIZATION_TESTING_DIR=${CMAKE_CURRENT_SOURCE_DIR}"
  )

foreach(_testsource ${FalaiseDigitizationPlugin_TESTS})
  get_filename_component(_testname ${_testsource} NAME_WE)
  set(_testname "falaisedigitizationplugin-${_testname}")
  add_executable(${_testname} ${_testsource})
  target_link_libraries(${_testname} Falaise_Digitization)
  # - On Apple, ensure dynamic_lookup of undefined symbols
  if(APPLE)
    set_target_properties(${_testname} PROPERTIES LINK_FLAGS "-undefined dynamic_lookup")
  endif()

  add_test(NAME ${_testname} COMMAND ${_testname})
  set_property(TEST ${_testname}
    APPEND PROPERTY ENVIRONMENT ${_FalaiseDigitizationPlugin_TEST_ENVIRONMENT}
    )
  # - For now, dump them into the testing output directory
  # set_target_properties(${_testname}
  #   PROPERTIES
  #   RUNTIME_OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/fltests/modules
  #   ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/fltests/modules
  #   )
endforeach()

# end of CMakeLists.txt
