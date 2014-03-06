// -*- mode: c++ ; -*-
/** \file falaise/snemo/processing/mock_tracker_s2c_module.h
 * Author(s) :    Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2011-01-09
 * Last modified: 2014-02-27
 *
 * License:
 *
 * Description:
 *
 *   Mock simulation data processor for tracker MC hits
 *
 * History:
 *
 */

#ifndef FALAISE_SNEMO_PROCESSING_MOCK_TRACKER_S2C_MODULE_H
#define FALAISE_SNEMO_PROCESSING_MOCK_TRACKER_S2C_MODULE_H 1

// Standard library:
#include <string>
#include <map>
#include <vector>

// Third party:
// - Bayeux/mygsl:
#include <mygsl/rng.h>
// - Bayeux/dpp:
#include <dpp/base_module.h>

// This project :
#include <falaise/snemo/datamodels/mock_raw_tracker_hit.h>
#include <falaise/snemo/processing/geiger_regime.h>
#include <falaise/snemo/datamodels/calibrated_data.h>

namespace geomtools {
  class manager;
}

namespace mctools {
  class simulated_data;
}

namespace snemo {

  namespace processing {

    /// \brief Simple modelling of the time and space measurement with the SuperNEMO drift cells in Geiger mode
    class mock_tracker_s2c_module : public dpp::base_module
    {
    public:

      /// Collection of raw tracker hit Intermediate :
      typedef std::list<snemo::datamodel::mock_raw_tracker_hit> raw_tracker_hit_col_type;

      /// Set the geometry manager
      void set_geom_manager(const geomtools::manager & gmgr_);

      /// Getting geometry manager
      const geomtools::manager & get_geom_manager() const;

      /// Set the external PRNG
      void set_external_random(mygsl::rng & rng_);

      /// Reset the external PRNG
      void reset_external_random();

      /// Check if the module use an external PRNG
      bool has_external_random() const;

      /// Return the drift time threshold for peripheral Geiger hits (far from the anode wire)
      double get_peripheral_drift_time_threshold() const;

      /// Return the drift time threshold for delayed Geiger hits
      double get_delayed_drift_time_threshold() const;

      /// Constructor
      mock_tracker_s2c_module(datatools::logger::priority = datatools::logger::PRIO_FATAL);

      /// Destructor
      virtual ~mock_tracker_s2c_module();

      /// Initialization
      virtual void initialize(const datatools::properties  & setup_,
                              datatools::service_manager   & service_manager_,
                              dpp::module_handle_dict_type & module_dict_);

      /// Reset
      virtual void reset();

      /// Data record processing
      virtual process_status process(datatools::things & data_);

    protected:

      /// Set default attributes values
      void _set_defaults();

      /// Getting random number generator
      mygsl::rng & _get_random();

      /// Digitize tracker hits
      void _process_tracker_digitization(const mctools::simulated_data & simulated_data_,
                                         raw_tracker_hit_col_type & raw_tracker_hits_);

      /// Calibrate tracker hits (longitudinal and transverse spread)
      void _process_tracker_calibration(const raw_tracker_hit_col_type & raw_tracker_hits_,
                                        snemo::datamodel::calibrated_data::tracker_hit_collection_type & calibrated_tracker_hits_);

      /// Main process function
      void _process(const mctools::simulated_data & simulated_data_,
                    snemo::datamodel::calibrated_data::tracker_hit_collection_type & calibrated_tracker_hits_);

    private:

      const geomtools::manager * _geom_manager_;       //!< The geometry manager
      std::string   _module_category_;                 //!< The geometry category of the SuperNEMO module
      std::string   _hit_category_;                    //!< The category of the input Geiger hits
      geiger_regime _geiger_;                          //!< Geiger regime tools
      mygsl::rng    _random_;                          //!< internal PRN generator
      mygsl::rng *  _external_random_;                 //!< external PRN generator
      double        _peripheral_drift_time_threshold_; //!< Peripheral drift time threshold
      double        _delayed_drift_time_threshold_;    //!< Delayed drift time threshold
      std::string   _SD_label_;                        //!< The label of the simulated data bank
      std::string   _CD_label_;                        //!< The label of the calibrated data bank
      std::string   _Geo_label_;                       //!< The label of the geometry service
      bool          _store_mc_hit_id_;                 //!< Flag to store the MC true hit ID
      bool          _store_mc_truth_track_ids_;        //!< The flag to reference the MC engine track and parent track IDs associated to this calibrated Geiger hit

      // Macro to automate the registration of the module :
      DPP_MODULE_REGISTRATION_INTERFACE(mock_tracker_s2c_module);

    };

  } // end of namespace processing

} // end of namespace snemo

/***************************
 * OCD support : interface *
 ***************************/

#include <datatools/ocd_macros.h>

// @arg snemo::processing::mock_tracker_s2c_module the name the registered class
DOCD_CLASS_DECLARATION(snemo::processing::mock_tracker_s2c_module)

#endif // FALAISE_SNEMO_PROCESSING_MOCK_TRACKER_S2C_MODULE_H

// end of falaise/snemo/processing/mock_tracker_s2c_module.h
