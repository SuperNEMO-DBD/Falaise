#.rst: Build and Run flreconstruct tests
#

# Test of FHiCL style properties
add_executable(testFhiclProperties testFhiclProperties.cc)
set_target_properties(testFhiclProperties
  PROPERTIES
  RUNTIME_OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/fltests
  ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/fltests
  )
target_link_libraries(testFhiclProperties FLCatch Bayeux::Bayeux Boost::boost)
add_test(NAME falaise-testFhiclProperties COMMAND testFhiclProperties)




