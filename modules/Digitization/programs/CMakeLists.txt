add_executable(fldigi_tracker_trigger_mem_maker 
  fldigi_tracker_trigger_mem_maker.cxx)

target_link_libraries(fldigi_tracker_trigger_mem_maker 
  Falaise_Digitization 
  Falaise)

# Install it:
# install(TARGETS fldigi_tracker_trigger_mem_maker ${CMAKE_INSTALL_BINDIR})