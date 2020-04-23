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

configure_file(mctools/g4/data_libraries.h.in mctools/g4/data_libraries.h @ONLY)

add_library(SNSimulation SHARED
  mctools/g4/base_physics_constructor.h
  mctools/g4/biasing_manager.h
  mctools/g4/detector_construction.h
  mctools/g4/electromagnetic_field.h
  mctools/g4/em_field_equation_of_motion.h
  mctools/g4/em_field_g4_stuff.h
  mctools/g4/em_field_g4_utils.h
  mctools/g4/em_physics_constructor.h
  mctools/g4/event_action.h
  mctools/g4/g4_prng.h
  mctools/g4/loggable_support.h
  mctools/g4/magnetic_field.h
  mctools/g4/manager.h
  mctools/g4/manager_parameters.h
  mctools/g4/neutrons_physics_constructor.h
  mctools/g4/particles_physics_constructor.h
  mctools/g4/physics_list.h
  mctools/g4/physics_list_utils.h
  mctools/g4/primary_generator.h
  mctools/g4/region_tools.h
  mctools/g4/run_action.h
  mctools/g4/sensitive_detector.h
  mctools/g4/sensitive_hit.h
  mctools/g4/sensitive_hit_collection.h
  mctools/g4/simulation_ctrl.h
  mctools/g4/simulation_module.h
  mctools/g4/stacking_action.h
  mctools/g4/stepping_action.h
  mctools/g4/stepping_verbose.h
  mctools/g4/track_history.h
  mctools/g4/tracking_action.h
  mctools/g4/processes/em_extra_models.h
  mctools/g4/processes/em_model_factory.h
  mctools/g4/processes/utils.h

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
target_link_libraries(SNSimulation PUBLIC Bayeux::Bayeux ${Geant4_LIBRARIES} Boost::thread)
target_clang_format(SNSimulation)
