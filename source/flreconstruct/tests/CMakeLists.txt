#.rst: Build and Run flreconstruct tests
#
set(_falaise_TEST_ENVIRONMENT
  "FALAISE_RESOURCE_DIR=${PROJECT_SOURCE_DIR}/resources"
  "PATH=${PROJECT_BUILD_BINDIR}:$ENV{PATH}"
  )
macro(set_falaise_test_environment _tname)
  set_property(TEST ${_tname}
    APPEND PROPERTY ENVIRONMENT ${_falaise_TEST_ENVIRONMENT}
    )
endmacro()

# Test of FHiCL style properties
add_executable(testFhiclProperties testFhiclProperties.cc)
set_target_properties(testFhiclProperties
  PROPERTIES
  RUNTIME_OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/fltests
  ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/fltests
  )
target_link_libraries(testFhiclProperties FLCatch Bayeux::Bayeux Boost::boost)
target_clang_format(testFhiclProperties)
add_test(NAME falaise-testFhiclProperties COMMAND testFhiclProperties)


# Tests of flreconstruct require an input file, so create a "test fixture"
# file using flsimulate
set(FLRECONSTRUCT_FIXTURE_FILE "${CMAKE_CURRENT_BINARY_DIR}/flreconstruct-fixture.brio")
add_test(NAME flreconstruct-fixture
  COMMAND flsimulate -o "${FLRECONSTRUCT_FIXTURE_FILE}"
  )
set_falaise_test_environment(flreconstruct-fixture)

# Simple smoke test of reading/dumping info from a file
add_test(NAME flreconstruct-smoketest
  COMMAND flreconstruct -i "${FLRECONSTRUCT_FIXTURE_FILE}"
  )
set_tests_properties(flreconstruct-smoketest PROPERTIES
  DEPENDS flreconstruct-fixture
  )
set_falaise_test_environment(flreconstruct-smoketest)

# Check that standard pipelines run
# - Command line via urn
add_test(NAME flreconstruct-standard-pipeline-cli
  COMMAND flreconstruct -i ${FLRECONSTRUCT_FIXTURE_FILE} -p "urn:snemo:demonstrator:reconstruction:1.0.0"
  )
set_tests_properties(flreconstruct-standard-pipeline-cli PROPERTIES
  DEPENDS flreconstruct-fixture
  )
set_falaise_test_environment(flreconstruct-standard-pipeline-cli)

# - Script with URN
add_test(NAME flreconstruct-standard-pipeline-script-urn
  COMMAND flreconstruct -i ${FLRECONSTRUCT_FIXTURE_FILE} -p "${CMAKE_CURRENT_SOURCE_DIR}/testStandardPipelineURN.conf"
  )
set_tests_properties(flreconstruct-standard-pipeline-script-urn PROPERTIES
  DEPENDS flreconstruct-fixture
  )
set_falaise_test_environment(flreconstruct-standard-pipeline-script-urn)

# - Non-URN internal scripts - "next/devel" pipeline
add_test(NAME flreconstruct-standard-pipeline-develscript
  COMMAND flreconstruct -i ${FLRECONSTRUCT_FIXTURE_FILE} -p "@falaise:snemo/demonstrator/reconstruction/official-2.0.0.conf"
  )
set_tests_properties(flreconstruct-standard-pipeline-develscript PROPERTIES
  DEPENDS flreconstruct-fixture
  )
set_falaise_test_environment(flreconstruct-standard-pipeline-develscript)

# Test of writing to an output file after standard pipeline
# TODO: Add test of contents of file (i.e. that it contains the right bank names/types)!!
add_test(NAME flreconstruct-standard-pipeline-output
  COMMAND flreconstruct -i ${FLRECONSTRUCT_FIXTURE_FILE} -p "urn:snemo:demonstrator:reconstruction:1.0.0" -o "${CMAKE_CURRENT_BINARY_DIR}/flreconstruct-standard-pipeline-output.brio"
  )
set_tests_properties(flreconstruct-standard-pipeline-output PROPERTIES
  DEPENDS flreconstruct-fixture
  )
set_falaise_test_environment(flreconstruct-standard-pipeline-output)

# Test Custom Pipeline scripts
add_test(NAME flreconstruct-custom-trivial-pipeline
  COMMAND flreconstruct -i ${FLRECONSTRUCT_FIXTURE_FILE} -p "${CMAKE_CURRENT_SOURCE_DIR}/flreconstruct-trivial-pipeline.conf"
  )
set_tests_properties(flreconstruct-custom-trivial-pipeline PROPERTIES
  DEPENDS flreconstruct-fixture
  )
set_falaise_test_environment(flreconstruct-custom-trivial-pipeline)

add_test(NAME flreconstruct-custom-chain-pipeline
  COMMAND flreconstruct -i ${FLRECONSTRUCT_FIXTURE_FILE} -p "${CMAKE_CURRENT_SOURCE_DIR}/flreconstruct-chain-pipeline.conf"
  )
set_tests_properties(flreconstruct-custom-chain-pipeline PROPERTIES
  DEPENDS flreconstruct-fixture
  )
set_falaise_test_environment(flreconstruct-custom-chain-pipeline)

add_test(NAME flreconstruct-custom-multimodule-pipeline
  COMMAND flreconstruct -i ${FLRECONSTRUCT_FIXTURE_FILE} -p "${CMAKE_CURRENT_SOURCE_DIR}/flreconstruct-multimodule-pipeline.conf"
  )
set_tests_properties(flreconstruct-custom-multimodule-pipeline PROPERTIES
  DEPENDS flreconstruct-fixture
  )
set_falaise_test_environment(flreconstruct-custom-multimodule-pipeline)

# Tests dedicated to the validation of fixes
# - Validation of issue #8 fix
add_test(NAME flreconstruct-fix-issue8-validation
  COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/fix-issue8-validation/run.sh
  --work-dir "${CMAKE_CURRENT_BINARY_DIR}"
  --cfg-dir "${CMAKE_CURRENT_SOURCE_DIR}/fix-issue8-validation"
  )
set_falaise_test_environment(flreconstruct-fix-issue8-validation)

