#.rst: Build/run tests for flsimulate{-configure}
# Scripts are taken from documentation
# TODO: crosslink these so there is one set that can
#       evolve together.

# Basic smoke test of default behaviour
# - write single event of default setup to output file
add_test(NAME flsimulate-smoke-test
  COMMAND flsimulate -o "${CMAKE_CURRENT_BINARY_DIR}/flsimulate-smoke-test.brio"
  )

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

# Tests dedicated to the validation of fixes
# - Validation of issue #16 fix
add_test(NAME flsimulate-fix-issue16-validation
  COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/fix-issue16-validation/run.sh
  --work-dir "${CMAKE_CURRENT_BINARY_DIR}"
  --cfg-dir "${CMAKE_CURRENT_SOURCE_DIR}/fix-issue16-validation"
  )
set_tests_properties(flsimulate-fix-issue16-validation
   PROPERTIES ENVIRONMENT "PATH=${PROJECT_BUILD_BINDIR}:$ENV{PATH}")

# - Flsimulate issue #21
# - Variant profile is given : flsimulate-issue21.profile
add_test(NAME flsimulate-issue21-run
  COMMAND flsimulate
          -d "issue21@${CMAKE_CURRENT_BINARY_DIR}"
          -d "issue21variant@${CMAKE_CURRENT_SOURCE_DIR}/"
          -d "issue21rng@${CMAKE_CURRENT_SOURCE_DIR}/data/"
          -c "${CMAKE_CURRENT_SOURCE_DIR}/flsimulate-issue21.conf" -o "${CMAKE_CURRENT_BINARY_DIR}/issue21.brio"
  )

# Tests dedicated to the validation of fixes
# - Validation of issue #59 fix
add_test(NAME flsimulate-fix-issue59-validation
  COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/fix-issue59-validation/run.sh
  --work-dir "${CMAKE_CURRENT_BINARY_DIR}"
  --cfg-dir "${CMAKE_CURRENT_SOURCE_DIR}/fix-issue59-validation"
  )
set_tests_properties(flsimulate-fix-issue59-validation
   PROPERTIES ENVIRONMENT "PATH=${PROJECT_BUILD_BINDIR}:$ENV{PATH}")

# Add test material for issue fixes here...

# - end