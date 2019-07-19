// -*- mode: c++; -*-

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

#include <falaise/snemo/datamodels/boost_io/timestamp.ipp>
DATATOOLS_SERIALIZATION_CLASS_SERIALIZE_INSTANTIATE_ALL(snemo::datamodel::timestamp)

/**********************************
 * snemo::datamodel::event_header *
 **********************************/

#include <falaise/snemo/datamodels/boost_io/event_header.ipp>
DATATOOLS_SERIALIZATION_CLASS_SERIALIZE_INSTANTIATE_ALL(snemo::datamodel::event_header)
BOOST_CLASS_EXPORT_IMPLEMENT(snemo::datamodel::event_header)

/************************************************
 * snemo::datamodel::calibrated_calorimeter_hit *
 ************************************************/

#include <falaise/snemo/datamodels/boost_io/calibrated_calorimeter_hit.ipp>
DATATOOLS_SERIALIZATION_CLASS_SERIALIZE_INSTANTIATE_ALL(
    snemo::datamodel::calibrated_calorimeter_hit)

/********************************************
 * snemo::datamodel::calibrated_tracker_hit *
 ********************************************/

#include <falaise/snemo/datamodels/boost_io/calibrated_tracker_hit.ipp>
DATATOOLS_SERIALIZATION_CLASS_SERIALIZE_INSTANTIATE_ALL(snemo::datamodel::calibrated_tracker_hit)

/*************************************
 * snemo::datamodel::calibrated_data *
 *************************************/

#include <falaise/snemo/datamodels/boost_io/calibrated_data.ipp>
DATATOOLS_SERIALIZATION_CLASS_SERIALIZE_INSTANTIATE_ALL(snemo::datamodel::calibrated_data)
BOOST_CLASS_EXPORT_IMPLEMENT(snemo::datamodel::calibrated_data)

/*********************************************
 * snemo::datamodel::tracker_clustering_data *
 *********************************************/

#include <falaise/snemo/datamodels/boost_io/tracker_cluster.ipp>
DATATOOLS_SERIALIZATION_CLASS_SERIALIZE_INSTANTIATE_ALL(snemo::datamodel::tracker_cluster)
#include <falaise/snemo/datamodels/boost_io/tracker_clustering_solution.ipp>
DATATOOLS_SERIALIZATION_CLASS_SERIALIZE_INSTANTIATE_ALL(
    snemo::datamodel::tracker_clustering_solution)
#include <falaise/snemo/datamodels/boost_io/tracker_clustering_data.ipp>
DATATOOLS_SERIALIZATION_CLASS_SERIALIZE_INSTANTIATE_ALL(snemo::datamodel::tracker_clustering_data)
BOOST_CLASS_EXPORT_IMPLEMENT(snemo::datamodel::tracker_clustering_data)

/*************************************************
 * snemo::datamodel::tracker_trajectory_solution *
 *************************************************/

#include <falaise/snemo/datamodels/boost_io/base_trajectory_pattern.ipp>
#include <falaise/snemo/datamodels/boost_io/line_trajectory_pattern.ipp>
DATATOOLS_SERIALIZATION_CLASS_SERIALIZE_INSTANTIATE_ALL(snemo::datamodel::line_trajectory_pattern)
BOOST_CLASS_EXPORT_IMPLEMENT(snemo::datamodel::line_trajectory_pattern)

#include <falaise/snemo/datamodels/boost_io/helix_trajectory_pattern.ipp>
DATATOOLS_SERIALIZATION_CLASS_SERIALIZE_INSTANTIATE_ALL(snemo::datamodel::helix_trajectory_pattern)
BOOST_CLASS_EXPORT_IMPLEMENT(snemo::datamodel::helix_trajectory_pattern)

#include <falaise/snemo/datamodels/boost_io/polyline_trajectory_pattern.ipp>
DATATOOLS_SERIALIZATION_CLASS_SERIALIZE_INSTANTIATE_ALL(
    snemo::datamodel::polyline_trajectory_pattern)
BOOST_CLASS_EXPORT_IMPLEMENT(snemo::datamodel::polyline_trajectory_pattern)

#include <falaise/snemo/datamodels/boost_io/tracker_trajectory.ipp>
#include <falaise/snemo/datamodels/boost_io/tracker_trajectory_solution.ipp>
DATATOOLS_SERIALIZATION_CLASS_SERIALIZE_INSTANTIATE_ALL(snemo::datamodel::tracker_trajectory)
DATATOOLS_SERIALIZATION_CLASS_SERIALIZE_INSTANTIATE_ALL(
    snemo::datamodel::tracker_trajectory_solution)
#include <falaise/snemo/datamodels/boost_io/tracker_trajectory_data.ipp>
DATATOOLS_SERIALIZATION_CLASS_SERIALIZE_INSTANTIATE_ALL(snemo::datamodel::tracker_trajectory_data)
BOOST_CLASS_EXPORT_IMPLEMENT(snemo::datamodel::tracker_trajectory_data)

/*****************************************
 * snemo::datamodel::particle_track_data *
 *****************************************/

#include <falaise/snemo/datamodels/boost_io/particle_track.ipp>
DATATOOLS_SERIALIZATION_CLASS_SERIALIZE_INSTANTIATE_ALL(snemo::datamodel::particle_track)
#include <falaise/snemo/datamodels/boost_io/particle_track_data.ipp>
DATATOOLS_SERIALIZATION_CLASS_SERIALIZE_INSTANTIATE_ALL(snemo::datamodel::particle_track_data)
BOOST_CLASS_EXPORT_IMPLEMENT(snemo::datamodel::particle_track_data)
