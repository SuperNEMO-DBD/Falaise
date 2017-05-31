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


# Tests of flreconstruct require an input file, so create a "test fixture"
# file using flsimulate
set(FLRECONSTRUCT_FIXTURE_FILE "${CMAKE_CURRENT_BINARY_DIR}/flreconstruct-fixture.brio")
add_test(NAME flreconstruct-fixture
  COMMAND flsimulate -o "${FLRECONSTRUCT_FIXTURE_FILE}"
  )

# Simple smoke test of reading/dumping info from a file
add_test(NAME flreconstruct-smoketest
  COMMAND flreconstruct -i "${FLRECONSTRUCT_FIXTURE_FILE}"
  )
set_tests_properties(flreconstruct-smoketest PROPERTIES
  DEPENDS flreconstruct-fixture
  )

# Check that standard pipelines run
# - Command line via urn
add_test(NAME flreconstruct-standard-pipeline-cli
  COMMAND flreconstruct -i ${FLRECONSTRUCT_FIXTURE_FILE} -p "urn:snemo:demonstrator:reconstruction:1.0.0"
  )
set_tests_properties(flreconstruct-standard-pipeline-cli PROPERTIES
  DEPENDS flreconstruct-fixture
  )

# - Script with URN
add_test(NAME flreconstruct-standard-pipeline-script-urn
  COMMAND flreconstruct -i ${FLRECONSTRUCT_FIXTURE_FILE} -p "${CMAKE_CURRENT_SOURCE_DIR}/testStandardPipelineURN.conf"
  )
set_tests_properties(flreconstruct-standard-pipeline-script-urn PROPERTIES
  DEPENDS flreconstruct-fixture
  )


