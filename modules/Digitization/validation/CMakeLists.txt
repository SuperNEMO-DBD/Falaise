add_executable(trigger_algorithm_efficiency_validation
  trigger_algorithm_efficiency_validation.cxx)

target_link_libraries(trigger_algorithm_efficiency_validation 
  Falaise_Digitization 
  Falaise)

# Install it:
# install(TARGETS trigger_algorithm_efficiency_validation ${CMAKE_INSTALL_BINDIR})