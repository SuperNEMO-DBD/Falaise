
# - List of test programs:
set(FalaiseCATPlugin_TESTS
  test_cat_driver.cxx
  test_cat_tracker_clustering_module.cxx
  test_sultan_driver.cxx
  test_sultan_tracker_clustering_module.cxx
  )

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

set(testing_SOURCES utilities.cc)

if(FalaiseCATPlugin_ENABLE_TESTING)
  foreach(_testsource ${FalaiseCATPlugin_TESTS})
    get_filename_component(_testname "${_testsource}" NAME_WE)
    set(_testname "falaisecatplugin-${_testname}")
    add_executable(${_testname} ${_testsource} ${testing_SOURCES})
    target_link_libraries(${_testname} Falaise_CAT Falaise)
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
