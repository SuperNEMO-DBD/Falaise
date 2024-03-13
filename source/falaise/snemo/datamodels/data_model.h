// -*- mode: c++ ; -*-
/// \file falaise/snemo/datamodels/data_model.h
/* Author(s) :    Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2011-03-21
 * Last modified: 2024-03-13
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

// This project:
#include <falaise/snemo/datamodels/event.h>
#include <bayeux/datatools/things.h>
#include <bayeux/datatools/i_tree_dump.h>
#include <bayeux/datatools/bit_mask.h>

namespace snedm {

  class labels {
  public:
    /// Return the default string label/name for the 'event record'
    static const std::string & event_record();

    /// Return the default string key for the 'event builder' flag key (boolean property in event header)
    static const std::string & event_builder_key();

    /// Return the default string key for the 'event builder' model key (string property in event header)
    static const std::string & event_builder_model_key();

    // Data bank standard labels/names :

    /// default string label/name for the 'event header'
    static const std::string & event_header();

    /// default string label/name for the 'simulated data'
    static const std::string & simulated_data();

    /// default string label/name for the 'unified digitized data'
    static const std::string & unified_digitized_data();

    /// default string label/name for the 'precalibrated data' (trigger driven)
    static const std::string & precalibrated_data();

    /// default string label/name for the 'calibrated data'
    static const std::string & calibrated_data();

    /// default string label/name for the 'tracker clustering data'
    static const std::string & tracker_clustering_data();

    /// Return the default string label/name for the 'tracker trajectory data'
    static const std::string & tracker_trajectory_data();

    /// Return the default string label/name for the 'particle track data'
    static const std::string & particle_track_data();
  };

  /// \brief Data record printer
  ///
  /// This class specializes the printing of a SuperNEMO data record of the type datatools::things
  ///
  /// Usage:
  /// \code
  /// datatools::things dataRecord;
  /// snedm::data_record_printer printer(dataRecord);
  /// boost::property_tree::ptree printerOptions;
  /// printerOptions.put("indent", "[debug] ");
  /// printerOptions.put("title", "Input data record");
  /// printerOptions.put("udd.list_hits", false);
  /// printerOptions.put("udd.hit_details", false);
  /// printerOptions.put("udd.hit_properties", false);
  /// printerOptions.put("pcd.list_hits", false);
  /// printerOptions.put("pcd.hit_properties", false);
  /// printerOptions.put("cd.list_hits", false);
  /// printerOptions.put("cd.hit_properties", false);
  /// printer.print(std::cout, printerOptions);
  /// \endcode
  struct data_record_printer
  {
    data_record_printer(const datatools::things & data_record_);
    void print(std::ostream & out_,
	       const boost::property_tree::ptree & options_) const;
    const datatools::things & _data_record_;
  };
  
} // end of namespace snedm

#endif  // FALAISE_SNEMO_DATAMODEL_DATA_MODEL_H
