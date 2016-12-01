# - List of test programs:
set(FalaiseAnalogSignalBuilderPlugin_TESTS
  test_version.cxx
 )

# # - Use C++11
# set(CMAKE_CXX_FLAGS "-std=c++11")

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
set(_FalaiseAnalogSignalBuilderPlugin_TEST_ENVIRONMENT
  "FALAISE_ASB_TESTING_DIR=${CMAKE_CURRENT_SOURCE_DIR}"
  )

foreach(_testsource ${FalaiseAnalogSignalBuilderPlugin_TESTS})
  get_filename_component(_testname ${_testsource} NAME_WE)
  set(_testname "falaiseasbplugin-${_testname}")
  add_executable(${_testname} ${_testsource})
  target_link_libraries(${_testname} Falaise_AnalogSignalBuilder)
  # - On Apple, ensure dynamic_lookup of undefined symbols
  if(APPLE)
    set_target_properties(${_testname} PROPERTIES LINK_FLAGS "-undefined dynamic_lookup")
  endif()

  add_test(NAME ${_testname} COMMAND ${_testname})
  set_property(TEST ${_testname}
    APPEND PROPERTY ENVIRONMENT ${_FalaiseAnalogSignalBuilderPlugin_TEST_ENVIRONMENT}
    )
  # - For now, dump them into the testing output directory
  set_target_properties(${_testname}
    PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/fltests/modules
    ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/fltests/modules
    )
endforeach()

# end of CMakeLists.txt
