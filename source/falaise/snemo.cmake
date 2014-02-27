# SuperNEMO specific classes:

list(APPEND FalaiseLibrary_HEADERS
  snemo/datamodels/timestamp.h
  snemo/datamodels/timestamp.ipp

  snemo/datamodels/event_header.h
  snemo/datamodels/event_header.ipp

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

  snemo/simulation/cosmic_muon_generator.h
  snemo/simulation/gg_step_hit_processor.h
  #snemo/simulation/pmt_signal_generator.h

  snemo/processing/calorimeter_regime.h
  snemo/processing/geiger_regime.h
  snemo/processing/mock_calorimeter_s2c_module.h
  snemo/processing/mock_tracker_s2c_module.h
  snemo/processing/base_tracker_clusterizer.h
  snemo/processing/base_tracker_fitter.h
  snemo/processing/services.h

  snemo/cuts/event_header_cut.h
  snemo/cuts/simulated_data_cut.h
  snemo/cuts/calibrated_data_cut.h

  snemo/visualization/toy_display.h
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
  snemo/datamodels/raw_trigger_infos.cc
  snemo/datamodels/raw_calorimeter_hit.cc
  snemo/datamodels/raw_tracker_hit.cc
  snemo/datamodels/raw_data.cc
  snemo/datamodels/mock_raw_tracker_hit.cc
  snemo/datamodels/data_model.cc
  snemo/datamodels/the_serializable.cc
  snemo/datamodels/gg_track_utils.cc

  snemo/geometry/calo_locator.cc
  snemo/geometry/xcalo_locator.cc
  snemo/geometry/gg_locator.cc
  snemo/geometry/gveto_locator.cc
  snemo/geometry/locator_plugin.cc
  snemo/geometry/utils.cc

  snemo/processing/calorimeter_regime.cc
  snemo/processing/geiger_regime.cc
  snemo/processing/mock_calorimeter_s2c_module.cc
  snemo/processing/mock_tracker_s2c_module.cc
  snemo/processing/base_tracker_clusterizer.cc
  snemo/processing/base_tracker_fitter.cc
  snemo/processing/services.cc

  snemo/simulation/cosmic_muon_generator.cc
  snemo/simulation/gg_step_hit_processor.cc
  #snemo/simulation/pmt_signal_generator.cc

  snemo/visualization/toy_display.cc

  snemo/cuts/event_header_cut.cc
  snemo/cuts/simulated_data_cut.cc
  snemo/cuts/calibrated_data_cut.cc
  )

list(APPEND FalaiseLibrary_TESTS
  snemo/testing/test_snemo_datamodel_event_header.cxx
  snemo/testing/test_snemo_datamodel_raw_trigger_infos.cxx
  snemo/testing/test_snemo_datamodel_raw_tracker_hit.cxx
  snemo/testing/test_snemo_datamodel_raw_calorimeter_hit.cxx
  snemo/testing/test_snemo_datamodel_raw_data.cxx
  snemo/testing/test_snemo_datamodel_calibrated_calorimeter_hit.cxx
  snemo/testing/test_snemo_datamodel_calibrated_tracker_hit.cxx
  snemo/testing/test_snemo_datamodel_timestamp.cxx
  snemo/testing/test_snemo_datamodel_tracker_cluster.cxx
  snemo/testing/test_snemo_datamodel_tracker_clustering_data.cxx
  snemo/testing/test_snemo_datamodel_tracker_clustering_solution.cxx
  snemo/testing/test_snemo_datamodel_tracker_trajectory.cxx
  snemo/testing/test_snemo_datamodel_tracker_trajectory_solution.cxx
  snemo/testing/test_snemo_geometry_calo_locator_1.cxx
  snemo/testing/test_snemo_geometry_gg_locator_1.cxx
  snemo/testing/test_snemo_geometry_gveto_locator_1.cxx
  snemo/testing/test_snemo_geometry_retrieve_info.cxx
  snemo/testing/test_snemo_geometry_xcalo_locator_1.cxx
  #snemo/testing/test_snemo_simulation_pmt_signal_generator.cxx
  )
