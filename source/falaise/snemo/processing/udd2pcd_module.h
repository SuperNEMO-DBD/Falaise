// -*- mode: c++ ; -*-
/// \file falaise/snemo/processing/udd2pcd_module.h
/* Author(s) :    Guillaume Oliviero <g.oliviero@ucl.ac.uk>
 *                Emmanuel Chauveau <chauveau@cenbg.in2p3.fr>
 * Creation date: 2022-08-11
 * Last modified: 2023-07-20
 *
 * Description:
 *
 *   Unified Digitized Data to precalibrated data processing module
 *
 * History:
 *
 */

#ifndef FALAISE_SNEMO_PROCESSING_UDD2PCD_MODULE_H
#define FALAISE_SNEMO_PROCESSING_UDD2PCD_MODULE_H 1

// Standard library:
#include <map>
#include <string>
#include <vector>

// Third party:
// - Bayeux/dpp:
#include <dpp/base_module.h>
// - CLHEP
#include <CLHEP/Units/SystemOfUnits.h>

// This project :
#include <falaise/snemo/datamodels/unified_digitized_data.h>
#include <falaise/snemo/datamodels/precalibrated_data.h>
#include <falaise/snemo/processing/module.h>
#include <falaise/snemo/services/geometry.h>
#include <falaise/snemo/services/service_handle.h>

namespace geomtools {
  class manager;
}


namespace snemo {

  namespace processing {

    /// \brief A processing module for UDD data to pCD tracker and calorimeter hits
    class udd2pcd_module : public dpp::base_module {

      enum calorimeter_precalibration_algorithm
      {
	ALGO_FWMEASUREMENT = 0,
	ALGO_STANDARD      = 1,
	ALGO_EXPERT        = 2,
      };

    public:
      // Because dpp::base_module is insane
      virtual ~udd2pcd_module() { this->reset(); }

      /// Initialization
      virtual void initialize(const datatools::properties& ps, datatools::service_manager& /*unused*/,
                              dpp::module_handle_dict_type& /*unused*/);

      /// Reset
      virtual void reset();

      /// Data record processing
      virtual process_status process(datatools::things& event);

    private:

      /// Precalibrate calorimeter hits with fwmeas
      void precalibrate_calo_hits_fwmeas(const snemo::datamodel::unified_digitized_data & udd_data_,
					 snemo::datamodel::PreCalibCalorimeterHitHdlCollection & calo_hits_);

      /// Main process calo function
      void process_calo_impl(const snemo::datamodel::unified_digitized_data & udd_data_,
                             snemo::datamodel::precalibrated_data & pcd_data_);

      /// Precalibrated tracker hits
      void precalibrate_tracker_hits(const snemo::datamodel::unified_digitized_data & udd_data_,
				     snemo::datamodel::PreCalibTrackerHitHdlCollection& tracker_hits_);

      /// Main process tracker function
      void process_tracker_impl(const snemo::datamodel::unified_digitized_data & udd_data_,
                                snemo::datamodel::precalibrated_data & pcd_data_);

    private:
      std::string _udd_input_tag_{};  //!< The label of the unified digitized bank
      std::string _pcd_output_tag_{}; //!< The label of the precalibrated data bank

      double _calo_adc2volt_;
      double _calo_sampling_period_;
      double _calo_postrigger_time_;
      bool _calo_discard_empty_waveform_;

      // Macro to automate the registration of the module :
      DPP_MODULE_REGISTRATION_INTERFACE(udd2pcd_module)

    };

  }  // end of namespace processing

}  // end of namespace snemo

/***************************
 * OCD support : interface *
 ***************************/

#include <datatools/ocd_macros.h>

// @arg snemo::processing::udd2pcd_module the name the registered class
DOCD_CLASS_DECLARATION(snemo::processing::udd2pcd_module)

#endif  // FALAISE_SNEMO_PROCESSING_UDD2PCD_MODULE_H

// end of falaise/snemo/processing/udd2pcd_module.h
