# - List of test programs:
set(FalaiseDigitizationPlugin_VALIDATION
  trigger_algorithm_efficiency_validation.cxx
  trigger_algorithm_efficiency_analysis.cxx
  )

# Use C++11
#set(CMAKE_CXX_FLAGS "-std=c++11")

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

foreach(_progsource ${FalaiseDigitizationPlugin_VALIDATION})
  get_filename_component(_progname ${_progsource} NAME_WE)
  set(_progname "${_progname}")
  add_executable(${_progname} ${_progsource})
  target_link_libraries(${_progname} Falaise_Digitization)
  # - On Apple, ensure dynamic_lookup of undefined symbols
  if(APPLE)
    set_target_properties(${_progname} PROPERTIES LINK_FLAGS "-undefined dynamic_lookup")
  endif()

endforeach()

# end of CMakeLists.txt