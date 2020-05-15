
# - List of test programs:
set(FalaiseGammaTrackingPlugin_TESTS
  # test_gamma_tracking.cxx
  )

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

foreach(_testsource ${FalaiseGammaTrackingPlugin_TESTS})
  get_filename_component(_testname ${_testsource} NAME_WE)
  set(_testname "falaisegammatrackingplugin-${_testname}")
  add_executable(${_testname} ${_testsource})
  target_link_libraries(${_testname} Falaise_GammaTracking)
  # - On Apple, ensure dynamic_lookup of undefined symbols
  if(APPLE)
    set_target_properties(${_testname} PROPERTIES LINK_FLAGS "-undefined dynamic_lookup")
  endif()
  add_test(NAME ${_testname} COMMAND ${_testname})
  set_falaise_test_environment(${_testname})
  # - For now, dump them into the testing output directory
  # set_target_properties(${_testname}
  #   PROPERTIES
  #   RUNTIME_OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/fltests/modules
  #   ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/fltests/modules
  #   )
endforeach()

# end of CMakeLists.txt
