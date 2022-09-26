#.rst: Build/run tests for flsimulate{-configure}
# Scripts are taken from documentation

# Basic smoke test of default behaviour
# - write single event of default setup to output file
add_test(NAME flsimulate-smoke-test
  COMMAND flsimulate -o "${CMAKE_CURRENT_BINARY_DIR}/flsimulate-smoke-test.brio"
  )
set_falaise_test_environment(flsimulate-smoke-test)

# Basic test scripts
# NB: these only check that scripts run, they do not validate the
# contents of the output files
# If that's required, add a suitable run of flreconstruct (or other program) as a secondary
# command to the test
set(FLSIMULATE_TESTSCRIPT_NAMES
  flsimulate-script-nevents
  flsimulate-script-eventgen
  flsimulate-script-vertexgen
  flsimulate-script-eventandvertex
  flsimulate-script-geometrylayout
  flsimulate-script-inlineseeds
  flsimulate-script-seedsfromfile
  flsimulate-script-outputprofile
  )

foreach(_test ${FLSIMULATE_TESTSCRIPT_NAMES})
  add_test(NAME ${_test}
    COMMAND flsimulate -d "testdata@${CMAKE_CURRENT_SOURCE_DIR}/data" -c "${CMAKE_CURRENT_SOURCE_DIR}/${_test}.conf" -o "${CMAKE_CURRENT_BINARY_DIR}/${_test}.brio"
    )
  set_falaise_test_environment(${_test})
endforeach()

# More detailed tests from examples
# - Example 2
# - Part 1: generate profile
add_test(NAME flsimulate-example-2-profile
  COMMAND flsimulate-configure
          --no-gui
          -t urn:snemo:demonstrator:simulation:2.1
          -s "geometry:layout=Basic"
          -s "vertexes:generator=field_wire_bulk"
          -s "primary_events:generator=Tl208"
          -s "simulation:output_profile=all_details"
          -o "${CMAKE_CURRENT_BINARY_DIR}/example-2-variant.profile"
  )
# Part 2: Use variant profile
add_test(NAME flsimulate-example-2-run
  COMMAND flsimulate -d "example2@${CMAKE_CURRENT_BINARY_DIR}" -c "${CMAKE_CURRENT_SOURCE_DIR}/example-2.conf" -o "${CMAKE_CURRENT_BINARY_DIR}/example-2.brio"
  )
set_tests_properties(flsimulate-example-2-run PROPERTIES DEPENDS flsimulate-example-2-profile)

set_falaise_test_environment(flsimulate-example-2-profile)
set_falaise_test_environment(flsimulate-example-2-run)

# - Example 3
# - Part 1: generate profile
add_test(NAME flsimulate-example-3-profile
  COMMAND flsimulate-configure
          --no-gui
          -t urn:snemo:demonstrator:simulation:2.1
          -s "geometry:layout=Basic"
          -s "vertexes:generator=field_wire_bulk"
          -s "primary_events:generator=Tl208"
          -s "simulation:output_profile=none"
          -o "${CMAKE_CURRENT_BINARY_DIR}/example-3-variant.profile"
  )
# Part 2: Use variant profile and seeds file
add_test(NAME flsimulate-example-3-run
  COMMAND flsimulate
          -d "example3@${CMAKE_CURRENT_BINARY_DIR}"
          -d "example3rng@${CMAKE_CURRENT_SOURCE_DIR}/data"
          -c "${CMAKE_CURRENT_SOURCE_DIR}/example-3.conf" -o "${CMAKE_CURRENT_BINARY_DIR}/example-3.brio"
  )
set_tests_properties(flsimulate-example-3-run PROPERTIES DEPENDS flsimulate-example-3-profile)

set_falaise_test_environment(flsimulate-example-3-profile)
set_falaise_test_environment(flsimulate-example-3-run)

# - Example 4
# - Part 1: generate profile
add_test(NAME flsimulate-example-4-profile
  COMMAND flsimulate-configure
          --no-gui
          -t urn:snemo:demonstrator:simulation:2.1
          -s "geometry:layout=Basic"
          -s "vertexes:generator=calo_8inch_scin_bulk"
          -s "primary_events:generator=versatile_generator"
	  -s "primary_events:generator/if_versatile/particle=gamma"
	  -s "primary_events:generator/if_versatile/energy=8000 keV"
          -s "simulation:output_profile=none"
          -o "${CMAKE_CURRENT_BINARY_DIR}/example-4-variant.profile"
  )
# Part 2: Use variant profile and seeds file
add_test(NAME flsimulate-example-4-run
  COMMAND flsimulate
          -d "example4@${CMAKE_CURRENT_BINARY_DIR}"
          -d "example4rng@${CMAKE_CURRENT_SOURCE_DIR}/data"
          -c "${CMAKE_CURRENT_SOURCE_DIR}/example-4.conf" -o "${CMAKE_CURRENT_BINARY_DIR}/example-4.brio"
  )
set_tests_properties(flsimulate-example-4-run PROPERTIES DEPENDS flsimulate-example-4-profile)

set_falaise_test_environment(flsimulate-example-4-profile)
set_falaise_test_environment(flsimulate-example-4-run)

# Tests dedicated to the validation of fixes
# - Validation of issue #16 fix
add_test(NAME flsimulate-fix-issue16-validation
  COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/fix-issue16-validation/run.sh
  --work-dir "${CMAKE_CURRENT_BINARY_DIR}"
  --cfg-dir "${CMAKE_CURRENT_SOURCE_DIR}/fix-issue16-validation"
  )
set_tests_properties(flsimulate-fix-issue16-validation
   PROPERTIES ENVIRONMENT "PATH=${PROJECT_BUILD_BINDIR}:$ENV{PATH}")
set_falaise_test_environment(flsimulate-fix-issue16-validation)

# - Flsimulate issue #21
# - Variant profile is given : flsimulate-issue21.profile
add_test(NAME flsimulate-issue21-run
  COMMAND flsimulate
          -d "issue21@${CMAKE_CURRENT_BINARY_DIR}"
          -d "issue21variant@${CMAKE_CURRENT_SOURCE_DIR}/"
          -d "issue21rng@${CMAKE_CURRENT_SOURCE_DIR}/data/"
          -c "${CMAKE_CURRENT_SOURCE_DIR}/flsimulate-issue21.conf" -o "${CMAKE_CURRENT_BINARY_DIR}/issue21.brio"
  )
set_falaise_test_environment(flsimulate-issue21-run)

# - Flsimulate issue #71
# - Variant profile is given : flsimulate-issue71.profile
add_test(NAME flsimulate-issue71-run
  COMMAND flsimulate
          -d "issue71@${CMAKE_CURRENT_BINARY_DIR}"
          -d "issue71variant@${CMAKE_CURRENT_SOURCE_DIR}/"
          -d "issue71rng@${CMAKE_CURRENT_SOURCE_DIR}/data/"
          -c "${CMAKE_CURRENT_SOURCE_DIR}/flsimulate-issue71.conf" -o "${CMAKE_CURRENT_BINARY_DIR}/issue71.brio"
  )
set_falaise_test_environment(flsimulate-issue71-run)

# - Flsimulate issue #97
# - Variant profile is given : flsimulate-issue97.profile
add_test(NAME flsimulate-issue97-run
  COMMAND flsimulate
          -d "issue97@${CMAKE_CURRENT_BINARY_DIR}"
          -d "issue97variant@${CMAKE_CURRENT_SOURCE_DIR}/"
          -d "issue97rng@${CMAKE_CURRENT_SOURCE_DIR}/data/"
          -c "${CMAKE_CURRENT_SOURCE_DIR}/flsimulate-issue97.conf" -o "${CMAKE_CURRENT_BINARY_DIR}/issue97.brio"
  )
set_falaise_test_environment(flsimulate-issue97-run)

# - Flsimulate issue #98
# - Variant profile is given : flsimulate-issue98.profile
add_test(NAME flsimulate-issue98-run
  COMMAND flsimulate
          -d "issue98@${CMAKE_CURRENT_BINARY_DIR}"
          -d "issue98variant@${CMAKE_CURRENT_SOURCE_DIR}/"
          -d "issue98rng@${CMAKE_CURRENT_SOURCE_DIR}/data/"
          -c "${CMAKE_CURRENT_SOURCE_DIR}/flsimulate-issue98.conf" -o "${CMAKE_CURRENT_BINARY_DIR}/issue98.brio"
  )
set_falaise_test_environment(flsimulate-issue98-run)

# Tests dedicated to the validation of fixes
# - Validation of issue #59 fix
add_test(NAME flsimulate-fix-issue59-validation
  COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/fix-issue59-validation/run.sh
  --work-dir "${CMAKE_CURRENT_BINARY_DIR}"
  --cfg-dir "${CMAKE_CURRENT_SOURCE_DIR}/fix-issue59-validation"
  )
set_falaise_test_environment(flsimulate-fix-issue59-validation)

# Tests dedicated to the validation of fixes
# - Validation of feature PR #75
add_test(NAME flsimulate-feature-pr75-validation
  COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/feature-pr75-validation/run.sh
  --work-dir "${CMAKE_CURRENT_BINARY_DIR}"
  --cfg-dir "${CMAKE_CURRENT_SOURCE_DIR}/feature-pr75-validation"
  )
set_tests_properties(flsimulate-feature-pr75-validation
   PROPERTIES ENVIRONMENT "PATH=${PROJECT_BUILD_BINDIR}:$ENV{PATH}")
set_falaise_test_environment(flsimulate-feature-pr75-validation)

# - Flsimulate Bi207 calibration source
# - Validation of issue #69 fix
# - Variant profile is given : flsimulate-script-calibBi207_source_calibration.profile
add_test(NAME flsimulate-script-calibBi207_source_calibration-run
  COMMAND flsimulate
          -d "calibBi207_source_calibration@${CMAKE_CURRENT_BINARY_DIR}"
          -d "calibBi207sourcecalibrationvariant@${CMAKE_CURRENT_SOURCE_DIR}/"
          -c "${CMAKE_CURRENT_SOURCE_DIR}/flsimulate-script-calibBi207_source_calibration.conf" -o "${CMAKE_CURRENT_BINARY_DIR}/flsimulate-script-calibBi207_source_calibration.brio"
  )
set_falaise_test_environment(flsimulate-script-calibBi207_source_calibration-run)

# - Variant profile is given : flsimulate-script-Bi207_source_calibration.profile
add_test(NAME flsimulate-script-Bi207_source_calibration-run
  COMMAND flsimulate
          -d "Bi207_source_calibration@${CMAKE_CURRENT_BINARY_DIR}"
          -d "Bi207sourcecalibrationvariant@${CMAKE_CURRENT_SOURCE_DIR}/"
          -c "${CMAKE_CURRENT_SOURCE_DIR}/flsimulate-script-Bi207_source_calibration.conf" -o "${CMAKE_CURRENT_BINARY_DIR}/flsimulate-script-Bi207_source_calibration.brio"
  )
set_falaise_test_environment(flsimulate-script-Bi207_source_calibration-run)

# - Flsimulate XWall-GVeto PMT glass bulk
# - Variant profile is given : flsimulate-script-xwall_gveto_pmt_glass.profile
add_test(NAME flsimulate-script-xwall_gveto_pmt_glass-run
  COMMAND flsimulate
          -d "xwall_gveto_pmt_glass@${CMAKE_CURRENT_BINARY_DIR}"
          -d "xwallgvetopmtglassvariant@${CMAKE_CURRENT_SOURCE_DIR}/"
          -c "${CMAKE_CURRENT_SOURCE_DIR}/flsimulate-script-xwall_gveto_pmt_glass.conf" -o "${CMAKE_CURRENT_BINARY_DIR}/flsimulate-script-xwall_gveto_pmt_glass.brio"
  )
set_falaise_test_environment(flsimulate-script-xwall_gveto_pmt_glass-run)

# - Flsimulate issue #198, and tests that mapped magnetic field can be used
add_test(NAME flsimulate-issue198-run
  COMMAND flsimulate
          -c "${CMAKE_CURRENT_SOURCE_DIR}/flsimulate-issue198.conf" -o "${CMAKE_CURRENT_BINARY_DIR}/issue198.brio"
  )
set_falaise_test_environment(flsimulate-issue198-run)


# Tests dedicated to the validation of fixes

# - Validation of feature issue #237
add_test(NAME flsimulate-feature-issue237-validation
  COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/feature-issue237-validation/run.sh
  --work-dir "${CMAKE_CURRENT_BINARY_DIR}"
  --cfg-dir "${CMAKE_CURRENT_SOURCE_DIR}/feature-issue237-validation"
  )
set_tests_properties(flsimulate-feature-issue237-validation
   PROPERTIES ENVIRONMENT "PATH=${PROJECT_BUILD_BINDIR}:$ENV{PATH}")

set_falaise_test_environment(flsimulate-feature-issue237-validation)

# - Validation of feature issue #241
add_test(NAME flsimulate-feature-issue241-validation
  COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/feature-issue241-validation/run.sh
  --work-dir "${CMAKE_CURRENT_BINARY_DIR}"
  --cfg-dir "${CMAKE_CURRENT_SOURCE_DIR}/feature-issue241-validation"
  )
set_tests_properties(flsimulate-feature-issue241-validation
   PROPERTIES ENVIRONMENT "PATH=${PROJECT_BUILD_BINDIR}:$ENV{PATH}")
set_falaise_test_environment(flsimulate-feature-issue241-validation)
