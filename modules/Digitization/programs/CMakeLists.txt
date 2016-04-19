# - List of test programs:
set(FalaiseDigitizationPlugin_PROGRAMS
  fldigi_tracker_trigger_mem_maker.cxx
  fldigi_tracker_trigger_mem_maker_new_strategy.cxx
  fldigi_delayed_alpha_near_source_validation.cxx
  fldigi_delayed_alpha_near_source_analysis.cxx
  fldigi_delayed_alpha_validation.cxx
  fldigi_spatial_distribution.cxx
  sd_physical_cuts.cxx
  )

# Use C++11
#set(CMAKE_CXX_FLAGS "-std=c++11")

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${ROOT_INCLUDE_DIR})

# DEBUG :  message( STATUS "ROOT_LIBRARIES='${ROOT_LIBRARIES}'")

#${ROOT_INCLUDES})
foreach(_progsource ${FalaiseDigitizationPlugin_PROGRAMS})
  get_filename_component(_progname ${_progsource} NAME_WE)
  set(_progname "falaisedigitizationplugin-${_progname}")
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
