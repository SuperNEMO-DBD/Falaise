# SuperNEMO specific classes:

list(APPEND FalaiseLibrary_HEADERS
  snemo/datamodels/timestamp.h
  snemo/datamodels/timestamp.ipp

  snemo/datamodels/event_header.h
  snemo/datamodels/event_header.ipp

  snemo/datamodels/sim_trigger_digi_data.h
  snemo/datamodels/sim_trigger_digi_data.ipp
  snemo/datamodels/sim_readout_digi_data.h
  snemo/datamodels/sim_readout_digi_data.ipp
  snemo/datamodels/sim_tracker_digi_hit.h
  snemo/datamodels/sim_tracker_digi_hit.ipp
  snemo/datamodels/sim_calo_digi_hit.h
  snemo/datamodels/sim_calo_digi_hit.ipp
  snemo/datamodels/sim_digi_event_data.h
  snemo/datamodels/sim_digi_event_data.ipp
  snemo/datamodels/sim_digi_data.h
  snemo/datamodels/sim_digi_data.ipp

  snemo/datamodels/raw_trigger_infos.h
  snemo/datamodels/raw_trigger_infos.ipp
  snemo/datamodels/raw_calorimeter_hit.h
  snemo/datamodels/raw_calorimeter_hit.ipp
  snemo/datamodels/raw_tracker_hit.h
  snemo/datamodels/raw_tracker_hit.ipp
  snemo/datamodels/raw_data.h
  snemo/datamodels/raw_data.ipp
  snemo/datamodels/mock_raw_tracker_hit.h

  snemo/datamodels/calibrated_calorimeter_hit.h
  snemo/datamodels/calibrated_calorimeter_hit.ipp
  snemo/datamodels/calibrated_tracker_hit.h
  snemo/datamodels/calibrated_tracker_hit.ipp
  snemo/datamodels/calibrated_data.h
  snemo/datamodels/calibrated_data.ipp

  snemo/datamodels/tracker_cluster.h
  snemo/datamodels/tracker_cluster.ipp
  snemo/datamodels/tracker_clustering_solution.h
  snemo/datamodels/tracker_clustering_solution.ipp
  snemo/datamodels/tracker_clustering_data.h
  snemo/datamodels/tracker_clustering_data.ipp

  snemo/datamodels/base_trajectory_pattern.h
  snemo/datamodels/base_trajectory_pattern.ipp
  snemo/datamodels/line_trajectory_pattern.h
  snemo/datamodels/line_trajectory_pattern.ipp
  snemo/datamodels/polyline_trajectory_pattern.h
  snemo/datamodels/polyline_trajectory_pattern.ipp
  snemo/datamodels/helix_trajectory_pattern.h
  snemo/datamodels/helix_trajectory_pattern.ipp
  snemo/datamodels/tracker_trajectory.h
  snemo/datamodels/tracker_trajectory.ipp
  snemo/datamodels/tracker_trajectory_solution.h
  snemo/datamodels/tracker_trajectory_solution.ipp
  snemo/datamodels/tracker_trajectory_data.h
  snemo/datamodels/tracker_trajectory_data.ipp
  snemo/datamodels/particle_track.h
  snemo/datamodels/particle_track.ipp
  snemo/datamodels/particle_track_data.h
  snemo/datamodels/particle_track_data.ipp

  snemo/datamodels/base_topology_measurement.h
  snemo/datamodels/base_topology_measurement.ipp
  snemo/datamodels/angle_measurement.h
  snemo/datamodels/angle_measurement.ipp
  snemo/datamodels/energy_measurement.h
  snemo/datamodels/energy_measurement.ipp
  snemo/datamodels/tof_measurement.h
  snemo/datamodels/tof_measurement.ipp
  snemo/datamodels/vertex_measurement.h
  snemo/datamodels/vertex_measurement.ipp
  snemo/datamodels/base_topology_pattern.h
  snemo/datamodels/base_topology_pattern.ipp
  snemo/datamodels/topology_1e_pattern.h
  snemo/datamodels/topology_1e_pattern.ipp
  snemo/datamodels/topology_1eNg_pattern.h
  snemo/datamodels/topology_1eNg_pattern.ipp
  snemo/datamodels/topology_1e1a_pattern.h
  snemo/datamodels/topology_1e1a_pattern.ipp
  snemo/datamodels/topology_2e_pattern.h
  snemo/datamodels/topology_2e_pattern.ipp
  snemo/datamodels/topology_2eNg_pattern.h
  snemo/datamodels/topology_2eNg_pattern.ipp
  snemo/datamodels/topology_data.h
  snemo/datamodels/topology_data.ipp

  snemo/datamodels/data_model.h

  snemo/datamodels/the_serializable.h
  snemo/datamodels/the_serializable.ipp

  snemo/datamodels/gg_track_utils.h

  snemo/geometry/utils.h
  snemo/geometry/calo_locator.h
  snemo/geometry/xcalo_locator.h
  snemo/geometry/gg_locator.h
  snemo/geometry/gveto_locator.h
  snemo/geometry/locator_plugin.h
  snemo/geometry/mapped_magnetic_field.h

  snemo/electronics/constants.h
  snemo/electronics/cabling.h
  snemo/electronics/manager.h
  snemo/electronics/electronics_service.h

  snemo/simulation/cosmic_muon_generator.h
  snemo/simulation/gg_step_hit_processor.h
  snemo/simulation/calorimeter_step_hit_processor.h

  snemo/processing/event_header_utils_module.h
  snemo/processing/calorimeter_regime.h
  snemo/processing/geiger_regime.h
  snemo/processing/mock_calorimeter_s2c_module.h
  snemo/processing/mock_tracker_s2c_module.h
  snemo/processing/base_tracker_clusterizer.h
  snemo/processing/base_tracker_fitter.h
  snemo/processing/services.h

  snemo/processing/base_gamma_builder.h

  snemo/cuts/event_header_cut.h
  snemo/cuts/simulated_data_cut.h
  snemo/cuts/calibrated_data_cut.h
  snemo/cuts/tracker_clustering_data_cut.h
  snemo/cuts/tracker_trajectory_data_cut.h
  snemo/cuts/particle_track_data_cut.h

  snemo/cuts/particle_track_cut.h
  )

list(APPEND FalaiseLibrary_SOURCES
  snemo/datamodels/timestamp.cc
  snemo/datamodels/event_header.cc
  snemo/datamodels/calibrated_calorimeter_hit.cc
  snemo/datamodels/calibrated_tracker_hit.cc
  snemo/datamodels/calibrated_data.cc
  snemo/datamodels/tracker_cluster.cc
  snemo/datamodels/tracker_clustering_solution.cc
  snemo/datamodels/tracker_clustering_data.cc
  snemo/datamodels/base_trajectory_pattern.cc
  snemo/datamodels/line_trajectory_pattern.cc
  snemo/datamodels/polyline_trajectory_pattern.cc
  snemo/datamodels/helix_trajectory_pattern.cc
  snemo/datamodels/tracker_trajectory.cc
  snemo/datamodels/tracker_trajectory_solution.cc
  snemo/datamodels/tracker_trajectory_data.cc
  snemo/datamodels/particle_track.cc
  snemo/datamodels/particle_track_data.cc
  snemo/datamodels/raw_trigger_infos.cc
  snemo/datamodels/raw_calorimeter_hit.cc
  snemo/datamodels/raw_tracker_hit.cc
  snemo/datamodels/raw_data.cc
  snemo/datamodels/mock_raw_tracker_hit.cc
  snemo/datamodels/data_model.cc
  snemo/datamodels/the_serializable.cc
  snemo/datamodels/gg_track_utils.cc

  snemo/datamodels/base_topology_measurement.cc
  snemo/datamodels/angle_measurement.cc
  snemo/datamodels/energy_measurement.cc
  snemo/datamodels/tof_measurement.cc
  snemo/datamodels/vertex_measurement.cc
  snemo/datamodels/base_topology_pattern.cc
  snemo/datamodels/topology_1e_pattern.cc
  snemo/datamodels/topology_1eNg_pattern.cc
  snemo/datamodels/topology_1e1a_pattern.cc
  snemo/datamodels/topology_2e_pattern.cc
  snemo/datamodels/topology_2eNg_pattern.cc
  snemo/datamodels/topology_data.cc

  snemo/datamodels/sim_trigger_digi_data.cc
  snemo/datamodels/sim_readout_digi_data.cc
  snemo/datamodels/sim_tracker_digi_hit.cc
  snemo/datamodels/sim_calo_digi_hit.cc
  snemo/datamodels/sim_digi_event_data.cc
  snemo/datamodels/sim_digi_data.cc

  snemo/geometry/calo_locator.cc
  snemo/geometry/xcalo_locator.cc
  snemo/geometry/gg_locator.cc
  snemo/geometry/gveto_locator.cc
  snemo/geometry/locator_plugin.cc
  snemo/geometry/utils.cc
  snemo/geometry/mapped_magnetic_field.cc

  snemo/electronics/constants.cc
  snemo/electronics/cabling.cc
  snemo/electronics/manager.cc
  snemo/electronics/electronics_service.cc

  snemo/processing/event_header_utils_module.cc
  snemo/processing/calorimeter_regime.cc
  snemo/processing/geiger_regime.cc
  snemo/processing/mock_calorimeter_s2c_module.cc
  snemo/processing/mock_tracker_s2c_module.cc
  snemo/processing/base_tracker_clusterizer.cc
  snemo/processing/base_tracker_fitter.cc
  snemo/processing/services.cc

  snemo/processing/base_gamma_builder.cc

  snemo/simulation/cosmic_muon_generator.cc
  snemo/simulation/gg_step_hit_processor.cc
  snemo/simulation/calorimeter_step_hit_processor.cc

  snemo/cuts/event_header_cut.cc
  snemo/cuts/simulated_data_cut.cc
  snemo/cuts/calibrated_data_cut.cc
  snemo/cuts/tracker_clustering_data_cut.cc
  snemo/cuts/tracker_trajectory_data_cut.cc
  snemo/cuts/particle_track_data_cut.cc
  snemo/cuts/particle_track_cut.cc
  )

list(APPEND FalaiseLibrary_TESTS_CATCH
  snemo/testing/test_snemo_datamodel_timestamp.cxx
  )
list(APPEND FalaiseLibrary_TESTS
  snemo/testing/test_snemo_datamodel_event_header.cxx
  snemo/testing/test_snemo_datamodel_raw_trigger_infos.cxx
  snemo/testing/test_snemo_datamodel_raw_tracker_hit.cxx
  snemo/testing/test_snemo_datamodel_raw_calorimeter_hit.cxx
  snemo/testing/test_snemo_datamodel_raw_data.cxx
  snemo/testing/test_snemo_datamodel_calibrated_calorimeter_hit.cxx
  snemo/testing/test_snemo_datamodel_calibrated_tracker_hit.cxx
  snemo/testing/test_snemo_datamodel_tracker_cluster.cxx
  snemo/testing/test_snemo_datamodel_tracker_clustering_data.cxx
  snemo/testing/test_snemo_datamodel_tracker_clustering_solution.cxx
  snemo/testing/test_snemo_datamodel_tracker_trajectory.cxx
  snemo/testing/test_snemo_datamodel_tracker_trajectory_solution.cxx
  snemo/testing/test_snemo_datamodel_particle_track.cxx
  snemo/testing/test_snemo_datamodel_particle_track_data.cxx
  snemo/testing/test_snemo_datamodel_vertex_measurement.cxx
  snemo/testing/test_snemo_datamodel_tof_measurement.cxx
  snemo/testing/test_snemo_datamodel_base_topology_pattern.cxx
  snemo/testing/test_snemo_datamodel_topology_data.cxx
  snemo/testing/test_snemo_geometry_calo_locator_1.cxx
  snemo/testing/test_snemo_geometry_gg_locator_1.cxx
  snemo/testing/test_snemo_geometry_gveto_locator_1.cxx
  snemo/testing/test_snemo_geometry_retrieve_info.cxx
  snemo/testing/test_snemo_geometry_xcalo_locator_1.cxx
  snemo/testing/test_snemo_geometry_mapped_magnetic_field.cxx
  # snemo/testing/test_snemo_electronics_mapping.cxx

  snemo/testing/test_snemo_cut_particle_track_cut.cxx
  snemo/testing/test_snemo_datamodel_sim_calo_digi_hit.cxx
  snemo/testing/test_snemo_datamodel_sim_tracker_digi_hit.cxx
  snemo/testing/test_snemo_datamodel_sim_digi_data.cxx
  )
