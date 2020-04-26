cmake_minimum_required(VERSION 3.3)
project(SNSimulation)

find_package(Geant4 REQUIRED gdml)
find_package(Boost REQUIRED thread)
if(Geant4_DATASETS)
  set(MCTOOLS_G4_DATA_ASSIGNMENTS)
  foreach(_ds ${Geant4_DATASETS})
    list(APPEND MCTOOLS_G4_DATA_ASSIGNMENTS "(\"${Geant4_DATASET_${_ds}_ENVVAR}\", \"${Geant4_DATASET_${_ds}_PATH}\")")
  endforeach()
  string(REPLACE ";" "" MCTOOLS_G4_DATA_ASSIGNMENTS ${MCTOOLS_G4_DATA_ASSIGNMENTS})
endif()

configure_file(snsim/data_libraries.h.in snsim/data_libraries.h @ONLY)

add_library(SNSimulation SHARED
  snsim/base_physics_constructor.h
  snsim/biasing_manager.h
  snsim/detector_construction.h
  snsim/electromagnetic_field.h
  snsim/em_field_equation_of_motion.h
  snsim/em_field_g4_stuff.h
  snsim/em_field_g4_utils.h
  snsim/em_physics_constructor.h
  snsim/event_action.h
  snsim/g4_prng.h
  snsim/loggable_support.h
  snsim/magnetic_field.h
  snsim/manager.h
  snsim/manager_parameters.h
  snsim/neutrons_physics_constructor.h
  snsim/particles_physics_constructor.h
  snsim/physics_list.h
  snsim/physics_list_utils.h
  snsim/primary_generator.h
  snsim/region_tools.h
  snsim/run_action.h
  snsim/sensitive_detector.h
  snsim/sensitive_hit.h
  snsim/sensitive_hit_collection.h
  snsim/simulation_ctrl.h
  snsim/simulation_module.h
  snsim/stacking_action.h
  snsim/stepping_action.h
  snsim/stepping_verbose.h
  snsim/track_history.h
  snsim/tracking_action.h
  snsim/processes/em_extra_models.h
  snsim/processes/em_model_factory.h
  snsim/processes/utils.h

  src/base_physics_constructor.cc
  src/biasing_manager.cc
  src/detector_construction.cc
  src/electromagnetic_field.cc
  src/em_field_equation_of_motion.cc
  src/em_field_g4_stuff.cc
  src/em_physics_constructor.cc
  src/event_action.cc
  src/g4_prng.cc
  src/loggable_support.cc
  src/magnetic_field.cc
  src/manager.cc
  src/manager_parameters.cc
  src/neutrons_physics_constructor.cc
  src/particles_physics_constructor.cc
  src/physics_list.cc
  src/physics_list_utils.cc
  src/primary_generator.cc
  src/region_tools.cc
  src/run_action.cc
  src/sensitive_detector.cc
  src/sensitive_hit.cc
  src/sensitive_hit_collection.cc
  src/simulation_ctrl.cc
  src/simulation_module.cc
  src/stacking_action.cc
  src/stepping_action.cc
  src/stepping_verbose.cc
  src/track_history.cc
  src/tracking_action.cc
  src/processes/em_extra_models.cc
  src/processes/em_model_factory.cc
  src/processes/utils.cc)

add_library(SNSimulation::SNSimulation ALIAS SNSimulation)

target_include_directories(SNSimulation PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
  $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
  ${Geant4_INCLUDE_DIRS})

# Hack - strip "-D" flag as we should only supply the def names
set(SNSimulation_DEFINITIONS)
foreach(_def ${Geant4_DEFINITIONS})
  string(REGEX REPLACE "^-D" "" _bxdef ${_def})
  list(APPEND SNSimulation_DEFINITIONS ${_bxdef})
endforeach()

set_target_properties(SNSimulation PROPERTIES COMPILE_DEFINITIONS "${SNSimulation_DEFINITIONS}")
target_link_libraries(SNSimulation PUBLIC Falaise Bayeux::Bayeux ${Geant4_LIBRARIES} Boost::thread)
target_clang_format(SNSimulation)
