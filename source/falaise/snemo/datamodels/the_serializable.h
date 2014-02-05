// -*- mode: c++; -*-
/** \file falaise/snemo/datamodels/the_serializable.h */

#ifndef FALAISE_SNEMO_DATAMODELS_THE_SERIALIZABLE_H
#define FALAISE_SNEMO_DATAMODELS_THE_SERIALIZABLE_H 1

// Third party
// - Boost
#include <boost/serialization/export.hpp>
// - Bayeux/datatools
#include <datatools/archives_instantiation.h>

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
DATATOOLS_SERIALIZATION_CLASS_SERIALIZE_INSTANTIATE_ALL(snemo::datamodel::calibrated_calorimeter_hit)


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
DATATOOLS_SERIALIZATION_CLASS_SERIALIZE_INSTANTIATE_ALL(snemo::datamodel::tracker_clustering_solution)
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
DATATOOLS_SERIALIZATION_CLASS_SERIALIZE_INSTANTIATE_ALL(snemo::datamodel::polyline_trajectory_pattern)
BOOST_CLASS_EXPORT_IMPLEMENT(snemo::datamodel::polyline_trajectory_pattern)

#include <falaise/snemo/datamodels/tracker_trajectory.ipp>
#include <falaise/snemo/datamodels/tracker_trajectory_solution.ipp>
DATATOOLS_SERIALIZATION_CLASS_SERIALIZE_INSTANTIATE_ALL(snemo::datamodel::tracker_trajectory)
DATATOOLS_SERIALIZATION_CLASS_SERIALIZE_INSTANTIATE_ALL(snemo::datamodel::tracker_trajectory_solution)
#include <falaise/snemo/datamodels/tracker_trajectory_data.ipp>
DATATOOLS_SERIALIZATION_CLASS_SERIALIZE_INSTANTIATE_ALL(snemo::datamodel::tracker_trajectory_data)
BOOST_CLASS_EXPORT_IMPLEMENT(snemo::datamodel::tracker_trajectory_data)


#endif // FALAISE_SNEMO_DATAMODELS_THE_SERIALIZABLE_H

/* end of falaise/sncore/models/the_serializable.h */
