# - List of test programs:
set(FalaiseDigitizationPlugin_HALF_COMMISSIONING_ANALYSIS
  hc_rate_analysis.cxx
  )

# Use C++11
#set(CMAKE_CXX_FLAGS "-std=c++11")

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${ROOT_INCLUDE_DIR})

# DEBUG :  message( STATUS "ROOT_LIBRARIES='${ROOT_LIBRARIES}'")

#${ROOT_INCLUDES})
foreach(_progsource ${FalaiseDigitizationPlugin_HALF_COMMISSIONING_ANALYSIS})
  get_filename_component(_progname ${_progsource} NAME_WE)
  set(_progname "half_commissioning-${_progname}")
  add_executable(${_progname} ${_progsource})
  target_link_libraries(${_progname} Falaise_Digitization
    ${ROOT_LIBRARIES}
    Falaise)
  # - On Apple, ensure dynamic_lookup of undefined symbols
  if(APPLE)
    set_target_properties(${_progname} PROPERTIES LINK_FLAGS "-undefined dynamic_lookup")
  endif()

endforeach()

# end of CMakeLists.txt
