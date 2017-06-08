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
          -s "geometry:layout=Basic"
          -s "vertexes:generator=field_wire_bulk"
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
# - Validatio of issue #16 fix
add_test(NAME flsimulate-fix-issue16-validation
  COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/fix-issue16-validation/run.sh
  --work-dir "${CMAKE_CURRENT_BINARY_DIR}"
  --cfg-dir "${CMAKE_CURRENT_SOURCE_DIR}/fix-issue16-validation"
  )
set_tests_properties(flsimulate-fix-issue16-validation
   PROPERTIES ENVIRONMENT "PATH=${PROJECT_BUILD_BINDIR}:$ENV{PATH}")

# Test implmentation of uniform_energy generator variant
# 1) Test basic usage
# 1.1) Create single event output
add_test(NAME flsimulate-uniform_energy_generator
  COMMAND flsimulate
          -c "${CMAKE_CURRENT_SOURCE_DIR}/flsimulate-uniform-energy-generator.conf"
          -o "${CMAKE_CURRENT_BINARY_DIR}/fisimulate-uniform_energy-run.brio"
  )
# 1.2) Validate that output primary event is of
#      - Correct label
#      - Correct particle [TODO]
#      - Energy in right range [TODO]
add_test(NAME flsimulate-uniform_energy_generator-validate
  COMMAND flreconstruct -i "${CMAKE_CURRENT_BINARY_DIR}/fisimulate-uniform_energy-run.brio"
  )
set_tests_properties(flsimulate-uniform_energy_generator-validate
  PROPERTIES
    DEPENDS_ON flsimulate-uniform_energy_generator
    PASS_REGULAR_EXPRESSION "     |   |-- Label : 'uniform_energy_generator'"
  )

# 2) Test failure case of bad particle name
add_test(NAME flsimulate-uniform_energy_generator-badparticle
  COMMAND flsimulate
          -c "${CMAKE_CURRENT_SOURCE_DIR}/flsimulate-uniform-energy-generator-badparticle.conf"
          -o "${CMAKE_CURRENT_BINARY_DIR}/bad.brio"
  )

# 3) Test failure case of max energy < min energy
add_test(NAME flsimulate-uniform_energy_generator-badrange
  COMMAND flsimulate
          -c "${CMAKE_CURRENT_SOURCE_DIR}/flsimulate-uniform-energy-generator-badrange.conf"
          -o "${CMAKE_CURRENT_BINARY_DIR}/fisimulate-uniform_energy-run.brio"
  )

set_tests_properties(flsimulate-uniform_energy_generator-badparticle flsimulate-uniform_energy_generator-badrange
  PROPERTIES WILL_FAIL TRUE
  )



