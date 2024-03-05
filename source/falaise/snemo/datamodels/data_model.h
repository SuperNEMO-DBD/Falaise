// -*- mode: c++ ; -*-
/// \file falaise/snemo/datamodels/data_model.h
/* Author(s) :    Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2011-03-21
 * Last modified: 2022-05-03
 *
 * Description:
 *
 *   SuperNEMO data model
 *
 * History:
 *
 */

#ifndef FALAISE_SNEMO_DATAMODEL_DATA_MODEL_H
#define FALAISE_SNEMO_DATAMODEL_DATA_MODEL_H 1

// Standard library:
#include <string>

// Third party:
// - Bayeux/datatools:
#include <falaise/snemo/datamodels/event.h>

namespace snedm {

class labels {
 public:
  /// Return the default string label/name for the 'event record'
  static const std::string& event_record();

  // Data bank standard labels/names :

  /// default string label/name for the 'event header'
  static const std::string& event_header();

  /// default string label/name for the 'simulated data'
  static const std::string& simulated_data();

  /// default string label/name for the 'unified digitized data'
  static const std::string& unified_digitized_data();

  /// default string label/name for the 'precalibrated data' (trigger driven)
  static const std::string& precalibrated_data();

  /// default string label/name for the 'clusterized precalibrated data' (event driven)
  static const std::string& clusterized_precalibrated_data();

  /// default string label/name for the 'clusterized calibrated data'
  static const std::string& clusterized_calibrated_data();

  /// default string label/name for the 'calibrated data'
  static const std::string& calibrated_data();

  /// default string label/name for the 'tracker clustering data'
  static const std::string& tracker_clustering_data();

  /// Return the default string label/name for the 'tracker trajectory data'
  static const std::string& tracker_trajectory_data();

  /// Return the default string label/name for the 'particle track data'
  static const std::string& particle_track_data();
};

}  // end of namespace snedm

#endif  // FALAISE_SNEMO_DATAMODEL_DATA_MODEL_H
