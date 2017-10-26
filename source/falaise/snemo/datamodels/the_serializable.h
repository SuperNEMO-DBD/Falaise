// -*- mode: c++; -*-
/// \file falaise/snemo/datamodels/the_serializable.h

#ifndef FALAISE_SNEMO_DATAMODELS_THE_SERIALIZABLE_H
#define FALAISE_SNEMO_DATAMODELS_THE_SERIALIZABLE_H 1

// Third party:
// - Boost:
#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-local-typedef"
#endif
#ifdef __GNUC__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-local-typedefs"
#pragma GCC diagnostic ignored "-Wshadow"
#endif
#include <boost/serialization/export.hpp>
#ifdef __GNUC__
#pragma GCC diagnostic pop
#endif
#ifdef __clang__
#pragma clang diagnostic pop
#endif

// - Bayeux/datatools:
#ifdef __GNUC__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wshadow"
#endif
#include <datatools/archives_instantiation.h>
#ifdef __GNUC__
#pragma GCC diagnostic pop
#endif

/*******************************
 * snemo::datamodel::timestamp *
 *******************************/

#include <falaise/snemo/datamodels/timestamp.ipp>
DATATOOLS_SERIALIZATION_CLASS_SERIALIZE_INSTANTIATE_ALL(snemo::datamodel::timestamp)

/**********************************
 * snemo::datamodel::event_header *
 **********************************/

#include <falaise/snemo/datamodels/event_header.ipp>
DATATOOLS_SERIALIZATION_CLASS_SERIALIZE_INSTANTIATE_ALL(snemo::datamodel::event_header)
BOOST_CLASS_EXPORT_IMPLEMENT(snemo::datamodel::event_header)

/***************************************
 * snemo::datamodel::raw_trigger_infos *
 ***************************************/

#include <falaise/snemo/datamodels/raw_trigger_infos.ipp>
DATATOOLS_SERIALIZATION_CLASS_SERIALIZE_INSTANTIATE_ALL(snemo::datamodel::raw_trigger_infos)

/*****************************************
 * snemo::datamodel::raw_calorimeter_hit *
 *****************************************/

#include <falaise/snemo/datamodels/raw_calorimeter_hit.ipp>
DATATOOLS_SERIALIZATION_CLASS_SERIALIZE_INSTANTIATE_ALL(snemo::datamodel::raw_calorimeter_hit)

/*************************************
 * snemo::datamodel::raw_tracker_hit *
 *************************************/

#include <falaise/snemo/datamodels/raw_tracker_hit.ipp>
DATATOOLS_SERIALIZATION_CLASS_SERIALIZE_INSTANTIATE_ALL(snemo::datamodel::raw_tracker_hit)

/******************************
 * snemo::datamodel::raw_data *
 ******************************/

#include <falaise/snemo/datamodels/raw_data.ipp>
DATATOOLS_SERIALIZATION_CLASS_SERIALIZE_INSTANTIATE_ALL(snemo::datamodel::raw_data)
BOOST_CLASS_EXPORT_IMPLEMENT(snemo::datamodel::raw_data)

/************************************************
 * snemo::datamodel::calibrated_calorimeter_hit *
 ************************************************/

#include <falaise/snemo/datamodels/calibrated_calorimeter_hit.ipp>
DATATOOLS_SERIALIZATION_CLASS_SERIALIZE_INSTANTIATE_ALL(
    snemo::datamodel::calibrated_calorimeter_hit)

/********************************************
 * snemo::datamodel::calibrated_tracker_hit *
 ********************************************/

#include <falaise/snemo/datamodels/calibrated_tracker_hit.ipp>
DATATOOLS_SERIALIZATION_CLASS_SERIALIZE_INSTANTIATE_ALL(snemo::datamodel::calibrated_tracker_hit)

/*************************************
 * snemo::datamodel::calibrated_data *
 *************************************/

#include <falaise/snemo/datamodels/calibrated_data.ipp>
DATATOOLS_SERIALIZATION_CLASS_SERIALIZE_INSTANTIATE_ALL(snemo::datamodel::calibrated_data)
BOOST_CLASS_EXPORT_IMPLEMENT(snemo::datamodel::calibrated_data)

/*********************************************
 * snemo::datamodel::tracker_clustering_data *
 *********************************************/

#include <falaise/snemo/datamodels/tracker_cluster.ipp>
DATATOOLS_SERIALIZATION_CLASS_SERIALIZE_INSTANTIATE_ALL(snemo::datamodel::tracker_cluster)
#include <falaise/snemo/datamodels/tracker_clustering_solution.ipp>
DATATOOLS_SERIALIZATION_CLASS_SERIALIZE_INSTANTIATE_ALL(
    snemo::datamodel::tracker_clustering_solution)
#include <falaise/snemo/datamodels/tracker_clustering_data.ipp>
DATATOOLS_SERIALIZATION_CLASS_SERIALIZE_INSTANTIATE_ALL(snemo::datamodel::tracker_clustering_data)
BOOST_CLASS_EXPORT_IMPLEMENT(snemo::datamodel::tracker_clustering_data)

/*************************************************
 * snemo::datamodel::tracker_trajectory_solution *
 *************************************************/

#include <falaise/snemo/datamodels/base_trajectory_pattern.ipp>
#include <falaise/snemo/datamodels/line_trajectory_pattern.ipp>
DATATOOLS_SERIALIZATION_CLASS_SERIALIZE_INSTANTIATE_ALL(snemo::datamodel::line_trajectory_pattern)
BOOST_CLASS_EXPORT_IMPLEMENT(snemo::datamodel::line_trajectory_pattern)

#include <falaise/snemo/datamodels/helix_trajectory_pattern.ipp>
DATATOOLS_SERIALIZATION_CLASS_SERIALIZE_INSTANTIATE_ALL(snemo::datamodel::helix_trajectory_pattern)
BOOST_CLASS_EXPORT_IMPLEMENT(snemo::datamodel::helix_trajectory_pattern)

#include <falaise/snemo/datamodels/polyline_trajectory_pattern.ipp>
DATATOOLS_SERIALIZATION_CLASS_SERIALIZE_INSTANTIATE_ALL(
    snemo::datamodel::polyline_trajectory_pattern)
BOOST_CLASS_EXPORT_IMPLEMENT(snemo::datamodel::polyline_trajectory_pattern)

#include <falaise/snemo/datamodels/tracker_trajectory.ipp>
#include <falaise/snemo/datamodels/tracker_trajectory_solution.ipp>
DATATOOLS_SERIALIZATION_CLASS_SERIALIZE_INSTANTIATE_ALL(snemo::datamodel::tracker_trajectory)
DATATOOLS_SERIALIZATION_CLASS_SERIALIZE_INSTANTIATE_ALL(
    snemo::datamodel::tracker_trajectory_solution)
#include <falaise/snemo/datamodels/tracker_trajectory_data.ipp>
DATATOOLS_SERIALIZATION_CLASS_SERIALIZE_INSTANTIATE_ALL(snemo::datamodel::tracker_trajectory_data)
BOOST_CLASS_EXPORT_IMPLEMENT(snemo::datamodel::tracker_trajectory_data)

/*****************************************
 * snemo::datamodel::particle_track_data *
 *****************************************/

#include <falaise/snemo/datamodels/particle_track.ipp>
DATATOOLS_SERIALIZATION_CLASS_SERIALIZE_INSTANTIATE_ALL(snemo::datamodel::particle_track)
#include <falaise/snemo/datamodels/particle_track_data.ipp>
DATATOOLS_SERIALIZATION_CLASS_SERIALIZE_INSTANTIATE_ALL(snemo::datamodel::particle_track_data)
BOOST_CLASS_EXPORT_IMPLEMENT(snemo::datamodel::particle_track_data)

/******************************************
 * snemo::datamodel::topology_measurement *
 ******************************************/
#include <falaise/snemo/datamodels/base_topology_measurement.ipp>
#include <falaise/snemo/datamodels/energy_measurement.ipp>
DATATOOLS_SERIALIZATION_CLASS_SERIALIZE_INSTANTIATE_ALL(snemo::datamodel::energy_measurement)
BOOST_CLASS_EXPORT_IMPLEMENT(snemo::datamodel::energy_measurement)
#include <falaise/snemo/datamodels/angle_measurement.ipp>
DATATOOLS_SERIALIZATION_CLASS_SERIALIZE_INSTANTIATE_ALL(snemo::datamodel::angle_measurement)
BOOST_CLASS_EXPORT_IMPLEMENT(snemo::datamodel::angle_measurement)
#include <falaise/snemo/datamodels/vertex_measurement.ipp>
DATATOOLS_SERIALIZATION_CLASS_SERIALIZE_INSTANTIATE_ALL(snemo::datamodel::vertex_measurement)
BOOST_CLASS_EXPORT_IMPLEMENT(snemo::datamodel::vertex_measurement)
#include <falaise/snemo/datamodels/tof_measurement.ipp>
DATATOOLS_SERIALIZATION_CLASS_SERIALIZE_INSTANTIATE_ALL(snemo::datamodel::tof_measurement)
BOOST_CLASS_EXPORT_IMPLEMENT(snemo::datamodel::tof_measurement)


/**************************************
 * snemo::datamodel::topology_pattern *
 **************************************/

#include <falaise/snemo/datamodels/base_topology_pattern.ipp>
#include <falaise/snemo/datamodels/topology_1e_pattern.ipp>
DATATOOLS_SERIALIZATION_CLASS_SERIALIZE_INSTANTIATE_ALL(snemo::datamodel::topology_1e_pattern)
BOOST_CLASS_EXPORT_IMPLEMENT(snemo::datamodel::topology_1e_pattern)

#include <falaise/snemo/datamodels/topology_1e1a_pattern.ipp>
DATATOOLS_SERIALIZATION_CLASS_SERIALIZE_INSTANTIATE_ALL(snemo::datamodel::topology_1e1a_pattern)
BOOST_CLASS_EXPORT_IMPLEMENT(snemo::datamodel::topology_1e1a_pattern)

#include <falaise/snemo/datamodels/topology_1eNg_pattern.ipp>
DATATOOLS_SERIALIZATION_CLASS_SERIALIZE_INSTANTIATE_ALL(snemo::datamodel::topology_1eNg_pattern)
BOOST_CLASS_EXPORT_IMPLEMENT(snemo::datamodel::topology_1eNg_pattern)

#include <falaise/snemo/datamodels/topology_2e_pattern.ipp>
DATATOOLS_SERIALIZATION_CLASS_SERIALIZE_INSTANTIATE_ALL(snemo::datamodel::topology_2e_pattern)
BOOST_CLASS_EXPORT_IMPLEMENT(snemo::datamodel::topology_2e_pattern)

#include <falaise/snemo/datamodels/topology_2eNg_pattern.ipp>
DATATOOLS_SERIALIZATION_CLASS_SERIALIZE_INSTANTIATE_ALL(snemo::datamodel::topology_2eNg_pattern)
BOOST_CLASS_EXPORT_IMPLEMENT(snemo::datamodel::topology_2eNg_pattern)

/***********************************
 * snemo::datamodel::topology_data *
 ***********************************/

#include <falaise/snemo/datamodels/topology_data.ipp>
DATATOOLS_SERIALIZATION_CLASS_SERIALIZE_INSTANTIATE_ALL(snemo::datamodel::topology_data)
BOOST_CLASS_EXPORT_IMPLEMENT(snemo::datamodel::topology_data)

/***********************************
 * snemo::datamodel::sim_digi_data *
 ***********************************/

#include <falaise/snemo/datamodels/sim_trigger_digi_data.ipp>
DATATOOLS_SERIALIZATION_CLASS_SERIALIZE_INSTANTIATE_ALL(snemo::datamodel::sim_trigger_digi_data)
#include <falaise/snemo/datamodels/sim_readout_digi_data.ipp>
DATATOOLS_SERIALIZATION_CLASS_SERIALIZE_INSTANTIATE_ALL(snemo::datamodel::sim_readout_digi_data)
#include <falaise/snemo/datamodels/sim_calo_digi_hit.ipp>
DATATOOLS_SERIALIZATION_CLASS_SERIALIZE_INSTANTIATE_ALL(snemo::datamodel::sim_calo_digi_hit)
#include <falaise/snemo/datamodels/sim_tracker_digi_hit.ipp>
DATATOOLS_SERIALIZATION_CLASS_SERIALIZE_INSTANTIATE_ALL(snemo::datamodel::sim_tracker_digi_hit)
#include <falaise/snemo/datamodels/sim_digi_data.ipp>
DATATOOLS_SERIALIZATION_CLASS_SERIALIZE_INSTANTIATE_ALL(snemo::datamodel::sim_digi_data)
BOOST_CLASS_EXPORT_IMPLEMENT(snemo::datamodel::sim_digi_data)

#endif  // FALAISE_SNEMO_DATAMODELS_THE_SERIALIZABLE_H
