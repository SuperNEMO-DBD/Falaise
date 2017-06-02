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



