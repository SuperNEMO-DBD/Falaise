# - List of test programs:
set(FalaiseLttcPlugin_TESTS
  test-point.cxx
  test-triplet.cxx
  test-line.cxx
  test-circle.cxx
  test-kinked_curve.cxx
  test-track_simulator.cxx
  test-hit_simulator.cxx
  test-lttc_algo.cxx
  )

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

if(FalaiseLttcPlugin_ENABLE_TESTING)
  foreach(_testsource ${FalaiseLttcPlugin_TESTS})
    get_filename_component(_testname "${_testsource}" NAME_WE)
    set(_testname "falaiselttcplugin-${_testname}")
    add_executable(${_testname} ${_testsource} ${testing_SOURCES})
    target_link_libraries(${_testname} Falaise_Lttc Falaise)
    # - On Apple, ensure dynamic_lookup of undefined symbols
    if(APPLE)
      set_target_properties(${_testname} PROPERTIES LINK_FLAGS "-undefined dynamic_lookup")
    endif()
    add_test(NAME ${_testname} COMMAND ${_testname})
    set_falaise_test_environment(${_testname})

    # - For now, dump them into the testing output directory
    set_target_properties(${_testname}
      PROPERTIES
      RUNTIME_OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/fltests/modules
      ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/fltests/modules
      )
  endforeach()
endif()
