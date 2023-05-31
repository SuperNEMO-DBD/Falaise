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
target_clang_format(testFhiclProperties)
add_test(NAME falaise-testFhiclProperties COMMAND testFhiclProperties)
set_falaise_test_environment(falaise-testFhiclProperties)

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
  COMMAND flreconstruct -i ${FLRECONSTRUCT_FIXTURE_FILE} -p "urn:snemo:demonstrator:reconstruction:3.0:config:default"
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
  COMMAND flreconstruct -i ${FLRECONSTRUCT_FIXTURE_FILE} -p "@falaise:snemo/demonstrator/reconstruction/3.0/pipelines/official-3.0.conf"
  )
set_tests_properties(flreconstruct-standard-pipeline-develscript PROPERTIES
  DEPENDS flreconstruct-fixture
  )
set_falaise_test_environment(flreconstruct-standard-pipeline-develscript)

# # - Same, but use symlinked name
# add_test(NAME flreconstruct-standard-pipeline-develscript-symlink
#   COMMAND flreconstruct -i ${FLRECONSTRUCT_FIXTURE_FILE} -p "@falaise:snemo/demonstrator/reconstruction/production.pipeline.conf"
#   )
# set_tests_properties(flreconstruct-standard-pipeline-develscript-symlink PROPERTIES
#   DEPENDS flreconstruct-fixture
#   )
# set_falaise_test_environment(flreconstruct-standard-pipeline-develscript-symlink)

# Test of writing to an output file after standard pipeline
# TODO: Add test of contents of file (i.e. that it contains the right bank names/types)!!
add_test(NAME flreconstruct-standard-pipeline-output
  COMMAND flreconstruct -i ${FLRECONSTRUCT_FIXTURE_FILE} -p "urn:snemo:demonstrator:reconstruction:3.0:config:default" -o "${CMAKE_CURRENT_BINARY_DIR}/flreconstruct-standard-pipeline-output.brio"
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

# Test basic event filtering
add_library(FilterTests SHARED filters/FilterTests.cc)
target_link_libraries(FilterTests Falaise)
set_target_properties(FilterTests PROPERTIES LIBRARY_OUTPUT_DIRECTORY "${PROJECT_BUILD_PREFIX}/${CMAKE_INSTALL_PLUGINDIR}")

set(FLRECONSTRUCT_FILTERS_SIM_FIXTURE_FILE "${CMAKE_CURRENT_BINARY_DIR}/flreconstruct-filters-sim-fixture.brio")
set(FLRECONSTRUCT_FILTERS_FILT_FIXTURE_FILE "${CMAKE_CURRENT_BINARY_DIR}/flreconstruct-filters-filt-fixture.brio")

add_test(NAME flreconstruct-filter-fixture
  COMMAND flsimulate
          -c "${CMAKE_CURRENT_SOURCE_DIR}/filters/sim.conf"
          -o "${FLRECONSTRUCT_FILTERS_SIM_FIXTURE_FILE}")
set_falaise_test_environment(flreconstruct-filter-fixture)

add_test(NAME flreconstruct-filter-do-filter
  COMMAND flreconstruct -i ${FLRECONSTRUCT_FILTERS_SIM_FIXTURE_FILE}
          -p "${CMAKE_CURRENT_SOURCE_DIR}/filters/filter_events.conf"
          -o ${FLRECONSTRUCT_FILTERS_FILT_FIXTURE_FILE})
set_tests_properties(flreconstruct-filter-do-filter PROPERTIES
          DEPENDS flreconstruct-filter-fixture)
set_falaise_test_environment(flreconstruct-filter-do-filter)

add_test(NAME flreconstruct-filter-check-filter
  COMMAND flreconstruct -i ${FLRECONSTRUCT_FILTERS_FILT_FIXTURE_FILE}
          -p "${CMAKE_CURRENT_SOURCE_DIR}/filters/check_filtering.conf")
set_tests_properties(flreconstruct-filter-check-filter PROPERTIES
          DEPENDS flreconstruct-filter-do-filter)
set_falaise_test_environment(flreconstruct-filter-check-filter)


# Tests dedicated to the validation of fixes
# - Validation of issue #8 fix
add_test(NAME flreconstruct-fix-issue8-validation
  COMMAND bash ${CMAKE_CURRENT_SOURCE_DIR}/fix-issue8-validation/run.sh
  --work-dir "${CMAKE_CURRENT_BINARY_DIR}"
  --cfg-dir "${CMAKE_CURRENT_SOURCE_DIR}/fix-issue8-validation"
  )
set_falaise_test_environment(flreconstruct-fix-issue8-validation)

# - Validation of Issue #201 fix
# 1. Need a module to test contents of PTD bank
add_library(TestIssue201 SHARED issue201/TestIssue201.cc)
target_link_libraries(TestIssue201 Falaise)
set_target_properties(TestIssue201 PROPERTIES LIBRARY_OUTPUT_DIRECTORY "${PROJECT_BUILD_PREFIX}/${CMAKE_INSTALL_PLUGINDIR}")
# 2. Run test as script to generate deterministic sim/reco file
add_test(NAME flreconstruct-fix-issue-201
  COMMAND bash ${CMAKE_CURRENT_SOURCE_DIR}/issue201/run.sh ${CMAKE_CURRENT_SOURCE_DIR}/issue201
  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
  )
set_falaise_test_environment(flreconstruct-fix-issue-201)
