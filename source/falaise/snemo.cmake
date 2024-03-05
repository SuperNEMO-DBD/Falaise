# SuperNEMO specific classes:

list(APPEND FalaiseLibrary_HEADERS
  snemo/datamodels/base_trajectory_pattern.h
  snemo/datamodels/calibrated_calorimeter_hit.h
  snemo/datamodels/calibrated_data.h
  snemo/datamodels/calibrated_tracker_hit.h
  snemo/datamodels/udd_utils.h
  snemo/datamodels/precalibrated_calorimeter_hit.h
  snemo/datamodels/precalibrated_data.h
  snemo/datamodels/precalibrated_tracker_hit.h
  snemo/datamodels/calorimeter_digitized_hit.h
  snemo/datamodels/unified_digitized_data.h
  snemo/datamodels/tracker_digitized_hit.h
  snemo/datamodels/data_model.h
  snemo/datamodels/event.h
  snemo/datamodels/event_header.h
  snemo/datamodels/geomid_utils.h
  snemo/datamodels/gg_track_utils.h
  snemo/datamodels/helix_trajectory_pattern.h
  snemo/datamodels/line_trajectory_pattern.h
  snemo/datamodels/particle_track.h
  snemo/datamodels/particle_track_data.h
  snemo/datamodels/polyline_trajectory_pattern.h
  snemo/datamodels/timestamp.h
  snemo/datamodels/vertex_utils.h
  snemo/datamodels/tracker_cluster.h
  snemo/datamodels/tracker_clustering_data.h
  snemo/datamodels/tracker_clustering_solution.h
  snemo/datamodels/track_fitting_utils.h
  snemo/datamodels/tracker_trajectory.h
  snemo/datamodels/tracker_trajectory_data.h
  snemo/datamodels/tracker_trajectory_solution.h

  snemo/datamodels/boost_io/base_trajectory_pattern.ipp
  snemo/datamodels/boost_io/calibrated_calorimeter_hit.ipp
  snemo/datamodels/boost_io/calibrated_data.ipp
  snemo/datamodels/boost_io/calibrated_tracker_hit.ipp
  snemo/datamodels/boost_io/precalibrated_calorimeter_hit.ipp
  snemo/datamodels/boost_io/precalibrated_data.ipp
  snemo/datamodels/boost_io/precalibrated_tracker_hit.ipp
  snemo/datamodels/boost_io/calorimeter_digitized_hit.ipp
  snemo/datamodels/boost_io/unified_digitized_data.ipp
  snemo/datamodels/boost_io/tracker_digitized_hit.ipp
  snemo/datamodels/boost_io/event_header.ipp
  snemo/datamodels/boost_io/helix_trajectory_pattern.ipp
  snemo/datamodels/boost_io/line_trajectory_pattern.ipp
  snemo/datamodels/boost_io/vertex_utils.ipp
  snemo/datamodels/boost_io/particle_track.ipp
  snemo/datamodels/boost_io/particle_track_data.ipp
  snemo/datamodels/boost_io/polyline_trajectory_pattern.ipp
  snemo/datamodels/boost_io/timestamp.ipp
  snemo/datamodels/boost_io/tracker_cluster.ipp
  snemo/datamodels/boost_io/tracker_clustering_data.ipp
  snemo/datamodels/boost_io/tracker_clustering_solution.ipp
  snemo/datamodels/boost_io/track_fitting_utils.ipp
  snemo/datamodels/boost_io/tracker_trajectory.ipp
  snemo/datamodels/boost_io/tracker_trajectory_data.ipp
  snemo/datamodels/boost_io/tracker_trajectory_solution.ipp

  snemo/geometry/config.h
  snemo/geometry/utils.h
  snemo/geometry/calo_locator.h
  snemo/geometry/xcalo_locator.h
  snemo/geometry/gg_locator.h
  snemo/geometry/gveto_locator.h
  snemo/geometry/locator_helpers.h
  snemo/geometry/locator_plugin.h
  snemo/geometry/mapped_magnetic_field.h
  snemo/geometry/helix_intercept.h
  snemo/geometry/manager.h

  snemo/simulation/config.h
  snemo/simulation/cosmic_muon_generator.h
  snemo/simulation/gg_step_hit_processor.h
  snemo/simulation/calorimeter_step_hit_processor.h
  snemo/simulation/arbitrary_event_generator_injector.h
  snemo/simulation/from_ascii_files_event_generator.h

  snemo/processing/config.h
  snemo/processing/calorimeter_regime.h
  snemo/processing/geiger_regime.h
  snemo/processing/base_tracker_clusterizer.h
  snemo/processing/base_tracker_fitter.h
  snemo/processing/filter.h
  snemo/processing/detector_description.h
  snemo/processing/module.h
  snemo/processing/black_hole_module.h
  snemo/processing/types.h
  snemo/processing/base_gamma_builder.h
  snemo/processing/detail/GeigerTimePartitioner.h
  snemo/processing/event_header_utils_module.h
  snemo/processing/udd2pcd_module.h
  snemo/processing/mock_calorimeter_s2c_module.h
  snemo/processing/mock_calorimeter_s2c_module_utils.h
  snemo/processing/mock_tracker_s2c_module.h

  snemo/services/services.h
  snemo/services/service_traits.h
  snemo/services/service_handle.h
  snemo/services/geometry.h
  snemo/services/hello_world.h
  snemo/services/dead_cells.h
  snemo/services/histogram.h
  snemo/services/db_service.h 
  snemo/services/run_info_service.h
  snemo/services/tracker_cell_status_service.h
  snemo/services/calorimeter_om_status_service.h

  snemo/cuts/event_header_cut.h
  snemo/cuts/simulated_data_cut.h

  snemo/time/time_utils.h

  snemo/physics_model/base_activity_model.h
  snemo/physics_model/constant_activity_model.h
  snemo/physics_model/scaled_activity_model.h
  snemo/physics_model/sliced_activity_model.h
  snemo/physics_model/decaying_activity_model.h
  snemo/physics_model/per_period_activity_model.h
  snemo/physics_model/activity_model_factory.h

  snemo/rc/run_description.h
  snemo/rc/run_list.h
  snemo/rc/base_event.h
  snemo/rc/run_event.h
  snemo/rc/run_statistics.h
  snemo/rc/mc_event_distribution.h
  snemo/rc/tracker_cell_status.h
  snemo/rc/calorimeter_om_status.h

)

list(APPEND FalaiseLibrary_SOURCES
  snemo/datamodels/timestamp.cc
  snemo/datamodels/event_header.cc
  snemo/datamodels/calibrated_calorimeter_hit.cc
  snemo/datamodels/calibrated_tracker_hit.cc
  snemo/datamodels/calibrated_data.cc
  snemo/datamodels/calorimeter_digitized_hit.cc
  snemo/datamodels/unified_digitized_data.cc
  snemo/datamodels/tracker_digitized_hit.cc
  snemo/datamodels/precalibrated_calorimeter_hit.cc
  snemo/datamodels/precalibrated_tracker_hit.cc
  snemo/datamodels/precalibrated_data.cc
  snemo/datamodels/tracker_cluster.cc
  snemo/datamodels/tracker_clustering_solution.cc
  snemo/datamodels/tracker_clustering_data.cc
  snemo/datamodels/base_trajectory_pattern.cc
  snemo/datamodels/line_trajectory_pattern.cc
  snemo/datamodels/polyline_trajectory_pattern.cc
  snemo/datamodels/helix_trajectory_pattern.cc
  snemo/datamodels/track_fitting_utils.cc
  snemo/datamodels/tracker_trajectory.cc
  snemo/datamodels/tracker_trajectory_solution.cc
  snemo/datamodels/tracker_trajectory_data.cc
  snemo/datamodels/particle_track.cc
  snemo/datamodels/particle_track_data.cc
  snemo/datamodels/data_model.cc
  snemo/datamodels/boost_io/the_serializable.cc
  snemo/datamodels/geomid_utils.cc
  snemo/datamodels/gg_track_utils.cc
  snemo/datamodels/vertex_utils.cc

  snemo/geometry/config.cc
  snemo/geometry/calo_locator.cc
  snemo/geometry/xcalo_locator.cc
  snemo/geometry/gg_locator.cc
  snemo/geometry/gveto_locator.cc
  snemo/geometry/locator_plugin.cc
  snemo/geometry/utils.cc
  snemo/geometry/mapped_magnetic_field.cc
  snemo/geometry/private/categories.h
  snemo/geometry/helix_intercept.cc
  snemo/geometry/manager.cc

  snemo/processing/config.cc
  snemo/processing/event_header_utils_module.cc
  snemo/processing/calorimeter_regime.cc
  snemo/processing/geiger_regime.cc
  snemo/processing/udd2pcd_module.cc
  snemo/processing/mock_calorimeter_s2c_module.cc
  snemo/processing/mock_calorimeter_s2c_module_utils.cc
  snemo/processing/mock_tracker_s2c_module.cc
  snemo/processing/detail/mock_raw_tracker_hit.h
  snemo/processing/black_hole_module.cc
  snemo/processing/base_tracker_clusterizer.cc
  snemo/processing/base_tracker_fitter.cc
  snemo/processing/base_gamma_builder.cc
  snemo/processing/detail/mock_raw_tracker_hit.cc
  snemo/processing/detail/GeigerTimePartitioner.cc
  snemo/processing/detail/testing/gg_hit.h
  snemo/processing/detail/testing/gg_hit.cc
  snemo/processing/detail/testing/event_generator.h
  snemo/processing/detail/testing/event_display.h
  snemo/processing/detail/testing/event_display.tpp
  snemo/processing/detail/testing/event_generator.cc
  snemo/processing/detail/testing/event_display.cc

  snemo/services/services.cc
  snemo/services/hello_world.cc
  snemo/services/dead_cells.cc
  snemo/services/db_service.cc
  snemo/services/run_info_service.cc
  snemo/services/tracker_cell_status_service.cc
  snemo/services/calorimeter_om_status_service.cc

  snemo/simulation/config.cc
  snemo/simulation/cosmic_muon_generator.cc
  snemo/simulation/gg_step_hit_processor.cc
  snemo/simulation/calorimeter_step_hit_processor.cc
  snemo/simulation/arbitrary_event_generator_injector.cc
  snemo/simulation/from_ascii_files_event_generator.cc

  snemo/cuts/event_header_cut.cc
  snemo/cuts/simulated_data_cut.cc

  snemo/time/time_utils.cc

  snemo/physics_model/base_activity_model.cc
  snemo/physics_model/constant_activity_model.cc
  snemo/physics_model/scaled_activity_model.cc
  snemo/physics_model/sliced_activity_model.cc
  snemo/physics_model/decaying_activity_model.cc
  snemo/physics_model/per_period_activity_model.cc
  snemo/physics_model/activity_model_factory.cc

  snemo/rc/run_description.cc
  snemo/rc/run_list.cc
  snemo/rc/base_event.cc
  snemo/rc/run_event.cc
  snemo/rc/run_statistics.cc
  snemo/rc/mc_event_distribution.cc
  snemo/rc/tracker_cell_status.cc
  snemo/rc/calorimeter_om_status.cc

)

list(APPEND FalaiseLibrary_TESTS_CATCH
  snemo/test/test_snemo_datamodel_event.cxx
  snemo/test/test_snemo_datamodel_timestamp.cxx
  snemo/test/test_snemo_geometry_gveto_locator_2.cxx
  snemo/test/test_filter.cxx
  snemo/test/test_module.cxx
  snemo/test/test_service.cxx
  snemo/test/test_dead_cells_service.cxx
  snemo/test/test_event_record.cxx
)

list(APPEND FalaiseLibrary_TESTS
  snemo/test/test_service_external.cxx
  snemo/test/test_snemo_datamodel_event_header.cxx
  snemo/test/test_snemo_datamodel_calibrated_calorimeter_hit.cxx
  snemo/test/test_snemo_datamodel_calibrated_tracker_hit.cxx
  snemo/test/test_snemo_datamodel_precalibrated_calorimeter_hit.cxx
  snemo/test/test_snemo_datamodel_precalibrated_tracker_hit.cxx
  snemo/test/test_snemo_datamodel_calorimeter_digitized_hit.cxx
  snemo/test/test_snemo_datamodel_tracker_digitized_hit.cxx
  snemo/test/test_snemo_datamodel_unified_digitized_data.cxx
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
  snemo/test/test_snemo_time_time_utils.cxx
  snemo/test/test_snemo_rc_run_description.cxx
  snemo/test/test_snemo_rc_run_list.cxx
  snemo/test/test_snemo_rc_run_event.cxx
  snemo/test/test_snemo_rc_run_statistics.cxx
  snemo/test/test_snemo_rc_tracker_cell_status.cxx
  snemo/test/test_snemo_rc_calorimeter_om_status.cxx
  snemo/test/test_snemo_physics_model_sliced_activity_model.cxx
  snemo/test/test_snemo_physics_model_per_period_activity_model.cxx
  snemo/test/test_snemo_physics_model_activity_model_factory.cxx
  snemo/test/test_snemo_services_run_info_service.cxx
  snemo/test/test_snemo_services_tracker_cell_status_service.cxx
  snemo/test/test_snemo_services_calorimeter_om_status_service.cxx
  # snemo/test/test_snemo_service_rc.cxx

  snemo/processing/detail/testing/test_trackerpreclustering.cxx

)

# - end
