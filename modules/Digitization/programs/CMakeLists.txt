add_executable(fldigi_tracker_trigger_mem_maker 
  fldigi_tracker_trigger_mem_maker.cxx)

add_executable(fldigi_delayed_alpha_near_source_validation
  fldigi_delayed_alpha_near_source_validation)

add_executable(fldigi_delayed_alpha_near_source_analysis
  fldigi_delayed_alpha_near_source_analysis.cxx)

target_link_libraries(fldigi_tracker_trigger_mem_maker 
  Falaise_Digitization 
  Falaise)

target_link_libraries(fldigi_delayed_alpha_near_source_validation
  Falaise_Digitization 
  Falaise)

target_link_libraries(fldigi_delayed_alpha_near_source_analysis
  Falaise_Digitization 
  Falaise)


# Install it:
# install(TARGETS fldigi_tracker_trigger_mem_maker ${CMAKE_INSTALL_BINDIR})