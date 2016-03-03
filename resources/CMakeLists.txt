# - Package resource files as needed
# Because there are many resources, use a script based GLOB to locate
# them together with a custom target to rebuild the resources
#

add_custom_target(falaise_resources
  COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/_BuildResourceList.sh
  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
  COMMENT "Bundling Falaise Resources"
  )


include(_resources.cmake)
foreach(_rfin ${Falaise_RESOURCES})
  string(REGEX REPLACE "\\.in$" "" _rfout "${_rfin}")
  string(REGEX REPLACE "^${PROJECT_SOURCE_DIR}/resources" "${FALAISE_DATAROOTDIR}/resources" _rfout "${_rfout}")
  get_filename_component (_rfin_ext ${_rfin} EXT)
  # message(STATUS "Resource file extension is '${_rfin_ext}'")
  if (_rfin_ext STREQUAL ".in")
    configure_file(${_rfin} ${_rfout} @ONLY)
  else()
    configure_file(${_rfin} ${_rfout} COPYONLY)
  endif()
endforeach()
