# Build and Run flpcd2cpcd tests

# - Validation of fix
# 1. Need a module to test contents of PTD bank
# add_library(Test1 SHARED test1)
# target_link_libraries(Test1 Falaise)
# set_target_properties(Test1 PROPERTIES LIBRARY_OUTPUT_DIRECTORY "${PROJECT_BUILD_PREFIX}/${CMAKE_INSTALL_PLUGINDIR}")
# 2. Run test as script to generate event builder pCD file

# Input real data file 'data/snemo_run-724_udd-10events.brio' is not compatible anymore
# with new version of Falaise because of obsolete metadata (old setup-2.0 vs new 3.0)
set(flpcd2cpcd_RUN_TEST1 0)
if (flpcd2cpcd_RUN_TEST1)
  add_test(NAME flpcd2cpcd-test1
    # COMMAND echo "Running flpcd2cpcd-test1..."
    COMMAND bash ${CMAKE_CURRENT_SOURCE_DIR}/test1/run.sh
    --work-dir ${CMAKE_CURRENT_BINARY_DIR}
    --cfg-dir ${CMAKE_CURRENT_SOURCE_DIR}/test1
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
  )
  set_tests_properties(flpcd2cpcd-test1
    PROPERTIES ENVIRONMENT "PATH=${PROJECT_BUILD_BINDIR}:$ENV{PATH}")
  set_falaise_test_environment(flpcd2cpcd-test1)
endif()

# - end
