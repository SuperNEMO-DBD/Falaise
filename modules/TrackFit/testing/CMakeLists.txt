
# - List of test programs:
set(FalaiseTrackFitPlugin_TESTS
  test_trackfit_drawing.cxx
  test_trackfit_drift_time_calibration.cxx
  test_trackfit_gg_hit.cxx
  test_trackfit_helix_fit_mgr.cxx
  test_trackfit_line_fit_mgr.cxx
  test_trackfit_driver.cxx
  # test_trackfit_tracker_fitting_module.cxx
  )

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

set(testing_SOURCES utilities.cc)

if(FalaiseTrackFitPlugin_ENABLE_TESTING)
  foreach(_testsource ${FalaiseTrackFitPlugin_TESTS})
    get_filename_component(_testname "${_testsource}" NAME_WE)
    set(_testname "falaisetrackfitplugin-${_testname}")
    add_executable(${_testname} ${_testsource} ${testing_SOURCES})
    target_link_libraries(${_testname} Falaise_TrackFit Falaise)
    # - On Apple, ensure dynamic_lookup of undefined symbols
    if(APPLE)
      set_target_properties(${_testname} PROPERTIES LINK_FLAGS "-undefined dynamic_lookup")
    endif()
    add_test(NAME ${_testname} COMMAND ${_testname})
    # - For now, dump them into the testing output directory
    set_target_properties(${_testname}
      PROPERTIES
      RUNTIME_OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/fltests/modules
      ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/fltests/modules
      )
  endforeach()
endif()
