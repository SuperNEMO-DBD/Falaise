
add_test(NAME flreconstruct-tkreconstruct-test1a
  COMMAND bash ${CMAKE_CURRENT_SOURCE_DIR}/test1/run.sh
  --cfg-dir "${CMAKE_CURRENT_SOURCE_DIR}/test1"
  --simu1
  --work-dir "${CMAKE_CURRENT_BINARY_DIR}/test1a"
  )
set_falaise_test_environment(flreconstruct-tkreconstruct-test1a)

add_test(NAME flreconstruct-tkreconstruct-test1b
  COMMAND bash ${CMAKE_CURRENT_SOURCE_DIR}/test1/run.sh
  --cfg-dir "${CMAKE_CURRENT_SOURCE_DIR}/test1"
  --simu2
  --work-dir "${CMAKE_CURRENT_BINARY_DIR}/test1b"
  )
set_falaise_test_environment(flreconstruct-tkreconstruct-test1b)

# end
