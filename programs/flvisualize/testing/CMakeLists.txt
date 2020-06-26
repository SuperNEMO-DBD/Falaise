
# - List of test programs:
set(FalaiseEventBrowserPlugin_TESTS
  # test_boost_access.cxx
  # test_brio_access.cxx
  # test_event_server.cxx
  # test_composite_volume.cxx
  # test_polycone.cxx
  # test_root_volume.cxx

  # These need testing on Linux. Fine on mac, fail on Linux at terminate stage:
  # terminate called after throwing an instance of 'std::logic_error'
  # 67973
  # 70:   what():  [datatools::base_service& datatools::service_manager::grab_service(const string&):278: No service named '��=8�U
  #
  #test_detector_manager.cxx
  #test_style_manager.cxx
  )

set(_FalaiseEventBrowserPlugin_TEST_ENVIRONMENT
  "FALAISE_RESOURCE_DIR=${CMAKE_SOURCE_DIR}/resources")
foreach(_testsource ${FalaiseEventBrowserPlugin_TESTS})
  get_filename_component(_testname ${_testsource} NAME_WE)
  set(_testname "falaiseeventbrowserplugin-${_testname}")
  add_executable(${_testname} ${_testsource})
  target_link_libraries(${_testname} Falaise_EventBrowser Falaise)
  # - On Apple, ensure dynamic_lookup of undefined symbols
  if(APPLE)
    set_target_properties(${_testname} PROPERTIES LINK_FLAGS "-undefined dynamic_lookup")
  endif()
  add_test(NAME ${_testname} COMMAND ${_testname})
  set_property(TEST ${_testname}
    APPEND PROPERTY ENVIRONMENT ${_FalaiseEventBrowserPlugin_TEST_ENVIRONMENT}
    )
  # - For now, dump them into the testing output directory
  set_target_properties(${_testname}
    PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/fltests/modules
    ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/fltests/modules
    )
endforeach()

# end of CMakeLists.txt
