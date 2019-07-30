# SuperNEMO specific classes:

list(APPEND FalaiseLibrary_HEADERS
  snemo/datamodels/base_trajectory_pattern.h
  snemo/datamodels/calibrated_calorimeter_hit.h
  snemo/datamodels/calibrated_data.h
  snemo/datamodels/calibrated_tracker_hit.h
  snemo/datamodels/data_model.h
  snemo/datamodels/event.h
  snemo/datamodels/event_header.h
  snemo/datamodels/gg_track_utils.h
  snemo/datamodels/helix_trajectory_pattern.h
  snemo/datamodels/line_trajectory_pattern.h
  snemo/datamodels/mock_raw_tracker_hit.h
  snemo/datamodels/particle_track.h
  snemo/datamodels/particle_track_data.h
  snemo/datamodels/polyline_trajectory_pattern.h
  snemo/datamodels/timestamp.h
  snemo/datamodels/tracker_cluster.h
  snemo/datamodels/tracker_clustering_data.h
  snemo/datamodels/tracker_clustering_solution.h
  snemo/datamodels/tracker_trajectory.h
  snemo/datamodels/tracker_trajectory_data.h
  snemo/datamodels/tracker_trajectory_solution.h

  snemo/datamodels/boost_io/base_trajectory_pattern.ipp
  snemo/datamodels/boost_io/calibrated_calorimeter_hit.ipp
  snemo/datamodels/boost_io/calibrated_data.ipp
  snemo/datamodels/boost_io/calibrated_tracker_hit.ipp
  snemo/datamodels/boost_io/event_header.ipp
  snemo/datamodels/boost_io/helix_trajectory_pattern.ipp
  snemo/datamodels/boost_io/line_trajectory_pattern.ipp
  snemo/datamodels/boost_io/particle_track.ipp
  snemo/datamodels/boost_io/particle_track_data.ipp
  snemo/datamodels/boost_io/polyline_trajectory_pattern.ipp
  snemo/datamodels/boost_io/timestamp.ipp
  snemo/datamodels/boost_io/tracker_cluster.ipp
  snemo/datamodels/boost_io/tracker_clustering_data.ipp
  snemo/datamodels/boost_io/tracker_clustering_solution.ipp
  snemo/datamodels/boost_io/tracker_trajectory.ipp
  snemo/datamodels/boost_io/tracker_trajectory_data.ipp
  snemo/datamodels/boost_io/tracker_trajectory_solution.ipp


  snemo/geometry/utils.h
  snemo/geometry/calo_locator.h
  snemo/geometry/xcalo_locator.h
  snemo/geometry/gg_locator.h
  snemo/geometry/gveto_locator.h
  snemo/geometry/locator_helpers.h
  snemo/geometry/locator_plugin.h
  snemo/geometry/mapped_magnetic_field.h

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
  snemo/processing/module.h
  snemo/processing/base_gamma_builder.h

  snemo/services/services.h
  snemo/services/service_traits.h
  snemo/services/service_handle.h
  snemo/services/geometry.h
  snemo/services/histogram.h

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
  snemo/datamodels/mock_raw_tracker_hit.cc
  snemo/datamodels/data_model.cc
  snemo/datamodels/boost_io/the_serializable.cc
  snemo/datamodels/gg_track_utils.cc

  snemo/geometry/calo_locator.cc
  snemo/geometry/xcalo_locator.cc
  snemo/geometry/gg_locator.cc
  snemo/geometry/gveto_locator.cc
  snemo/geometry/locator_plugin.cc
  snemo/geometry/utils.cc
  snemo/geometry/mapped_magnetic_field.cc

  snemo/processing/event_header_utils_module.cc
  snemo/processing/calorimeter_regime.cc
  snemo/processing/geiger_regime.cc
  snemo/processing/mock_calorimeter_s2c_module.cc
  snemo/processing/mock_tracker_s2c_module.cc
  snemo/processing/base_tracker_clusterizer.cc
  snemo/processing/base_tracker_fitter.cc
  snemo/processing/base_gamma_builder.cc

  snemo/services/services.cc

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
  snemo/test/test_snemo_datamodel_event.cxx
  snemo/test/test_snemo_datamodel_timestamp.cxx
  snemo/test/test_module.cxx
  snemo/test/test_service.cxx
  )
list(APPEND FalaiseLibrary_TESTS
  snemo/test/test_snemo_datamodel_event_header.cxx
  snemo/test/test_snemo_datamodel_calibrated_calorimeter_hit.cxx
  snemo/test/test_snemo_datamodel_calibrated_tracker_hit.cxx
  snemo/test/test_snemo_datamodel_tracker_cluster.cxx
  snemo/test/test_snemo_datamodel_tracker_clustering_data.cxx
  snemo/test/test_snemo_datamodel_tracker_clustering_solution.cxx
  snemo/test/test_snemo_datamodel_tracker_trajectory.cxx
  snemo/test/test_snemo_datamodel_tracker_trajectory_solution.cxx
  snemo/test/test_snemo_datamodel_particle_track.cxx
  snemo/test/test_snemo_datamodel_particle_track_data.cxx
  snemo/test/test_snemo_geometry_calo_locator_1.cxx
  snemo/test/test_snemo_geometry_gg_locator_1.cxx
  snemo/test/test_snemo_geometry_gveto_locator_1.cxx
  snemo/test/test_snemo_geometry_retrieve_info.cxx
  snemo/test/test_snemo_geometry_xcalo_locator_1.cxx
  snemo/test/test_snemo_geometry_mapped_magnetic_field.cxx

  snemo/test/test_snemo_cut_particle_track_cut.cxx
  )
